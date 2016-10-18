import QtQuick 2.4
import QtQuick.Controls 1.0
import QtQuick.Layouts 1.0

import WGControls 2.0
import WGControls.Views 2.0

WGPanel {
    id: testTreePanel

	ListModel {
		id: qmlModel
		ListElement { 
			display: "This"
		}
        ListElement {
			display: "Is"
		}
        ListElement {
			display: "A"
		}
        ListElement {
			display: "QML"
		}
        ListElement {
			display: "Model"
		}
	}

    property bool useCppModel: true
    property var sourceModel: WGColumnLayoutProxy {
		sourceModel: useCppModel ? source : qmlModel
		columnSequence: [0,0,0]
	}
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
        text: useCppModel ? "Use QML Model" : "Use C++ Model"

        onClicked: {
            useCppModel = !useCppModel;
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
                columnDelegates: [columnDelegate, columnDelegate, editDelegate]
                // show header text for column 0&1 and footer text only for column 0
                headerDelegate: myHeaderDelegate
                footerDelegates: [myFooterDelegate]// this equals [myFooterDelegate, null]
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
                columnDelegates: [columnDelegate, columnDelegate, editDelegate]
                // show header text for column 0&1 and footer text only for column 0
                headerDelegate: myHeaderDelegate
                footerDelegates: [myFooterDelegate]// this equals [myFooterDelegate, null]
                clamp: true
            }
        }

        Component {
            id: editDelegate
            WGTextBox {
                objectName: "editDelegate_" + text
                width: parent.width
                implicitWidth: contentWidth + defaultSpacing.standardMargin
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

            WGTextBox {
                id: textBoxHeader
                textColor: palette.textColor
                text: valid ? headerData.headerText : ""
                width: parent.width
                height: 24

                property bool valid: headerData !== null &&
                    typeof headerData !== "undefined" &&
                    typeof headerData.headerText !== "undefined"

                onEditAccepted: {
                    if (valid) {
                        headerData.headerText = text;
                    }
                }
            }
        }

        Component {
            id: myFooterDelegate

            WGTextBox {
                id: textBoxFooter
                textColor: palette.textColor
                text: valid ? headerData.footerText : ""
                width: parent.width
                height: 24

                property bool valid: headerData !== null &&
                    typeof headerData !== "undefined" &&
                    typeof headerData.footerText !== "undefined"

                onEditAccepted: {
                    if (valid) {
                        headerData.footerText = text;
                    }
                }
            }
        }
    }
}
