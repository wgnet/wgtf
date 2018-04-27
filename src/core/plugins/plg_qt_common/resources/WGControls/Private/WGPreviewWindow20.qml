import QtQuick 2.5
import QtQuick.Controls 1.4
import QtQuick.Layouts 1.3
import QtQuick.Window 2.2

import WGControls 2.0
import WGControls.Private 2.0
import WGControls.Styles 2.0
import WGControls.Views 2.0

/*!
 \ingroup wgcontrols
 \brief A panel that shows a preview image and a path for WGAssetBrowser
*/

Item {
    id: previewFrame
    objectName: "WGPreviewWindow"
    WGComponent { type: "WGPreviewWindow20" }

    property var assetBrowser

    property bool showPreview: assetBrowser != null ? assetBrowser.showPreview : false

    // TODO - Reenable this when thumbnails and previews work properly.
    property var currentPreview: assetBrowser != null && assetBrowser.currentPreview != null ? assetBrowser.currentPreview : null

    property bool validImagePreview: currentPreview != null && typeof currentPreview.imagePath != "undefined" && currentPreview.imagePath != "undefined" ? true : false
    property bool validTextureImage: currentPreview != null && typeof currentPreview.texturePath != "undefined" && currentPreview.texturePath != "undefined" ? true : false
    property bool validTextureProperty: currentPreview != null && typeof currentPreview.textureProperty != "undefined" && currentPreview.textureProperty != "undefined" ? true : false
    property bool validTexturePreview: validTextureImage && validTextureProperty
    property bool validTextPreview: currentPreview != null && typeof currentPreview.textContent != "undefined" && currentPreview.textContent != "undefined" ? true : false
    property bool validPhysicsPreview: currentPreview != null && typeof currentPreview.physicsProperty != "undefined" && currentPreview.physicsProperty != "undefined" ? true : false

    function getPreview() {
        if(!showPreview) {
            return undefined
        }
        if(validImagePreview) {
            return preview
        }
        if(validTexturePreview) {
            return texturePreview
        }
        if(validTextPreview) {
            return textPreview
        }
        if(validPhysicsPreview) {
            return physicsPreview
        }
        return undefined
    }

    property Component header : RowLayout {
        id: headerContainer
        clip: true
        Text {
            Layout.fillWidth: true
            Layout.preferredHeight: defaultSpacing.minimumRowHeight
            Layout.alignment: Qt.AlignVCenter
            Layout.leftMargin: defaultSpacing.standardMargin
            color: palette.placeholderTextColor
            text: "Asset Preview"
            verticalAlignment: Text.AlignVCenter
        }


        WGToolButton {
            id: btnPreviewFrameOrientation
            iconSource: "icons/rows_16x16.png"
            tooltip: "Horizontal/Vertical/Auto Toggle"

            onClicked: {
                if (assetBrowser.previewViewOrientation == assetBrowser.horizontalLayout) {
                    assetBrowser.previewViewOrientation = assetBrowser.verticalLayout
                    assetBrowser.previewAutoOrientation = false
                }
                else if (assetBrowser.previewViewOrientation == assetBrowser.verticalLayout) {
                    assetBrowser.previewViewOrientation = assetBrowser.autoLayout
                    assetBrowser.previewAutoOrientation = true
                    assetBrowser.callCheckPreviewWidthToggleOrientation()
                }
                else if (assetBrowser.previewViewOrientation == assetBrowser.autoLayout) {
                    assetBrowser.previewViewOrientation = assetBrowser.horizontalLayout
                    assetBrowser.previewAutoOrientation = false
                }
                else { // catch for preferences in bad state
                    assetBrowser.previewViewOrientation = assetBrowser.verticalLayout
                    assetBrowser.previewAutoOrientation = false
                }
            }

            states: [
                State {
                    name: "VERTICAL"
                    PropertyChanges { target: btnPreviewFrameOrientation; iconSource: "icons/rows_16x16.png" }
                },
                State {
                    name: "HORIZONTAL"
                    PropertyChanges { target: btnPreviewFrameOrientation; iconSource: "icons/columns_16x16.png" }
                },
                State {
                    name: "AUTO"
                    PropertyChanges { target: btnPreviewFrameOrientation; iconSource: "icons/align_a_16x16.png" }
                }
            ]

            Connections{
                target: assetBrowser
                ignoreUnknownSignals: true
                onPreviewViewOrientationChanged: {
                    btnPreviewFrameOrientation.state = assetBrowser.previewViewOrientation
                    if (assetBrowser.previewViewOrientation != "AUTO") {
                        assetBrowser.itemviewPreviewSplitterState = assetBrowser.previewViewOrientation
                    }
                }
            }
        }

        WGToolButton {
            iconSource: "icons/close_sml_16x16.png"
            onClicked: {
                assetBrowser.showPreview = false
            }
        }
    }

    property Component preview : Item {
        id: imagePreview
        WGImage {
            anchors.fill: parent
            fillMode: sourceSize.width > parent.width || sourceSize.height > parent.height ? Image.PreserveAspectFit : Image.Pad
            source: validImagePreview ? "image://QtThumbnailProvider/" + currentPreview.imagePath : ""
        }
        visible: validImagePreview
    }
    property Component texturePreview: ColumnLayout {
        WGImage {
            Layout.fillHeight: true
            Layout.fillWidth: true
            fillMode: sourceSize.width > parent.width || sourceSize.height > parent.height ? Image.PreserveAspectFit : Image.Pad
            source: validTexturePreview ? "image://QtThumbnailProvider/" + currentPreview.texturePath : ""
        }

        WGScrollView {
            Layout.preferredHeight: defaultSpacing.minimumRowHeight * 3
            Layout.fillWidth: true
            WGPropertyTreeView
            {
                model: validTexturePreview ? currentPreview.textureProperty.model : null
                clamp: true
                readOnly: true
            }
        }
    }
    property Component textPreview: TextArea {
        readOnly: true
        textColor: palette.textColor
        text: validTextPreview ? currentPreview.textContent : ""
    }
    property Component physicsPreview: WGScrollView {
        WGPropertyTreeView
        {
            model: validPhysicsPreview ? currentPreview.physicsProperty.model : null
            clamp: true
            readOnly: true
        }
    }

    property Component content : Rectangle {
        id: contentContainer
        color: palette.darkShade

        Loader {
            id: previewLoader
            anchors.fill: parent
            anchors.margins: defaultSpacing.standardBorderSize

            sourceComponent: getPreview()
            asynchronous: true
        }
    }

    property Component footer : Item {
        id: footerContainer
        implicitHeight: defaultSpacing.minimumRowHeight
        clip: true

        WGTextBox {
            id: fileText
            anchors.leftMargin: defaultSpacing.standardMargin
            anchors.rightMargin: defaultSpacing.standardMargin
            anchors.fill: parent
            style: WGInvisTextBoxStyle{
                 textColor: palette.neutralTextColor
             }
            text: currentPreview == null ? "(No preview)" : currentPreview.assetName
            textColor: palette.neutralTextColor
            readOnly: true
        }
    }

    ColumnLayout {
        anchors.fill: parent
        spacing: 1

        Loader {
            id: headerLoader
            Layout.fillWidth: true
            sourceComponent: header
        }

        Loader {
            id: contentLoader
            Layout.fillWidth: true
            Layout.fillHeight: true
            sourceComponent: content
        }

        Loader {
            id: footerLoader
            Layout.fillWidth: true
            Layout.preferredHeight: defaultSpacing.minimumRowHeight
            sourceComponent: footer
        }
    }
}

