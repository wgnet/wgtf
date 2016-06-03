import QtQuick 2.5
import QtQuick.Controls 1.2
import QtQuick.Layouts 1.0

/*!
    A delegate for the ListView inside WGTimelineControl to show a WGTimelineBarSlider
*/

WGTimelineBarSlider {
    id: barSlider

    property QtObject view
    property QtObject grid
    property QtObject rootFrame

    property bool barSelected: false

    // tell the view a drag has started
    onBeginDrag: {
        view.itemDragging = true
        __handlePosList[index].handleDragging = true
    }

    // reset when the drag has stopped
    onEndDrag: {
        view.itemDragging = false
        __handlePosList[index].handleDragging = false
        initialValues = []
    }

    // tell the view a bar is being dragged
    onBarDragging: {
        view.itemDragging = true
        view.itemDragged(delta, false, false, true)
    }

    onBarEndDragging: {
        view.itemDragging = false
    }

    onBarPressed: {
        // add bar to selection
        if (modifiers == Qt.ShiftModifier)
        {
            if (view.selectedBars.indexOf(barIndex) == -1)
            {
                view.selectedBars.push(barIndex)
            }
        }

        // add or remove bar from selection
        else if (modifiers == Qt.ControlModifier)
        {
            var barIndexLocation = view.selectedBars.indexOf(barIndex)
            if (barIndexLocation == -1)
            {
                view.selectedBars.push(barIndex)
            }
            else
            {
                view.selectedBars.splice(barIndexLocation, 1)
            }
        }
        // clear bar and handle selection and re add only this bar if not already selected
        else
        {
           if (view.selectedBars.indexOf(barIndex) == -1)
            {
               view.selectedBars = [barIndex]
               view.selectedHandles = []
            }
        }
        view.selectionChanged();

    }

    // if a handle is dragged, tell the view and whether or not it's a min or max handle
    onChangeValue: {
        var handle = barSlider.__handlePosList[index]
        if (handle.handleDragging)
        {
            var handleDelta = (handle.value - barSlider.initialValues[index]) * (view.width / rootFrame.totalFrames)
            view.itemDragged(handleDelta, !handle.maxHandle, handle.maxHandle, false)
        }
    }

    onHandleClicked: {
        // pre-emptively populate the initial drag values before any dragging
        initialValues[0] = __handlePosList[0].value
        initialValues[1] = __handlePosList[1].value

        // reset selection if this bar is not selected.
        if (view.selectedBars.indexOf(barSlider.barIndex) == -1)
        {
            view.selectedHandles = []
            view.selectedBars = []
            view.selectionChanged()
        }
    }

    Connections {
        target: view
        // check to see if the bar is still selected or not and then auto-select the handles
        onSelectionChanged: {
            if (view.selectedBars.indexOf(barSlider.barIndex) != -1)
            {
                barSlider.updateSelection(true)
                barSlider.__handlePosList[0].selected = true
                barSlider.__handlePosList[1].selected = true
                barSelected = true
            }
            else
            {
                barSlider.updateSelection(false)
                barSlider.__handlePosList[0].selected = false
                barSlider.__handlePosList[1].selected = false
                barSelected = false
            }
        }

        onSelectAll: {
            if (view.selectedBars.indexOf(barSlider.barIndex) == -1)
            {
                view.selectedBars.push(barIndex)
            }
        }

        // if anything in the view starts dragging, populate the initial values
        onItemDraggingChanged: {
            if (view.itemDragging && view.selectedBars.indexOf(barSlider.barIndex) != -1)
            {
                barSlider.initialValues[0] = barSlider.__handlePosList[0].value
                barSlider.initialValues[1] = barSlider.__handlePosList[1].value
            }
        }

        onItemDragged: {
            // if this bar is selected
            if (view.selectedBars.indexOf(barSlider.barIndex) != -1)
            {
                // convert delta in pixels to delta in value
                var clampedDelta = delta / (view.width / rootFrame.totalFrames)

                // if a bar is moved, or a non-min or max handle is moved, move the whole bar (both handles)
                if (bar || (!minHandle && !maxHandle && !bar))
                {
                    // make sure neither handle is out of range so duration stays the same
                    if(barSlider.initialValues[0] + clampedDelta < barSlider.minimumValue)
                    {
                        clampedDelta = barSlider.minimumValue - barSlider.initialValues[0]
                    }
                    else if (barSlider.initialValues[1] + clampedDelta > barSlider.maximumValue)
                    {
                        clampedDelta = barSlider.maximumValue - barSlider.initialValues[1]
                    }
                    barSlider.setHandleValue(barSlider.initialValues[0] + clampedDelta, 0)
                    barSlider.setHandleValue(barSlider.initialValues[1] + clampedDelta, 1)
                }
                // if a min or max handle moves, move the appropriate handle only.
                else if (minHandle && !bar)
                {
                    barSlider.setHandleValue(barSlider.initialValues[0] + clampedDelta, 0)
                }
                else if (maxHandle && !bar)
                {
                    barSlider.setHandleValue(barSlider.initialValues[1] + clampedDelta, 1)
                }
            }
        }
    }

    WGContextArea {
        id: sliderContextMenu

        parent: barArea
        anchors.fill: parent
        anchors.leftMargin: 3
        anchors.rightMargin: 3

        //if within the bar, update selection and open context menu.
        onPressed: {
            view.selectedBars = [barSlider.barIndex]
            view.selectedHandles = []
            view.selectionChanged()
        }

        Action {
            id: setStartEndTime
            text: qsTr("Set Start and End Time")
            shortcut: "Ctrl+T"
            //iconName: "setTime"

            onTriggered: {
                view.changeTime(barSlider.__handlePosList[0], barSlider.__handlePosList[1])
            }
        }

        contextMenu: WGMenu {

            MenuItem {
                action: setStartEndTime
            }
        }
    }

    Connections {
        target: grid

        // check to see if bar is selected
        onPreviewSelectArea: {

            // find the bar area
            var minPoint = barSlider.mapToItem(grid,barSlider.__handlePosList[0].range.position,0)
            var maxPoint = barSlider.mapToItem(grid,barSlider.__handlePosList[1].range.position,barSlider.height)

            var barSelected = view.checkSelection(min,max,minPoint,maxPoint)

            // add or remove selections as necessary
            // is it causing poor performance to do this onPreviewSelectArea???
            var barIndexLocation = -1
            if (barSelected)
            {
                barIndexLocation = view.selectedBars.indexOf(barSlider.barIndex)
                if (barIndexLocation == -1)
                {
                    view.selectedBars.push(barSlider.barIndex)
                    view.selectionChanged()
                }
            }
            else
            {
                barIndexLocation = view.selectedBars.indexOf(barSlider.barIndex)
                if (barIndexLocation != -1)
                {
                    view.selectedBars.splice(barIndexLocation, 1)
                    view.selectionChanged()
                }
            }
        }
    }
}
