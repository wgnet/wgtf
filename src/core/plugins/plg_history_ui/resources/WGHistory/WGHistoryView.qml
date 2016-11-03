import QtQuick 2.1
import QtQuick.Controls 1.0
import QtQuick.Layouts 1.0

import WGControls 1.0
import WGControls.Views 1.0
import WGControls.Layouts 1.0

// This component is for displaying the history panel
WGPanel {
    id: root
    objectName: "WGHistoryView"
    WGComponent { type: "WGHistoryView" }
    
    color: palette.mainWindowColor

    title: "History"
    layoutHints: { 'history': 1.0 }

    property alias historySelectionExtension: historyModelSelectionExtension

    WGListModel {
        id : historyModel
        source : History

        ValueExtension {}
        ColumnExtension {}
        SelectionExtension {
            id: historyModelSelectionExtension
            multiSelect: true
            onSelectionChanged: {
                historySelectionHelper.select( getSelection() );
            }
        }
    }

    WGSelectionHelper {
        id: historySelectionHelper
        source: SelectionHandlerSource
        onSourceChanged: {
            select( historyModelSelectionExtension.getSelection() );
        }
    }

    WGDataChangeNotifier {
        id: historySelection
        source: CurrentIndexSource
        // When the model changes, update the selection on the view
        onSourceChanged: {
            history.currentIndex = data
        }
        onDataChanged: {
            history.currentIndex = data
        }
    }

    WGFrame {
        id: mainFrame
        objectName: "historyMainFrame"
        anchors.fill: parent

        WGColumnLayout {
            id: mainColumnLayout
            anchors.fill: parent
            anchors.margins: defaultSpacing.standardMargin

            //Placeholder buttons add to as required
            WGExpandingRowLayout {
                id: buttons
                visible: IsClearButtonVisible || IsMakeMacroButtonVisible
                Layout.preferredHeight: defaultSpacing.minimumRowHeight + defaultSpacing.doubleBorderSize
                Layout.fillWidth: true

                WGPushButton {
                    id: clearButton
                    objectName: "clearButton"
                    visible: IsClearButtonVisible
                    text: "Clear"
                    onClicked: {
                        console.assert( historyModel.canClear(),
                            "List is not modifiable" )
                        historyModel.clear()
                    }
                }

                Rectangle {
                    id: spacer
                    color: "transparent"
                    Layout.fillWidth: true
                }

                WGPushButton {
                    id: macroButton
                    objectName: "macroButton"
                    text: "Make Macro..."
                    visible: IsMakeMacroButtonVisible
                    tooltip: "Select a history to make a macro."
                    onClicked: {
                        CreateMacro;
                    }
                }
            }

            //Frame around the List View
            WGTextBoxFrame {
                id: historyFrame
                Layout.fillHeight: true
                Layout.fillWidth: true

                // History list
                WGListView {
                    id: history
                    objectName: "historyList"
                    model: historyModel
                    anchors.fill: parent
                    anchors.margins: defaultSpacing.standardMargin
                    selectionExtension: root.historySelectionExtension
                    columnDelegates: [columnDelegate]

                    Component {
                        id: columnDelegate

                        Loader {
                            source: "WGTimelineEntryDelegate.qml"
                        }
                    }

                    onRowDoubleClicked: {
                        historySelection.data = historyModel.indexRow(modelIndex);
                    }
                }
            }
        }
    }
}

