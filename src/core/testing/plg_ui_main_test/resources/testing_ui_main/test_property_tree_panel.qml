import QtQuick 2.1
import QtQuick.Controls 1.0
import QtQuick.Layouts 1.3
import WGControls 2.0
import WGControls.Layouts 2.0
import WGControls.Views 2.0

WGPanel {
    id: propertyTreeTest20Panel
    objectName: "propertyTreeTest20Panel"
    color: palette.mainWindowColor
    title: "PropertyTree Test"
    layoutHints: { 'test': 0.1 }
    property int topControlsHeight: 20
	property var source: self
	
    // TODO NGT-2493 ScrollView steals keyboard focus
    Keys.forwardTo: [testTreeView]
    focus: true

    WGColumnLayout {
        id: mainColumnLayout
        objectName: "mainColumnLayout"
        anchors.fill: parent

        RowLayout {
            id: topControls
            objectName: "topControls"
            height: topControlsHeight
			
            WGPushButton {
                id: testButton
                objectName: "testButton"
                text: "Run Test Script"
                onClicked: {
                    runTestScript();
                }
            }
			
            WGPushButton {
                id: reloadButton
                objectName: "reloadButton"
                text: "Reload"
                onClicked: {
                    reloadEnvironment();
                }
            }

            WGPushButton {
                id: readOnlyButton
                objectName: "readOnlyButton"
                text: "Read Only"
                onClicked: {
                    toggleReadOnly();
					testTreeView.model = null;
					testTreeView.model = getTreeModel();
                }
            }

            Label {
                id: searchBoxLabel
                Layout.alignment: Qt.AlignVCenter
                text: "Search:"
            }

            WGTextBox {
                id: searchBox
                Layout.fillWidth: true

                onEditAccepted: {
                    mainColumnLayout.setFilter(text);
                    searchBox.focus = true;
                    deselect();
                }
            }
        }

        function setFilter(text)
        {
            var filterText = "(" + text.replace(/ /g, "|") + ")";
            filterObject.filter = new RegExp(filterText, "i");
            testTreeView.view.proxyModel.invalidateFilter();
        }

        property var filterObject: QtObject {
            property var filter: /.*/

            function filterAcceptsItem(item) {
                return item.column == 0 && filter.test(item.display);
            }
        }
		
		Connections {
			target: source
			onTreeVisibleChanged: {
				testTreeView.visible = treeVisible;
			}
		}
		
        WGScrollView {
            anchors.margins: 10
            Layout.fillHeight: true
            Layout.fillWidth: true		
			WGPropertyTreeView {
				id: testTreeView
				objectName: "testTreeView"
				Layout.fillHeight: true
				Layout.fillWidth: true
				model: getTreeModel()
				clamp: true
                visible: false
				filterObject: mainColumnLayout.filterObject
			}
		}
    }
}
