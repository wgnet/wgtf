import QtQuick 2.3
import QtQuick.Controls 1.2
import QtQuick.Layouts 1.0
import WGControls 2.0
import WGControls.Views 2.0
import WGControls.Layouts 2.0

WGPanel {
    title: "Active Filters Control Test"
    layoutHints: { 'test': 0.1 }

    color: palette.mainWindowColor

    // TODO NGT-2493 ScrollView steals keyboard focus
    Keys.forwardTo: [sampleDataListView]
    focus: true

    WGColumnLayout {
        anchors.fill: parent

        Rectangle {
            id: activeFiltersRect
            Layout.fillWidth: true
            Layout.minimumHeight: defaultSpacing.minimumRowHeight + defaultSpacing.doubleBorderSize
            Layout.preferredHeight: childrenRect.height
            color: "transparent"

            WGActiveFilters {
                id: activeFilters
                objectName: "testActiveFilters"
                anchors {left: parent.left; top: parent.top; right: parent.right}
                height: childrenRect.height
                splitterChar: ","
            }
        }

        WGSeparator {
            id: separator
        }

        Rectangle {
            anchors.top: separator.bottom
            Layout.fillWidth: true
            Layout.fillHeight: true
            color: "transparent"

            WGTreeView {
                id: sampleDataListView
                model: sampleDataToFilter
                anchors.fill: parent
                clamp: true
                filterObject: QtObject {
                    property var filterText: activeFilters.filterString
                    property var filterTokens: []

                    onFilterTextChanged: {
                        filterTokens = [];
                        if (!filterText.empty) {
                            filterTokens = filterText.split( activeFilters.splitterChar );
                        }
                        sampleDataListView.view.proxyModel.invalidateFilter();
                    }

                    function filterAcceptsRow(item) {
                        var tokenCount = filterTokens.length;
                        if (tokenCount == 0) {
                            return true;
                        }

                        var itemValue = item.display;
                        if (itemValue === undefined) {
                            return true;
                        }

                        for (var i = 0; i < tokenCount; ++i) {
                            var filterToken = filterTokens[i];
                            if (itemValue.indexOf(filterToken) == -1) {
                                return false;
                            }
                        }

                        return true;
                    }
                }
            }
        }
    }
}
