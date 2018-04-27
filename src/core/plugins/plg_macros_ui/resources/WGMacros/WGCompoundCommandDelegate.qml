import QtQuick 2.3
import QtQuick.Controls 1.2
import QtQuick.Layouts 1.0
import WGControls 2.0
import WGControls.Views 2.0
import WGControls.Layouts 2.0

WGSubPanel {
    objectName: "WGCompoundCommandDelegate"
    WGComponent { type: "WGCompoundCommandDelegate" }
    id: macroItem
    text: itemData.value.macroName
    property var stepsModel: itemData.value.macroStepsModel
    property bool editing: false
    headerObject: WGExpandingRowLayout {
        clip: true
        Item {
            Layout.fillWidth: true
        }

        WGPushButton {
            Layout.preferredHeight: defaultSpacing.minimumRowHeight
            Layout.alignment: Qt.AlignVCenter | Qt.AlignRight
            iconSource: "icons/play_16x16.png"
            tooltip: "Run Macro"
            onClicked: {
                queueCommand(itemData.value.macroId);
            }
        }
        WGPushButton {
            Layout.preferredHeight: defaultSpacing.minimumRowHeight
            Layout.alignment: Qt.AlignVCenter | Qt.AlignRight
            checkable: true
            checked: macroItem.editing
            iconSource: "icons/edit_16x16.png"
            tooltip: "Edit Macro"
            onClicked: {
                macroItem.editing = !macroItem.editing;
            }
        }
        WGPushButton {
            Layout.preferredHeight: defaultSpacing.minimumRowHeight
            Layout.alignment: Qt.AlignVCenter | Qt.AlignRight
            enabled: true
            iconSource: "icons/delete_16x16.png"
            tooltip: "Delete Macro"
            onClicked: {
                macroModel.removeItem(itemRowIndex);
            }
        }
    }

    childObject: WGScrollView {
        id: stepPanel
        WGListView {
            anchors.fill: parent
            model: stepsModel
            columnWidth: parent.width
            columnDelegates: [macroStepsComponent]
            property Component macroStepsComponent: WGFrame {
                id: macroFrame
                width: columnWidth - defaultSpacing.standardMargin * 2
                height: contentItem.height + defaultSpacing.standardMargin * 3

                childObject: Item {
                    anchors.left: parent.left
                    anchors.right: parent.right
                    height: defaultSpacing.minimumRowHeight * 4

                    ColumnLayout {
                        anchors.fill: parent

                        WGLabel {
                            text: "Step " + (itemRowIndex + 1) + ": " + itemData.commandName
                            font.bold: true
                        }

                        WGPropertyTreeView {
                            Layout.fillHeight: true
                            Layout.fillWidth: true
                            model: itemData.commandParameters
                            clamp: true
                            readOnly: !macroItem.editing
                        }
                    }
                }
            }
        }
    }
}
