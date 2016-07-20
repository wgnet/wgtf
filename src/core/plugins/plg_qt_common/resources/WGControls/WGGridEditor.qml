import QtQuick 2.4
import QtQuick.Controls 1.2
import QtQml.Models 2.2
import QtQuick.Layouts 1.1

import WGControls 2.0

/*!
 \brief An editable version of WGTableView with a custom delegate that changes depending on the cell contents.

 Cell data can be edited in the cell, or using the edit box.

 Currently supported cells include:
 "TEXT":    Uneditable text
 "STRING":  Editable text
 "NUMBER":  Editable numbers
 "COMBO":   Editable combo box

 TODO: Model needs to support display and edit values better

Example:
\code{.js}

WGGridEditor {
    id: gridEditor
    anchors.fill: parent

    gridModel: sourceModel
}

\endcode
*/

Item {
    id: gridViewBase

    /*! The Display Value of the current cell, i.e. what appears in the table when it is not being edited.*/
    property var currentDisplayValue: currentCell  ? activeCell.displayValue : ""

    /*! The Edit Value of the current cell, i.e. what appears in the table when it is being edited.*/
    property var currentEditValue: currentCell  ? activeCell.editValue : ""

    /*! This property determines whether the current cell is being edited or not.*/
    property bool cellEditing: currentCell  ? activeCell.editing : false

    /*! The model for the gridView.*/
    property var gridModel

    /*! The active cell delegate.*/
    property QtObject activeCell: null

    /*! This property determines if there is an activeCell object.*/
    property bool currentCell: typeof activeCell == "undefined" || activeCell == null ? false : true

    Keys.forwardTo: gridEditor

    RowLayout {
        id: editBar

        anchors {top: parent.top; left: parent.left; right: parent.right;}

        height: defaultSpacing.minimumRowHeight

        WGTextBox {
            Layout.maximumWidth: parent.width / 2
            Layout.fillWidth: true
            Layout.fillHeight: true

            text: {
                if (currentCell)
                {
                    if (activeCell.hiddenEditValue)
                    {
                        return activeCell.editValue
                    }
                    else
                    {
                        return activeCell.displayValue
                    }
                }
                else
                {
                    return ""
                }
            }
            readOnly: currentCell  ? !activeCell.editable : false

            onEditAccepted: {
                if (activeCell.hiddenEditValue)
                {
                    setValueHelper(activeCell, "editValue",text)
                }
                else
                {
                    setValueHelper(activeCell, "displayValue",text)
                }
                activeCell.editing = false
            }
        }

        Item {
            id: gridToolbar
            Layout.fillWidth: true
            Layout.fillHeight: true

            // TODO: Add GridEditor actions etc.
        }
    }

    Item {
        id: gridSeparator
        anchors {top: editBar.bottom; left: parent.left; right: parent.right;}
        height: defaultSpacing.separatorWidth + defaultSpacing.doubleBorderSize
        WGSeparator {
            anchors {verticalCenter: parent.verticalCenter; left: parent.left; right: parent.right;}
        }
    }

    WGScrollView {
        anchors {top: gridSeparator.bottom; left: parent.left; right: parent.right; bottom: parent.bottom;}

        WGTableView {
            id: gridEditor
            columnSpacing: 1
            headerDelegate: myHeaderDelegate
            footerDelegate: myFooterDelegate
            roles: ["value", "headerText", "footerText"]
            model: gridModel
            clamp: false
            currentIndex: 0

            columnDelegate: WGGridEditorDelegate {
                id: cellDelegate
                displayValue: itemData !== null ? itemData.display : ""
                state: "STRING"
                active: isCurrent
                focus: isCurrent

                implicitWidth: 100

                onActiveChanged: {
                    if (active)
                    {
                        activeCell = cellDelegate
                    }
                }

                onDisplayValueChanged: {
					if (itemData === null) {
						return;
					}
                    itemData.display = displayValue
                }
            }
        }
    }

    property alias myHeaderDelegate: myHeaderDelegate
    property alias myFooterDelegate: myFooterDelegate

    Component {
        id: myHeaderDelegate

        Text {
            id: textBoxHeader
            color: palette.textColor
            text: valid ? headerData.headerText : ""
            height: defaultSpacing.minimumRowHeight + defaultSpacing.doubleBorderSize

            property bool valid: headerData !== null &&
                typeof headerData !== "undefined" &&
                typeof headerData.headerText !== "undefined"
        }
    }

    Component {
        id: myFooterDelegate

        Text {
            id: textBoxFooter
            color: palette.textColor
            text: valid ? headerData.footerText : ""
            height: defaultSpacing.minimumRowHeight + defaultSpacing.doubleBorderSize

            property bool valid: headerData !== null &&
                typeof headerData !== "undefined" &&
                typeof headerData.footerText !== "undefined"
        }
    }
}

