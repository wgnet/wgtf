import QtQuick 2.1
import QtQuick.Controls 1.2
import WGControls 1.0

WGContextArea
{
    property var menuModel
    property var contextObjectUid : 0

    contextMenu : WGMenu
    {
        id : dynamicContextMenu
        WGListModel
        {
            id : contextMenuModel
            source : menuModel
            
            ValueExtension {}
        }
        
        Instantiator
        {
            id : menuActions
            model : contextMenuModel
            onObjectAdded: dynamicContextMenu.insertItem( index, object )
            onObjectRemoved: dynamicContextMenu.removeItem( object )
            delegate : MenuItem
            {
                text : value.title
                onTriggered :
                {
                    var pt = graphCanvas.viewTransform.inverseTransform(popupPoint)
                    value.trigger(pt.x, pt.y, contextObjectUid)
                }
            }
        }
    }
}
