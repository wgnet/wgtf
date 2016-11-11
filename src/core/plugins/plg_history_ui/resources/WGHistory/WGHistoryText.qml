import QtQuick 2.1
import WGControls 2.0

Text {
    id: text
    objectName: "WGHistoryText"
    WGComponent { type: "WGHistoryText" }
    text: "Unknown"
    clip: false
    color: currentItem ? palette.textColor : palette.neutralTextColor
    font.bold: currentItem
    font.italic: !applied
    opacity: applied ? 1 : 0.35

    property bool currentItem: false
    property bool applied: false
}

