import QtQuick 2.5
import QtQuick.Controls 1.4
import QtQuick.Layouts 1.3
import QtWebEngine 1.2
import WGControls 1.0

/*!
  \ingroup wgcontrols
 \brief Output window for logging messages from ILoggingSystem

Example:
\code{.js}
WGLoggingPanel {
	text: loggingModel.text
}
\endcode
*/
 
WGPanel {
	id: loggingPanel
    WGComponent { type: "WGLoggingPanel" }
    
	title: "Log"
    color: palette.mainWindowColor

    property alias text: logTextArea.text
    property color scrollBarColor: palette.highlightColor
    property color scrollBarTrackColor: palette.darkerShade

    onColorChanged: logTextArea.updatePage()
	
    WebEngineView {
		id: logTextArea
		anchors.fill: parent
        url: "about:blank"

        property string text: ""

        onTextChanged: updatePage()

        function rgbaString(color)
        {
            return "rgba(" +
                    color.r * 255 + "," +
                    color.g * 255 + "," +
                    color.b * 255 + "," +
                    color.a + ");";
        }

        function updatePage()
        {
            var safeText = text.replace("\"", "'")
            var style = "\
<style>\
    body { background-color: " + rgbaString(color) + " }\
    body::-webkit-scrollbar {\
        background-color: " + rgbaString(scrollBarTrackColor) + "\
        width: .8em;\
        height: .8em;\
    }\
    body::-webkit-scrollbar-track {\
        -webkit-box-shadow: inset 0 0 6px rgba(0,0,0,0.3);\
    }\
    body::-webkit-scrollbar-thumb {\
        background-color: " + rgbaString(scrollBarColor) + "\
        outline: 1px solid " + rgbaString(color) + "\
    }\
    body::-webkit-scrollbar-corner { background-color: " + rgbaString(color) + " }\
</style>";
            runJavaScript("document.body.innerHTML = \"" + style + safeText + "\"")
        }
    }

    WGContextArea{
        contextMenu: WGMenu {
            MenuItem {
                action: Action {
                    text: qsTr("Copy")
                    shortcut: "Ctrl+C"
                    onTriggered: logTextArea.triggerWebAction(WebEngineView.Copy)
                }
            }
            MenuItem {
                action: Action {
                    text: qsTr("Select All")
                    shortcut: "Ctrl+A"
                    onTriggered: logTextArea.triggerWebAction(WebEngineView.SelectAll)
                }
            }
       }
	}
}