import QtQuick 2.1
import QtQuick.Layouts 1.1
import WGControls 1.0

Item
{
    id: rootSlot

    objectName : "ConnectionSlot"
    property var slotModel
    property var slotObject
    property var nodeObject
    property bool isInput

    property var slotPlug

    implicitWidth : slotLayout.width
    implicitHeight : slotLayout.height

    property color slotColor: isInput ? "#7AC943" : "#3FA9F5"

    // This var seemed pretty circular now that functionality is here
    // property var slot

    // This is a bit of a hack to reposition the connector when the node is collapsed
    function getGlobalCenter()
    {
        if (nodeIsExpanded)
        {
            return mapToItem(objectsContainer, (slotPlug.x + slotPlug.width/2), (slotPlug.y + slotPlug.height/2))
        }
        else
        {
            if (isInput)
            {
                return Qt.point(nodeObject.x + defaultSpacing.doubleMargin, nodeObject.y + nodeObject.height/2)
            }
            else
            {
                return Qt.point(nodeObject.x + nodeObject.width - defaultSpacing.doubleMargin, nodeObject.y + nodeObject.height/2)
            }
        }
    }

    Component.onCompleted :
    {
        slotsIndex[slotObject.uid] = rootSlot;
    }

    Component.onDestruction :
    {
        delete slotsIndex[slotObject.uid];
    }

    // Feels like a slot should be one object with an input and/or output and potentially an edit control in the middle rather than two separate slots.
    // This would make it easier to line everything up into three columns as well.

    // This property adds any child object added to the Slot to the __editControls layout below. This is then parented next to the slot itself.

    default property alias defaultEditControl: __editControls.children

    ColumnLayout {
        id: __editControls
        Layout.fillWidth: true

    }

    MouseArea
    {
        id : mouseArea
        anchors.fill : parent
        acceptedButtons : Qt.LeftButton

        onPressed :
        {

            var pt = getGlobalCenter();
            interactiveConnectionCurve.startX = pt.x;
            interactiveConnectionCurve.startY = pt.y;
            interactiveConnectionCurve.endX = pt.x;
            interactiveConnectionCurve.endY = pt.y;
            interactiveConnectionCurve.visible = true;
            connectionStartSlot = rootSlot


            //TODO would be good to make this connection wire on top of the component when it is being dragged
        }

        onPositionChanged :
        {
            var pt = mapToItem(objectsContainer, mouse.x, mouse.y)
            interactiveConnectionCurve.endX = pt.x;
            interactiveConnectionCurve.endY = pt.y;
        }

        function findSlot(root, point)
        {
            for(var i = 0; i < root.visibleChildren.length; ++i)
            {
                var child = root.visibleChildren[i];
                if(child == interactiveConnectionCurve)
                    continue;

                var pt = root.mapToItem(child, point.x, point.y)
                if (pt.x >= 0 && pt.x <= child.width &&
                    pt.y >= 0 && pt.y <= child.height)
                {
                    if (child.objectName === "ConnectionSlot")
                        return child

                    var obj =  findSlot(child, pt);
                    if (obj != null)
                        return obj
                }
            }

            return null
        }

        onReleased :
        {
            connectionStartSlot = null

            interactiveConnectionCurve.startX = 0;
            interactiveConnectionCurve.startY = 0;
            interactiveConnectionCurve.endX = 0;
            interactiveConnectionCurve.endY = 0;
            interactiveConnectionCurve.visible = false;

            var point = mapToItem(objectsContainer, mouse.x, mouse.y)

            var child = findSlot(objectsContainer, point)
            if (child == null)
                return;

            if (isInput)
                graphEditorComponent.connect(child.slotObject.uid, slotObject.uid)
            else
                graphEditorComponent.connect(slotObject.uid, child.slotObject.uid)
        }

        ContextMenu
        {
            menuModel : graphEditorComponent.getSlotContextMenu()
            z : rootSlot.z + 10
            contextObjectUid : slotObject.uid
        }
    }

    WGExpandingRowLayout
    {
        id : slotLayout

        layoutDirection: isInput ? Qt.LeftToRight : Qt.RightToLeft

        SlotImage {
            id: slotImage
            Layout.alignment : Qt.AlignVCenter | Qt.AlignLeft
            Component.onCompleted: {
                slotPlug = slotImage
            }
        }

        WGLabel {
            Layout.alignment: isInput ? (Qt.AlignTop | Qt.AlignLeft) : (Qt.AlignTop | Qt.AlignRight)
            horizontalAlignment: isInput ? Text.AlignLeft : Text.AlignRight
            text: slotObject.title
            // Text.QtRendering looks a bit fuzzier but is much better for arbitrary scaling
            renderType: Text.QtRendering
        }

        Component.onCompleted: {
            __editControls.parent = slotLayout
        }
    }
}
