import QtQuick 2.1
import QtQuick.Controls 1.0
import QtQuick.Layouts 1.0

import WGControls 2.0
import WGControls.Views 2.0
import WGControls.Layouts 2.0

// This component is for displaying the history panel
WGPanel {
    id: root
    objectName: "WGHistoryView"
    color: palette.mainWindowColor
    title: "History"
    layoutHints: { 'history': 1.0 }

    WGComponent { type: "WGHistoryView" }

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
                    onClicked: clearHistory();
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

                WGScrollView {
                    anchors.fill: parent
                    anchors.margins: defaultSpacing.standardMargin

                    WGListView {
                        id: historyList
                        objectName: "historyList"
                        model: historyListModel
                        columnWidth: width
                        columnDelegates: [historyItemComponent]

                        property int currentCommandRow: 0

                        property Component historyItemComponent: WGTimelineEntryDelegate {
                            view: historyList
                            historyItem: extractItemDetail(itemData.value)
                            columnIndex: itemColumnIndex
                            currentItem: itemRowIndex === historyList.currentCommandRow
                            applied: itemRowIndex <= historyList.currentCommandRow
                        }

                        onItemDoubleClicked: historySelection.data = rowIndex.row
                        onSelectionChanged: historySelectionHelper.select(selectionModel.selectedIndexes)

                        WGSelectionHelper {
                            id: historySelectionHelper
                            source: SelectionHandlerSource

                            onSourceChanged: {
                                if (typeof selectionModel === "undefined") {
                                    return;
                                }

                                select(historyList.selectionModel.selectedIndexes);
                            }
                        }

                        WGDataChangeNotifier {
                            id: historySelection
                            source: CurrentIndexSource

                            onSourceChanged: historyList.currentCommandRow = data
                            onDataChanged: historyList.currentCommandRow = data
                        }
                    }
                }
            }
        }
    }
}

