import QtQuick 2.5
import QtQuick.Controls 1.4
import QtQuick.Layouts 1.3
import QtQml.Models 2.2
import WGControls 2.0
import WGControls.Views 2.0

/*!
 \ingroup wgcontrols
 \brief
    Creates a grid of icons/thumbnails with filnames.

Example:
\code{.js}
    WGIconView {
        anchors.fill: parent
        assetBrowser: myAssetBrowser
        contextMenu: myAssetBrowser.contextMenu

        model: myContentsModel
    }
\endcode

  \todo - Make this a proper custom IconView with multi-select, drag and drop etc.

*/

WGIconViewBase {
    id: iconView
    objectName: "WGIconView"
    WGComponent { type: "WGIconView20" }

    view: itemView
    internalModel: itemView.extendedModel

    /** The adapted model used internally.*/
	property alias internalModel: iconView.model
    /** The data model providing the view with information to display.*/
    property alias model: itemView.model
	 /** The model containing the selected items/indices.*/
    property alias selectionModel: itemView.selectionModel
	property alias internalSelectionModel: itemView.internalSelectionModel
    /** A replacement for GridView's currentIndex that can use either a number or a QModelIndex.*/
    property var currentIndex: itemView.selectionModel.currentIndex
	property var extensions: []

    clip: true

    /*Specifies if icon view needs to support multiple selection, default value is true*/
    property alias supportMultiSelect: itemView.supportMultiSelect
    /*! Override this for determine if the node can be selected */
    property var allowSelect: function(rowIndexToBeSelected, exisingSelectedIndexes, modifiers) {
        return true;
    }

    signal selectionChanged()

    onCurrentIndexChanged: {
        if (typeof(currentIndex) == "number") {
            currentRow = currentIndex
            return;
        }
        currentRow = itemView.getRow(currentIndex);
		if (itemView.selectionModel.currentIndex != currentIndex && currentIndex != null) {
			itemView.selectionModel.setCurrentIndex(currentIndex, ItemSelectionModel.NoUpdate);
		}
    }
    onCurrentRowChanged: {
        currentIndex = currentRow < 0 ? null : internalModel.index(currentRow, 0);
    }
    Connections {
        target: itemView

        onSelectionChanged: selectionChanged()
        onCurrentChanged: {
            if (currentIndex != itemView.selectionModel.currentIndex) {
                currentIndex = itemView.selectionModel.currentIndex;
            }
        }
    }

	Keys.onUpPressed: {
		if (flow == GridView.FlowLeftToRight) {
            itemView.moveBackwards(event);
        }
        else if (flow == GridView.FlowTopToBottom) {
            itemView.movePrevious(event);
        }
    }
    Keys.onDownPressed: {
		if (flow == GridView.FlowLeftToRight) {
            itemView.moveForwards(event);
        }
        else if (flow == GridView.FlowTopToBottom ) {
            itemView.moveNext(event);
        }
    }
    Keys.onLeftPressed: {
        if (flow == GridView.FlowLeftToRight) {
            itemView.movePrevious(event);
        }
		else if (flow == GridView.FlowTopToBottom) {
            itemView.moveBackwards(event);
        }
    }
    Keys.onRightPressed: {
        if (flow == GridView.FlowLeftToRight) {
            itemView.moveNext(event);
        }
		else if (flow == GridView.FlowTopToBottom) {
            itemView.moveForwards(event);
        }
    }

    property bool __skippedPress: false
    onItemPressed: {
        var selected = itemView.selectionModel.selectedIndexes;
        for ( var i = 0; i < selected.length; ++i ) {
            if ( selected[i] === rowIndex ) {
                __skippedPress = true;
                return;
            }
        }
        itemView.select(mouse, rowIndex);
        forceActiveFocus();
    }
    onItemClicked: {
        if ( __skippedPress ) {
            itemView.select(mouse, rowIndex);
            forceActiveFocus();
        }
        __skippedPress = false;
    }

	WGItemViewCommon {
        id: itemView
		viewExtension: createExtension("GridExtension")
        allowSelect: function(rowIndexToBeSelected, exisingSelectedIndexes, modifiers) {
            return iconView.allowSelect(rowIndexToBeSelected, exisingSelectedIndexes, modifiers);
        }

		onViewExtensionChanged: {
			if (viewExtension == null) {
				return;
			}
			viewExtension.flowCount = Qt.binding(function() { 
				var flowCount = -1;
				if (iconView.flow == GridView.FlowLeftToRight) {
					flowCount = iconView.width / iconView.cellWidth;
				}
				else if (iconView.flow == GridView.FlowTopToBottom) {
					flowCount = iconView.height / iconView.cellHeight;
				}
				return flowCount;
			})
		}
    }
}
