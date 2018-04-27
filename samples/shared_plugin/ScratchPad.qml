import QtQuick 2.5
import QtQuick.Controls 1.4
import QtQml.Models 2.2
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
            text: "Use this page to tinker"
            color: palette.textColor
        }

        ScrollView{
            frameVisible: true
            Layout.fillHeight: true
            Layout.fillWidth: true


        }
    }
}

