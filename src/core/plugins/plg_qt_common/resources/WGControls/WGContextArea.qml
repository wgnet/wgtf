import QtQuick 2.5
import QtQuick.Controls 1.4
import WGControls 1.0

/*!
 \ingroup wgcontrols
 \brief Context sensitive Right Click Menu

Example:
\code{.js}
Component {
    id: contextMenuComponent
    WGContextArea {
        onAboutToShow: {
            // Prepare the context menu by setting the context object as appropriate
            // This context is used to determine if menu items are available
            // IMPORTANT: To support shortcuts set the context as soon as it changes (i.e. onSelectionChanged)
            contextMenu.contextObject = listModelSelection.selectedItem;
        }
        WGContextMenu
        {
            // Set this to the path specified in your actions.xml
            path: "ContextMenuPath"
            windowId: ""
            contextManager: componentContext
        }
    }
}
\endcode
*/

MouseArea {
    objectName: "WGContextArea"
    WGComponent { type: "WGContextArea" }

    property var popupPoint
    default property var contextMenu

    signal aboutToShow;

    onClicked: {
        if (mouse.button == Qt.RightButton)
        {
            if (contextMenu)
            {
                popupPoint = Qt.point(mouse.x, mouse.y)
                mouse.accepted = true
                aboutToShow();
                contextMenu.popup()
            }
        }
        else
        {
            mouse.accepted = false
        }
    }

    anchors.fill: parent
    propagateComposedEvents: true
    preventStealing: true
    acceptedButtons: Qt.RightButton
}
