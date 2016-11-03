import QtQuick 2.1
import QtQuick.Controls 1.0
import WGControls 2.0
import WGControls.Views 2.0

WGPanel {
    title: "Custom Model Interface"
    layoutHints: { 'test': 0.1 }

    color: palette.mainWindowColor

    Column {
        id: original
        CustomModelInterfaceTestControl {
            source: Implementation1
        }

        CustomModelInterfaceTestControl {
            source: Implementation2
        }

        CustomModelInterfaceTestControl {
            source: Implementation3
        }
    }


    Column {
        id: clones
        y: original.y + original.height + 20
        CustomModelInterfaceTestControl {
            source: Implementation1
        }

        CustomModelInterfaceTestControl {
            source: Implementation2
        }

        CustomModelInterfaceTestControl {
            source: Implementation3
        }
    }

    property var folderModel : fileSystemModel

    WGTreeView {
        id: testTreeView

        anchors.top: clones.bottom
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.bottom: parent.bottom
        model: folderModel
        clamp: true
    }
}
