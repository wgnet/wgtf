import QtQuick 2.5
import QtQuick.Controls 1.4
import QtQuick.Layouts 1.3

import WGControls 2.0
import WGControls.Styles 2.0
import WGControls.Views 2.0

Rectangle {
    color: palette.mainWindowColor

    ColumnLayout {
        anchors.fill: parent
        spacing: defaultSpacing.standardMargin

        Item {
            id:spacerItem
            height: defaultSpacing.standardMargin
        }

        Text {
            id: titleText
            text: "Example of WGControls"
            color: palette.textColor
        }

        ScrollView{
            frameVisible: true
            Layout.fillHeight: true
            Layout.fillWidth: true

            ColumnLayout {
                Text {
                    text: "fill me with the WGControls test panel when its done"
                    color: palette.textColor
                }

                WGPushButton {
                    text: "I'm sorry, Dave. I'm afraid I can't do that."
                }
                WGNumberBox{

                }

            }

        }
    }
}

