import QtQuick 2.3
import QtQuick.Controls 1.2
import QtQuick.Layouts 1.0
import QtQuick.Dialogs 1.2

import WGControls 2.0
import WGControls.Canvas 2.0
import WGControls.Global 2.0


Item
{
    id: graphView
    WGComponent { type: componentType }
    property var componentType: "GraphView"

    anchors.fill: parent

    property var nodesModel
    property var connectionsModel
    property var nodeClassesModel
    property var groupModel

    property alias contextMenu: graphContextMenu
    property alias contextArea: graphContextArea

    /*! internal */
    property var __dialogInstance: null

    signal redrawGraph();

    function getNodeViewById(nodeId)
    {
        var nodeView = null;
        var size = nodeRepeater.count;
        for (var i = 0; i < size; ++i)
        {
            nodeView = nodeRepeater.itemAt(i);
            if (nodeView.nodeObj.id == nodeId)
            {
                return nodeView;
            }
        }

        return null;
    }

    function selectNode(node)
    {
        node.selected = true;
        multiDrag.addDragObj(node);
    }

    function unselectNode(node)
    {
        node.selected = false;
        multiDrag.removeDragObj(node);
    }

    function resetNodesSelection()
    {
        for(var i = 0; i < nodeRepeater.count; ++i)
        {
            var node = nodeRepeater.itemAt(i);
            unselectNode(node);
        }
        for(var j = 0; j < groupRepeater.count; ++j)
        {
            var group = groupRepeater.itemAt(j);
            unselectNode(group);
        }
        multiDrag.dragObjects = []
    }

    function selectNodesInArea(areaRect, isAddMode)
    {
        var selAreaTopLeft = Qt.point(areaRect.x, areaRect.y);
        var selAreaBottomRight = Qt.point(areaRect.x + areaRect.width, areaRect.y + areaRect.height);
        for(var i = 0; i < nodeRepeater.count; ++i)
        {
            var node = nodeRepeater.itemAt(i);
            var nodeTopLeft = Qt.point(node.x, node.y);
            var nodeBottomRight = Qt.point(node.x + node.width, node.y + node.height);

            var isIntersects = Math.max(0, Math.min(nodeBottomRight.x, selAreaBottomRight.x) - Math.max(nodeTopLeft.x, selAreaTopLeft.x)) *
                    Math.max(0, Math.min(nodeBottomRight.y, selAreaBottomRight.y) - Math.max(nodeTopLeft.y, selAreaTopLeft.y)) != 0;

            if (isAddMode && isIntersects)
                selectNode(node);
            else if (!isAddMode)
            {
                var funcNodeState = (isIntersects) ? selectNode : unselectNode;
                funcNodeState(node);
            }
        }
    }

    function createGroupBox(x, y, name, color, height, width) {
        var localCoords = groupRepeater.mapFromItem(graphView, x, y);
        var rectangleVector = Qt.vector4d(localCoords.x, localCoords.y, width, height);
        var colorVector = Qt.vector4d(color.r, color.g, color.b, color.a);
        createGroup(groupModel, rectangleVector, name, colorVector);
    }

    /*! This function opens the desired dialog box.
    */
    function openColorDialog(curColor) {
        __dialogInstance = WGDialogs.customColorPickerDialog
        if(typeof viewId != "undefined")
        {
            __dialogInstance.viewId = viewId
        }
        if(typeof viewPreference != "undefined")
        {
            __dialogInstance.viewPreference = viewPreference
        }
        __dialogInstance.modality = Qt.ApplicationModal
        __dialogInstance.showAlphaChannel = false
        __dialogInstance.open(600, 380, curColor)
    }

    /*! This function closes the desired dialog box depending on whether useAssetBrowser == true or not.
    */
    function closeColorDialog() {
        __dialogInstance.close()
    }

    MouseArea {
        id: graphContextArea
        anchors.fill: parent
        acceptedButtons: Qt.RightButton;

        onClicked: {
            contextMenu.contextObject = graphView;
            contextMenu.popupPointX = mouse.x;
            contextMenu.popupPointY = mouse.y;
            contextMenu.popup();
        }
    }

    ContextMenu {
        id: graphContextMenu
        path: "NodeEditorMenu"
        graph: graphView
    }

    WGGridCanvas
    {
        id: canvasContainer
        anchors.fill: parent
        clip: false

        keepAspectRatio: true
        useAxisScaleLimit: xyScaleLimit

        showMouseLine: false
        useGridLabels: false
        useBorders: false

        // bools to check for slot connection suggestions
        property bool creatingConnection: false
        property bool creatingFromInput: false
        property color creatingColor: "#000000"
        property var currentConnectionSlot: null
        property var creatingNode: null

        property var colorEditingIndex

        signal updateConnections()

        signal deleteNodes()

        viewTransform: WGViewTransform{
            container: canvasContainer
            xScale: 1
            yScale: -1
        }

        onCanvasPressed:
        {
            resetNodesSelection();
            canvasContainer.focus = true
        }

        onSelectArea:
        {
            selectNodesInArea(canvasContainer.mapToItem(canvasItem, min.x, min.y, Math.abs(max.x - min.x), Math.abs(max.y - min.y)), mouse.modifiers & Qt.ShiftModifier);
            canvasContainer.focus = true
        }

        onPreviewSelectArea:
        {
            selectNodesInArea(canvasContainer.mapToItem(canvasItem, min.x, min.y, Math.abs(max.x - min.x), Math.abs(max.y - min.y)), mouse.modifiers & Qt.ShiftModifier);
        }

        property var connectionComponent: Qt.createComponent("ConnectionNodes.qml")
        property var currentConnection: null

        function startCreatingNewConnection(fromSlotObj, parentNode)
        {
            currentConnection = connectionComponent.createObject(nodeEditorView, {"firstSlot": fromSlotObj,
                                                                     "secondSlot": null,
                                                                     "viewTransform": viewTransform});
            creatingConnection = true
            creatingFromInput = fromSlotObj.isInput
            creatingColor = fromSlotObj.color
            currentConnectionSlot = fromSlotObj
            creatingNode = parentNode
        }

        function finishCreatingNewConnection(endPos)
        {
            var nodePos = graphView.mapToItem(canvasItem, endPos.x, endPos.y);
            var node = canvasItem.childAt(nodePos.x, nodePos.y);
            if (node)
            {
                var endSlot = node.getSlotByPos(canvasItem.mapToItem(node, nodePos.x, nodePos.y))
                if (endSlot && endSlot.objectName == "Slot")
                {
                    currentConnection.tryConnectToSlot(endSlot);
                }
            }

            currentConnection.destroy();
            currentConnection = null;

            creatingConnection = false
            creatingFromInput = false
            creatingColor = "#000000"
            currentConnectionSlot = null
            creatingNode = null
        }

        Keys.onPressed: {
            if (event.key == Qt.Key_Delete) {
                if (multiDrag.dragObjects.length > 0)
                {
                    deleteDialog.open()
                    event.accepted = true;
                }
            }
        }

        Connections {
            target: graphView
            onRedrawGraph: {
                canvasContainer.requestPaint();
            }
        }

        Item
        {
            id: multiDrag
            property var dragObjects: []

            function addDragObj(obj)
            {
                var index = dragObjects.indexOf(obj);
                if (index != -1)
                    return;

                dragObjects.push(obj);
                obj.dragStateChanged.connect(dragStateChanged);
            }

            function removeDragObj(obj)
            {
                var index = dragObjects.indexOf(obj);
                if (index != -1)
                {
                    dragObjects.splice(index, 1);
                    obj.dragStateChanged.disconnect(dragStateChanged);

                    // need to unbind this here again or groupBoxes don't work.
                    obj.x = obj.x;
                    obj.y = obj.y;
                }
            }

            function dragStateChanged(node)
            {
                dragObjects.forEach(function(dragObj){
                    if(dragObj === node)
                        return;

                    if (node.isDragActive)
                    {
                        var oldPos = Qt.point(node.x, node.y);
                        var dragObjPos = Qt.point(dragObj.x, dragObj.y);

                        dragObj.x = Qt.binding(function(){ return dragObjPos.x - (oldPos.x - node.x); })
                        dragObj.y = Qt.binding(function(){ return dragObjPos.y - (oldPos.y - node.y); })
                    }
                    else
                    {
                        //unbind
                        dragObj.x = dragObj.x;
                        dragObj.y = dragObj.y;
                    }
                });
            }
        }

        Repeater
        {
            id: connectionRepeater
            model: connectionsModel
            delegate: ConnectionNodes
            {
                connectionObj: value
                connectionID: connectionObj.id
                firstSlot: connectionObj.output
                secondSlot: connectionObj.input
                viewTransform: canvasContainer.viewTransform
            }
            onCountChanged: {
                canvasContainer.updateConnections()
            }
        }

        WGGridCanvasItem
        {
            id: canvasItem
            anchors.fill: parent
            clip: false
            viewTransform: canvasContainer.viewTransform


            Repeater
            {
                id: groupRepeater
                model: groupModel
                delegate: NodeGroupArea
                {
                    groupTitle: typeof value !== "undefined" ? value.name : "No name"
                    groupColor: {
                        // Lookup value color from the model once
                        var vector = value.color;
                        return Qt.vector4d(vector.x, vector.y, vector.z, vector.w)
                    }
                    x: typeof value !== "undefined" ? value.rectangle.x : 0
                    y: typeof value !== "undefined" ? value.rectangle.y : 0
                    z: -1
                    width: typeof value !== "undefined" ? value.rectangle.z : 100
                    height: typeof value !== "undefined" ? value.rectangle.w : 100

                    onChangeTitle: {
                        if (typeof value == "undefined") {
                            return;
                        }
                        value.name = title;
                    }

                    onChangeColor: {
                        beginUndoFrame()
                        canvasContainer.colorEditingIndex = index
                        openColorDialog(groupColor)
                    }

                    onSetPosition: {
                        if (typeof value == "undefined") {
                            return;
                        }
                        var localPos = mapFromItem(graphView, xPos, yPos);
                        // value.rectangle calls set once, which will add 1 item to history
                        // Setting x and y individually will add 2 items to history
                        value.rectangle = Qt.vector4d(localPos.x, localPos.y, width, height);
                    }

                    onSetDimensions: {
                        if (typeof value == "undefined") {
                            return;
                        }
                        // value.rectangle calls set once, which will add 1 item to history
                        // Setting x, y, w & h individually will add 4 items to history
                        value.rectangle = Qt.vector4d(newX, newY, newWidth, newHeight);
                    }

                    onDeleteNode: {
                        groupModel.removeItem(index);
                    }

                    // Receive changes from the model's undo/redo
                    Connections {
                        target: typeof value !== "undefined" ? value : null
                        onRectangleChanged: {
                            if (typeof value == "undefined") {
                                return;
                            }
                            x = value.rectangle.x;
                            y = value.rectangle.y;
                            width = value.rectangle.z;
                            height = value.rectangle.w;
                        }
                    }
                }
            }

            Repeater
            {
                id: nodeRepeater
                model: nodesModel
                delegate: Node
                {
                    id: nodeContainer
                    nodeObj: value
                    nodeID: nodeObj.id
                    nodeTitle: nodeObj.nodeTitle
                    inputSlotsModel: nodeObj.inputSlotsModel
                    outputSlotsModel: nodeObj.outputSlotsModel
                    x: mapFromItem(graphView, nodeObj.nodeCoordX, nodeObj.nodeCoordY).x
                    y: mapFromItem(graphView, nodeObj.nodeCoordX, nodeObj.nodeCoordY).y

                    Connections {
                        target: canvasContainer
                        onDeleteNodes: {
                            if (nodeContainer.selected)
                            {
                                deleteNode(nodeContainer.nodeID);
                            }
                        }
                    }
                }
            }
        }
    }

    Connections {
        target: __dialogInstance
        ignoreUnknownSignals: true

        onAccepted: {
            groupModel.item(canvasContainer.colorEditingIndex).value.color = selectedValue
            endUndoFrame();
        }

        onRejected: {
            abortUndoFrame();
        }

        onClosed: {
            __dialogInstance = null
        }
    }

    Dialog {
        id: deleteDialog
        visible: false
        title: "Delete Nodes"
        width: 240
        height: 80

        contentItem: Rectangle {
                color: palette.mainWindowColor
                ColumnLayout {
                    anchors.top: parent.top
                    anchors.left: parent.left
                    anchors.right: parent.right
                    anchors.margins: defaultSpacing.standardMargin
                    height: 60
                    WGLabel {
                        text: "Delete selected nodes?"
                        anchors.leftMargin: defaultSpacing.standardMargin
                    }
                    Item {
                        Layout.fillHeight: true
                        Layout.fillWidth: true
                    }
                    RowLayout {
                        Layout.fillWidth: true
                        Layout.preferredHeight: defaultSpacing.minimumRowHeight

                        Item {
                            Layout.fillWidth: true
                            Layout.preferredHeight: defaultSpacing.minimumRowHeight
                        }
                        WGPushButton {
                            text: "Yes"
                            Layout.preferredWidth: 60
                            onClicked: {
                                deleteDialog.accepted()
                                deleteDialog.close()
                            }
                        }
                        WGPushButton {
                            text: "No"
                            Layout.preferredWidth: 60
                            onClicked: {
                                deleteDialog.rejected()
                                deleteDialog.close()
                            }
                        }
                    }
                }
        }

        onAccepted: {
            canvasContainer.deleteNodes()
        }
    }
}
