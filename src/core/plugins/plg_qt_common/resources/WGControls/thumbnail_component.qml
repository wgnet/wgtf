import QtQuick 2.3
import QtQuick.Controls 1.2

Item {
    objectName:  itemData != null ? itemData.indexPath : "thumbnail_component"
    WGThumbnail {
        objectName: "thumbnail"
        source: itemData.thumbnail
        anchors.left: parent.left
        height: defaultSpacing.minimumRowHeight
        width: defaultSpacing.minimumRowHeight
    }
}


