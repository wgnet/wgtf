import QtQuick 2.5
import QtQuick.Controls 1.4
import QtQuick.Dialogs 1.2
import WGControls 2.0

/*!
 \ingroup wgcontrols
 \brief Standard QML Image with a fallback state if image is not found.

\code{.js}
Image{
    source: "icons/myImage.png";
}
\endcode
*/

Image {
    id: image
    objectName: "WGImage"
    WGComponent { type: "WGImage20" }

    property real lastImageHeight: 0
    property real lastImageWidth: 0

    onStatusChanged: {
        if (status == Image.Ready)
        {
            lastImageHeight = image.sourceSize.height
            lastImageWidth = image.sourceSize.width
        }
    }

    Image {
        anchors.centerIn: parent

        height: lastImageHeight != 0 ? lastImageHeight : Math.min(sourceSize.height, image.parent.height)
        width: lastImageWidth != 0 ? lastImageWidth : Math.min(sourceSize.width, image.parent.width)

        source: "icons/defaultImage.png"
        visible: image.status == Image.Error

        fillMode: Image.PreserveAspectFit
    }
}
