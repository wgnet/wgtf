import QtQuick 2.5
import QtQuick.Controls 1.4

import WGControls 1.0

Item {
    id: control
    objectName: typeof itemData.indexPath == "undefined" ? "thumbnail_component" : itemData.indexPath
    enabled: itemData.enabled && !itemData.readOnly

    implicitWidth: thumbnail.width
    implicitHeight: thumbnail.height

    WGThumbnail {
        id: thumbnail
        objectName: "thumbnail"
        source: typeof itemData.image == "undefined" ? itemData.thumbnail : itemData.image
        anchors.left: parent.left
        multipleValues: itemData.multipleValues
    }
}


