import QtQuick 2.5
import QtQuick.Controls 1.4

import WGControls 1.0

WGExpandingComponent {
    id: control
    objectName: typeof itemData.indexPath == "undefined" ? "thumbnail_component" : itemData.indexPath
    enabled: itemData.enabled && !itemData.readOnly

    collapsedHeight: Math.min( thumbnail.icon.sourceSize.width + defaultSpacing.doubleBorderSize, defaultSpacing.minimumRowHeight)
    collapsedWidth: Math.min( thumbnail.icon.sourceSize.width + defaultSpacing.doubleBorderSize, defaultSpacing.minimumRowHeight)

    expandedHeight: Math.min( thumbnail.icon.sourceSize.width + defaultSpacing.doubleBorderSize, defaultSpacing.minimumRowHeight * 5)
    expandedWidth: Math.min( thumbnail.icon.sourceSize.width + defaultSpacing.doubleBorderSize, defaultSpacing.minimumRowHeight * 5)

    mainComponent: WGThumbnail {
        id: thumbnail
        objectName: "thumbnail"
        source: typeof itemData.image == "undefined" ? itemData.thumbnail : itemData.image
        multipleValues: itemData.multipleValues
    }
}


