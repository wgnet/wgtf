import QtQuick 2.4
import QtQuick.Controls 1.2
import QtQuick.Layouts 1.0

import WGControls 2.0
import WGControls.Views 2.0

WGPanel {
    id: testListPanel

    property var sourceModel: WGColumnLayoutProxy {
		sourceModel: source
		columnSequence: [0,0,0,0,0,0,0,0,0,0,0,0,0]
	}

    title: "Async Loader ListModel Test 2.0"
    layoutHints: { 'default': 0.1 }
    color: palette.mainWindowColor

    WGLoader{
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
                model: sourceModel
                currentIndex: 0
                clamp: false

                Component {
                    id: nameDelegate
                    Text {
                        text: itemData.name
                        color: palette.TextColor
                        width: parent.width
                    }
                }

                Component {
                    id: descriptionDelegate
                    Text {
                        text: itemData.description
                        color: palette.TextColor
                        width: parent.width
                    }
                }
                Component {
                    id: numberDelegate
                    WGNumberBox {
                        number: itemData.number
                        minimumValue: 0
                        maximumValue: 100
                        stepSize: 1
                        decimals: 0
                        width: parent.width
                    }
                }
            }
        }
    }
}
