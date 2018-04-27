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

    WGReflectedEnum {
        id: enumControl1
        objectName: "enum1"
        y: clones.y + clones.height

        object: self
        path: "enum1"
        value: enum1

        onValueChanged: {
            enum1 = value;
        }
    }

    WGReflectedEnum {
        id: enumControl2
        objectName: "enum2"
        y: enumControl1.y + enumControl1.height

        object: self
        path: "enum2"
        value: enum2

        onValueChanged: {
            enum2 = value;
        }
    }

    ParentObjectTestRow {
        id: parentTestRow
        objectName: "parent test row"

        anchors.top: enumControl2.bottom
        anchors.left: parent.left
        anchors.right: parent.right
    }

    property var folderModel : fileSystemModel

    WGTreeView {
        id: testTreeView

        anchors.top: parentTestRow.bottom
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.bottom: parent.bottom
        model: folderModel
        clamp: true
    }
}
