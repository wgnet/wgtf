import QtQuick 2.5
import QtQuick.Controls 1.4
import QtQuick.Layouts 1.3
import WGControls 2.0

Item {
    id: iconDelegateInternal
    objectName: "IconDelegate" + index
    WGComponent { type: "WGIconViewDelegate20" }

    /** This is a link to the containing WGItemView.*/
    property var view: null
    /** The index for this row. This is a model index exposed by the model.*/
    property var rowIndex: modelIndex
    /** Switch to load asynchronously or synchronously.*/
    property bool asynchronous: false
    /** item row index.*/
    property int itemRowIndex: index

    /** Signals that this item received a mouse press.
    \param mouse The mouse data at the time.
    \param itemIndex The index of the item.*/
    signal itemPressed(var mouse, var itemIndex)
    /** Signals that this item was clicked.
    \param mouse The mouse data at the time.
    \param itemIndex The index of the item.*/
    signal itemClicked(var mouse, var itemIndex)
    /** Signals that this item was double clicked.
    \param mouse The mouse data at the time.
    \param itemIndex The index of the item.*/
    signal itemDoubleClicked(var mouse, var itemIndex)
    /** Signals that this item was double clicked.
    \param mouse The mouse data at the time.
    \param itemIndex The index of the item.*/
    signal itemHovered(var itemIndex)

    /* Aliasing the isSelected state for automated testing.*/
    property alias isSelected: iconViewItemBackground.isSelected
    property var hoveredIndex: null

    /** Checks if the mouse is in this item.*/
    property bool isHovered: iconViewMouseArea.isHovered

    property var getItemImageSource: function(itemData) {
        return "";
    }

    property var getItemText: function(itemData) {
        return "";
    }

    property var itemMouseArea: iconViewMouseArea;

    onIsHoveredChanged: {
        if (isHovered) {
            hoveredIndex = modelIndex;
            itemHovered(modelIndex);
        }
        else if (hoveredIndex == modelIndex) {
            hoveredIndex = null;
        }
    }

    Loader {
        id: iconViewItemBackground
        objectName: "IconViewItemBackground"
        anchors.fill: parent
        sourceComponent: Item {
		    Rectangle {
			    id: backgroundArea
			    anchors.fill: parent
			    color: palette.highlightShade
			    opacity: isSelected ? 1 : 0.5
			    visible: isHovered || isSelected
		    }

		    Rectangle {
			    id: keyboardFocusArea
			    anchors.fill: parent
			    color: palette.highlightShade
			    opacity: 0.25
			    visible: isCurrent
		    }
	    }

        property bool isSelected: view != null ? view.selectionModel.isSelected(modelIndex) : false
        property bool isCurrent: view != null ? view.selectionModel.currentIndex === modelIndex : false;
        property bool isHovered: hoveredIndex != null

        Connections {
            target: view
            onSelectionChanged: {
                iconViewItemBackground.isSelected = Qt.binding( function() { return view.selectionModel.isSelected(modelIndex); } )
            }
        }
    }

    MouseArea {
        id: iconViewMouseArea
        anchors.fill: parent
        cursorShape: Qt.PointingHandCursor
        hoverEnabled: true
        drag.target: iconViewMouseArea
        acceptedButtons: Qt.RightButton | Qt.LeftButton

        property var dragActive: drag.active
        property bool isHovered: false
        onDragActiveChanged: {
            if (drag.active) {
                Drag.mimeData = iconDelegateInternal.view.mimeData(iconDelegateInternal.view.selectionModel.selectedIndexes)
                Drag.active = true
            }
        }

        onContainsMouseChanged: { isHovered = containsMouse }

        Drag.dragType: Drag.Automatic
        Drag.proposedAction: Qt.MoveAction
        Drag.supportedActions: Qt.MoveAction | Qt.CopyAction

        onPressed: {
            isHovered = false;
            iconDelegateInternal.itemPressed(mouse, modelIndex);
        }

        onClicked: {
            isHovered = false;
            iconDelegateInternal.itemClicked(mouse, modelIndex);
        }

        onDoubleClicked: {
            isHovered = false;
            iconDelegateInternal.itemDoubleClicked(mouse, modelIndex);
        }

        Drag.onDragStarted: {
            Drag.hotSpot.x = mouseX
            Drag.hotSpot.y = mouseY
        }

        Drag.onDragFinished: {
            makeFakeMouseRelease();
        }
    }

    Loader {
        id: iconViewItem
        objectName: "iconViewItem" + index
        anchors.fill: parent
        sourceComponent: itemDelegate

        property var itemData: model
        property var isSelected: view != null ? view.selectionModel.isSelected(modelIndex) : false
    }

    property Component itemDelegate: ColumnLayout {
        objectName: typeof(iconLabel.text) != "undefined" ? iconLabel.text : "itemArea"
        spacing: 0
        anchors.fill: parent
        anchors.margins: defaultSpacing.standardBorderSize

        Item {
            Layout.preferredHeight: 64
            Layout.preferredWidth: 64
            Layout.alignment: Qt.AlignVCenter | Qt.AlignHCenter

            WGImage {
                id: iconLoader
                height: 64
                width: 64
                fillMode: sourceSize.width > parent.width || sourceSize.height > parent.height ? Image.PreserveAspectFit : Image.Pad
                asynchronous: true
                source: itemImageSource
            }
        }

        Text {
            id: iconLabel
            objectName: "IconLabel"

            text: itemText
            horizontalAlignment: Text.AlignHCenter
            lineHeightMode: Text.FixedHeight
            lineHeight: 11
            color: enabled ? palette.textColor : palette.disabledTextColor
            y: -2

            Layout.preferredWidth: parent.width - defaultSpacing.rowSpacing * 2
            Layout.preferredHeight: defaultSpacing.minimumRowHeight * 2
            Layout.alignment: Qt.AlignTop | Qt.AlignHCenter

            maximumLineCount: {
                var lines = 2
                if (isSelected) {
                    lines += 2
                }
                return lines
            }
            wrapMode: Text.WrapAnywhere
            font.pointSize: 9
            font.bold: false
            elide: Text.ElideRight

            Connections {
                target: iconDelegateInternal
                onIsSelectedChanged: {
                    if (isSelected) {
                        if (iconLabel.truncated) {
                            iconLabel.font.pointSize = 7
                        }
                    }
                    else {
                        iconLabel.font.pointSize = 9
                    }
                }
            }
        }
    }
}
