import QtQuick 2.4
import QtQuick.Controls 1.0
import QtQuick.Layouts 1.0

import WGControls 1.0 as WG1
import WGControls 2.0

WG1.WGPanel {
    id: testTreePanel

    property bool useModel: true
    property var sourceModel: useModel ? source : null
    property int topControlsHeight: 20

    title: "TreeModel Test 2.0"
    layoutHints: { 'test': 0.1 }
    color: palette.mainWindowColor

    // TODO NGT-2493 ScrollView steals keyboard focus
    Keys.forwardTo: [treeView1, treeView2]
    focus: true

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

    Column {
        id: testTrees

        property alias myHeaderDelegate: myHeaderDelegate
        property alias myFooterDelegate: myFooterDelegate
        property alias editDelegate: editDelegate

        anchors.top: switchModelButton.bottom
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.bottom: parent.bottom

        WGScrollView {
            anchors.left: parent.left
            anchors.right: parent.right
            height: parent.height / 2

            WGTreeView {
                id: treeView1
                model: sourceModel
                columnSpacing: 1
                columnSequence: [0,0,0]
                columnDelegates: [columnDelegate, columnDelegate, editDelegate]
                // show header text for column 0&1 and footer text only for column 0
                headerDelegate: myHeaderDelegate
                footerDelegates: [myFooterDelegate]// this equals [myFooterDelegate, null]
                roles: ["headerText", "footerText"]
                clamp: false
            }
        }

        WGScrollView {
            anchors.left: parent.left
            anchors.right: parent.right
            height: parent.height / 2

            WGTreeView {
                id: treeView2
                model: sourceModel
                columnSpacing: 1
                columnSequence: [0,0,0]
                columnDelegates: [columnDelegate, columnDelegate, editDelegate]
                // show header text for column 0&1 and footer text only for column 0
                headerDelegate: myHeaderDelegate
                footerDelegates: [myFooterDelegate]// this equals [myFooterDelegate, null]
                roles: ["headerText", "footerText"]
                clamp: true
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
    }
}
