import QtQuick 2.3
import QtQuick.Controls 1.2
import QtQuick.Layouts 1.0
import WGControls 2.0
import WGControls.Views 2.0

import "wg_hotloading.js" as WGHotloadingJS

WGPanel {

    title: WGHotloadingJS.getPanelTitle()
    layoutHints: { 'test': 0.1 }
    color: palette.mainWindowColor
    property var source: sourceModel

    ColumnLayout {
        anchors.fill: parent
        
        WGLabel {
            id: errorText
            text: ""
            color: "red"
        }

        GridLayout {
            anchors.fill: parent
            columns: 3

            WGPushButton {
                text: "Save File"
                Layout.fillWidth: true
                Layout.minimumHeight: 50
                onClicked: {
                    source.setHotloadingText(hotloadingText.text);
                    errorText.text = source.getErrorText();
                }
            }

            WGPushButton {
                text: "Save File"
                Layout.fillWidth: true
                Layout.minimumHeight: 50
                onClicked: {
                    source.setHotloadingBaseText(hotloadingBaseText.text);
                    errorText.text = source.getErrorText();
                }
            }

            WGPushButton {
                text: "Save File"
                Layout.fillWidth: true
                Layout.minimumHeight: 50
                onClicked: {
                    source.setHotloadingJSText(hotloadingJSText.text);
                    errorText.text = source.getErrorText();
                }
            } 

            TextArea {
                id: hotloadingText
                text: source.getHotloadingText()
                verticalAlignment: Text.AlignTop
                Layout.fillWidth: true
                Layout.fillHeight: true
                textColor: palette.textColor
            }
        
            TextArea {
                id: hotloadingBaseText
                text: source.getHotloadingBaseText()
                verticalAlignment: Text.AlignTop
                Layout.fillWidth: true
                Layout.fillHeight: true
                textColor: palette.textColor
            }

            TextArea {
                id: hotloadingJSText
                text: source.getHotloadingJSText()
                verticalAlignment: Text.AlignTop
                Layout.fillWidth: true
                Layout.fillHeight: true
                textColor: palette.textColor
            }
        }

        WGHotloading {
            Layout.fillWidth: true
        }
    }
}