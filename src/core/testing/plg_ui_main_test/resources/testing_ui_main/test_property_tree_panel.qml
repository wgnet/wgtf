import QtQuick 2.1
import QtQuick.Controls 1.0
import WGControls 2.0

Rectangle {
    color: palette.mainWindowColor
    property var title: "PropertyTree Test"
    property var layoutHints: { 'test': 0.1 }

    WGPropertyTreeView {
        id: testTreeView
        anchors.fill: parent
        model: source
    }
}
