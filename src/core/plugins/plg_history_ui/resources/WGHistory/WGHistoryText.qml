import QtQuick 2.1

Text {
    id: text
    objectName: "WGHistoryText"

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

