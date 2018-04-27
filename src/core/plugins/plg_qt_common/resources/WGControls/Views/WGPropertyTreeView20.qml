import QtQuick 2.5
import QtQuick.Controls 1.4
import QtQuick.Dialogs 1.2
import QtQuick.Controls.Private 1.0

import WGControls 2.0
import WGControls.Views 2.0
import WGControls.Styles 2.0

/*!
 \ingroup wgcontrols
*/
WGTreeView {
    id: propertyTreeView
    WGComponent { type: "WGPropertyTreeView20" }

    property bool readOnly: false

    style: WGPropertyTreeViewStyle {}

    property var componentExtension: createExtension("ComponentExtension")

    extensions: [componentExtension]

    property var selectedPath: null
    property var selectedPathBeforeReset: null

    onCurrentItemChanged: {
        if(currentItem == null) {
            selectedPath = null;
        }
    }

    Connections {
        target: typeof(model) != "undefined" ? model : null
        ignoreUnknownSignals: true
        onLayoutAboutToBeChanged: {
            selectedPathBeforeReset = selectedPath;
        }
        onModelAboutToBeReset: {
            selectedPathBeforeReset = selectedPath;
        }
    }

    function getPropertyPath(itemData) {
        if(itemData == null) {
            return null;
        }
        if(typeof(itemData.indexPath) != "undefined") {
            return itemData.indexPath;
        }
        if(typeof(itemData.path) != "undefined") {
            return itemData.path;
        }
        return null;
    }

    property Component propertyDelegate: Item {
        id: propertyComponentParent
        height: delegateLoader.status == Loader.Ready ? delegateLoader.item.height : defaultSpacing.minimumRowHeight;
        width: parent != null ? parent.width : 0
        implicitWidth: delegateLoader.status == Loader.Ready ? delegateLoader.item.implicitWidth : 0

        property var path: getPropertyPath(itemData)
        property bool rowCurrent: rowIsCurrent

        Connections {
            target: typeof(model) != "undefined" ? model : null
            ignoreUnknownSignals: true
            onLayoutChangedComplete: {
                onModelReset();
            }
            onModelResetComplete: {
                onModelReset();
            }
        }

        function onModelReset() {
            if(delegateLoader.item != null &&
                !delegateLoader.item.activeFocus &&
                selectedPathBeforeReset != null &&
                selectedPathBeforeReset == path) {
                forcePropertyFocused();
            }
        }

        function forcePropertyFocused() {
            if(typeof(delegateLoader.item.forceInputFocus) != "undefined") {
                delegateLoader.item.forceInputFocus();
            } else {
                delegateLoader.item.forceActiveFocus();
            }
        }

        Component.onCompleted: {
            getImplicitWidths = function() {
                return Qt.vector3d(implicitWidth, implicitWidth, -1);
            }
        }

        onRowCurrentChanged: {
            if (rowCurrent && delegateLoader.item != null) {
                forcePropertyFocused();
                selectedPath = path;
            }
        }

        Connections {
            target: delegateLoader.item
            onActiveFocusChanged: {
                if (delegateLoader.item.activeFocus && !propertyComponentParent.rowCurrent) {
                    propertyTreeView.selectionModel.setCurrentIndex(itemRowModelIndex, 0)
                }
            }
        }

        Loader {
            id: delegateLoader
			asynchronous : itemData != null && itemData.componentSupportsAsync
			visible : status == Loader.Ready
            width: parent.width
            property bool readOnlyComponent: propertyTreeView.readOnly

            sourceComponent: itemData != null ? itemData.component : null
        }

        Text {
            anchors.verticalCenter: parent.verticalCenter
            anchors.left: parent.left
            anchors.leftMargin: defaultSpacing.rowSpacing
            visible: itemDepth == 0 && propertyTreeView.readOnly && delegateLoader.sourceComponent == null
            color: palette.readonlyColor
            font.bold: false
            text: "(Read Only)"
        }

        WGPushButton {
            anchors.right: parent.right
            anchors.rightMargin: defaultSpacing.rowSpacing
            anchors.verticalCenter: parent.verticalCenter
            radius: defaultSpacing.halfRadius
            height: 16
            width: 16
            iconSource: canUnlock ? "../icons/locked_red_8x8.png" : "../icons/unlocked_red_8x8.png"
            visible: canUnlock || canLock
            tooltip: canUnlock ? "Unlock Component" : "Lock Component"

			// TODO: This does not belong in the WGControls layer.
			// Not going to pull this out into a wgs extension for now as we should look into creating
			// a property panel tool bar that can leverage the layout manager to auto popular these action extensions.
			// could be used to add + & - icons for adding and removing from collections as well
			property var canUnlock: itemData ? canExecuteAction("StreamingEditorUnlockComponent", [itemData.value]) : false;
			property var canLock: itemData ? canExecuteAction("StreamingEditorLockComponent", [itemData.value]) : false;

            onClicked: {
                if (canUnlock) {
					executeAction("StreamingEditorUnlockComponent", [itemData.value]);
				}
				else if (canLock) {
					executeAction("StreamingEditorLockComponent", [itemData.value]);
				}

				canUnlock = canExecuteAction("StreamingEditorUnlockComponent", [itemData.value]);
				canLock = canExecuteAction("StreamingEditorLockComponent", [itemData.value]);
            }
        }
    }

    property Component labelDelegate: Text {
        id: labelDelegate
        objectName: "defaultColumnDelegate_" + text
        // extra space added to give a bit of margin in column. implicitWidth for text objects is readonly.
        text: (itemValue !== undefined ? itemValue + "  " : "")
        color: propertyTreeView.readOnly ? palette.readonlyColor : palette.textColor
        verticalAlignment: Text.AlignVCenter
        height: defaultSpacing.minimumRowHeight

        Component.onCompleted: {
            getImplicitWidths = function() {
                return Qt.vector3d(implicitWidth, 100, implicitWidth);
            }
        }

        font.bold: itemDepth == 0 && itemData != null && itemData.hasChildren

        WGToolTipBase {
            title: itemData !== null && itemData !== undefined && itemData.tooltip !== undefined? itemData.tooltip : itemData !== null && itemData !== undefined && itemData.display !== undefined ? itemData.display : ""
            text: itemData !== null && itemData !== undefined && itemData.description !== undefined ? itemData.description : ""
            tooltipArea: colMouseArea
        }
    }

    columnDelegates: [labelDelegate, propertyDelegate]
    columnSpacing: 1

    property var proxyModel: WGColumnLayoutProxy {
        id: columnLayoutProxy
        columnSequence: [0, 0]
    }

    property alias propertyModel: propertyTreeView.model
    property alias model: columnLayoutProxy.sourceModel
    propertyModel: proxyModel

    Dialog {
        id: keyDialog
        modality: Qt.ApplicationModal
        standardButtons: StandardButton.Ok | StandardButton.Cancel

        property var onAcceptedFunc: null

        Row {
            width: childrenRect.width
            height: childrenRect.height
            spacing: 5

            WGLabel{
                id: label
                text: "Key Value:"
            }

            WGTextBox {
                id: input
                width: 120
                height: label.height
            }
        }

        onAccepted: {
            if (onAcceptedFunc != null) {
                onAcceptedFunc(input.text)
            }
            onAcceptedFunc = null
        }

        onRejected: {
            onAcceptedFunc = null
        }
    }
	
	WGMenu {
		id: contextMenu
		path: "WGPropertyTreeMenu"

		property var contextIndexes: []
		property var contextItems: []

		Connections {
			target: propertyTreeView
			onSelectionChanged: {
				var indexes = [];
				var items = [];
				var objects = [];

				if (propertyTreeView.selectionModel.isSelected(propertyTreeView.selectionModel.currentIndex)) {
					indexes.push(propertyTreeView.selectionModel.currentIndex);
				}
				var selectedIndexes = propertyTreeView.selectionModel.selectedIndexes;
				for (var i = 0; i < selectedIndexes.length; ++i) {
					var selectedIndex = selectedIndexes[i];
					if (selectedIndex != propertyTreeView.selectionModel.currentIndex) {
						indexes.push(selectedIndex);
					}
				}
				for (var i = 0; i < indexes.length; ++i) {
					items.push(propertyTreeView.view.extendedModel.indexToItem(indexes[i]));
					objects.push(items[i].value);
				}

				contextMenu.contextIndexes = indexes;
				contextMenu.contextItems = items;
				contextMenu.contextObject = objects;
			}
		}

		Connections {
			target: propertyTreeView.model
			onModelReset: {
				contextMenu.contextIndexes = [];
				contextMenu.contextItems = [];
				contextMenu.contextObject = [];
			}
		}
    }
	
	WGAction {
        actionId: "PropertyTreeCopy"
		active: propertyTreeView.activeFocus && propertyTreeView.view.canCopy(contextMenu.contextIndexes);

        onTriggered: {
            propertyTreeView.view.copy(contextMenu.contextIndexes);
        }
    }

	WGAction {
        actionId: "PropertyTreePaste"
		active: propertyTreeView.activeFocus && propertyTreeView.view.canPaste(contextMenu.contextIndexes);
        onTriggered: {
            propertyTreeView.view.paste(contextMenu.contextIndexes);
        }
    }

	WGAction {
        actionId: "WGPropertyTreeMenu|.Push"

        onTriggered: {
            var collection = item.value
            if (collection.isMapping()) {
                keyDialog.onAcceptedFunc = function(key) {
                    if (key == "") {
                        return
                    }
                    collection.insertItem(key)
                }
                keyDialog.open()
            }
            else {
                var index = collection.count()
                collection.insertItem(index)
            }
        }

        property var item: contextMenu.contextItems.length == 1 ? contextMenu.contextItems[0] : null

        onItemChanged: {
            if (item != null && item.isCollection) {
                active = propertyTreeView.activeFocus && !propertyTreeView.readOnly && item.canInsert
            }
            else {
                active = false
            }
        }
    }

	WGAction {
        actionId: "WGPropertyTreeMenu|.Pop"

        onTriggered: {
            var collection = item.value
            if (collection.isMapping()) {
                var index = collection.count() - 1
                var childItem = propertyTreeView.internalModel.item(index, 0, item)
                var key = childItem.key
                collection.removeItem(key)
            }
            else {
                var index = collection.count() - 1
                collection.removeItem(index)
            }
        }

        property var item: contextMenu.contextItems.length == 1 ? contextMenu.contextItems[0] : null

        onItemChanged: {
            if (item != null && item.isCollection) {
                active = propertyTreeView.activeFocus && !propertyTreeView.readOnly && item.canRemove
            }
            else {
                active = false
            }
        }
    }

	WGAction {
        actionId: "WGPropertyTreeMenu|.Insert"

        onTriggered: {
            if (collection.isMapping()) {
                keyDialog.onAcceptedFunc = function(key) {
                    if (key == "") {
                        return
                    }
                    collection.insertItem(key)
                }
                keyDialog.open()
            }
            else {
				var index = item.collectionIndex;
                collection.insertItem(index)
            }
        }

		property var item: contextMenu.contextItems.length == 1 ? contextMenu.contextItems[0] : null
		property var collection: item != null ? item.parentCollection : null

        active: propertyTreeView.activeFocus && collection != null && !propertyTreeView.readOnly && item.canInsertIntoParent
    }
	
	WGAction {
        actionId: "WGPropertyTreeMenu|.Remove"

        onTriggered: {
            var index = collection.isMapping() ? item.key : item.collectionIndex
            collection.removeItem(index);
        }

		property var item: contextMenu.contextItems.length == 1 ? contextMenu.contextItems[0] : null
		property var collection: item != null ? item.parentCollection : null

        active: propertyTreeView.activeFocus && collection != null && !propertyTreeView.readOnly && item.canRemoveFromParent
    }

	onItemClicked: {
        if (mouse.button != Qt.RightButton) {
            return
        }

        contextMenu.popup()
    }
}
