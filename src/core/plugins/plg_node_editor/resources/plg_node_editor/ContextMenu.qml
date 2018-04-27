import QtQuick 2.3
import QtQuick.Controls 1.2
import WGControls 2.0

/**
* Drop down menu when right mouse clicking on the graph
*/
WGContextMenu {
    id: contextMenu
    WGComponent { type: "ContextMenu" }    
    
    property var graph: null
    property int popupPointX: 0
    property int popupPointY: 0    
    
    /**
    * Determines if the context menu is set and the object is of this type
    */
    function contextMenuValid(component)
    {
        return typeof contextMenu.contextObject !== 'undefined' &&
               contextMenu.contextObject.componentType === component;
    }    
    
    /**
    * Deletes a node
    */
    WGAction
    {
        active: contextMenuValid("Node")
        actionId: "NodeEditor_DeleteNode"
        onTriggered: {
            deleteNode(contextMenu.contextObject.nodeID);
        }
    }    
    
    /**
    * Creates a new group box
    */
    WGAction
    {
        active: true
        actionId: "NodeEditor_CreateGroupBox"
        onTriggered: {

            //generate a random 'nice' color
            var colorPool = 2;
            var colorArray = [];

            colorArray[0] = Math.random();
            colorPool -= colorArray[0];
            colorArray[1] = Math.random();
            colorPool -= colorArray[1];
            colorArray[2] = Math.min(Math.max(Math.random(), colorPool), 1.0);

            for (var i = colorArray.length - 1; i > 0; i--) {
                var j = Math.floor(Math.random() * (i + 1));
                var temp = colorArray[i];
                colorArray[i] = colorArray[j];
                colorArray[j] = temp;
            }

            var newColor = Qt.rgba(colorArray[0],colorArray[1],colorArray[2],1.0);
            graph.createGroupBox(contextMenu.popupPointX, contextMenu.popupPointY, "New Group", newColor, 500, 500);
        }
    }

    /**
    * Deletes the selected group box
    */
    WGAction
    {
        active: contextMenuValid("NodeGroupArea") && !contextMenu.contextObject.locked;
        actionId: "NodeEditor_DeleteGroupBox"
        onTriggered: {
            contextMenu.contextObject.deleteNode();
        }
    }
    
    /**
    * Locks the selected group box
    */    
    WGAction
    {
        active: contextMenuValid("NodeGroupArea")
        actionId: "NodeEditor_LockGroupBox"
        checkable: true
        checked: typeof(contextMenu.contextObject) != "undefined" && 
                 typeof(contextMenu.contextObject.locked) != "undefined" &&
                 contextMenu.contextObject.locked;

        onTriggered: {
            contextMenu.contextObject.locked = !contextMenu.contextObject.locked
        }
    }
    
    /**
    * Changes the colour of the selected group box
    */        
    WGAction
    {
        active: contextMenuValid("NodeGroupArea") && !contextMenu.contextObject.locked;
        actionId: "NodeEditor_ColorGroupBox"
        onTriggered: {
            contextMenu.contextObject.changeColor()
        }
    }
    
    Instantiator
    {
        id : menuItemInstant
        model : graph.nodeClassesModel
        delegate: WGAction
        {
            active: true
            actionId: qsTr("NodeEditorMenu|.Create Node.") + value
            onTriggered :
            {
                createNode(contextMenu.popupPointX, contextMenu.popupPointY, value);
            }
        }
    }    
}
