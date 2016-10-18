import QtQuick 2.5
import QtQuick.Layouts 1.3

import WGControls.Private 1.0
import WGControls.Layouts 1.0

/*!
 \ingroup wgcontrols
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

 \todo Can we fix the problems with WGSubPanels? Or prevent the use of one in the other?
*/

WGColumnLayout {
    objectName: "WGFormLayout"
    WGComponent { type: "WGFormLayout" }
    formLayout: true
}
