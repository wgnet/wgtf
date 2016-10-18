import QtQuick 2.5
import QtQuick.Controls 1.4
import QtQuick.Layouts 1.3
import WGControls 1.0
import WGControls.Layouts 1.0

/*!
 \ingroup wgcontrols
 \brief A control used to represent breadcrumb navigation of a tree view

Example:
\code{.js}
WGBreadcrumbs {
    id: breadcrumbsFrame
    dataModel: breadcrumbsModel
}
\endcode
*/

Rectangle {
    id: rootFrame
    objectName: "WGBreadCrumbs"
    WGComponent { type: "WGBreadcrumbs20" }

    // Public properties
    /*! This property holds the dataModel containing all breadcrumbs data */
    property var dataModel

	onDataModelChanged: {
		breadcrumbRepeater.model = dataModel.crumbs;
	}

    /*! These properties holds references to various components used to determine the size of the frame */
    property var breadcrumbRepeater_: breadcrumbRepeater
    property var breadcrumbRowLayout_: breadcrumbRowLayout

    // Private properties
    property bool __showBreadcrumbs: true

    // Signals
    /*! This signal is sent when a top level breadcrumb is clicked */
    signal breadcrumbClicked(var index)

    /*! This signal is sent when a subitem is clicked from the child folder menu */
    signal breadcrumbChildClicked(var index, var childIndex)

    /*! This signal is sent when the user enters a path manually and hits "enter" to confirm navigation */
    signal breadcrumbPathEntered(var path)

    // Object properties
    Layout.fillHeight: false
    Layout.preferredHeight: defaultSpacing.minimumRowHeight
    Layout.fillWidth: true
    color: "transparent"

    // Mouse area over the path text box
    MouseArea {
        objectName: "pathSelect"
        anchors.fill: parent
        enabled: rootFrame.__showBreadcrumbs
        hoverEnabled: true

        cursorShape: Qt.IBeamCursor

        onClicked: {
            rootFrame.__showBreadcrumbs = false
            pathTextBox.forceActiveFocus()
        }
    }

    // Text box to store the full, raw path of the breadcrumbs. May be used to manually navigate to
    // a specific path in the tree.
    //TODO MUCH LATER: Auto complete.
    WGTextBox {
        id: pathTextBox
        objectName: "pathTextBox"
        anchors.fill: parent
        visible: !rootFrame.__showBreadcrumbs

        text: rootFrame.dataModel.path

        onEditingFinished: {
            rootFrame.__showBreadcrumbs = true
        }

        onAccepted: {
            breadcrumbPathEntered(text)
        }
    }

    // Main layout of the breadcrumbs control.
    RowLayout {
        id: breadcrumbLayout
        anchors.fill: parent
        spacing: 0

        visible: rootFrame.__showBreadcrumbs

        Component {
            id: breadcrumbDelegate

            RowLayout {
                id: breadcrumbRowLayout
                objectName: typeof value.displayValue != "undefined" ? "breadcrumbRowLayout_" + value.displayValue
                                                                     : "breadcrumbRowLayout"

                property var breadcrumbIndex_

                Layout.fillWidth: false
                spacing: 1

				property var subItemsListModel: value.subItems

                WGLabel {
                    id: breadcrumbLabel
                    objectName: typeof value.displayValue != "undefined" ? "breadcrumbLabel_" + value.displayValue
                                                                         : "breadcrumbLabel"

                    Layout.fillWidth: true
                    Layout.preferredHeight: defaultSpacing.minimumRowHeight
                    Layout.alignment: Qt.AlignLeft | Qt.AlignVCenter

                    elide: Text.ElideRight

                    text: value.displayValue

                    font.bold: true
                    font.pointSize: 11

                    verticalAlignment: Text.AlignVCenter

                    color: breadcrumbMouseArea.containsMouse ? palette.textColor : palette.neutralTextColor;

                    Component.onCompleted: {
                        breadcrumbRowLayout.breadcrumbIndex_ = index;
                    }

                    MouseArea {
                        id: breadcrumbMouseArea
                        objectName: typeof value.displayValue != "undefined" ? "breadcrumbMouseArea_" + value.displayValue
                                                                             : "breadcrumbMouseArea"
                        anchors.fill: parent
                        cursorShape: Qt.PointingHandCursor
                        hoverEnabled: true
                        onPressed: {
                            breadcrumbClicked(index );
                        }
                    }
                }

                WGToolButton {
                    objectName: typeof value.displayValue != "undefined" ? "folderdivide_" + value.displayValue
                                                                          : "folderdivide"
                    visible: index < breadcrumbRepeater.count - 1

                    Layout.preferredWidth: 16
                    Layout.preferredHeight: defaultSpacing.minimumRowHeight
                    showMenuIndicator: false

                    iconSource: "icons/arrow_right_small_16x16.png"

                    menu: WGMenu {
                        id: siblingFolderMenu

                        Instantiator {
                            model: subItemsListModel

                            delegate: MenuItem {
                                text: value.displayValue
                                onTriggered: {
                                    breadcrumbChildClicked(breadcrumbRowLayout.breadcrumbIndex_, index);
                                }
                            }

                            onObjectAdded: siblingFolderMenu.insertItem(index, object)
                            onObjectRemoved: siblingFolderMenu.removeItem(object)
                        }
                    }
                }
            }
        }

        WGExpandingRowLayout {
            id: breadcrumbRowLayout
            objectName: "breadcrumbRowLayout"
            Layout.fillWidth: true
            Layout.preferredHeight: defaultSpacing.minimumRowHeight + defaultSpacing.doubleBorderSize

            onWidthChanged: checkAssetBrowserWidth()

            spacing: 1

            Repeater {
                id: breadcrumbRepeater
                model: rootFrame.dataModel
                delegate: breadcrumbDelegate

                onItemAdded: {
                    pathTextBox.text = rootFrame.dataModel.path;
                }

                onItemRemoved: {
                    pathTextBox.text = rootFrame.dataModel.path;
                }
            }
        }
    }
} // rootFrame
