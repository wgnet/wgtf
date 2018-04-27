import QtQuick 2.5
import QtQuick.Controls 1.4
import QtQuick.Layouts 1.3

import WGControls 2.0


Item {
    id: control
    objectName: typeof itemData.indexPath == "undefined" ? "thumbnail_component" : itemData.indexPath
    enabled: itemData.enabled && !itemData.readOnly && (typeof readOnlyComponent == "undefined" || !readOnlyComponent)

    implicitHeight: defaultSpacing.minimumRowHeight
    implicitWidth: defaultSpacing.minimumRowHeight

    WGThumbnail {
        id: thumbnail
        objectName: "thumbnail"
        source: typeof itemData.image == "undefined" ? itemData.thumbnail : itemData.image
        multipleValues: itemData.multipleValues

        height: defaultSpacing.minimumRowHeight
        width: defaultSpacing.minimumRowHeight

        MouseArea {
            anchors.fill: parent
            hoverEnabled: true
            cursorShape: Qt.PointingHandCursor

            WGPopup {
                openPolicy: openOnHover
                padding: defaultSpacing.doubleBorderSize

                ColumnLayout {
                    WGThumbnail {
                        id: preview
                        source: typeof itemData.image == "undefined" ? itemData.thumbnail : itemData.image
                        Layout.preferredHeight: sourceSize.height + defaultSpacing.doubleBorderSize
                        Layout.preferredWidth: sourceSize.width + defaultSpacing.doubleBorderSize
                        Layout.maximumHeight: sourceSize.height > sourceSize.width ? defaultSpacing.minimumRowHeight * 15 : Math.max(paintedHeight + defaultSpacing.doubleBorderSize, defaultSpacing.minimumRowHeight * 4 + defaultSpacing.doubleBorderSize)
                        Layout.maximumWidth: sourceSize.height > sourceSize.width ? Math.max(paintedWidth + defaultSpacing.doubleBorderSize, defaultSpacing.minimumRowHeight * 4 + defaultSpacing.doubleBorderSize) : defaultSpacing.minimumRowHeight * 15
                        Layout.minimumHeight: defaultSpacing.minimumRowHeight * 4 + defaultSpacing.doubleBorderSize
                        Layout.minimumWidth: defaultSpacing.minimumRowHeight * 4 + defaultSpacing.doubleBorderSize
                    }
                    Item {
                        Layout.fillWidth: true
                        Layout.maximumWidth: preview.width
                        Layout.minimumHeight: imageInfo.font.pixelSize + defaultSpacing.doubleMargin
                        Layout.maximumHeight: imageInfo.font.pixelSize * 2 + defaultSpacing.doubleMargin
                        Layout.preferredHeight: childrenRect.height
                        clip: true
                        WGMultiLineText {
                            id: imageInfo
                            anchors.left: parent.left
                            anchors.right: parent.right
                            anchors.top: parent.top
                            color: palette.textColor
                            horizontalAlignment: Text.AlignHCenter
                            font.pixelSize: 10
                            font.italic: true

                            wrapMode: Text.NoWrap
                            elide: Text.ElideNone

                            height: lineWrap ? imageInfo.font.pixelSize * 2 + defaultSpacing.doubleMargin :
                                               imageInfo.font.pixelSize + defaultSpacing.doubleMargin

                            property bool lineWrap: false

                            property string path: preview.source.toString()
                            property string imageName: path.split('\\').pop().split('/').pop();

                            text: imageName + ", " + preview.sourceSize.width + "x" + preview.sourceSize.height + "px"

                            onVisibleChanged: {
                                if (visible && contentWidth > preview.width)
                                {
                                    imageInfo.lineWrap = true
                                    imageInfo.wrapMode = Text.Wrap
                                    imageInfo.elide = Text.ElideRight
                                }
                            }
                        }
                    }
                }
            }
        }
    }
}


