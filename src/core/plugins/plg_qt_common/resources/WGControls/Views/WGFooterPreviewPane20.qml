import QtQuick 2.6
import QtQuick.Controls 1.4
import QtQuick.Layouts 1.3
import WGControls 2.0
import WGControls.Private 2.0

/** A frame that holds an image and description text to appear below views such as the property tree.
\ingroup wgcontrols*/


RowLayout {
    id: viewHeader
    objectName: "WGFooterPreviewPane"
    WGComponent { type: "WGFooterPreviewPane20" }

    property string title: ""
    property string description: ""
    property int imageSize: 64

    property Component imageBackground: Item {
        anchors.fill: parent
        anchors.margins: defaultSpacing.standardMargin
    }

    property Component previewImage: null

    spacing: 0

    Item {
        Layout.preferredHeight: parent.height
        Layout.preferredWidth: height

        visible: previewImage != null

        Loader {
            id: bgLoader
            sourceComponent: imageBackground
            anchors.fill: parent
        }

        Loader {
            id: previewLoader
            anchors.centerIn: parent
            sourceComponent: previewImage
        }
    }

    Item {
        Layout.preferredHeight: parent.height
        Layout.fillWidth: true
        WGTextBoxFrame {
            id: textFrame
            anchors.fill: parent
            WGScrollView {
                id: textScrollView
                anchors.fill: parent
                horizontalScrollBarPolicy: Qt.ScrollBarAlwaysOff
                Text {
                    id: descriptionText
                    width: textFrame.width - 10
                    textFormat: TextEdit.RichText
                    padding: defaultSpacing.rowSpacing
                    color: palette.textColor
                    wrapMode: Text.WordWrap
                    text: "<b>" + title + "</b><br>" + description
                }
            }
        }
    }
    Item {
        Layout.preferredWidth: defaultSpacing.standardMargin
        Layout.preferredHeight: parent.height
    }
}
