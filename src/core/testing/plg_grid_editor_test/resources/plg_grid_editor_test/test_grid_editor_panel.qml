import QtQuick 2.4
import QtQuick.Controls 1.2
import QtQuick.Layouts 1.0
import QtQuick.Dialogs 1.2
import WGControls 2.0
import WGControls.Views 2.0
import WGControls.Styles 2.0

WGPanel {
    id: testGridPanel

    title: "Grid Editor Test"
    layoutHints: { 'test': 0.1 }
    color: palette.mainWindowColor

	// our fake enums for perforce status
	readonly property var noIcon: 0
	readonly property var add: 1
	readonly property var markDelete: 2
	readonly property var edit: 3
	property int p4status: noIcon
	
    property var modelCount: 0
	property var models: getObjects()
	
	function getObjects(){
		var objectArr = []
		for(var i = 0; i < maxObjectCount; ++i) {
			objectArr.push(objects.value(i));
		}
		return objectArr;
	}
	
    WGTransposeProxy {
        id: objectsModel
        sourceModel: WGMergeProxy {
            id: mergeProxy
        }
    }

	ColumnLayout {
        id: layout
        anchors.fill: parent
		
		Row {
			id: buttons
			height: 20

			WGPushButton {
				id: addButton
				text: "Add Model"
				width: 100
				height: 20
				onClicked: {
					if (modelCount >= maxObjectCount) {
						return;
					}

					mergeProxy.addModel(models[modelCount]);
					++modelCount;
				}
			}

			WGPushButton {
				id: removeButton
				text: "Remove Model"
				width: 100
				height: 20
				onClicked: {
					if (modelCount == 0) {
						return;
					}

					--modelCount;
					mergeProxy.removeModel(models[modelCount]);
				}
			}
			
			MessageDialog {
				id: cannotCommitDialog
				title: "Error"
				text: "Objects are not checked in"
				modality: Qt.ApplicationModal
				visible: false
			}

			WGPushButton {
				id: commitButton
				text: "Commit"
				width: 100
				height: 20
				onClicked: {
					if(testP4Status.currentIndex != testGridPanel.noIcon){
						tableView.spreadsheetExtension.extension.commitData();
						testP4Status.currentIndex = testGridPanel.edit;	
					}
					else {
						cannotCommitDialog.open()
					}
				}
			}
						
			WGDropDownBox {
				id: testP4Status
				model: ListModel {}
				Component.onCompleted: {
					model.append({"label": "Test P4 No Icon"})
					model.append({"label": "Test P4 Add"})
					model.append({"label": "Test P4 Delete"})
					model.append({"label": "Test P4 Edit"})
					currentIndex = testGridPanel.edit;
				}
				onCurrentIndexChanged: {
					testGridPanel.p4status = currentIndex;
				}
			}
		}

		WGScrollView
		{
			id: scrollView
			Layout.fillWidth: true
			Layout.fillHeight: true

			property var spreadsheetExtension: tableView.spreadsheetExtension

			WGTableView
			{
				id: tableView
				model: objectsModel
				columnSpacing: 2
				clamp: false

				property var dirtyRows: []

				signal updateRows()

				columnDelegate: Item
				{
					id: columnDelegateItem
					width: columnWidth
					height: Math.max(childrenRect.height, defaultSpacing.minimumRowHeight)
					implicitWidth: delegateLoader.status == Loader.Ready ? delegateLoader.item.implicitWidth : 0

					// if item has been edited
					property bool isDirty: itemData != null ? itemData.dirty : false
					property bool rowIsDirty: false
					
					// update rows that have been edited
					onIsDirtyChanged: {
						if(isDirty) {
							tableView.dirtyRows.push(itemData.modelIndex.row)
						}
						else {
							tableView.dirtyRows.splice(tableView.dirtyRows.indexOf(itemData.modelIndex.row), 1);
						}
						
						tableView.updateRows()
					}

					Connections {
						target: tableView
						onUpdateRows: {
							rowIsDirty = tableView.dirtyRows.indexOf(itemData.modelIndex.row) != -1
						}
					}

					Rectangle
					{
						id: dirtyBar
						width: parent.width
						height: defaultSpacing.minimumRowHeight + defaultSpacing.doubleBorderSize
						color: palette.darkColor
						opacity: 0.25

						states: [
							State {
								name: "DIRTY_CHECKED_OUT"
								when: columnDelegateItem.rowIsDirty && testGridPanel.p4status !== testGridPanel.noIcon
								PropertyChanges { target: dirtyBar; color: "red" }
							},
							State {
								name: "DIRTY_NOT_CHECKED_OUT"
								when: columnDelegateItem.rowIsDirty && testGridPanel.p4status === testGridPanel.noIcon
								PropertyChanges { target: dirtyBar; color: "maroon" }
							},
							State {
								name: "NOT_DIRTY_EXCEL"
								when: testGridPanel.p4status === testGridPanel.noIcon
								PropertyChanges { target: dirtyBar; color: palette.darkColor }
							},							
							State {
								name: "ADD"
								when: testGridPanel.p4status === testGridPanel.add
								PropertyChanges { target: dirtyBar; color: "green" }
							},
							State {
								name: "EDIT"
								when: testGridPanel.p4status === testGridPanel.edit
								PropertyChanges { target: dirtyBar; color: palette.mainWindowColor }
							},
							State {
								name: "DELETE"
								when: testGridPanel.p4status === testGridPanel.markDelete
								PropertyChanges { target: dirtyBar; color: "purple" }
							}
						]
					}

					Loader
					{
						id: delegateLoader
						width: parent.width
						property bool readOnlyComponent: false

						property var itemData
						Component.onCompleted: {
							itemData = columnDelegateItem.parent.itemData;
							columnDelegateItem.parent.itemDataChanged.connect( updateSourceComponent );
							updateSourceComponent();
						}

						function unloadSourceComponent() {
							sourceComponent = undefined;
						}

						function updateSourceComponent() {
							if (itemData == null) {
								return;
							}
								
							if (itemData.component == null) {
								unloadSourceComponent()
								return;
							}

							if (sourceComponent == itemData.component) {
								unloadSourceComponent()
							}
							sourceComponent = itemData.component;
						}
					}
				}
				
				property var spreadsheetExtension: createExtension("SpreadsheetExtension")
				property var componentExtension: createExtension("ComponentExtension")
				extensions: [spreadsheetExtension, componentExtension]

				sortObject: QtObject {
					function lessThan(left, right) {
						return left.value < right.value;
					}
				}
			}
		}
	}
}
