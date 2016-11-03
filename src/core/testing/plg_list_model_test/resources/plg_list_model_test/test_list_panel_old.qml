import QtQuick 2.3
import QtQuick.Controls 1.2
import QtQuick.Layouts 1.0

import WGControls 1.0
import WGControls.Views 1.0

WGPanel {
    title: "ListModel Test"
    layoutHints: { 'test': 0.1 }

    property var sourceModel: useModel ? source : null
    color: palette.mainWindowColor

    property var useModel: 1
    property var topControlsHeight: 20

    Button {
        id: switchModelButton
        anchors.top: parent.top
        anchors.left: parent.left
        width: 150
        height: topControlsHeight
        text: useModel ? "Switch Model Off" : "Switch Model On"

        onClicked: {
            useModel = useModel == 0 ? 1 : 0;
        }
    }

    WGListModel {
        id: listModel
        source: sourceModel

        HeaderFooterTextExtension {}
        ValueExtension {}
        ColumnExtension {}
        SelectionExtension {
            id: listModelSelection
            multiSelect: true
        }
    }

    WGListView {
        id: testListView
        anchors.top: switchModelButton.bottom
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.bottom: parent.bottom
        spacing: 1
        showColumnsFrame: true
        showColumnHeaders: true
        showColumnFooters: true
        model: listModel
        selectionExtension: listModelSelection
        columnDelegates: [defaultColumnDelegate, columnDelegate]

        Component {
            id: columnDelegate

            Item {
                Layout.fillWidth: true
                Layout.preferredHeight: testListView.minimumRowHeight

                Rectangle {
                    anchors.fill: parent
                    anchors.margins: 1
                    color: {
                        if (typeof itemData.value === "string")
                        {
                            return "transparent";
                        }

                        var colour = itemData.value;
                        var r = colour > 9999 ? (colour / 10000) % 100 + 156 : 0;
                        var g = colour > 99 ? (colour / 100) % 100 + 156 : 0;
                        var b = colour % 100 + 156;

                        return Qt.rgba(r / 255, g / 255, b / 255, 1);
                    }
                }

                Text {
                    clip: true
                    anchors.left: parent.left
                    anchors.top: parent.top
                    anchors.bottom: parent.bottom
                    anchors.margins: 4
                    verticalAlignment: Text.AlignVCenter
                    visible: typeof itemData.value === "string"
                    text: typeof itemData.value === "string" ? itemData.value : ""
                    color: palette.textColor
                }
            }
        }
    }
}
