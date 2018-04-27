import QtQuick 2.4
import QtQuick.Controls 1.2
import QtQuick.Layouts 1.0
import WGControls 2.0
import WGControls.Private 2.0

/** A title bar and toolbar to appear above views such as the property tree.
\ingroup wgcontrols*/

WGButtonFrame {
    id: viewHeaderBar
    objectName: "WGViewHeader"
    WGComponent { type: "WGHeaderToolbar20" } 
    radius: 0

    property string headerTitle: ""

    property var headerIcon: ""
    property var headerIconOverlay: ""
    property bool enableHeaderToggle: false
    property bool headerToggled: false
    property bool displayheaderMenu : true
    property var toolbarModel: null
    property var headerMenu: null
    property int buttonSize: viewHeaderBar.height - defaultSpacing.doubleBorderSize

    property Component toolbarDelegate: WGPushButton {
        implicitHeight: buttonSize
        implicitWidth: buttonSize
        iconSource: model.icon
    }

    signal headerIconClicked()
    signal headerMenuOpened()

    RowLayout {
        anchors.fill: parent
        anchors.leftMargin: defaultSpacing.doubleBorderSize
        anchors.rightMargin: defaultSpacing.doubleBorderSize

        Rectangle {
            Layout.preferredHeight: parent.height - defaultSpacing.doubleBorderSize
            width: defaultSpacing.rowSpacing
            color: palette.highlightShade
        }

        WGImage {
            Layout.preferredHeight: 16
            Layout.preferredWidth: 16
            source: headerIcon

            visible: headerIcon != ""

            Image {
                anchors.centerIn: parent
                visible: headerToggled
                source: headerIconOverlay
            }

            MouseArea {
                anchors.fill: parent
                hoverEnabled: enableHeaderToggle
                enabled: enableHeaderToggle
                cursorShape: enableHeaderToggle ? Qt.PointingHandCursor : Qt.ArrowCursor
                onClicked: {
                    headerIconClicked()
                }
            }
        }

        WGLabel {
            Layout.alignment: Qt.AlignVCenter | Qt.AlignLeft
            verticalAlignment: Text.AlignVCenter
            font.bold: true
            font.pixelSize: 11
            text: headerTitle
            visible: headerTitle != ""
        }

        Item {
            id: toolbarFrame
            Layout.fillWidth: true
            Layout.preferredHeight: parent.height
            GridLayout {
                id: filterLayout
                columns: toolbarFrame.width / (24 + columnSpacing)
                anchors.fill: parent
                visible: toolbarModel != null
                Repeater {
                    model: toolbarModel
                    delegate: toolbarDelegate
                }
            }
        }

        WGPushButton {
            visible: headerMenu != null && displayheaderMenu
            Layout.preferredHeight: buttonSize
            Layout.preferredWidth: buttonSize
            iconSource: "../icons/sort_down_10x10.png"
            showMenuIndicator: false
            onClicked: {
                headerMenuOpened();
            }
            menu: headerMenu
        }

        Rectangle {
            Layout.preferredHeight: parent.height - defaultSpacing.doubleBorderSize
            width: defaultSpacing.rowSpacing
            color: palette.highlightShade
        }
    }
}
