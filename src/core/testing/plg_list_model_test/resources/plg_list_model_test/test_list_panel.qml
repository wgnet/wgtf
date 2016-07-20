import QtQuick 2.4
import QtQuick.Controls 1.2
import QtQuick.Layouts 1.0
import WGControls 1.0 as WG1
import WGControls 2.0

WG1.WGPanel {
    id: testListPanel

    property var sourceModel: useModel ? source : null
    property bool useModel: true
	property bool sortAsc: true
    property int topControlsHeight: 20

    title: "ListModel Test 2.0"
    layoutHints: { 'test': 0.1 }
    color: palette.mainWindowColor

    // TODO NGT-2493 ScrollView steals keyboard focus
    Keys.forwardTo: [listView1, listView2]
    focus: true

	WGSortFilterProxy {
		id: proxy
		sourceModel: testListPanel.sourceModel
	}

    Button {
        id: switchModelButton
        anchors.top: parent.top
        anchors.left: parent.left
        width: 150
        height: topControlsHeight
        text: useModel ? "Switch Model Off" : "Switch Model On"

        onClicked: {
            useModel = !useModel;
        }
    }

	Button {
        id: sortModelButton
        anchors.top: parent.top
        anchors.left: switchModelButton.right
        width: 150
        height: topControlsHeight
        text: sortAsc ? "Sort Descending" : "Sort Ascending"

        onClicked: {
            sortAsc = !sortAsc;
			proxy.sort( 0, sortAsc ? Qt.AscendingOrder : Qt.DescendingOrder );
        }
    }

    Column {
        id: testLists

        property alias myHeaderDelegate: myHeaderDelegate
        property alias myFooterDelegate: myFooterDelegate
        property alias editDelegate: editDelegate
        property alias colorDelegate: colorDelegate

        anchors.top: switchModelButton.bottom
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.bottom: parent.bottom

        WGScrollView {
            anchors.left: parent.left
            anchors.right: parent.right
            height: parent.height / 2

            WGListView {
                id: listView1
                //anchors.margins: 10
                //leftMargin: 50
                //rightMargin: 50
                //topMargin: 50
                //bottomMargin: 50
                columnWidths: [70, 150]
                columnSpacing: 1
                columnSequence: [0,1,0]
                columnDelegates: [columnDelegate, colorDelegate, editDelegate]
                headerDelegate: myHeaderDelegate
                footerDelegate: myFooterDelegate
                roles: ["value", "headerText", "footerText"]
                model: proxy
                clamp: false
                currentIndex: 0
            }
        }
        WGScrollView {
            anchors.left: parent.left
            anchors.right: parent.right
            height: parent.height / 2

            WGListView {
                id: listView2
                //anchors.margins: 10
                //leftMargin: 50
                //rightMargin: 50
                //topMargin: 50
                //bottomMargin: 50
                columnWidths: [70, 150]
                columnSpacing: 1
                columnSequence: [0,1,0]
                columnDelegates: [columnDelegate, colorDelegate, editDelegate]
                headerDelegate: myHeaderDelegate
                footerDelegate: myFooterDelegate
                roles: ["value", "headerText", "footerText"]
                model: sourceModel
                clamp: true
                currentIndex: 0
            }
        }

        Component {
            id: myHeaderDelegate

            Text {
                id: textBoxHeader
                color: palette.textColor
                text: valid ? headerData.headerText : ""
                height: 24

                property bool valid: headerData !== null &&
                    typeof headerData !== "undefined" &&
                    typeof headerData.headerText !== "undefined"
            }
        }

        Component {
            id: myFooterDelegate

            Text {
                id: textBoxFooter
                color: palette.textColor
                text: valid ? headerData.footerText : ""
                height: 24

                property bool valid: headerData !== null &&
                    typeof headerData !== "undefined" &&
                    typeof headerData.footerText !== "undefined"
            }
        }

        Component {
            id: colorDelegate

            Item {
                width: itemWidth
                implicitWidth: Math.max(textItem.implicitWidth, 200)
                implicitHeight: 24

				property var value: itemData !== null ? itemData.value : ""

                Rectangle {
                    id: colorItem
                    objectName: "colorDelegate_color_" + textItem.text

                    anchors.fill: parent
                    anchors.margins: 1
                    color: {
                        if (typeof value === "string")
                        {
                            return "transparent";
                        }

                        var colour = value;
                        var r = colour > 9999 ? (colour / 10000) % 100 + 156 : 0;
                        var g = colour > 99 ? (colour / 100) % 100 + 156 : 0;
                        var b = colour % 100 + 156;

                        return Qt.rgba(r / 255, g / 255, b / 255, 1);
                    }
                }

                Text {
                    id: textItem
                    objectName: "colorDelegate_text_" + textItem.text

                    visible: typeof value === "string"
                    text: typeof value === "string" ? value : ""
                    color: palette.textColor
                }
            }
        }

        Component {
            id: editDelegate
            WG1.WGTextBox {
                objectName: "editDelegate_" + text
                width: itemWidth
                text: itemValue !== undefined ? itemValue : ""
                textColor: palette.textColor
                onEditAccepted: {
                    // For some reason, this must be display and not value
                    // Even though listView.roles does not contain "display"
                    itemData.display = text;
                }
            }
        }
    }
}
