import QtQuick 2.4
import QtQuick.Controls 1.2
import QtQuick.Layouts 1.0
import WGControls 1.0 as WG1
import WGControls 2.0

WG1.WGPanel {
    id: testListPanel

    property var sourceModel: source

    title: "Async Loader ListModel Test 2.0"
    layoutHints: { 'default': 0.1 }
    color: palette.mainWindowColor

    WG1.WGLoader{
        id: loader
        asynchronous: true
        active: qmlView.needsToLoad
        loading: !(status == Loader.Ready)
        anchors.fill: parent
        sourceComponent: ScrollView {
            anchors.fill: parent
            WGListView {
                id: listView
                asynchronous: loader.asynchronous
                columnWidth: 100
                columnSpacing: 1
                columnDelegates: [nameDelegate, descriptionDelegate]
                columnDelegate: numberDelegate
                columnSequence: [0,0,0,0,0,0,0,0,0,0,0,0,0]
                roles: ["name", "description", "number"]
                model: sourceModel
                currentIndex: 0
                clamp: false

                Component {
                    id: nameDelegate
                    Text {
                        text: itemData.name
                        color: palette.TextColor
                        width: itemWidth
                    }
                }

                Component { 
                    id: descriptionDelegate
                    Text {
                        text: itemData.description
                        color: palette.TextColor
                        width: itemWidth
                    } 
                }
                Component {
                    id: numberDelegate
                    WG1.WGNumberBox {
                        number: itemData.number
                        minimumValue: 0
                        maximumValue: 100
                        stepSize: 1
                        decimals: 0
                        width: itemWidth
                    }
                }
            }
        }
    }
}
