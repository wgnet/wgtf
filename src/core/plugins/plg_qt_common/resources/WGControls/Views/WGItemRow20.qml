import QtQuick 2.5
import QtQuick.Controls 1.4
import QtQuick.Layouts 1.3

import WGControls 2.0

/** Shared component for the list of cells in a row of a WGItemView.
\ingroup wgcontrols */
Item {
    id: itemRow
    objectName: "WGItemRow_" + index
    WGComponent { type: "WGItemRow20" }

    width: row.width != 0 ? row.width : 1024
    height: row.height != 0 ? row.height : 1024
    clip: true

    /** This is a link to the containing WGItemView.*/
    property var view
    /** Specifies the level down a tree hierarchy.*/
    property var depth: 0
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

    Loader {
        id: rowBackground
        objectName: "RowBackground"
        anchors.fill: row
		asynchronous: itemRow.asynchronous
        sourceComponent: view.style.rowBackground

        property bool isSelected: view.selectionModel.isSelected(modelIndex)
        /** Checks if this is the current focused row.*/
        property bool isCurrent: view.selectionModel.currentIndex === modelIndex

        Connections {
            target: view.selectionModel
            onSelectionChanged: {
                rowBackground.isSelected = view.selectionModel.isSelected(modelIndex)
            }
        }
    }

    /** Row contents with traling column sizer.*/
    Row {
        id: row

        property var minX: rowHeader.width
        property var maxX: rowFooter.x - lastColumnHandleSpacer.width

        /** Row contents (cells) layout.*/
        Row {
            id: columnsLayout
            objectName: "Row"
            spacing: view.columnSpacing

            /** Indent gap, followed by column cells.*/
            Repeater {
                id: columns
                objectName: "Columns"

				model: WGSequenceList {
                    id: rowModel
                    model: columnModel
                    sequence: view.columnSequence
                }

                delegate: Item {
                    id: columnContainer
                    objectName: "ColumnContainer_" + index
                    width: view.columnWidths[index]
                    height: childrenRect.height
                    clip: true

                    /** Checks if this row is selected.*/
                    property bool isSelected: view.selectionModel.isSelected(modelIndex)
                    /** Checks if this is the current row.*/
                    property bool isCurrent: view.selectionModel.currentIndex === modelIndex

                    Connections {
                        target: view.selectionModel
                        onSelectionChanged: {
                            columnBackground.isSelected = view.selectionModel.isSelected(modelIndex)
                        }
                    }

                    /** Optional background behind a single cell, specified in the style component.*/
                    Loader {
                        id: columnBackground
                        objectName: "ColumnBackground"
                        anchors.fill: column
		                asynchronous: itemRow.asynchronous
                        sourceComponent: view.style.columnBackground

                        /** Checks if this row is selected.*/
                        property bool isSelected: columnContainer.isSelected
                        /** Checks if this is the current focused row.*/
                        property bool isCurrent: columnContainer.isCurrent
                    }

					DropArea {
						id: dropArea
						anchors.fill: columnBackground
						keys: itemRow.view.mimeTypes()

						onDropped: {
							var mimeData = {};
							for (var i = 0; i < drop.formats.length; ++i) {
								var format = drop.formats[i];
								var value = drop.getDataAsArrayBuffer(format);
								mimeData[format] = value;
							}

							if (view.dropMimeData(mimeData, drop.proposedAction, modelIndex)) {
								drop.acceptProposedAction();
							}
						}
					}

                    MouseArea {
                        id: columnMouseArea
                        objectName: "ColumnMouseArea"
                        anchors.fill: parent
                        acceptedButtons: Qt.RightButton | Qt.LeftButton;
						drag.target: columnMouseArea

						property var dragActive: drag.active
						onDragActiveChanged: {
							if (drag.active) {
								Drag.mimeData = view.mimeData(itemRow.view.selectionModel.selectedIndexes)
								Drag.active = true
							}
						}

						Drag.dragType: Drag.Automatic 
						Drag.proposedAction: Qt.MoveAction
						Drag.supportedActions: Qt.MoveAction | Qt.CopyAction

                        /** Signals that this item received a mouse press.
                        \param mouse The mouse data at the time.
                        \param modelIndex The index of the item.*/
                        onPressed: itemPressed(mouse, modelIndex)
                        /** Signals that this item was clicked.
                        \param mouse The mouse data at the time.
                        \param modelIndex The index of the item.*/
                        onClicked: itemClicked(mouse, modelIndex)
                        /** Signals that this item was double clicked.
                        \param mouse The mouse data at the time.
                        \param modelIndex The index of the item.*/
                        onDoubleClicked: itemDoubleClicked(mouse, modelIndex)

						Drag.onDragStarted: {
                            Drag.hotSpot.x = mouseX
							Drag.hotSpot.y = mouseY
                        }

                        Drag.onDragFinished: {
                            makeFakeMouseRelease();
                        }
                    }

                    /** Cell with styling, excluding indent gap in front of first cell.*/
                    Item {
                        id: column
                        objectName: "Column"
                        x: Math.max(row.minX - columnContainer.x, 0)
                        width: Math.max(Math.min(row.maxX - columnContainer.x, columnContainer.width) - x, 0)
                        height: childrenRect.height
                        clip: true

                        /** Specifies minimum bound for cell.*/
                        property var minX: columnHeader.width
                        /** Specifies maximum bound for cell.*/
                        property var maxX: columnFooter.x

                        /** Cell inside styling.*/
                        Item {
                            id: itemContainer
                            objectName: "ItemContainer"
                            x: column.minX
                            width: Math.max(column.maxX - x, 0)
                            height: childrenRect.height
                            clip: true

                            Loader {
                                id: itemDelegate
                                objectName: "ItemDelegate"
                                asynchronous: itemRow.asynchronous
                                sourceComponent: view.getColumnDelegate(index)

                                /** Exposes the value role to the custom cell component.
                                The role to use as the value role is specified in view.columnRoles.*/
                                property var itemValue: model[view.getColumnRole(index)]
                                /** Exposes all roles to the custom cell component.*/
                                property var itemData: model
                                /** Exposes depth in hierarchy to the custom cell component.*/
                                property var itemDepth: depth
                                /** Exposes selected check to the custom cell component.*/
                                property bool isSelected: columnContainer.isSelected
                                /** Exposes current index check to the custom cell component.*/
                                property bool isCurrent: columnContainer.isCurrent
								/** Exposes current row index to the custom cell component.*/
								property int itemRowIndex: itemRow.itemRowIndex
								/** Exposes current column index to the custom cell component.*/
								property int itemColumnIndex: index
								/** Exposes current modelIndex to the custom cell component.*/
								property var itemModelIndex: modelIndex

                                onLoaded: {
                                    item.parent = itemContainer
                                    itemContainer.updateColumnImplicitWidths();
                                }
                            }

                            Connections {
                                target: itemDelegate.item
                                onImplicitWidthChanged: {
                                    if (itemDelegate.status === Loader.Ready) {
                                        itemContainer.updateColumnImplicitWidths();
                                    }
                                }
                            }

                            /** Updates the implicit widths of the row,
                            when custom cell components' implict widths change.*/
                            function updateColumnImplicitWidths()
                            {
                                var implicitWidths = view.implicitColumnWidths;
                                var oldImplicitWidth = index in implicitWidths ? implicitWidths[index] : 0;
                                var indent = index === 0 ? row.minX : 0;
                                var newImplicitWidth = itemDelegate.item.implicitWidth + indent;
                                implicitWidths[index] = Math.max(oldImplicitWidth, newImplicitWidth);
                                view.implicitColumnWidths = implicitWidths;
                            }
                        }

                        /** Styling in front of the cell.*/
                        Loader {
                            id: columnHeader
                            objectName: "ColumnHeader"
                            asynchronous: itemRow.asynchronous
                            height: column.height
                            width: sourceComponent.width
                            sourceComponent: view.style.columnHeader

                            /** Exposes roles to custom style component.*/
                            property var itemData: model
                            /** Exposes depth in hierarchy to custom style component.*/
                            property var itemDepth: depth
                        }

                        /** Styling behind the cell.*/
                        Loader {
                            id: columnFooter
                            objectName: "ColumnFooter"
                            asynchronous: itemRow.asynchronous
                            x: column.width - width
                            height: column.height
                            width: sourceComponent.width
                            sourceComponent: view.style.columnFooter

                            /** Exposes roles to custom style component.*/
                            property var itemData: model
                            /** Exposes depth in hierarchy to custom style component.*/
                            property var itemDepth: depth
                        }
                    }
                }
            }
        }

        Item {
            id: lastColumnHandleSpacer
            anchors.top: parent.top
            anchors.bottom: parent.bottom
            width: view.clamp ? 0 : columnSpacing
        }
    }

    /** This loads the part in front of the row.*/
    Loader {
        id: rowHeader
        objectName: "RowHeader"
        asynchronous: itemRow.asynchronous
        height: row.height
        width: sourceComponent.width
        sourceComponent: view.style.rowHeader

        /** Exposes roles to custom style component.*/
        property var itemData: model
        /** Exposes depth in hierarchy to custom style component.*/
        property var itemDepth: depth
    }

    /** This loads the part behind the row.*/
    Loader {
        id: rowFooter
        objectName: "RowFooter"
		asynchronous: itemRow.asynchronous
        x: row.width - width
        height: row.height
        width: sourceComponent.width
        sourceComponent: view.style.rowFooter

        /** Exposes roles to custom style component.*/
        property var itemData: model
        /** Exposes depth in hierarchy to custom style component.*/
        property var itemDepth: depth
    }
}

