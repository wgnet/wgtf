import QtQuick 2.3
import QtQuick.Controls 1.2
import QtQuick.Layouts 1.0
import WGControls 2.0
import WGControls.Views 2.0
import WGControls.Layouts 2.0

WGPanel {
    color: palette.mainWindowColor
    property string title: panelName
    property var layoutHints: { 'test': 0.1 }
    property int topControlsHeight: 20
    property var source: sourceModel

    WGColumnLayout {
        id: mainColumnLayout
        anchors.fill: parent

        RowLayout {
            // Refresh button for debugging - refreshes entire tree
            WGPushButton {
                id: refreshButton
                height: topControlsHeight
                text: "Refresh"

                onClicked: {
                    // Fire signal to update UI
                    pythonObjectsChanged(pythonObjects);
                }
            }

            // Button that runs script for debugging
            WGPushButton {
                id: testButton
                height: topControlsHeight
                text: "Run Test Script"

                onClicked: {
                    runTestScript();
                }
            }
            
            WGLabel {
                Layout.preferredHeight: topControlsHeight
                Layout.fillWidth: true
                verticalAlignment: Text.AlignVCenter
                text: source.testScriptDescription
            }

            Connections {
                target: source
                onRunTestScriptInvoked: {
                    source.testScriptDescriptionChanged(0);
                }
            }
        }

        WGPropertyTreeView {
            id: testTreeView
            Layout.fillHeight: true
            Layout.fillWidth: true
            model: source.pythonObjects
        }
    }
}
