import QtQuick 2.1
import WGControls 2.0

Image {
    id: thumbnail
    objectName: "WGHistoryImage"
    WGComponent { type: "WGHistoryImage" }
    opacity: applied ? 1 : 0.35

    property string type: undefined
    property bool applied: false

    source: {
        if (typeof type == "undefined") {
            return "icons/16/unknown_16x16.png";
        }
        if (type == "Batch") {
            return "icons/16/add_16x16.png";
        }
        return "icons/16/edit_16x16.png";
    }
}

