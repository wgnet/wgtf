import QtQuick 2.3

/*!
 \brief WGHighlightFrame is a WG styled recctangle that can be used to highlight an object

Example:
\code{.js}
GridView {
    id: assetGrid
    visible: showIcons

    height: folderContentsRect.height
    width: folderContentsRect.width

    cellWidth: folderContentsRect.width / Math.floor(folderContentsRect.width / iconSize)
    cellHeight: iconSize + 36

    model: folderContentsModel
    delegate: folderContentsDelegate

    snapMode: GridView.SnapToRow

    highlight: WGHighlightFrame {
    }

    highlightMoveDuration: 0

    WGScrollBar {
         id: verticalScrollBar
         width: defaultSpacing.rightMargin
         anchors.top: assetGrid.top
         anchors.right: assetGrid.right
         anchors.bottom: assetGrid.bottom
         orientation: Qt.Vertical
         position: assetGrid.visibleArea.yPosition
         pageSize: assetGrid.visibleArea.heightRatio
         scrollFlickable: assetGrid
         visible: assetGrid.contentHeight > assetGrid.height
     }
}
\endcode
*/

Rectangle {
    objectName: "WGHighlightFrame"
    color: palette.highlightShade
    border.width: defaultSpacing.standardBorderSize
    border.color: palette.highlightColor
}
