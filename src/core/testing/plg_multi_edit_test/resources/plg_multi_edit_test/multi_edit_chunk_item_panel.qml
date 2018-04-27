import QtQuick 2.4
import QtQuick.Controls 1.2
import QtQuick.Layouts 1.0
import WGControls.Layouts 2.0
import WGControls 2.0
import WGControls.Views 2.0
import WGControls.Styles 2.0

WGPanel {
    id: multiEditChunkItemPanel

    title: "Multi Edit Chunk Item Test"
    layoutHints: { 'test': 0.1 }
    color: palette.mainWindowColor

    property int topControlsHeight: 20

    property int modelCount: 0
    property var models: [chunkModelData(), chunkTreeData()]

    WGMultiEditProxy {
        id: multiEditProxy
        mergeKeys: ["display"]
    }//WGMultiEditProxy

    WGColumnLayout {
        id: mainColumnLayout
        objectName: "mainColumnLayout"
        anchors.fill: parent

        RowLayout {
            id: topControls
            objectName: "topControls"
            height: 20
            Layout.preferredHeight: topControlsHeight

            WGLabel {
                Layout.preferredHeight: topControlsHeight
                Layout.fillWidth: true
                verticalAlignment: Text.AlignVCenter
                text: "Selection"
            }

            WGPushButton {
                id: addButton
                text: "Add Selection"
                width: 100
                height: topControlsHeight
                enabled: modelCount < models.length
                onClicked: {
                    multiEditProxy.addModel(models[modelCount]);
                    ++modelCount;
                }
            }//WGPushButton

            WGPushButton {
                id: removeButton
                text: "Remove Selection"
                width: 100
                height: topControlsHeight
                enabled: modelCount > 0
                onClicked: {
                    --modelCount;
                    multiEditProxy.removeModel(models[modelCount]);
                }
            }//WGPushButton
        }
        WGScrollView {
            Layout.fillHeight: true
            Layout.fillWidth: true

            WGPropertyTreeView {
                id: mergeTreeView
                objectName: "mergeTreeView"
                model: multiEditProxy
                clamp: true
                expandToDepth: 0
            } //WGPropertyTreeView
        }

        WGLabel {
            Layout.preferredHeight: topControlsHeight
            Layout.fillWidth: true
            verticalAlignment: Text.AlignVCenter
            text: "test_file.model"
        }
        WGScrollView {
            Layout.preferredHeight: 200
            Layout.fillWidth: true

            WGPropertyTreeView {
                id: modelTreeView1
                objectName: "modelTreeView1"
                model: models[0]
                clamp: true
                expandToDepth: 0
            }
        }

        WGLabel {
            Layout.preferredHeight: topControlsHeight
            Layout.fillWidth: true
            verticalAlignment: Text.AlignVCenter
            text: "test_file.srt"
        }
        WGScrollView {
            Layout.preferredHeight: 200
            Layout.fillWidth: true

            WGPropertyTreeView {
                id: modelTreeView2
                objectName: "modelTreeView2"
                model: models[1]
                clamp: true
                expandToDepth: 0
            }
        }
    }
} //WGPanel
