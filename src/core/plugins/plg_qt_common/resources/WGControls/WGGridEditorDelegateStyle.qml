import QtQuick 2.6
import QtQuick.Controls 1.3

import WGControls 2.0

/*!
 \brief A style file for WGTableViewEditDelegate

 Contains several different controls depending on the state of the parent delegate.



Example:
\code{.js}

WGTableView {
    id: tableView1
    columnSpacing: 1
    roles: ["value"]
    model: sourceModel
    clamp: false
    currentIndex: 0

    columnDelegate: WGTableViewEditDelegate {
        displayValue: itemData.display
        format: "STRING"
        active: isCurrent
        implicitWidth: 100

        style: WGTableViewEditDelegateStyle {
            display: Text {
                id: displayText
                anchors.fill: parent
                text: control.displayValue
                color: "red"
            }
        }

        onDisplayValueChanged:{
            itemData.display = displayValue
        }
    }
}

\endcode
*/

Item {
    id: baseStyle
    objectName: "TableViewEditDelegateStyle"

    /* This property determines the parent cell delegate */
    property QtObject control: parent

    /* This is a non-editable text component */
    property Component display: WGLabel {
        id: displayText
        anchors.fill: parent
        text: control.displayValue
        horizontalAlignment: baseStyle.state == "NUMBER" ? Text.AlignRight : Text.AlignLeft
        anchors.topMargin: defaultSpacing.doubleBorderSize
        anchors.leftMargin: defaultSpacing.rowSpacing
        anchors.rightMargin: defaultSpacing.rowSpacing
    }

    /* This is an editable textbox component eg WGTextBox */
    property Component textEdit: WGTextBox {
        id: textEditBox
        anchors.fill: parent
        text: control.hiddenEditValue ? control.editValue : control.displayValue

        Connections {
            target: control.state == "STRING" ? control : null
            onEditingChanged: {
                if (editing)
                {
                    forceActiveFocus()
                    selectAll()
                }
                else
                {
                    focus = false
                    deselect()
                }
            }
        }

        onEditAccepted: {
            cellEditingFinished(text, text)
        }
    }

    /* This is an editable number component eg WGNumberBox */
    property Component numberEdit: WGNumberBox {
        id: numberEditBox
        anchors.fill: parent
        value: control.hiddenEditValue ? control.editValue : control.displayValue

        Connections {
            target: control.state == "NUMBER" ? control : null
            onEditingChanged: {
                if (editing)
                {
                    forceActiveFocus()
                }
                else
                {
                    focus = false
                }
            }
        }

        onEditingFinished: {
            cellEditingFinished(value, value)
        }
    }

    /* This is an editable drop down menu eg WGDropDownBox */
    property Component comboEdit: WGDropDownBox {
        id: comboEditBox
        model: control.comboModel

        textRole: control.comboTextRole
        imageRole: control.comboImageRole

        currentIndex: control.editValue

        onActivated: {
            cellEditingFinished(index, currentText)
        }
    }

    signal cellEditingFinished(var newEditValue, var newDisplayValue)
}
