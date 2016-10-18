import QtQuick 2.5
import QtQuick.Layouts 1.3

/*!
  \ingroup wgcontrols
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
    WGComponent { type: "WGMultiLineText" }
    
    Layout.minimumHeight: lineCount * defaultSpacing.minimumRowHeight
    formLabel: false
    wrapMode: Text.WordWrap
	
	/*! Deprecated */
	property alias formLabel_: multiLineText.formLabel
}
