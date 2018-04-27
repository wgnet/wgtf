import QtQuick 2.4
import QtQuick.Controls 1.2
import QtQuick.Layouts 1.0

import WGControls 2.0
import WGControls.Views 2.0

WGPanel {
    id: shortcutPanel
    WGComponent { type: "shortcut_dialog" }
    property var userModel: shortcutModel
    property var proxyModel: WGColumnLayoutProxy {
        sourceModel: userModel
        columnSequence: [0,0]
    }
    title: "Actions Shortcut Configuration"
    layoutHints: { 'default': 0.1 }
    color: palette.mainWindowColor
    property int margin: 8
    width: 400
    height: 600

    property bool hideEmpty: false
    readonly property bool filterValue: hideEmpty

    function setFilter(text)
    {
        listView.currentIndex = null
        listView.positionViewAtBeginning()
        var filterText = "(" + text.replace(/ /g, "|") + ")";
        filterObject.filter = new RegExp(filterText, "i");
        listView.view.proxyModel.invalidateFilter();

        if (text == "")
        {
            listView.view.proxyModel.sort(0, Qt.AscendingOrder);
        }
    }

    property var filterObject: QtObject {
        property var filter: /.*/

        function filterAcceptsItem(item) {
            if (filterValue)
            {
                return filter.test(item.value);
            }
            else
            {
                return filter.test(item.key);
            }
        }
    }

    Component.onCompleted: {
        viewWidget.minimumWidth = 400
        viewWidget.minimumHeight = 300
    }

    ColumnLayout {
        id: mainLayout
        anchors.fill: parent
        anchors.margins: margin
        RowLayout {
            id: searchWidget
            Layout.fillWidth: true
            Layout.preferredHeight: defaultSpacing.minimumRowHeight + defaultSpacing.standardMargin
            height: 26

            WGLabel {
                id: searchBoxLabel
                text: "Search:"
                enabled: !hideEmpty
                Layout.leftMargin: defaultSpacing.standardMargin
            }

            WGTextBox {
                id: searchBox
                onTextChanged: setFilter(text)
                Layout.fillWidth: true
                Layout.rightMargin: defaultSpacing.standardMargin
                enabled: !hideEmpty

                WGToolButton {
                    id: clearCurrentFilterButton
                    objectName: "clearFilterButton"
                    iconSource: "../WGControls/icons/close_sml_16x16.png"
                    anchors.right: parent.right
                    visible: searchBox.text != ""

                    tooltip: "Clear"

                    onClicked: {
                        searchBox.text = ""
                    }
                }
            }
            WGPushButton {
                text: "Hide Empty"
                checkable: true
                checked: hideEmpty
                onClicked: {
                    hideEmpty = !hideEmpty
                    if(hideEmpty)
                    {
                        setFilter("^(?!\s*$).+")
                    }
                    else
                    {
                        setFilter("")
                    }
                }
            }
        }
        Item {
            Layout.fillHeight: true
            Layout.fillWidth: true
            WGScrollView {
                id: scrollView
                height: parent.height
                width: parent.width

                WGListView {
                    id: listView
                    columnSpacing: 2
                    columnDelegates: [keyDelegate, valueDelegate]
                    headerDelegates: [headerDelegate, headerDelegate]
                    model: proxyModel
                    currentIndex: 0
                    clamp: true

                    columnRoles: ["key", "value"]
                    filterObject: shortcutPanel.filterObject

                    Component {
                        id: keyDelegate
                        Text {
                            text: itemData != null ? itemData.key : ""
                            color: palette.TextColor
                            width: parent != null ? parent.width : columnWidth
                            height: defaultSpacing.minimumRowHeight
                        }
                    }

                    Component {
                        id: valueDelegate
                        WGTextBox {
                            text: itemData != null ? itemData.value : ""
                            width: parent != null ? parent.width : columnWidth
                            height: defaultSpacing.minimumRowHeight
                            onEditAccepted: {
                                itemData.value = text;
                            }
                        }
                    }

                    Component {
                        id: headerDelegate
                            Item {
                            implicitWidth: shortcutPanel.width / 2
                            height: defaultSpacing.minimumRowHeight
                            WGLabel {
                                text: valid ? headerData.headerText : (headerData != null && headerData.index != null ? (headerData.index == 0 ? "Action" : "Shortcut") : "")
                                width: parent !== null ? parent.width : 0
                                anchors.left: parent.left
                                anchors.leftMargin: defaultSpacing.rowSpacing
                                anchors.verticalCenter: parent.verticalCenter
                                font.bold: true

                                property bool valid: headerData !== null &&
                                    typeof headerData !== "undefined" &&
                                    typeof headerData.headerText !== "undefined"
                            }
                        }
                    }

                    Component.onCompleted: {
                        listView.view.proxyModel.sort(0, Qt.AscendingOrder);
                    }
                }
            }
        }
        RowLayout {
            id: buttons
            Layout.fillWidth: true
            Layout.preferredHeight: defaultSpacing.minimumRowHeight
            WGPushButton {
                id: reset
                text: "Reset to Default"
                onClicked: {
                    hideEmpty = false
                    resetToDefault();
                }
            }

            Item {
                Layout.fillWidth: true
                Layout.preferredHeight: defaultSpacing.minimumRowHeight
            }

            WGPushButton {
                id: ok
                text: "Ok"
                Layout.preferredWidth: cancel.width
                onClicked: {
                    applyChanges();
                    viewWidget.close();
                }
            }

            WGPushButton {
                id: cancel
                text: "Cancel"
                onClicked: {
                    viewWidget.close();
                }
            }
        }
    }
}
