import QtQuick 2.1
import QtQuick.Controls 1.0
import QtQuick.Layouts 1.0
import QtQuick.Window 2.2
import WGControls 1.0
import WGCopyableFunctions 1.0

Window {
	id: root

	minimumWidth: defaultSpacing.minimumPanelWidth

	flags: Qt.Window | Qt.WindowTitleHint | Qt.CustomizeWindowHint
	color: palette.mainWindowColor
	modality: Qt.ApplicationModal
	property bool accepted: false
	property variant sourceData;

	WGFrame {
		id: mainFrame
		anchors.fill: parent

		Label {
			id: searchBoxLabel
			x: editMacro.leftMargin
			y: 2
			text: "Search:"
		}

		WGTextBox {
			id: searchBox
			y: 2
			anchors.left: searchBoxLabel.right
			anchors.right: parent.right
			Component.onCompleted: {
				WGCopyableHelper.disableChildrenCopyable(searchBox);
			}
		}

		WGFilteredTreeModel {
			id: editModel
			source: sourceData

			filter: WGTokenizedStringFilter {
				id: stringFilter
				filterText: searchBox.text
				splitterChar: " "
			}

			ValueExtension {}
			ColumnExtension {}
			ComponentExtension {}
			TreeExtension {
				id: treeModelExtension
				selectionExtension: treeModelSelection
			}
			ThumbnailExtension {}
			SelectionExtension {
				id: treeModelSelection
				multiSelect: true
			}
		}

		WGTreeView {
			id: editMacro
			anchors.top: searchBox.bottom
			anchors.left: parent.left
			anchors.right: parent.right
			anchors.bottom: buttons.top
			model: editModel
			rightMargin: 8 // leaves just enought space for conventional slider
			columnSequence: [0, 0]
			columnDelegates: [defaultColumnDelegate, propertyDelegate]
			selectionExtension: treeModelSelection
			treeExtension: treeModelExtension
			childRowMargin: 2
			columnSpacing: 4
			lineSeparator: false
			autoUpdateLabelWidths: true
			//flatColourisation: false
			//depthColourisation: 5
			property Component propertyDelegate: Loader {
				clip: true
				sourceComponent: itemData != null ? itemData.component : null
			}
		}

		WGExpandingRowLayout {
			id: buttons
			Layout.preferredHeight: defaultSpacing.minimumRowHeight + defaultSpacing.doubleBorderSize
			Layout.fillWidth: true

			anchors.left: parent.left
			anchors.right: parent.right
			anchors.bottom: parent.bottom

			WGPushButton {
				id: okButton
				text: "Ok"
				Layout.preferredWidth: 60
				checkable: false
				signal onOk();
				onClicked: {
					beginUndoFrame();
					onOk();
					if(root.accepted)
					{
						endUndoFrame();
					}
					else
					{
						abortUndoFrame();
					}
					root.closing( root.close )
					root.close()
				}
			}

			WGPushButton {
				id: cancelButton
				text: "Cancel"
				Layout.preferredWidth: 60
				checkable: false
				signal onCancel();
				onClicked: {
					onCancel();
					root.closing( root.close )
					root.close()
				}
			}
		}
	}
}
