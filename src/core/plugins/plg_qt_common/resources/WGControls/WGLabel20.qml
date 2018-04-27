import QtQuick 2.5
import QtQuick.Controls 1.4
import QtQuick.Layouts 1.3

/*!
 \ingroup wgcontrols
 \brief A non-editable single line of text with disabled styling
\code{.js}
WGLabel {
    text: "Example text"
}
\endcode
*/

Text {
    id: labelText
    objectName: "WGLabel"
    WGComponent { type: "WGLabel20" }

    color: enabled ? palette.textColor : palette.disabledTextColor

    renderType: globalSettings.wgNativeRendering ? Text.NativeRendering : Text.QtRendering
}
