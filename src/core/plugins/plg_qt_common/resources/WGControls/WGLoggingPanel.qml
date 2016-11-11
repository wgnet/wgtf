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
    property color textColor: palette.textColor
    property color scrollBarColor: palette.highlightColor
    property color scrollBarTrackColor: palette.mainWindowColor
    signal clearText;

    onColorChanged: _.updatePage()

    Action {
        id: selectAll
        text: qsTr("Select All")
        shortcut: "Ctrl+A"
        onTriggered: logTextArea.triggerWebAction(WebEngineView.SelectAll)
    }

    Action {
        id: copy
        text: qsTr("Copy")
        shortcut: "Ctrl+C"
        iconSource: "qrc:///WGControls/icons/copy_small_16x16.png"
        onTriggered: logTextArea.triggerWebAction(WebEngineView.Copy)
    }

    Action {
        id: clear
        text: qsTr("Clear")
        tooltip: qsTr("Clear the log")
        iconSource: "qrc:///WGControls/icons/delete_sml_16x16.png"
        onTriggered: clearText()
    }

    Action {
        id: findPrev
        tooltip: qsTr("Find Previous")
        enabled: findText.searchTextFound
        shortcut: "Shift+F3"
        iconSource: "qrc:///WGControls/icons/back_16x16.png"
        onTriggered: logTextArea.findText(findText.text, WebEngineView.FindBackward)
    }

    Action {
        id: findNext
        tooltip: qsTr("Find Next")
        enabled: findText.searchTextFound
        shortcut: "F3"
        iconSource: "qrc:///WGControls/icons/fwd_16x16.png"
        onTriggered: logTextArea.findText(findText.text)
    }

    ColumnLayout
    {
        id: layout
        anchors.fill: parent

        RowLayout
        {
            id: toolbar
            WGPushButton { action: selectAll }
            WGPushButton { action: copy }
            WGPushButton { action: clear }
            WGTextBox{
                id: findText
                implicitWidth: 200
                placeholderText: qsTr("Search")
                onTextChanged: {
                    logTextArea.findText(text, 0,function(found){
                        searchTextFound = found
                    })
                }
                Keys.onPressed: {
                    if (event.key == Qt.Key_Enter || event.key == Qt.Key_Return)
                    {
                        logTextArea.findText(text)
                    }
                    else if(event.key == Qt.Key_F3)
                    {
                        var dir = (event.modifiers & Qt.ShiftModifier) ? WebEngineView.FindBackward : 0
                        logTextArea.findText(text, dir)
                    }
                }

                property bool searchTextFound: false
            }
            WGPushButton { action: findPrev }
            WGPushButton { action: findNext }
        }

        Item
        {
            id: logArea
            Layout.fillWidth: true
            Layout.fillHeight: true

            WebEngineView
            {
                id: logTextArea
                anchors.fill: parent
                url: "about:blank"
                backgroundColor: palette.mainWindowColor

                property string text: ""

                onTextChanged: requestLoadTimer.start()

                // Rate limit how often the page is updated using a timer to update the log when text has been updated
                // Currently we update the entire page but this causes flickering with the scroll positions
                // Using runJavaScript to update innerHTML worked nicely with scroll positions but caused QtWebEngineProcess to leak
                // To avoid the flickering the logging system will need to be reworked to allow access to log entries and this could
                // then update the DOM by adding divs, this would also allow removing the C++ code that wraps entries in HTML
                Timer
                {
                    id: requestLoadTimer
                    interval: 300
                    onTriggered: _.updatePage()
                }

                QtObject
                {
                    id: _
                    property bool autoScroll: true
                    property int scrollX: 0
                    property int scrollY: 0

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
                        if(!logTextArea.loading)
                        {
                            // Use callbacks to ensure we get the values before attempting to load the new page
                            // Get the current scroll position from the current page
                            logTextArea.runJavaScript("window.scrollX", function(x){
                                scrollX = x
                                logTextArea.runJavaScript("window.scrollY", function(y){
                                    scrollY = y
                                    // If the page is scrolled all the way to the bottom enable autoscroll
                                    logTextArea.runJavaScript("document.body ? window.scrollY >= document.body.scrollHeight - document.body.clientHeight : false", function(scroll){
                                        autoScroll = scroll
                                        loadPage()
                                    });
                                });
                            });
                        }
                        else
                        {
                            requestLoadTimer.start()
                        }
                    }

                    function loadPage()
                    {
                        var safeText = text.replace("\"", "'")
                        var head = "<head>\
                        <style>\
                            body {\
                                background-color: " + rgbaString(palette.textBoxColor) + "\
                                font-family:Lucida Console;\
                                font-size:0.8em;\
                            }\
                            body::-webkit-scrollbar {\
                                background-color: " + rgbaString(loggingPanel.scrollBarTrackColor) + "\
                                width: .8em;\
                                height: .8em;\
                            }\
                            body::-webkit-scrollbar-track {\
                                -webkit-box-shadow: inset 0 0 6px rgba(0,0,0,0.3);\
                            }\
                            body::-webkit-scrollbar-thumb {\
                                background-color: " + rgbaString(loggingPanel.scrollBarColor) + "\
                                outline: 1px solid " + rgbaString(loggingPanel.color) + "\
                            }\
                            body::-webkit-scrollbar-corner { background-color: " + rgbaString(loggingPanel.color) + " }\
                            .error{\
                                color: red\
                            }\
                            .warning{\
                                color: gold\
                            }\
                            .alert{\
                                color: green\
                            }\
                            .info{\
                                color: " + rgbaString(loggingPanel.textColor) + "\
                            }\
                        </style>\
                        <script language='javascript'>\
                            document.onreadystatechange = function ()\
                            {\
                                if(document.readyState === 'interactive' || document.readyState === 'complete')\
                                {\
                                    window.scrollTo(" + scrollX + "," + (autoScroll ? "document.body.scrollHeight" : scrollY) +")\
                                }\
                            }\
                        </script>\
                        </head>";

                        var html = "<html>" + head + "<body>" + safeText + "</body></html>";
                        logTextArea.loadHtml(html);
                    }
                }
            }
            WGContextArea{
                contextMenu: WGMenu{
                    MenuItem { action: copy }
                    MenuItem { action: selectAll }
                    MenuItem { action: clear }
                }
            }
        }
    }
}
