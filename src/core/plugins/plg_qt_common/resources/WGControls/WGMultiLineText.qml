import QtQuick 2.3
import QtQuick.Layouts 1.1

/*!
 \brief WGlabel that wraps multiple lines.

Example:
\code{.js}
WGMultiLineText {
    Layout.fillWidth: true
    text: "This is some mulitline text. It goes on and on."
}
\endcode
*/

WGLabel {
	id: multiLineText
    objectName: "WGMultiLineText"
    Layout.minimumHeight: lineCount * defaultSpacing.minimumRowHeight
    formLabel: false
    wrapMode: Text.WordWrap
	
	/*! Deprecated */
	property alias formLabel_: multiLineText.formLabel
}
