import QtQuick 2.5

/*!
    \ingroup wgcontrols
    \brief The top level panel container

Example:
\code{.js}
WGPanel {
    id: historyPanel

    autoReload: true
    title: "History"
    layoutHints: { 'history': 1.0 }
    ...
}
\endcode
*/

FocusScope {
    id: basePanel
    objectName: "WGPanel"
    WGComponent { type: "WGPanel" }

    implicitHeight: defaultSpacing.minimumRowHeight
    implicitWidth: defaultSpacing.minimumRowHeight

    property alias color: frame.color

    /*!
        Determines if the panel should be automatically reloaded when edited and saved
        The default value is \c true
    */
    property bool autoReload: true

    /*!
        This property sets the title of this panel view
        The default value is \c "Panel"
    */
    property string title: {
        if (typeof self != "undefined" && self.title != null) {
            return self.title;
        }
        return "Panel";
    }

    /*!
        This property sets the window ID of this panel view
        The default value is \c ""
    */
    property string windowId: {
        if (typeof(self) !== "undefined" && self !== null && self.windowId != null) {
            return self.windowId;
        }
        return "";
    }

    /*!
        This property sets the defaut layout hints
        The default value is \c { 'test' : 0.1 }
    */
    property var layoutHints: {
        if (typeof(self) !== "undefined" && self !== null && self.layoutHints != null) {
            var layoutHints = {};
            var map = self.layoutHints;
            var rowCount = map.rowCount(null);
            for (var row = 0; row < rowCount; ++row) {
                var index = map.index(row, 0);
                var item = map.indexToItem(index);
                layoutHints[item.key] = item.value;
            }
            return layoutHints;
        }
        return { 'test' : 0.1 };
    }

    Rectangle {
        id: frame
        anchors.fill: parent

        color: typeof palette != 'undefined' ? palette.mainWindowColor : "transparent"

        MouseArea {
            anchors.fill: parent
            onClicked: basePanel.focus = true
        }
    }
}
