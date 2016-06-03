import QtQuick 2.3
import CustomConnection 1.0

ConnectionCurve
{
    id: connectionNodes
    anchors.fill: parent

    property var connectionObj
    property var connectionID

    property var firstSlot
    property var secondSlot

    property var firstSlotView
    property var secondSlotView

    property var firstNode: (firstSlot) ? firstSlot.node : null
    property var secondNode: (secondSlot) ? secondSlot.node : null

    property var firstNodeView
    property var secondNodeView

    property var firstNodePos
    property var secondNodePos

    property bool firstNodeExpanded
    property bool secondNodeExpanded

    property var endPos

    property var viewTransform

    contentScale: viewTransform.xScale
    contentTranslate: Qt.point(viewTransform.origin.x, viewTransform.origin.y)

    onFirstNodeChanged: initNodeView()
    onSecondNodeChanged: initNodeView()

    onFirstNodeViewChanged: 
    { 
        firstSlotView = firstNodeView.getSlotViewBySlotObj(firstSlot);
        connectionColor = firstSlotView.slotColor
    }

    onSecondNodeViewChanged:
    { 
        secondSlotView = secondNodeView.getSlotViewBySlotObj(secondSlot) 
    }

    onFirstSlotViewChanged:     updatePos()
    onSecondSlotViewChanged:    updatePos()

    onFirstNodePosChanged:      updatePos()
    onSecondNodePosChanged:     updatePos()

    onFirstNodeExpandedChanged: updatePos()
    onSecondNodeExpandedChanged: updatePos()

    onEndPosChanged:            updatePos()
    onContentTranslateChanged:  updatePos()

    onConnectionClicked: deleteConnection(connectionID)

    function initNodeView()
    {
        if (firstNode && !firstNodeView)
        {
            var node = graphView.getNodeViewById(firstNode.id);

            firstNodeView = node;
            firstNodePos = Qt.binding(function() { return Qt.point(firstNodeView.x, firstNodeView.y) });
            firstNodeExpanded = Qt.binding(function() { return firstNodeView.nodeIsExpanded; });
        }

        if (secondNode && !secondNodeView)
        {
            var node = graphView.getNodeViewById(secondNode.id);

            secondNodeView = node;
            secondNodePos = Qt.binding(function() { return Qt.point(secondNodeView.x, secondNodeView.y) });
            secondNodeExpanded = Qt.binding(function() { return secondNodeView.nodeIsExpanded; });
        }
    }

    function tryConnectToSlot(slot)
    {
        if (firstSlot && secondSlot)
            return;

        if (firstSlot)
        {
            var slotObj = slot.slotObj;
            var isInput = slot.isInput;

            if (isInput)
                createConnection(firstNode.id, firstSlot.id, slotObj.node.id, slotObj.id);
            else
                createConnection(slotObj.node.id, slotObj.id, firstNode.id, firstSlot.id);
        }
    }

    function updatePos()
    {
        fromNode = Qt.rect(firstNodeView.x, firstNodeView.y,  firstNodeView.width, firstNodeView.height);
        if (secondNodeView)
            toNode = Qt.rect(secondNodeView.x, secondNodeView.y, secondNodeView.width, secondNodeView.height);

        fromPoint = firstSlotView.getSlotAnchor();
        if (secondSlotView || endPos)
            toPoint = (secondSlotView) ? secondSlotView.getSlotAnchor() : endPos;
    }

    Connections {
        ignoreUnknownSignals: true
        target: firstSlotView ? firstSlotView : null
        onRemoveConnection: {
            deleteConnection(connectionID)
        }
    }
    Connections {
        ignoreUnknownSignals: true
        target: secondSlotView ? secondSlotView : null
        onRemoveConnection: {
            deleteConnection(connectionID)
        }
    }
}
