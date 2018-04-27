import QtQuick 2.5
import QtQuick.Controls 1.4
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
                        columnWidth: parent.width
                        columnDelegates: [historyItemComponent]

                        property int currentCommandRow: 0

                        property Component historyItemComponent: WGTimelineEntryDelegate {
                            id: historyDelegate
                            view: historyList
                            width: parent.width
                            height: implicitHeight
                            historyItem: itemData.value
                            columnIndex: itemColumnIndex
                            currentItem: itemRowIndex === historyList.currentCommandRow
                            applied: itemRowIndex <= historyList.currentCommandRow
                            Rectangle {
                                color: palette.lightShade
                                anchors.bottom: parent.bottom
                                width: parent.width
                                height: 1
                            }
                        }

                        onCountChanged: {
                            contentY = contentHeight
                        }

                        onItemDoubleClicked: {
                            setCommandIndex(rowIndex.row);
                        }
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

                        property int historyIndex: commandIndex
                        onHistoryIndexChanged: {
                            historyList.currentCommandRow = commandIndex
                        }
                    }
                }
            }
        }
    }
}

