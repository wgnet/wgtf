import QtQuick 2.3
import QtQuick.Controls 1.2
import QtQuick.Layouts 1.1
import WGControls 1.0

/*!
 \brief A Loader that showing a busy indicator when loading component in an async mode
*/
Loader {
    id: loader
    objectName: "WGLoader"

    WGBusyIndicator {
        id: busyIndicator
        objectName: "WGLoaderBusyIndicator"
        parent: loader.parent
        anchors.fill: loader
        visible: busyIndicator.running
    }//WGBusyIndicator

    QtObject {
        id: internal
        property bool visible: true
        property bool external: false
    }

    property alias loading: busyIndicator.running

    onVisibleChanged: {
        if (loading) {
            if (!internal.external) {
                internal.external = true
                internal.visible = visible
                visible = false
            }
            else if (internal.external) {
                internal.external = false
            }
        }
    }

    onLoadingChanged: {
        if (loading) {
            internal.external = true
            internal.visible = visible
            visible = false
        }
        else {
            visible = internal.visible
        }
    }
}//Loader