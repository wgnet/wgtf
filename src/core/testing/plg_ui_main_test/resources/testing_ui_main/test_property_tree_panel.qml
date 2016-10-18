import QtQuick 2.1
import QtQuick.Controls 1.0
import QtQuick.Layouts 1.3
import WGControls 2.0
import WGControls.Layouts 2.0
import WGControls.Views 2.0

WGPanel {
    id: propertyTreeTest20Panel
    objectName: "propertyTreeTest20Panel"
    color: palette.mainWindowColor
    property string title: "PropertyTree Test 2.0"
    property var layoutHints: { 'test': 0.1 }
    property int topControlsHeight: 20

    // TODO NGT-2493 ScrollView steals keyboard focus
    Keys.forwardTo: [testTreeView]
    focus: true

    WGColumnLayout {
        id: mainColumnLayout
        objectName: "mainColumnLayout"
        anchors.fill: parent

        RowLayout {
            id: topControls
            objectName: "topControls"
            height: topControlsHeight
            WGPushButton {
                id: testButton
                objectName: "testButton"
                text: "Run Test Script"

                onClicked: {
                    runTestScript();
                }
            }
        }

        WGPropertyTreeView {
            id: testTreeView
            objectName: "testTreeView"
            Layout.fillHeight: true
            Layout.fillWidth: true
            model: treeModel
            clamp: true
        }
    }
}
