import QtQuick 2.3
import QtQuick.Layouts 1.1

//TODO: Can we fix the problems with WGSubPanels? Or prevent the use of one in the other?

/*!
 \brief Moves the children into a second column and creates labels for them based off the label_ property in the first column
  WARNING! Will do strange things to WGSubPanels

Example:
\code{.js}

WGSubPanel {
    text: "Form Layout"
    childObject :
        WGFormLayout {
            id: topForm
            localForm: true

            WGPushButton {
                text: "Button"
                label: "Buttons Label: "
            }
        }
    }
}

\endcode
*/

WGColumnLayout {
    objectName: "WGFormLayout"
    formLayout: true
}
