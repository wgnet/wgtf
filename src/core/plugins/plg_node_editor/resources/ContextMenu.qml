import QtQuick 2.3
import QtQuick.Controls 1.2
import WGControls 1.0


WGContextArea
{
    id: contextArea

    property string menuPath: "NodeEditor"

    property alias contextObject: dynamicContextMenu.contextObject

    contextMenu: WGMenu
    {
        id: dynamicContextMenu
        path: menuPath

        WGListModel
        {
            id : contextMenuModel
            source : graphView.nodeClassesModel

            ValueExtension {}
        }

        Instantiator
        {
            id : menuItemInstant
            model : contextMenuModel
            onObjectAdded: dynamicContextMenu.insertItem( index, object )
            onObjectRemoved: dynamicContextMenu.removeItem( object )
            delegate : MenuItem
            {
                text : qsTr("Create Node.") + value
                onTriggered :
                {
                    createNode(mapToItem(graphView, contextArea.popupPoint.x, contextArea.popupPoint.y).x,
                               mapToItem(graphView, contextArea.popupPoint.x, contextArea.popupPoint.y).y, value);
                }
            }
        }

        MenuItem
        {
            text: qsTr("Groups.Create Group Box")
            onTriggered: {

                //generate a random 'nice' color

                var colorPool = 2
                var colorArray = []

                colorArray[0] = Math.random()
                colorPool -= colorArray[0]
                colorArray[1] = Math.random()
                colorPool -= colorArray[1]
                colorArray[2] = Math.min(Math.max(Math.random(), colorPool), 1.0)

                for (var i = colorArray.length - 1; i > 0; i--) {
                    var j = Math.floor(Math.random() * (i + 1));
                    var temp = colorArray[i];
                    colorArray[i] = colorArray[j];
                    colorArray[j] = temp;
                }

                var newColor = Qt.rgba(colorArray[0],colorArray[1],colorArray[2],1.0)

                createGroup(mapToItem(graphView, contextArea.popupPoint.x, contextArea.popupPoint.y).x,
                            mapToItem(graphView, contextArea.popupPoint.x, contextArea.popupPoint.y).y,
                            "New Group",
                            newColor,
                            500, 500)
            }
        }
    }
}
