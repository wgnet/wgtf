import QtQuick 2.3
import QtQuick.Controls 1.2
import QtQuick.Dialogs 1.2
import WGControls 2.0
import WGControls 1.0 as WG1

WGTreeView {
	id: propertyTreeView

	ComponentExtension {
		id: componentExtension
	}

	extensions: [componentExtension]

	property Component propertyDelegate: Loader {
		width: itemWidth
		height: 22
        sourceComponent: itemData.component
    }

	columnDelegates: [columnDelegate, propertyDelegate]
	columnSequence: [0, 0]
	columnSpacing: 1

	roles: [
		"value",
		"valueType",
		"key",
		"keyType",
		"isCollection",
		"elementValueType",
		"elementKeyType",
        "readOnly",
        "enabled",
        "multipleValues",
		//DEPRECATED
		"enumModel",
		"definition",
		"definitionModel",
		"object",
		"rootObject",
		"minValue",
		"maxValue",
		"stepSize",
		"decimals",
		"indexPath",
		"urlIsAssetBrowser",
		"urlDialogTitle",
		"urlDialogDefaultFolder",
		"urlDialogNameFilters",
		"urlDialogSelectedNameFilter",
		"urlDialogModality",
		"isReadOnly",
		"thumbnail"
	]

	Dialog {
		id: keyDialog
		modality: Qt.ApplicationModal
		standardButtons: StandardButton.Ok | StandardButton.Cancel

		property var onAcceptedFunc: null

		Row {
			width: childrenRect.width
			height: childrenRect.height
			spacing: 5

			WG1.WGLabel{
				id: label
				text: "Key Value:"
			}

			WG1.WGTextBox {
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

	WG1.WGContextMenu {
        id: contextMenu
        path: "WGPropertyTreeMenu"

		WG1.WGAction {
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

			property var item: contextMenu.contextObject
			
			onItemChanged: {
				if (item != null && item.isCollection) {
					active = true
				}
				else {
					active = false
				}
			}
		}

		WG1.WGAction {
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

			property var item: contextMenu.contextObject

			onItemChanged: {
				if (item != null && item.isCollection && item.hasChildren) {
					active = true
				}
				else {
					active = false
				}
			}
		}

		WG1.WGAction {
			actionId: "WGPropertyTreeMenu|.Insert"

			onTriggered: {
				var collection = parentItem.value
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
					var index = propertyTreeView.internalModel.rowIndex(item)
					collection.insertItem(index)
				}
			}

			property var item: contextMenu.contextObject
			property var parentItem: propertyTreeView.internalModel.parent(item)

			onParentItemChanged: {
				if (parentItem != null && parentItem.isCollection == true) {
					active = true
				}
				else {
					active = false
				}
			}
		}

		WG1.WGAction {
			actionId: "WGPropertyTreeMenu|.Remove"

			onTriggered: {
				var collection = parentItem.value
				if (collection.isMapping()) {
					var key = item.key
					collection.removeItem(key)
				}
				else {
					var index = propertyTreeView.internalModel.rowIndex(item)
					collection.removeItem(index)
				}
			}

			property var item: contextMenu.contextObject
			property var parentItem: propertyTreeView.internalModel.parent(item)

			onParentItemChanged: {
				if (parentItem != null && parentItem.isCollection == true) {
					active = true
				}
				else {
					active = false
				}
			}
		}
    }

	onItemClicked: {
		if (mouse.button != Qt.RightButton) {
			return
		}

		contextMenu.contextObject = internalModel.indexToItem(rowIndex)
		contextMenu.popup()
	}
}
