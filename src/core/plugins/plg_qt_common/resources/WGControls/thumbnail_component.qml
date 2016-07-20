import QtQuick 2.3
import QtQuick.Controls 1.2

Item {
    id: control
    objectName: typeof itemData.indexPath == "undefined" ? "thumbnail_component" : itemData.indexPath
    enabled: itemData.enabled && !itemData.readOnly

    WGThumbnail {
        objectName: "thumbnail"
        source: itemData.thumbnail
        anchors.left: parent.left
        height: defaultSpacing.minimumRowHeight
        width: defaultSpacing.minimumRowHeight
        multipleValues: itemData.multipleValues
    }
}


