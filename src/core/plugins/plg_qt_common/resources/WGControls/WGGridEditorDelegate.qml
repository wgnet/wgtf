import QtQuick 2.6
import QtQuick.Controls 1.3

import WGControls 2.0

/*!
 \brief An editable delegate for WGTableView used in WGGridEditor. Can be edited by double clicking the cell or pressing Enter

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

        onDisplayValueChanged:{
            itemData.display = displayValue
        }
    }
}

\endcode
*/

Item {
    id: base
    objectName: "TableViewEditDelegate"

    /*! This property determines the visible value when it is not being edited.*/
    property var displayValue

    /*! This property determines the value that is edited.

        If hiddenEditvalue == true, this may be different to the displayValue,
        otherwise it will be the same as the displayValue */
    property var editValue

    /*! This property determines the model for a combobox if state == "COMBO"*/
    property var comboModel

    /*! This property determines the textRole for a combobox if state == "COMBO"*/
    property string comboTextRole: "label"

    /*! This property determines the imageRole for a combobox if state == "COMBO"*/
    property string comboImageRole: ""

    /*! This property indicates if the cell is able to be edited or not */
    property bool editable: true

    /*! This property indicates if the cell is being edited or not */
    property bool editing: false

    /*! This property indicates if the cell is the active, editable cell */
    property bool active: false

    /*! This property indicates if the cell has a hidden editable value that determines the display value

        For example if the cell contains a formula that displays a result */
    property bool hiddenEditValue: false

    state: "TEXT"

    property var style: WGGridEditorDelegateStyle { control: base }

    implicitHeight: defaultSpacing.minimumRowHeight

    Connections {
        target: style
        onCellEditingFinished: {
            editValue = newEditValue
            displayValue = newDisplayValue

            editing = false
        }
    }

    Keys.onReturnPressed: {
        if (active)
        {
            if (!hiddenEditValue)
            {
                editValue = displayValue
            }
            editing = true
        }
    }

    onActiveChanged: {
        editing = false
    }

    Loader {
        id: displayControl
        anchors.fill: parent
        sourceComponent: style.display
        visible: !editing || !active
        z: 0
    }

    MouseArea {
        anchors.fill: parent
        z: 1
        propagateComposedEvents: true

        hoverEnabled: editable && !editing

        acceptedButtons: active ? Qt.LeftButton : Qt.NoButton

        cursorShape: editing ? Qt.IBeamCursor : Qt.PointingHandCursor

        onDoubleClicked: {
            if (!hiddenEditValue)
            {
                editValue = displayValue
            }
            editing = true
        }
    }

    Loader {
        id: editControl
        anchors.fill: parent
        sourceComponent: style.display
        visible: editing && active
        z: 2
    }

    states: [
        State {
            name: "TEXT"
            PropertyChanges {target: base; editable: false}
        },
        State {
            name: "STRING"
            PropertyChanges {target: base; editable: true}
            PropertyChanges {target: editControl; sourceComponent: style.textEdit}
        },
        State {
            name: "NUMBER"
            PropertyChanges {target: base; editable: true}
            PropertyChanges {target: editControl; sourceComponent: style.numberEdit}
        },
        State {
            name: "COMBO"
            PropertyChanges {target: base; editable: true}
            PropertyChanges {target: editControl; sourceComponent: style.comboEdit}
        }
    ]
}
