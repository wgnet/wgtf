import QtQuick 2.3
import QtQuick.Controls 1.2
import QtQuick.Layouts 1.0
import WGControls 1.0

WGPanel {
    color: palette.mainWindowColor
    property var title: panelName
    property var layoutHints: { 'test': 0.1 }
    property var topControlsHeight: 20

    WGTreeModel {
        id: testModel
        source: pythonObjects

        ValueExtension {}
        ColumnExtension {}
        ComponentExtension {}
        TreeExtension {
            id: treeModelExtension
            selectionExtension: treeModelSelection
        }
        SelectionExtension {
            id: treeModelSelection
        }
    }

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
        }

        WGTreeView {
            id: testTreeView

            Layout.fillHeight: true
            Layout.fillWidth: true

            model: testModel
            selectionExtension: treeModelSelection
            treeExtension: treeModelExtension
            columnDelegates: [defaultColumnDelegate, propertyDelegate]

            rightMargin: 8
            childRowMargin: 2
            columnSpacing: 4
            lineSeparator: false
            autoUpdateLabelWidths: true
            backgroundColourMode: incrementalGroupBackgroundColours
            backgroundColourIncrements: 5

            // Delegate to use Reflected components for the second column.
            property Component propertyDelegate: Loader {
                clip: true
                sourceComponent: itemData != null ? itemData.component : null
            }
        }
    }
}
