import QtQuick 2.3
import QtQuick.Controls 1.2

/*!
    \brief a styleable frame that can be resized in 8 directions (horizontal, vertical and diagonal)

Example:
\code{.js}
WGResizeableFrame {

    frameStyle: Rectangle {
        color: "white"
    }

    edgeStyle: Rectangle {
        color: {
            if (edge == Qt.TopEdge)
            {
                "green"
            }
            else if (edge == Qt.RightEdge)
            {
                "red"
            }
            else if (edge == Qt.BottomEdge)
            {
                "blue"
            }
            else
            {
                "yellow"
            }
        }
    }
}
\endcode
*/

Item {
    id: baseFrame
    objectName: "WGResizeableFrame"

    /*! The visual style of the frame. Can be any Item/Rectangle based QML object. Default is WGButtonFrame */
    property Component frameStyle: WGButtonFrame {}

    /*! The visual style of the corner handles. Can be any Item/Rectangle based QML object.

    The corner can be determined by the 'corner' property. eg. corner == Qt.TopRightCorner

    The default is no object (invisible handle) */
    property Component cornerStyle

    /*! The visual style of the edge handles. Can be any Item/Rectangle based QML object.

    The edge can be determined by the 'edge' property. eg. corner == Qt.Top

    The default is no object (invisible handle) */
    property Component edgeStyle

    /*! The visual frame object */
    property QtObject frameBorder: groupBox

    /*! Enables/Disabled resizing the frame. Default is false */
    property bool locked: false

    /*! The minimum height of the frame. (while still allowing enough height for the handles) Default is 50 pixels. */
    property int minimumHeight: 50

    /*! The minimum width of the frame. (while still allowing enough width for the handles) Default is 50 pixels. */
    property int minimumWidth: 50

    /*! An array determining which sides can be resized [top, right, bottom, left]. By default all are true */
    property var orthogonalEnabled: [true, true, true, true]

    /*! An array determining which corners can be resized [top L, top R, bottom R, bottom L]. By default all are true */
    property var diagonalEnabled: [true, true, true, true]

    /*! Internal */
    property int __initialX: 0

    /*! Internal */
    property int __initialY: 0

    /*! Internal */
    property int __initialWidth: 0

    /*! Internal */
    property int __initialHeight: 0

    Loader {
        id: groupBox
        sourceComponent: frameStyle
        asynchronous: true

        anchors.left: tLeftCornerDrag.left
        anchors.right: tRightCornerDrag.right
        anchors.bottom: bLeftCornerDrag.bottom
        anchors.top: tLeftCornerDrag.top
        z: -1
    }

    MouseArea {
        id: tLeftCornerDrag
        anchors.left: parent.left
        anchors.top: parent.top

        drag.target: tLeftCornerDrag
        drag.axis: Drag.XAndYAxis
        drag.maximumX: tRightCornerDrag.x - minimumWidth
        drag.maximumY: bLeftCornerDrag.y - minimumHeight

        enabled: !locked && diagonalEnabled[0] == true

        cursorShape: enabled ? Qt.SizeFDiagCursor : Qt.ArrowCursor

        width: defaultSpacing.doubleMargin
        height: defaultSpacing.doubleMargin

        onPressed: {
            __initialX = x
            __initialY = y
            __initialWidth = groupItem.width
            __initialHeight = groupItem.height
            anchors.left = undefined
            anchors.top = undefined
        }

        onReleased: {
            groupItem.width = __initialWidth - (tLeftCornerDrag.x - __initialX)
            groupItem.height = __initialHeight - (tLeftCornerDrag.y - __initialY)
            groupItem.x = groupItem.x + (tLeftCornerDrag.x - __initialX)
            groupItem.y = groupItem.y + (tLeftCornerDrag.y - __initialY)
            anchors.left = parent.left
            anchors.top = parent.top
        }

        Loader {
            enabled: cornerStyle && diagonalEnabled[0] == true
            sourceComponent: cornerStyle && diagonalEnabled[0] == true ? cornerStyle : null
            asynchronous: true
            anchors.fill: parent
            property int corner: Qt.TopLeftCorner
        }
    }

    MouseArea {
        id: tRightCornerDrag
        anchors.right: parent.right
        anchors.top: parent.top

        drag.target: tRightCornerDrag
        drag.axis: Drag.XAndYAxis
        drag.minimumX: tLeftCornerDrag.x + tLeftCornerDrag.width + minimumWidth
        drag.maximumY: bRightCornerDrag.y - minimumHeight

        enabled: !locked && diagonalEnabled[1] == true

        cursorShape: enabled ? Qt.SizeBDiagCursor : Qt.ArrowCursor

        width: defaultSpacing.doubleMargin
        height: defaultSpacing.doubleMargin

        onPressed: {
            __initialX = x
            __initialY = y
            __initialWidth = groupItem.width
            __initialHeight = groupItem.height
            anchors.right = undefined
            anchors.top = undefined
            groupBox.anchors.top = tRightCornerDrag.top
        }

        onReleased: {
            groupItem.width = __initialWidth + (tRightCornerDrag.x - __initialX)
            groupItem.height = __initialHeight - (tRightCornerDrag.y - __initialY)
            groupItem.y = groupItem.y + (tRightCornerDrag.y - __initialY)
            anchors.right = parent.right
            anchors.top = parent.top
            groupBox.anchors.top = tLeftCornerDrag.top
        }

        Loader {
            enabled: cornerStyle && diagonalEnabled[1] == true
            sourceComponent: cornerStyle && diagonalEnabled[1] == true ? cornerStyle : null
            asynchronous: true
            anchors.fill: parent
            property int corner: Qt.TopRightCorner
        }
    }



    MouseArea {
        id: bRightCornerDrag
        anchors.right: parent.right
        anchors.bottom: parent.bottom

        drag.target: bRightCornerDrag
        drag.axis: Drag.XAndYAxis
        drag.minimumX: bLeftCornerDrag.x + minimumWidth
        drag.minimumY: tLeftCornerDrag.y + tLeftCornerDrag.height + minimumHeight

        enabled: !locked && diagonalEnabled[2] == true

        cursorShape: enabled ? Qt.SizeFDiagCursor : Qt.ArrowCursor

        width: defaultSpacing.doubleMargin
        height: defaultSpacing.doubleMargin

        onPressed: {
            __initialX = x
            __initialY = y
            __initialWidth = groupItem.width
            __initialHeight = groupItem.height
            anchors.right = undefined
            anchors.bottom = undefined
            groupBox.anchors.right = bRightCornerDrag.right
            groupBox.anchors.bottom = bRightCornerDrag.bottom
        }

        onReleased: {
            groupItem.width = __initialWidth + (bRightCornerDrag.x - __initialX)
            groupItem.height = __initialHeight + (bRightCornerDrag.y - __initialY)
            anchors.right = parent.right
            anchors.bottom = parent.bottom
            groupBox.anchors.right = tRightCornerDrag.right
            groupBox.anchors.bottom = bLeftCornerDrag.bottom
        }

        Loader {
            enabled: cornerStyle && diagonalEnabled[2] == true
            sourceComponent: cornerStyle && diagonalEnabled[2] == true ? cornerStyle : null
            asynchronous: true
            anchors.fill: parent
            property int corner: Qt.BottomRightCorner
        }
    }

    MouseArea {
        id: bLeftCornerDrag
        anchors.left: parent.left
        anchors.bottom: parent.bottom

        drag.target: bLeftCornerDrag
        drag.axis: Drag.XAndYAxis
        drag.maximumX: bRightCornerDrag.x - minimumWidth
        drag.minimumY: tRightCornerDrag.y + tRightCornerDrag.height + minimumHeight

        enabled: !locked && diagonalEnabled[3] == true

        cursorShape: enabled ? Qt.SizeBDiagCursor : Qt.ArrowCursor

        width: defaultSpacing.doubleMargin
        height: defaultSpacing.doubleMargin

        onPressed: {
            __initialX = x
            __initialY = y
            __initialWidth = groupItem.width
            __initialHeight = groupItem.height
            anchors.left = undefined
            anchors.bottom = undefined
            groupBox.anchors.left = bLeftCornerDrag.left
        }

        onReleased: {
            groupItem.width = __initialWidth - (bLeftCornerDrag.x - __initialX)
            groupItem.height = __initialHeight + (bLeftCornerDrag.y - __initialY)
            groupItem.x = groupItem.x + (bLeftCornerDrag.x - __initialX)
            anchors.left = parent.left
            anchors.bottom = parent.bottom
            groupBox.anchors.left = tLeftCornerDrag.left
        }

        Loader {
            enabled: cornerStyle && diagonalEnabled[3] == true
            sourceComponent: cornerStyle && diagonalEnabled[3] == true ? cornerStyle : null
            asynchronous: true
            anchors.fill: parent
            property int corner: Qt.BottomLeftCorner
        }
    }

    MouseArea {
        id: topDrag
        anchors.left: tLeftCornerDrag.right
        anchors.right: tRightCornerDrag.left
        anchors.top: parent.top

        drag.target: topDrag
        drag.axis: Drag.YAxis
        drag.maximumY: bottomDrag.y - minimumHeight

        enabled: !locked && orthogonalEnabled[0] == true

        cursorShape: enabled ? Qt.SizeVerCursor : Qt.ArrowCursor

        height: defaultSpacing.doubleMargin

        onPressed: {
            __initialY = y
            __initialHeight = groupItem.height
            anchors.top = undefined
            groupBox.anchors.top = topDrag.top
        }

        onReleased: {
            groupItem.height = __initialHeight - (topDrag.y - __initialY)
            groupItem.y = groupItem.y + (topDrag.y - __initialY)
            anchors.top = parent.top
            groupBox.anchors.top = tLeftCornerDrag.top
        }

        Loader {
            enabled: edgeStyle && orthogonalEnabled[0] == true
            sourceComponent: edgeStyle && orthogonalEnabled[0] == true ? edgeStyle : null
            asynchronous: true
            anchors.fill: parent
            property int edge: Qt.TopEdge
        }
    }

    MouseArea {
        id: rightDrag
        anchors.top: tRightCornerDrag.bottom
        anchors.right:  parent.right
        anchors.bottom: bRightCornerDrag.top

        drag.target: rightDrag
        drag.axis: Drag.XAxis
        drag.minimumX: leftDrag.x + minimumWidth

        enabled: !locked && orthogonalEnabled[1] == true

        cursorShape: enabled ? Qt.SizeHorCursor : Qt.ArrowCursor

        width: defaultSpacing.doubleMargin

        onPressed: {
            __initialX = x
            __initialWidth = groupItem.width
            anchors.right = undefined
            groupBox.anchors.right = rightDrag.right
        }

        onReleased: {
            groupItem.width = __initialWidth + (rightDrag.x - __initialX)
            anchors.right = parent.right
            groupBox.anchors.right = tRightCornerDrag.right
        }

        Loader {
            enabled: edgeStyle
            sourceComponent: edgeStyle ? edgeStyle : null
            asynchronous: true
            anchors.fill: parent
            property int edge: Qt.RightEdge
        }
    }

    MouseArea {
        id: bottomDrag
        anchors.left: bLeftCornerDrag.right
        anchors.right: bRightCornerDrag.left
        anchors.bottom: parent.bottom

        drag.target: bottomDrag
        drag.axis: Drag.YAxis
        drag.minimumY: topDrag.y + topDrag.height + minimumHeight

        enabled: !locked && orthogonalEnabled[2] == true

        cursorShape: enabled ? Qt.SizeVerCursor : Qt.ArrowCursor

        height: defaultSpacing.doubleMargin

        onPressed: {
            __initialY = y
            __initialHeight = groupItem.height
            anchors.bottom = undefined
            groupBox.anchors.bottom = bottomDrag.bottom
        }

        onReleased: {
            groupItem.height = __initialHeight + (bottomDrag.y - __initialY)
            anchors.bottom = parent.bottom
            groupBox.anchors.bottom = bLeftCornerDrag.bottom
        }

        Loader {
            enabled: edgeStyle && orthogonalEnabled[2] == true
            sourceComponent: edgeStyle && orthogonalEnabled[2] == true ? edgeStyle : null
            asynchronous: true
            anchors.fill: parent
            property int edge: Qt.BottomEdge
        }
    }

    MouseArea {
        id: leftDrag
        anchors.top: tLeftCornerDrag.bottom
        anchors.left: parent.left
        anchors.bottom: bLeftCornerDrag.top

        drag.target: leftDrag
        drag.axis: Drag.XAxis
        drag.maximumX: rightDrag.x - minimumWidth

        enabled: !locked && orthogonalEnabled[3] == true

        cursorShape: enabled ? Qt.SizeHorCursor : Qt.ArrowCursor

        width: defaultSpacing.doubleMargin

        onPressed: {
            __initialX = x
            __initialWidth = groupItem.width
            anchors.left = undefined
            groupBox.anchors.left = leftDrag.left
        }

        onReleased: {
            groupItem.width = __initialWidth - (leftDrag.x - __initialX)
            groupItem.x = groupItem.x + (leftDrag.x - __initialX)
            anchors.left = parent.left
            groupBox.anchors.left = tLeftCornerDrag.left
        }

        Loader {
            enabled: edgeStyle && orthogonalEnabled[3] == true
            sourceComponent: edgeStyle && orthogonalEnabled[3] == true ? edgeStyle : null
            asynchronous: true
            anchors.fill: parent
            property int edge: Qt.LeftEdge
        }
    }
}

