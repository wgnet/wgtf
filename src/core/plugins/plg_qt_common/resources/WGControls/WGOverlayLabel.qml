import QtQuick 2.3
import QtQuick.Controls 1.2
import QtQuick.Layouts 1.1

/*!
 \brief A non-editable single line of text intended to be used in a 3D viewport
 that can align to a panel wide width in defaultSpacing
 Will appear in the left column if placed in a WGFormLayout && formLabel_ == true

\code{.js}
WGLabel {
    text: "Example text"
}
\endcode
*/

WGLabel {
    id: labelText
    objectName: "WGOverlayLabel"

    color: enabled ? "white" : palette.disabledTextColor
    style: Text.Raised

    styleColor: enabled ? palette.overlayTextColor : palette.overlayDarkShade
}

