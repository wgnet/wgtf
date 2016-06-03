import QtQuick 2.3
import QtQuick.Controls 1.2
import QtQuick.Controls.Styles 1.2

/*!
 \brief A styled tab control.

Example:
\code{.js}
WGTabView{
    tabPosition: Qt.TopEdge
    Layout.preferredHeight: 180

    Tab {
        title: "Test one"
        }
}
\endcode
*/

TabView {
    id: baseTabView
    objectName: "WGTabView"

    clip: true

    implicitHeight: defaultSpacing.minimumRowHeight
    implicitWidth: defaultSpacing.standardMargin

    style: WGTabViewStyle{}
}
