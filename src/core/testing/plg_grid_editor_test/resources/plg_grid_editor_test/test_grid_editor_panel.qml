import QtQuick 2.4
import QtQuick.Controls 1.2
import QtQuick.Layouts 1.0

import WGControls 2.0
import WGControls.Views 2.0
import WGControls.Styles 2.0

WGPanel {
    id: testGridPanel

    title: "Grid Editor Test"
    layoutHints: { 'test': 0.1 }
    color: palette.mainWindowColor

    property var currentTime
    property var modelCount: 0
    property var models: [objects.item(0), objects.item(1), objects.item(2), objects.item(3), objects.item(4)]

    WGTransposeProxy {
        id: gridModel
        sourceModel: WGMergeProxy {
            id: mergeProxy
        }
    }

    Row {
        id: buttons
        height: 20

        WGPushButton {
            id: addButton
            text: "Add Model"
            width: 100
            height: 20
            onClicked: {
                if (modelCount > 4) {
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

        WGPushButton {
            id: commitButton
            text: "Commit"
            width: 100
            height: 20
            onClicked: {
                gridView.spreadsheetExtension.commitData();
            }
        }
    }

    WGTableView {
        id: gridView
        anchors.top: buttons.bottom
        anchors.bottom: parent.bottom
        anchors.left: parent.left
        anchors.right: parent.right

        columnSpacing: 1

        // Todo: Delete when model works properly \/
        property var dirtyRows: []
        property var checkedOutRows: [1, 3]
        property var dirtyExcelRows: [2, 3, 4]

        signal updateRows()

        // fired if a control in (row) expands (rowExpanding) or contracts (!rowExpanding)
        signal rowExpanded(var row, var rowExpanding)

        // Todo: Delete when model works properly /\

        columnDelegate: Item {
            id: columnDelegate
            width: parent.width
            height: Math.max(childrenRect.height, defaultSpacing.minimumRowHeight)

            property bool isDirty: itemData.dirty

            // Todo: Link to proper data value when available
            property bool excelIsDirty: (itemData.modelIndex.row == 2 && itemData.index == 2) ||
                                        (itemData.modelIndex.row == 3 && itemData.index == 2) ||
                                        (itemData.modelIndex.row == 4 && itemData.index == 2)

            // Todo: Link to proper data value when available
            property bool rowIsDirty: false

            // Todo: Link to proper data value when available
            property bool rowIsCheckedOut: gridView.checkedOutRows.indexOf(itemData.modelIndex.row) != -1

            // Todo: Link to proper data value when available
            property bool excelRowIsDirty: gridView.dirtyExcelRows.indexOf(itemData.modelIndex.row) != -1

            // Todo: Delete when model works properly \/
            onIsDirtyChanged: {
                gridView.dirtyRows.push(itemData.modelIndex.row)
                gridView.updateRows()
            }

            Connections {
                target: gridView
                onUpdateRows: {
                    rowIsDirty = gridView.dirtyRows.indexOf(itemData.modelIndex.row) != -1
                }
            }

            Item {
                width: parent.width
                height: controlLoader.height + defaultSpacing.doubleBorderSize

                /*  Annoying that a lot of this is here and not in the style. This is partly because
                    of temporary debug code, however it's Frustrating that the style is only BEHIND
                    the cell contents. MouseAreas need to be in front of the cell contents with more control. */

                Rectangle {
                    id: dirtyBar
                    anchors.fill: parent
                    anchors.margins: defaultSpacing.standardBorderSize
                    visible: columnDelegate.rowIsDirty || columnDelegate.excelRowIsDirty
                    color: "#3300FF00"

                    states: [
                        State {
                            name: "DIRTY_CHECKED_OUT"
                            when: columnDelegate.rowIsDirty && columnDelegate.rowIsCheckedOut && !columnDelegate.excelRowIsDirty
                            PropertyChanges { target: dirtyBar; color: "#3300FF00"}
                        },
                        State {
                            name: "DIRTY_NOT_CHECKED_OUT"
                            when: columnDelegate.rowIsDirty && !columnDelegate.rowIsCheckedOut && !columnDelegate.excelRowIsDirty
                            PropertyChanges { target: dirtyBar; color: "#33FF0000"}
                        },
                        State {
                            name: "NOT_DIRTY_EXCEL"
                            when: !columnDelegate.rowIsDirty && columnDelegate.excelRowIsDirty
                            PropertyChanges { target: dirtyBar; color: "#330071FF"}
                        },
                        State {
                            name: "DIRTY_EXCEL_CHECKED_OUT"
                            when: columnDelegate.rowIsDirty && columnDelegate.excelRowIsDirty && columnDelegate.rowIsCheckedOut
                            PropertyChanges { target: dirtyBar; color: "#3300FFFF"}
                        },
                        State {
                            name: "DIRTY_EXCEL_NOT_CHECKED_OUT"
                            when: columnDelegate.rowIsDirty && columnDelegate.excelRowIsDirty && !columnDelegate.rowIsCheckedOut
                            PropertyChanges { target: dirtyBar; color: "#339800FF"}
                        }
                    ]
                }

                Rectangle {
                    id: cellDirtyBorder
                    anchors.fill: parent
                    visible: itemData != null && (columnDelegate.isDirty || columnDelegate.excelIsDirty)
                    color: "transparent"
                    border.color: "#3300BF41"
                    border.width: defaultSpacing.standardBorderSize
                    states: [
                        State {
                            name: "DIRTY_CHECKED_OUT"
                            when: columnDelegate.isDirty && columnDelegate.rowIsCheckedOut && !columnDelegate.excelIsDirty
                            PropertyChanges { target: cellDirtyBorder; border.color: "#8800FF00"}
                        },
                        State {
                            name: "DIRTY_NOT_CHECKED_OUT"
                            when: columnDelegate.isDirty && !columnDelegate.rowIsCheckedOut && !columnDelegate.excelIsDirty
                            PropertyChanges { target: cellDirtyBorder; border.color: "#88FF0000"}
                        },
                        State {
                            name: "NOT_DIRTY_EXCEL"
                            when: !columnDelegate.isDirty && columnDelegate.excelIsDirty
                            PropertyChanges { target: cellDirtyBorder; border.color: "#880071FF"}
                        },
                        State {
                            name: "DIRTY_EXCEL_CHECKED_OUT"
                            when: columnDelegate.isDirty && columnDelegate.rowIsCheckedOut && columnDelegate.excelIsDirty
                            PropertyChanges { target: cellDirtyBorder; border.color: "#8800FFFF"}
                        },
                        State {
                            name: "DIRTY_EXCEL_NOT_CHECKED_OUT"
                            when: columnDelegate.isDirty && !columnDelegate.rowIsCheckedOut && columnDelegate.excelIsDirty
                            PropertyChanges { target: cellDirtyBorder; border.color: "#889800FF"}
                        }
                    ]
                }

                Rectangle {
                    id: hoverArea
                    anchors.fill: parent
                    color: palette.highlightShade
                    visible: cellSelectionArea.ctrlPressed
                }

                // Todo: Delete when model works properly /\
                Loader {
                    id: controlLoader
                    width: parent.width - defaultSpacing.doubleBorderSize
                    x: defaultSpacing.standardBorderSize
                    y: defaultSpacing.standardBorderSize
                    sourceComponent: itemData.component

                    onLoaded: {
                        columnDelegate.implicitWidth = Qt.binding( function() {return item.implicitWidth + controlLoader.x} );
                    }

                    // if rowExpanded, compare row and expand if possible.
                    Connections {
                        target: gridView
                        onRowExpanded: {
                            if(controlLoader.item != null)
                            {
                                if (typeof controlLoader.item.expanded !== 'undefined')
                                {
                                    if (row == itemData.modelIndex.row)
                                    {
                                        controlLoader.item.expand(rowExpanding, false)
                                    }
                                }
                            }
                        }
                    }

                    // send rowExpanded if item expands/contracts
                    Connections {
                        target: controlLoader.item
                        ignoreUnknownSignals: true
                        onComponentExpanding: {
                            gridView.rowExpanded(itemData.modelIndex.row, expanding)
                        }
                    }
                }


                MouseArea {
                    id: cellSelectionArea
                    objectName: "cellSelectionArea"
                    anchors.fill: parent

                    hoverEnabled: true

                    acceptedButtons: Qt.LeftButton | Qt.RightButton

                    property bool ctrlPressed: false

                    cursorShape: ctrlPressed ? Qt.PointingHandCursor : Qt.ArrowCursor

                    onPressed: {
                        var modifierDown = ((mouse.modifiers & Qt.ControlModifier) || (mouse.modifiers & Qt.ShiftModifier))
                        // Select the cell if it doesn't have focus or a modifier key is down
                        if(!controlLoader.activeFocus || modifierDown)
                        {
                            gridView.view.select(mouse, itemData.modelIndex)
                            controlLoader.forceActiveFocus()
                        }

                        // TODO: If the cell has focus and is no longer selected focus the next cell in the selection
                        if(controlLoader.activeFocus)
                        {
                        }
                        // TODO: If the cell was added to the selection give focus to the new cell
                        else
                        {
                        }
                        mouse.accepted = modifierDown
                    }

                    // Not sure about the performance hit of this
                    onPositionChanged: {
                        if ((mouse.button == Qt.NoButton) && (mouse.modifiers & Qt.ControlModifier))
                        {
                            ctrlPressed = true
                        }
                        else
                        {
                            ctrlPressed = false
                        }
                    }

                    onExited: {
                        ctrlPressed = false
                    }
                }
            }
        }

        headerDelegate: Text {
            text: headerData != null ? headerData.display : ""
            color: palette.textColor
        }

        sortObject: QtObject {
            function lessThan(left, right) {
                return left["value"] < right["value"];
            }
        }

        SpreadsheetExtension {
            id: spreadsheetExtension
        }

        ComponentExtension {
            id: componentExtension
        }

        extensions: [spreadsheetExtension, componentExtension]

        style: WGItemViewStyle {
            columnBackground: Item {
                Rectangle {
                    id: backgroundArea
                    anchors.fill: parent
                    color: palette.highlightShade
                    opacity: isSelected ? 1 : 0.5
                    visible: isSelected
                }

                Rectangle {
                    id: keyboardFocusArea
                    anchors.fill: parent
                    color: "transparent"
                    border.width: defaultSpacing.standardBorderSize
                    border.color: palette.lightestShade
                    radius: defaultSpacing.halfRadius
                    visible: isCurrent
                }
            }
        }


        model: gridModel
    }
}
