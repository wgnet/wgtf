import QtQuick 2.4
import QtQuick.Controls 1.2
import QtQuick.Layouts 1.0

import WGControls 2.0
import WGControls.Views 2.0

WGPanel {

    id: helloPanelList
    title: "Hello Panel List"
    color: palette.mainWindowColor

    /*! Last selected row
    */
    property var selectedRow: helloPanelListView.currentIndex.row

    ColumnLayout {
        anchors.fill: parent

        Rectangle {
            id: labelRectangle
            anchors.margins: 1
            color: palette.lightPanelColor
            anchors.left: parent.left
            anchors.right: parent.right
            height: 30

            WGLabel {
                id: labelSelected
                text: "Selected: " + (typeof selectedRow != 'undefined' ? getCollectionItem(selectedRow) : "-")
                font.bold: true
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
                anchors.fill: parent
            }
        }

        WGScrollView {
            id: helloPanelScrollView
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.margins: 10
            Layout.fillHeight: true
            Layout.fillWidth: true

            WGListView {
                id: helloPanelListView
                columnDelegates: [customDelegate]
                columnSpacing: 1
                columnSequence: [0]
                model: sampleCollection
                clamp: true
                currentIndex: 0
            }
        }

        /*! Delegate that dictates how to display each individual item in the model
        */
        Component {
            id: customDelegate

            Item {
                implicitWidth: textItem.implicitWidth
                implicitHeight: textItem.implicitHeight + 2

                Text {
                    id: textItem
                    objectName: "customDelegate_text_" + textItem.text
                    visible: true
                    text: itemData.value
                    color: palette.textColor
                }
            }
        }
    }
}
