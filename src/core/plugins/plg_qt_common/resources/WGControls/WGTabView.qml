import QtQuick 2.5
import QtQuick.Controls 1.4
import QtQuick.Controls.Styles 1.4
import WGControls.Styles 1.0

/*!
 \ingroup wgcontrols
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
    WGComponent { type: "WGTabView" }

    clip: true

    implicitHeight: defaultSpacing.minimumRowHeight
    implicitWidth: defaultSpacing.standardMargin

    style: WGTabViewStyle{}
}
