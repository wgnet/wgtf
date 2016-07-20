import QtQuick 2.1
import QtQuick.Controls 1.0
import WGControls 1.0 as WG1
import WGControls 2.0

WG1.WGPanel {
    color: palette.mainWindowColor
    title: "Async Loader PropertyTree Test"
    layoutHints: { 'default': 0.1 }

    WGPropertyTreeView {
        id: testTreeView
        anchors.fill: parent
        model: source
    }
}
