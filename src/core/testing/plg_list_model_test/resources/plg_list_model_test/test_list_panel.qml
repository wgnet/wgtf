import QtQuick 2.4
import QtQuick.Controls 1.2
import QtQuick.Layouts 1.0
import QtQml.Models 2.2
import WGControls 2.0
import WGControls.Views 2.0

WGPanel {
    id: testListPanel

    WGListModel {
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
        columnSequence: [0,1,0]
    }

    property var sourceModel: useCppModel ? columnProxyModel : qmlModel
    property bool useCppModel: true
    property bool sortAsc: true
    property int topControlsHeight: defaultSpacing.minimumRowHeight

    title: "ListModel Test"
    layoutHints: { 'test': 0.1 }
    color: palette.mainWindowColor

    Item {
        id: mainFrame
        anchors.fill: parent
        anchors.leftMargin: defaultSpacing.leftMargin
        anchors.topMargin: defaultSpacing.topBottomMargin

        ColumnLayout {
            id: testLists

            property alias myHeaderDelegate: myHeaderDelegate
            property alias myFooterDelegate: myFooterDelegate
            property alias editDelegate: editDelegate
            property alias colorDelegate: colorDelegate

            anchors.fill: parent

            Button {
                id: switchModelButton
                Layout.preferredWidth: 150
                Layout.preferredHeight: topControlsHeight
                text: useCppModel ? "Use QML Model" : "Use C++ Model"

                onClicked: {
                    useCppModel = !useCppModel;
                }
            }

            Item {
                Layout.fillWidth: true
                Layout.fillHeight: true

                WGScrollView {
                    anchors.top: parent.top
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
                        columnDelegates: useCppModel ?
                            [columnDelegate, colorDelegate, editDelegate] : [editDelegate]
                        headerDelegate: myHeaderDelegate
                        footerDelegate: myFooterDelegate
                        model: sourceModel
                        clamp: false
                        currentIndex: 0
                        minimumColumnWidth: 5
                    }
                }

                WGScrollView {
                    anchors.bottom: parent.bottom
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
                        columnDelegates: useCppModel ?
                            [columnDelegate, colorDelegate, editDelegate] : [editDelegate]
                        headerDelegate: myHeaderDelegate
                        footerDelegate: myFooterDelegate
                        model: sourceModel
                        clamp: true
                        currentIndex: 0
                        minimumColumnWidth: 10
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

            Component {
                id: colorDelegate

                Item {
                    width: parent.width
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
                        verticalAlignment: Text.AlignVCenter
                        anchors.verticalCenter: parent.verticalCenter
                    }
                }
            }

            Component {
                id: editDelegate
                WGTextBox {
                    objectName: "editDelegate_" + text
                    width: parent !== null ? parent.width : 0
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
}
