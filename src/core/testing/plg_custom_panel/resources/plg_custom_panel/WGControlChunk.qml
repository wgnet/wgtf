import QtQuick 2.3
import QtQuick.Layouts 1.1


import WGControls 1.0

//This is a prototype control for mocking up advanced panel features.
//Many hacky things here.

Rectangle {
	id: chunk
	property bool pinned: true

	property bool filtered: false

	property bool newItem: false

	property QtObject rootPanel
	property QtObject parentPanel

	property QtObject rootFrame
	property QtObject parentFrame

	property QtObject contentObject

	property bool clonedControl: false
	property bool cloned: false

	property string tags: ""

	property bool panelChunk: false

	property int panelDepth

	property int layoutRow
	/*
	onParentPanelChanged: {
		console.log(chunk.toString() + " changed parent panel to " + parentPanel.toString())
    }*/

	function pinChildren(parentObject,pin)
	{
		for(var i=0; i<parentObject.children.length; i++){
			if(typeof parentObject.children[i].pinned != "undefined")
			{
				if(pin)
				{
					parentObject.children[i].pinned = true
				}
				else
				{
					parentObject.children[i].pinned = false
				}
			}
			pinChildren(parentObject.children[i],pin)
		}
	}

	function setParentChunk(parentObject){
		for(var i=0; i<parentObject.children.length; i++){
			if(typeof parentObject.children[i].parentChunk != "undefined"){
				if(parentObject.children[i].parentChunk == null){
					parentObject.children[i].parentChunk = chunk
				}
			}
			setParentChunk(parentObject.children[i])
		}
	}

	function searchTag(query)
	{
		var lowQuery = query.toLowerCase()
		if(tags.indexOf(lowQuery) == -1)
		{
			filtered = false
		}
		else
		{
			filtered = true
		}
	}

	Component.onCompleted: {
		contentObject = chunk.children[chunk.children.length-1]
		parentFrame = parent
		setParentChunk(chunk)
	}


	Rectangle {
		id: dragObject
		color: "transparent"
		Drag.active: mouseArea.drag.active
		height: contentObject.height
		width: contentObject.width
		opacity: 0.5
		ShaderEffectSource {
			id: dragSnapshot
			anchors.fill: parent
		}
	}

	Rectangle {
		color: palette.highlightShade
		anchors{left: parent.left; right: parent.right; bottom: parent.top}
		height: 5
		visible: draggedObject != null && chunkBorder.border.color == palette.highlightShade && dropAreaTop.containsDrag
	}
	Rectangle {
		color: palette.highlightShade
		anchors{left: parent.left; right: parent.right; top: parent.bottom}
		height: 5
		visible: draggedObject != null && chunkBorder.border.color == palette.highlightShade && dropAreaBottom.containsDrag
	}

	DropArea {
		id: dropAreaTop
		anchors{left: parent.left; right: parent.right; top: parent.top}
		height: parent.height/2
		enabled: draggedPanelDepth == panelDepth

		onEntered: {
			chunkBorder.border.color = palette.highlightShade
		}
		onExited: {
			chunkBorder.border.color = palette.lighterShade
		}
		onDropped: {

			if(draggedObject.panelDepth == panelDepth)
			{
				if(typeof draggedObject.layoutRow != "undefined")
				{
					var targetRow
					if(chunk.layoutRow == 0)
					{
						targetRow = 0
					}
					else if (chunk.layoutRow == chunk.parentFrame.children.length - 1 )
					{
						targetRow = chunk.layoutRow - 1
					}
					else
					{
						targetRow = chunk.layoutRow
					}
					var sourceRow = draggedObject.layoutRow
					draggedObject.layoutRow = -1
					if(draggedObject.parentFrame != chunk.parentFrame)
					{
						draggedObject.parent = chunk.parentFrame

						draggedObject.parentPanel.decrementLayoutRow(sourceRow)
						draggedObject.parentPanel.resetPanelOrder()

						chunk.parentPanel.incrementLayoutRow(targetRow)
						draggedObject.layoutRow = targetRow
						draggedObject.parentFrame = chunk.parentFrame
						draggedObject.parentPanel = chunk.parentPanel
					}
					else
					{
						parentPanel.decrementLayoutRow(sourceRow)
						parentPanel.incrementLayoutRow(targetRow)
						draggedObject.layoutRow = targetRow
					}
					chunkBorder.state = ""
					chunk.parentPanel.resetPanelOrder()

					draggedObject = null
					drop.acceptProposedAction()
				}
			}
			parentPanel.resetPanelOrder()
		}
	}

	DropArea {
		id: dropAreaBottom
		anchors{left: parent.left; right: parent.right; bottom: parent.bottom}
		height: parent.height/2
		enabled: draggedPanelDepth == panelDepth

		onEntered: {
			chunkBorder.border.color = palette.highlightShade
		}
		onExited: {
			chunkBorder.border.color = palette.lighterShade
		}
		onDropped: {

			if(draggedObject.panelDepth == panelDepth)
			{
				if(typeof draggedObject.layoutRow != "undefined")
				{
					var targetRow
					if(chunk.layoutRow == 0)
					{
						targetRow = 1
					}
					else if (chunk.layoutRow == chunk.parentFrame.children.length - 1 )
					{
						targetRow = chunk.parentFrame.children.length - 1
					}
					else
					{
						targetRow = chunk.layoutRow
					}
					var sourceRow = draggedObject.layoutRow
					draggedObject.layoutRow = -1
					if(draggedObject.parentFrame != chunk.parentFrame)
					{
						draggedObject.parent = chunk.parentFrame

						draggedObject.parentPanel.decrementLayoutRow(sourceRow)
						draggedObject.parentPanel.resetPanelOrder()

						parentPanel.incrementLayoutRow(targetRow)
						draggedObject.layoutRow = targetRow
						draggedObject.parentFrame = chunk.parentFrame
						draggedObject.parentPanel = chunk.parentPanel
					}
					else
					{
						parentPanel.decrementLayoutRow(sourceRow)
						parentPanel.incrementLayoutRow(targetRow)
						draggedObject.layoutRow = targetRow
					}
					chunkBorder.state = ""
					draggedObject = null
					drop.acceptProposedAction()
				}
			}
			parentPanel.resetPanelOrder()
		}
	}

	color: "transparent"

	Rectangle {
		id: chunkBorder
		anchors.fill: parent
		anchors.leftMargin: -2
		anchors.rightMargin: -2
		color: "transparent"

		border.width: defaultSpacing.standardBorderSize
		border.color: "transparent"

        OpacityAnimator {
            target: chunkBorder;
			from: 1;
			to: 0;
            duration: 10000
            running: chunk.newItem
        }

        onOpacityChanged: {
            if(chunkBorder.opacity == 0)
            {
                chunk.newItem = false
                chunkBorder.opacity = 1
			}
        }

		onStateChanged: {
			if(state != "NEW_ITEM")
			{
                chunk.newItem = false
				chunkBorder.opacity = 1
				tooltip.visible = false
            }
        }

		states: [
			State {
				name: "PIN_NORMAL"
				when: rootPanel.choosePinned && !mouseArea.containsMouse
				PropertyChanges{ target: chunkBorder; border.color: palette.lighterShade}
			},
			State {
				name: "PIN_OVER"
				when: rootPanel.choosePinned && mouseArea.containsMouse
				PropertyChanges{ target: chunkBorder; border.color: palette.highlightShade}
			},
			State {
				name: "DRAG_NULL"
				when: draggedPanelDepth > -1 && draggedPanelDepth != panelDepth
				PropertyChanges{ target: chunkBorder; border.color: "transparent"}
			},
			State {
				name: "DRAG_NORMAL"
				when: rootPanel.chunkDragEnabled && !mouseArea.containsMouse
				PropertyChanges{ target: chunkBorder; border.color: palette.lighterShade}
			},
			State {
				name: "DRAG_OVER"
				when: rootPanel.chunkDragEnabled && mouseArea.containsMouse
				PropertyChanges{ target: chunkBorder; border.color: palette.highlightShade}
			},
			State {
				name: "FILTERED"
				when: filtered
				PropertyChanges{ target: chunkBorder; border.color: palette.brightTextColor}
			},
			State {
				name: "NEW_ITEM"
				when: newItem
				PropertyChanges{ target: chunkBorder; border.color: "#99c65d"}
			}

		]
	}

	Layout.fillWidth: true
	Layout.preferredHeight: contentObject.height

	visible: {
		if (rootPanel.expanded_ == 2)
		{
			if(clonedControl && !cloned)
			{
				false
			}
			else
			{
				true
			}
		}
		else if(rootPanel.expanded_ == 1 && pinned)
		{
			true
		}
		else
		{
			false
		}
	}

	onPinnedChanged: {
		if(pinned)
		{
			rootPanel.pinnedChildren += 1
		}
		else
		{
			rootPanel.pinnedChildren -= 1
		}
	}

	onFilteredChanged: {
		if(filtered)
		{
			chunkBorder.state = "FILTERED"
		}
		else
		{
			chunkBorder.state = ""
		}
	}

	Connections{
		target: chunk.rootPanel
		onShowPinned:{
			if(chunk.pinned && chunk.parentPanel.pinned == false)
			{
				chunk.parent = chunk.rootFrame
			}
		}
		onExpanded:{
			if(chunk.parent != chunk.parentFrame)
			{
				chunk.parent = chunk.parentFrame
				chunk.parentPanel.resetPanelOrder()
			}
		}
		onCollapsed:{
			if(chunk.parent != chunk.parentFrame)
			{
				chunk.parent = chunk.parentFrame
				chunk.parentPanel.resetPanelOrder()
			}
		}
	}

	Connections{
		target: chunk.parentPanel
        ignoreUnknownSignals: true
		onCollectChildren: {
			if(chunk.parent != chunk.parentFrame)
			{
				chunk.parent = chunk.parentFrame
			}
		}
	}

	Connections{
		target: chunk.rootPanel.controlFilter
        ignoreUnknownSignals: true
		onTextChanged: {
			if(rootPanel.controlFilter.text == "")
			{
				chunk.filtered = false
			}
			else
			{
				chunk.searchTag(chunk.rootPanel.controlFilter.text)
			}
		}
	}

	Rectangle {
		z: 10
		anchors.horizontalCenter: parent.horizontalCenter
		anchors.verticalCenter: panelChunk ? undefined : parent.verticalCenter
		anchors.top: panelChunk ? parent.top : undefined
		anchors.topMargin: panelChunk ? 7 : 0

		width: defaultSpacing.minimumRowHeight
		height: defaultSpacing.minimumRowHeight

		visible: {
			if(chunk.rootPanel.choosePinned)
			{
				return true
			}
			else if (rootPanel.chunkDragEnabled)
			{
				if(draggedPanelDepth == panelDepth || draggedPanelDepth == -1)
				{
					return true
				}
				else
				{
					return false
				}
			}
			else
			{
				return false
			}
		}
		color: mouseArea.containsMouse ? palette.highlightShade : palette.lighterShade
		radius: defaultSpacing.minimumRowHeight

		Image {
			anchors.centerIn: parent
			source: {
				if (rootPanel.choosePinned)
				{
					if(pinned)
					{
                        return "icons/pinned_16x16.png"
					}
					else
					{
                        return "icons/pin_16x16.png"
					}
				}
				else
				{
                    return "icons/unlock_16x16.png"
				}
			}
		}

		MouseArea {
			id: mouseArea
			z: 10
			anchors.fill: parent
			anchors.margins: -5
			//width: defaultSpacing.standardMargin + defaultSpacing.minimumRowHeight
			enabled: rootPanel.choosePinned || rootPanel.chunkDragEnabled
			propagateComposedEvents: false
			cursorShape: {
				if(rootPanel.choosePinned)
				{
					return Qt.PointingHandCursor
				}
				else if (rootPanel.chunkDragEnabled)
				{
					return Qt.OpenHandCursor
				}
				else
				{
					return Qt.ArrowCursor
				}
			}
			hoverEnabled: true

			drag.target: {
				if(rootPanel.chunkDragEnabled)
				{
					dragObject
				}
				else
				{
					null
				}
			}

			drag.onActiveChanged: {
				if (mouseArea.drag.active)
				{
					draggedObject = chunk
					draggedPanelDepth = panelDepth
					dragObject.parent = baseLayout
					dragSnapshot.sourceItem = contentObject
					dragObject.z = 100
				}

			}

			onPressed: {
				if(rootPanel.chunkDragEnabled)
				{
					dragObject.Drag.hotSpot.x = dragObject.width/2
					dragObject.Drag.hotSpot.y = mouseArea.mapToItem(chunk,mouse.x,mouse.y).y
				}
			}

			onClicked: {
				if(rootPanel.choosePinned)
				{
					chunk.pinned = !chunk.pinned
				}
			}

			onDoubleClicked: {
				if(chunk.pinned && rootPanel.choosePinned)
				{
					pinChildren(chunk, true)
				}
				else if (!chunk.pinned && rootPanel.choosePinned)
				{
					pinChildren(chunk, false)
				}
			}

			onReleased: {
				if(mouseArea.drag.active)
				{
					var response = dragObject.Drag.drop();
					dragObject.x = 0
					dragObject.y = 0
					dragObject.z = 0
					dragSnapshot.sourceItem = null
					dragObject.parent = chunk
					if(response == Qt.IgnoreAction)
					{
						if(clonedControl){
							chunk.pinned = false
							cloned = false
							copiedControls -= 1
						}
						parentPanel.resetPanelOrder()
					}

					draggedObject = null
					draggedPanelDepth = -1
					//console.log("DPD: " + draggedPanelDepth + " PD: " + panelDepth)
				}
			}
		}
	}

	MouseArea {
		anchors.fill: parent
		enabled: newItem
		hoverEnabled: true

		onClicked: {
			mouse.accepted = false
		}
		onPressed: {
			mouse.accepted = false
		}
		onDoubleClicked: {
			mouse.accepted = false
		}
		onEntered: {
			tooltipTimer.start()
			tooltip.x = chunk.mapToItem(baseLayout,mouseX,mouseY).x
			tooltip.y = chunk.mapToItem(baseLayout,mouseX,mouseY).y
		}
		onExited: {
			tooltipTimer.stop()
			tooltip.visible = false
		}
		onEnabledChanged: {
			if(!newItem)
			{
				tooltip.visible = false
				hoverEnabled = false
			}
		}
	}

	Timer {
		id: tooltipTimer
		interval: 200
		running: false
		onTriggered: {
			tooltip.visible = true
		}
	}
}

