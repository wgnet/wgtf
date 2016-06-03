import QtQuick 2.3
import QtQuick.Controls 1.2
import QtQuick.Layouts 1.0

import WGControls 1.0

WGPanel {
    id: rootFrame
    objectName: "test_contextmenu_panel"
    title: "WGContextMenu Test"
    layoutHints: { 'test': 0.1 }
    property var sourceModel: source
    color: palette.mainWindowColor
    z: 1

    WGExpandingRowLayout {
        anchors.fill: parent

        WGLabel {
            text: "Test the context menu by right-clicking on this label."

            MouseArea {
                anchors.fill: parent
                acceptedButtons: Qt.AllButtons;
                onClicked: {
                    if (mouse.button == Qt.RightButton) {
                        // This context object may be anything that can be converted from a QVariant into a Variant.
                        // Ideally developers will use an ObjectHandle, but this integer is a quick example.
                        testContextMenu.contextObject = 12345;

                        // After setting the context object, tell the menu to show. This will prepare the menu
                        // by attaching the contextObject data to the IActions so their bound functions can
                        // reference it if needed.
                        testContextMenu.popup();
                    }
                }
            }
        }

        WGContextMenu {
            id: testContextMenu
            objectName: "testContextMenu"
            path: "ContextMenu"
        }
    }
}
