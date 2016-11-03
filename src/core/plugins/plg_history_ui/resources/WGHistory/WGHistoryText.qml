import QtQuick 2.1
import WGControls 2.0

Text {
    id: text
    objectName: "WGHistoryText"
    WGComponent { type: "WGHistoryText" }

    // -- Begin Interface
    text: "Unknown"
    property bool isCurrentItem: false
    property bool isApplied: false
    // -- End Interface

    clip: false
    color: isCurrentItem ?
        palette.textColor :
        palette.neutralTextColor
    font.bold: isCurrentItem
    font.italic: !isApplied
    opacity: isApplied ? 1 : 0.35
}

