import QtQuick 2.6
import QtQuick.Controls 1.4
import QtQuick.Layouts 1.3

import WGControls 2.0
import WGControls.Views 2.0

WGPanel
{
    id: propertyPanel
    objectName: "WGPropertyPanel"
    WGComponent { type: "WGPropertyPanel" }
    color: palette.darkColor
    property alias expandToDepth: propertyTreeView.expandToDepth
    property bool readOnly: typeof self.object != 'undefined' && typeof self.object.isReadOnly != 'undefined' && self.object.isReadOnly
    property bool showHelpPanel: false
    property bool showObjectBar: false

    property alias headerBar: viewHeaderBar

    ColumnLayout {
        anchors.fill: parent
        spacing: 0

        WGHeaderToolbar {
            id: viewHeaderBar
            Layout.preferredHeight: defaultSpacing.minimumRowHeight
            Layout.fillWidth: true

            visible: showObjectBar

            headerTitle: "Nothing Selected"
            headerIcon: "icons/dialogue_off_16x16.png"
            headerIconOverlay: "../icons/locked_red_8x8.png"
            headerToggled: false //readOnly **** TODO Uncomment when readOnly correctly linked
            enableHeaderToggle: false //true **** TODO Uncomment when readOnly correctly linked
            displayheaderMenu : false

            headerMenu: WGMenu {
                title: "Object Options"

                MenuItem {
                    text: "Open Object..."
                    enabled: false //**** ENABLE IF SELECTED OBJECT CAN BE OPENED IN A NEW ENVIRONMENT
                    onTriggered: {
                        //**** OPEN THE SELECTED OBJECT IN A NEW ENVIRONMENT IF APPROPRIATE
                    }
                }
                //**** TODO Uncomment when readOnly correctly linked
//                MenuItem {
//                    text: "Lock"
//                    checkable: true
//                    checked: readOnly
//                    enabled: true //**** LINK IF IT'S POSSIBLE TO LOCK OR UNLOCK THIS OBJECT
//                    onTriggered: {
//                        readOnly = !readOnly //**** LINK IF IT'S POSSIBLE TO LOCK OR UNLOCK THIS OBJECT
//                    }
//                }
            }

            //**** TODO Uncomment when readOnly correctly linked
//            onHeaderIconClicked: {
//                readOnly = !readOnly //**** LINK IF IT'S POSSIBLE TO LOCK OR UNLOCK THIS OBJECT
//            }
        }

        WGScrollView {
            Layout.fillHeight: true
            Layout.fillWidth: true
            WGPropertyTreeView
            {
                id: propertyTreeView
                model: self.model
                clamp: true
                readOnly: propertyPanel.readOnly
                property var perforceExtension: createExtension("PerforceExtension")
                extensions: [ componentExtension, perforceExtension ]

                Connections {
                    target: self.model
                    ignoreUnknownSignals: true

                    property int position: 0

                    onModelAboutToBeReset: {
                        position = propertyTreeView.contentY;
                    }

                    onModelReset: {
                        var visibleHeight = propertyTreeView.contentHeight - propertyPanel.height;
                        position += Math.min(visibleHeight - position, 0);
                        position = Math.max(position, 0);
                        propertyTreeView.contentY = position;
                        position = 0;
                        var metaData = typeof self.object != 'undefined' && typeof self.object.getMetaObject != 'undefined' ? self.object.getMetaObject("", "DisplayName") : null;
                        var headerString = "";
                        var objectSelected = typeof self.object != 'undefined' && typeof self.object.name != "undefined";
                        if (metaData == null && objectSelected == false)
                        {
                            headerString = "Nothing Selected"
                            headerBar.displayheaderMenu = false
                        }
                        else
                        {
                            if (objectSelected != false)
                            {
                                headerString += self.object.name
                            }
                            if (metaData != null)
                            {
                                headerString += " (" + metaData.displayName + ")"
                            }
                            headerBar.displayheaderMenu = true
                        }
                        headerBar.headerTitle = headerString

                        //**** TODO Update headerIcon
                    }
                }

                onCurrentIndexChanged: {
                    var item = view.extendedModel.indexToItem(currentIndex);

                    if (item != null)
                    {
                        currentPropertyChanged(item.display);
                    }
                }
            }
        }

        WGFooterPreviewPane {
            Layout.preferredHeight: 60
            Layout.fillWidth: true
            visible: showHelpPanel

            //**** REPLACE WITH LINKS TO HELP TEXT DESCRIPTION PROPERTIES AND THE TITLE
            title: "Replace with selected item name"
            description: "Replace with selected item description"
        }
    }
}
