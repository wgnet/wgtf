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

    QtObject {
        id: internal
        property var internalImageSource: ""
        property bool external: false

        onInternalImageSourceChanged: {
            if(typeof internalImageSource == "undefined")
            {
                return;
            }
            var imageUrl = getIconUrlFromImageProvider(internalImageSource);
            if(imageUrl != "")
            {
                //if image can be found in image provider
                image.realImageSource = imageUrl;
            }
            else
            {
                //if image cannot be found in image provider
                image.realImageSource = internalImageSource;
            }
        }
    }

    property alias realImageSource: image.source
    property alias source: internal.internalImageSource

    Image {
        anchors.centerIn: parent

        height: lastImageHeight != 0 ? Math.min(image.height, lastImageHeight) : image.parent != null ? (Math.min(sourceSize.height, image.parent.height)) : Math.min(image.height, sourceSize.height)
        width: lastImageWidth != 0 ? Math.min(image.width, lastImageWidth) : image.parent != null ? (Math.min(sourceSize.width, image.parent.width)) : Math.min(image.width, sourceSize.width)

        source: "icons/defaultImage.png"
        visible: image.status == Image.Error

        fillMode: Image.PreserveAspectFit
    }
}
