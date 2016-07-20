import QtQuick 2.3
import QtQuick.Controls 1.2
import QtQuick.Layouts 1.1
import QtQuick.Dialogs 1.2
import BWControls 1.0
import WGControls 1.0

// WGPanel triggers auto-reloading when QML file is saved
WGPanel {
    color: palette.MainWindowColor
    property var title: "Test Async Loader List View 1.0"
    property var layoutHints: { 'default': 0.1 }
    property var topControlsHeight: 20

    // -- Data models
    // Provided from Python scripts
    WGListModel {
        id: listModel
        source: listSourceModel

        ValueExtension {}
        ColumnExtension {}
        ComponentExtension {}
        SelectionExtension {
            id: listModelSelection
            multiSelect: false
        }
    }


    // -- View
    WGLoader{
        id: loader
        asynchronous: true
        loading: true
        anchors.fill: parent
        active: qmlView.needsToLoad
        sourceComponent: WGListView {
            id: listView
            anchors.fill: parent
            asynchronous: loader.asynchronous
            model: listModel
            onReadyToShow: {
                loader.loading = false;
            }

            selectionExtension: listModelSelection
            columnDelegates: [
                nameDelegate,
                descriptionDelegate,
                ]
            defaultColumnDelegate: indexDelegate

            columnSequence: [0,0,0,0,0,0,0,0,0,0,0,0,0]

            Layout.minimumHeight: 50
            Layout.minimumWidth: 100
            Layout.fillHeight: true
            Layout.fillWidth: true

            property Component nameDelegate: Text {
                id: mapNameDelegate
                text: itemData.value.name
                color: palette.TextColor
            }
            property Component descriptionDelegate: Text {
                id: scenarioNameDelegate
                text: itemData.value.description
                color: palette.TextColor
            }
            property Component indexDelegate: WGNumberBox {
                id: rankDelegate
                number: itemData.value.index
                minimumValue: 0
                maximumValue: 100
                stepSize: 1
                decimals: 0
            }
        }
    }
}
