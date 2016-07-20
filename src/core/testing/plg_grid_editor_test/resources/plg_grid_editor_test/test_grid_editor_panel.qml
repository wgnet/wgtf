import QtQuick 2.4
import QtQuick.Controls 1.2
import QtQuick.Layouts 1.0
import WGControls 1.0 as WG1
import WGControls 2.0

WG1.WGPanel {
    id: testGridPanel

    property var sourceModel: source

    title: "Grid Editor Test"
    layoutHints: { 'test': 0.1 }
    color: palette.mainWindowColor

    // TODO NGT-2493 ScrollView steals keyboard focus
    Keys.forwardTo: gridEditor
    focus: true

    WGGridEditor {
        id: gridEditor
        gridModel: sourceModel
        anchors.fill: parent
    }
}
