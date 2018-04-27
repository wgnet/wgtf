import QtQuick 2.4
import QtQuick.Controls 1.2
import QtQuick.Layouts 1.0

import WGControls 2.0
import WGControls.Views 2.0

WGPanel {
    id: testTablePanel

    WGTableModel {
        id: qmlModel

        header: WGModelRow {
            WGModelData {
                roles: {"headerText": "h1", "footerText": "f1"}
            }

            WGModelData {
                roles: {"headerText": "h2", "footerText": "f2"}
            }

            WGModelData {
                roles: {"headerText": "h3", "footerText": "f3"}
            }

            WGModelData {
                roles: {"headerText": "h4", "footerText": "f4"}
            }

            WGModelData {
                roles: {"headerText": "h5", "footerText": "f5"}
            }
        }

        WGModelRow {
            WGModelData {
                roles: {"display": "This"}
            }

            WGModelData {
                roles: {"display": "is"}
            }

            WGModelData {
                roles: {"display": "a"}
            }

            WGModelData {
                roles: {"display": "QML"}
            }

            WGModelData {
                roles: {"display": "model"}
            }
        }

        WGModelRow {
            WGModelData {
                roles: {"display": "This"}
            }

            WGModelData {
                roles: {"display": "is"}
            }

            WGModelData {
                roles: {"display": "a"}
            }

            WGModelData {
                roles: {"display": "QML"}
            }

            WGModelData {
                roles: {"display": "model"}
            }
        }
    }

    property var columnProxyModel: WGColumnLayoutProxy {
        sourceModel: source
        columnSequence: [0,1,2,3,0]
    }

    property var sourceModel: useCppModel ? columnProxyModel : qmlModel
    property bool useCppModel: true
    property int topControlsHeight: 20

    title: "TableModel Test"
    layoutHints: { 'test': 0.1 }
    color: palette.mainWindowColor

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
        id: testTables

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

            WGTableView {
                id: tableView1
                columnSpacing: 1
                columnDelegates: [columnDelegate, columnDelegate, columnDelegate, columnDelegate, editDelegate]
                headerDelegate: myHeaderDelegate
                footerDelegate: myFooterDelegate
                model: sourceModel
                clamp: false
                currentIndex: 0
            }
        }
        WGScrollView {
            anchors.left: parent.left
            anchors.right: parent.right
            height: parent.height / 2

            WGTableView {
                id: tableView2
                columnSpacing: 1
                columnDelegates: [columnDelegate, columnDelegate, columnDelegate, columnDelegate, editDelegate]
                headerDelegate: myHeaderDelegate
                footerDelegate: myFooterDelegate
                model: sourceModel
                clamp: true
                currentIndex: 0
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
                width: parent !== null ? parent.width : 0
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
                width: parent !== null ? parent.width : 0
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
