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
    property string title: "PropertyTree Test"
    property var layoutHints: { 'test': 0.1 }
    property int topControlsHeight: 20

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

            Label {
                id: searchBoxLabel
                Layout.alignment: Qt.AlignVCenter
                text: "Search:"
            }

            WGTextBox {
                id: searchBox
                Layout.fillWidth: true
                onTextChanged: mainColumnLayout.setFilter(text);
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
                var value = (item.column == 0 ? item.display : item.value);
                return filter.test(value);
            }
        }

        WGPropertyTreeView {
            id: testTreeView
            objectName: "testTreeView"
            Layout.fillHeight: true
            Layout.fillWidth: true
            model: treeModel
            clamp: true
            filterObject: mainColumnLayout.filterObject
        }
    }
}
