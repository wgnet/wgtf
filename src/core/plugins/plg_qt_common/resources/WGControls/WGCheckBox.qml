import QtQuick 2.3
import QtQuick.Controls 1.2
import WGControls 1.0

/*!
 \brief A WG styled control that allows the user to make a binary choice.

Example:
\code{.js}
WGCheckBox {
    label: "Option"
    text: "Save Automatically?"
}
\endcode
*/

WGCheckBase {
    id: checkBox
    objectName: "WGCheckBox"

    /*! This property is used to define the buttons label when used in a WGFormLayout
        The default value is an empty string
    */
    property string label: ""

    /*! This property determines the checked state of the control
        The default value is false
    */
    property bool checked: false

    checkedState: multipleValues ? Qt.PartiallyChecked : checked ? Qt.Checked : Qt.Unchecked

    activeFocusOnTab: enabled

    implicitHeight: defaultSpacing.minimumRowHeight

    onCheckedStateChanged: {
        if (checkedState === Qt.PartiallyChecked) {
            partiallyCheckedEnabled = true;
            setValueHelper( checkBox, "checked", false);
        } else {
            setValueHelper( checkBox, "checked", checkedState === Qt.Checked);
        }
    }

    onCheckedChanged: {
        if (!partiallyCheckedEnabled)
        {
            checkedState = checked ? Qt.Checked : Qt.Unchecked
        }
        else if (checked)
        {
            checkedState = Qt.Checked
        }
    }

    // support copy&paste
    WGCopyable {
        id: copyableControl

        WGCopyController {
            id: copyableObject

            onDataCopied : {
                setValue( checkBox.checked )
            }

            onDataPasted : {
                setValueHelper( checkBox, "checked", data );
            }
        }

        onSelectedChanged : {
            if (selected)
            {
                selectControl( copyableObject )
            }
            else
            {
                deselectControl( copyableObject )
            }
        }
    }

    //Fix to stop half pixel offsets making the checkmarks look off centre
    Component.onCompleted:{
        implicitWidth = (Math.round(implicitWidth))
    }

    style: WGCheckStyle {
    }

    /*! Deprecated */
    property alias label_: checkBox.label
}
