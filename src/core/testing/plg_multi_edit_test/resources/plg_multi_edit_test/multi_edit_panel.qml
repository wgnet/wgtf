import QtQuick 2.4
import QtQuick.Controls 1.2
import QtQuick.Layouts 1.0
import WGControls.Layouts 2.0
import WGControls 2.0
import WGControls.Views 2.0
import WGControls.Styles 2.0

WGPanel {
    id: multiEditPanel

    title: "Multi Edit Test"
    layoutHints: { 'test': 0.1 }
    color: palette.mainWindowColor

    property var currentTime
    property var modelCount: 0
    property var models: [objects.value(0), objects.value(1), objects.value(2), objects.value(3)]

    WGMultiEditProxy {
        id: multiEditProxy
        mergeKeys: ["indexPath", "valueType"]
    }//WGMultiEditProxy

    WGColumnLayout {
        id: mainColumnLayout
        objectName: "mainColumnLayout"
        anchors.fill: parent

        RowLayout {
            id: topControls
            objectName: "topControls"
            height: 20
            WGPushButton {
                id: addButton
                text: "Add Model"
                width: 100
                height: 20
                enabled: modelCount < 4
                onClicked: {
                    multiEditProxy.addModel(models[modelCount]);
                    ++modelCount;
                }
            }//WGPushButton

            WGPushButton {
                id: removeButton
                text: "Remove Model"
                width: 100
                height: 20
                enabled: modelCount > 0
                onClicked: {
                    --modelCount;
                    multiEditProxy.removeModel(models[modelCount]);
                }
            }//WGPushButton
        }

        WGPropertyTreeView {
            id: mergeTreeView
            objectName: "mergeTreeView"
            model: multiEditProxy
            Layout.fillHeight: true
            Layout.fillWidth: true
            clamp: true
        } //WGPropertyTreeView

        WGPropertyTreeView {
            id: modelTreeView1
            objectName: "modelTreeView1"
            Layout.fillHeight: true
            Layout.fillWidth: true
            model: models[0]
            clamp: true
        }
		WGPropertyTreeView {
            id: modelTreeView2
            objectName: "modelTreeView2"
            Layout.fillHeight: true
            Layout.fillWidth: true
            model: models[1]
            clamp: true
        }
		WGPropertyTreeView {
            id: modelTreeView3
            objectName: "modelTreeView3"
            Layout.fillHeight: true
            Layout.fillWidth: true
            model: models[2]
            clamp: true
        }
		WGPropertyTreeView {
            id: modelTreeView4
            objectName: "modelTreeView4"
            Layout.fillHeight: true
            Layout.fillWidth: true
            model: models[3]
            clamp: true
        }
    }
} //WGPanel
