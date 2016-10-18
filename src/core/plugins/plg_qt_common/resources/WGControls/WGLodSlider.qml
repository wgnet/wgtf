import QtQuick 2.5
import QtQuick.Controls 1.4
import QtQuick.Layouts 1.3
import QtQuick.Controls.Private 1.0
import QtQuick.Dialogs 1.2
import WGControls.Layouts 1.0

/*!
 \ingroup wgcontrols
 \brief A Level of Detail slider control designed to replace ModelEditors LOD control

Example:
\code{.js}
WGLodSlider {
    Layout.fillWidth: true
}
\endcode
*/

//TODO: An example of this hooked up to real world data is required above

WGColumnLayout {
    id: mainBody
    objectName: "WGLodSlider"
    WGComponent { type: "WGLodSlider" }

    /*! This property holds a Sample LOD data set if used if one isnt explicitly set */
    property QtObject lodList: ListModel{
        ListElement {
            objectName: "testLod1_"
            model_: "testLod1_.model"
            text_: "testLOD1"
            minDist_: 0
            maxDist_: 25
            selected_: false
            hidden_: false
            locked_:false
        }
        ListElement {
            objectName: "testLod2_"
            model_: "testLod2_.model"
            text_: "testLOD2"
            minDist_: 25
            maxDist_: 75
            selected_: false
            hidden_: false
            locked_:false
        }
        ListElement {
            objectName: "testLod3_"
            model_: "testLod3_.model"
            text_: "testLOD3"
            minDist_: 75
            maxDist_: 150
            selected_: false
            hidden_: false
            locked_:false
        }
        ListElement {
            objectName: "testLod4_"
            model_: "testLod4_.model"
            text_: "testLOD4"
            minDist_: 150
            maxDist_: 250
            selected_: false
            hidden_: false
            locked_:false
        }
        ListElement {
            objectName: "testLod5_"
            model_: "testLod5_.model"
            text_: "testLOD5"
            minDist_: 250
            maxDist_: 350
            selected_: false
            hidden_: false
            locked_:false
        }
    }

    /*!
        This property defines the absolute maximum distance a LoD model can be set to
        The default value is \c 999
        */
    property int maxDistanceAbs: 999

    /*!
        This property is determines the width taken up by the WGNumberBox's either side of the LOD control
        The default value is \c 60
    */
    property int valueBoxWidth: 60

    /*!
        This property is determines the width of the gap to the right of the LoD sliders which is used to indicate a
        LoD value goes off to infinity
        The default value is \c 10
    */
    property int rightGapWidth: 10

    /*!
        This property holds the current camera distance
        The default value is \c 0
    */
    property int currentDistance: 0

    /*!
        This property holds the currently selected LOD
    */
    property int selectedLOD: -1

    /*! \internal */
    property real upperBound: {
        if(__infinite >= 0)
        {
            Math.max(lodList.get(lodList.count - 1).maxDist_, rightGapWidth) //if infinite upper value = last max value
        }
        else
        {
            lodList.get(lodList.count - 1).maxDist_ + rightGapWidth //if not infinite upper value = last max value plus an extra gap
        }
    }

    //disable bindings when deleting a LOD to prevent errors
    /*! \internal */
    property bool __deletingLOD: false

    /*! \internal */
    property bool __addingLOD: false

    /*! \internal */
    property int __infinite: -1 //does the last LOD stretch to infinity

    /*! \internal */
    property bool __virtual: false

    /*! \internal */
    property real __unitWidth: (lodFrame.width - (defaultSpacing.standardMargin * 2)) / upperBound

    /*! \internal */
    property bool __changingMin: false

    /*! \internal */
    property bool __changingMax: false

    implicitHeight: defaultSpacing.minimumRowHeight * 5
    implicitWidth: defaultSpacing.standardMargin

    //stop the distance slider overrunning when upperBound_ is changed
    onUpperBoundChanged: {
        if (currentDistance > upperBound){
            currentDistance = upperBound
        }
    }

    onSelectedLODChanged: {
        if (selectedLOD < 0)
        { // if no LOD is selected disable buttons and set hide to false
            var i
            for (i = 0; i < lodOptions.buttonList.count; i++)
            {
                lodOptions.buttonList[index].enabled = false
            }
            button_hide.checked = false
            button_lock_selected_lod.checked = false

        }
        else
        { // A LOD is selected, enable relevant buttons
            button_open.enabled = true
            button_hide.enabled = true
            button_use_camera.enabled = true
            button_lock_selected_lod.enabled = true

            button_lock_selected_lod.checked = lodList.get(selectedLOD).locked_
            button_hide.checked = lodList.get(selectedLOD).hidden_

            if (selectedLOD == 0)
            { //first lod is selected
                lodOptions.buttonList[1].enabled = false
            }
            else
            {
                lodOptions.buttonList[1].enabled = true
            }

            if (selectedLOD == lodList.count - 1)
            { //last LOD is selected
                lodOptions.buttonList[2].enabled = false
            } else
            {
                lodOptions.buttonList[2].enabled = true
            }
        }
    }

    //Checks if any required changes while a lod is being dragged
    function checkValues (index, newVal)
    {
        //checks max and min of current LOD are ok
        if (lodList.get(index).maxDist_ < newVal)
        {
            lodList.set(index,{"maxDist_": newVal})
        }
        else if (lodList.get(index).minDist_ > newVal)
        {
            lodList.set(index,{"minDist_": newVal})
        }

        //set hidden on current LOD
        if (lodList.get(index).maxDist_ == lodList.get(index).minDist_)
        {
            lodList.set(index,{"hidden_": true})
            if(index == selectedLOD)
            {
                button_hide.checked = true
            }
        }
        else
        {
            lodList.set(index,{"hidden_": false})
            if(index == selectedLOD)
            {
                button_hide.checked = false
            }
        }

        //Checks all mins and maxes of LODS closer than current LOD are not higher than the new value
        if (index > 0)
        {
            for(var i = 0; i < index; i++)
            {
                var checkMax = lodList.get(i).maxDist_
                if (checkMax > newVal)
                {
                    lodList.set(i,{"maxDist_": newVal})
                }
                var checkMin = lodList.get(i).minDist_
                if (checkMin > newVal)
                {
                    lodList.set(i,{"minDist_": newVal})
                }
            }
        }

        //Checks all mins and maxes of LODS further than current LOD are not lower than the new value
        if (index < (lodList.count - 1))
        {
            for(var j = index + 1; j < lodList.count; j++)
            {
                var checkMax = lodList.get(j).maxDist_
                if (checkMax < newVal)
                {
                    lodList.set(j,{"maxDist_": newVal})
                }
                var checkMin = lodList.get(j).minDist_
                if (checkMin < newVal)
                {
                    lodList.set(j,{"minDist_": newVal})
                }
            }
        }
    }

    //checks that all the lods are continuous and that the upperBound is correct
    function checkLods()
    {
        for(var i = 0; i < lodList.count; i++)
        {
            if(lodList.get(i).minDist_ > lodList.get(i).maxDist_)
            {
                lodList.set(i,{"maxDist_": lodList.get(i).minDist_})
            }

            if (i != lodList.count - 1)
            {
                if(lodList.get(i).maxDist_ != lodList.get(i + 1).minDist_)
                {
                    lodList.set(i + 1,{"minDist_": lodList.get(i).maxDist_})
                }
            }

            if(lodList.get(i).maxDist_ == lodList.get(i).minDist_)
            {
                lodList.set(i,{"hidden_": true})
                if(i == selectedLOD)
                {
                    button_hide.checked = true
                }
            }
            else
            {
                lodList.set(i,{"hidden_": false})
                if(i == selectedLOD)
                {
                    button_hide.checked = false
                }
            }
        }

        if(__infinite >= 0)
        {
            setInfinite()
        }

        if(__infinite >= 0 && upperBound <= maxDistanceAbs)
        {
            upperBound = Math.max(lodList.get(lodList.count - 1).maxDist_, rightGapWidth) //if infinite upper value = last max value
        }
        else  if (__infinite == -1 && upperBound <= maxDistanceAbs)
        {
            upperBound = lodList.get(lodList.count - 1).maxDist_ + rightGapWidth //if not infinite upper value = last max value plus an extra gap
        }

        if (upperBound > maxDistanceAbs)
        {
            upperBound = maxDistanceAbs
        }
        else if (upperBound < rightGapWidth)
        {
            upperBound = rightGapWidth
        }
    }

    //Moves a LOD in fromIndex up or down the list (direction = +1/-1), count number of times)
    function moveLOD(fromIndex, direction, count)
    {
        if(Math.abs(direction) != 1)
        {
            console.log("Move LOD failed. Direction must be 1 or -1")
            return
        }

        for (var i=0; i < count; i++)
        {
            if (fromIndex == 0 && direction == -1)
            {
                console.log("Reached start of LOD list")
                i = count
            }
            else if (fromIndex == lodList.count -1 && direction == 1)
            {
                console.log("Reached end of LOD list")
                i = count
            }

            var toIndex = fromIndex + direction
            var toModel = lodList.get(toIndex).model_
            var toName = lodList.get(toIndex).text_

            lodList.set(toIndex, {"model_": lodList.get(fromIndex).model_, "text_": lodList.get(fromIndex).text_,})
            lodList.set(fromIndex, {"model_": toModel, "text_": toName,})

            var fromWidth = lodList.get(fromIndex).maxDist_ - lodList.get(fromIndex).minDist_
            var toWidth = lodList.get(toIndex).maxDist_ - lodList.get(toIndex).minDist_

            if (direction == 1)
            {
                lodList.set(fromIndex, {"maxDist_": lodList.get(fromIndex).minDist_ + toWidth})
            }
            else if (direction == -1)
            {
                lodList.set(toIndex, {"maxDist_": lodList.get(toIndex).minDist_ + fromWidth})
            }

            checkLods()
            fromIndex += direction
        }
    }

    //Change a LOD's model and name
    function changeLOD(filename)
    {
        //get the trimmed modelname from the URL
        var lastSlash = filename.toString().lastIndexOf("/")
        var lastDot = filename.toString().lastIndexOf(".")
        var trimmedName = filename.toString().substring(lastSlash + 1, lastDot)
    }

    //add a new LOD
    function addLOD(filename)
    {
        //get the trimmed modelname from the URL
        var lastSlash = filename.toString().lastIndexOf("/")
        var lastDot = filename.toString().lastIndexOf(".")
        var trimmedName = filename.toString().substring(lastSlash + 1, lastDot)

        var oldMaxDist = lodList.get(lodList.count - 1).maxDist_

        lodList.append({
                    "model_": filename.toString(),
                    "text_": trimmedName,
                    "minDist_": oldMaxDist,
                    "maxDist_": oldMaxDist + rightGapWidth
                        })
        __addingLOD = false
        checkLods()
        selectedLOD = lodList.count - 1
    }

    //Set hidden_ and change LOD width
    function hideLOD(index)
    {
        if(lodList.get(index).hidden_)
        {
            lodList.set(index,{"hidden_": false, "maxDist_": lodList.get(index).maxDist_ + 10})
        } else
        {
            lodList.set(index,{"hidden_": true, "maxDist_": lodList.get(index).minDist_})
        }
        checkValues(index, lodList.get(index).maxDist_)
        checkLods()
    }

    //Delete a LOD and shuffle everything up.
    function deleteLOD(index)
    {
        __deletingLOD = true

        var deletedWidth = lodList.get(index).maxDist_ - lodList.get(index).minDist_
        for (var i = index + 1; i < lodList.count - 1; i++)
        {
            lodList.set(i,{"minDist_": lodList.get(i).minDist_ - deletedWidth, "maxDist_": lodList.get(i).maxDist_ - deletedWidth})
        }

        lodStack.children[index].Layout.row += 1

        lodList.remove(index)

        checkLods()

        selectedLOD = -1
        __deletingLOD = false
    }

    function setInfinite()
    {
        __infinite = -1
        for (var i = lodList.count - 1; i >= 0; i--)
        {
            if (lodList.get(i).minDist_ != lodList.get(i).maxDist_)
            {
                __infinite = i
                i = -1
            }
        }
    }

    //Labels for value boxes and bar list.
    WGExpandingRowLayout {
        Layout.fillWidth: true
        spacing: defaultSpacing.rowSpacing

        WGLabel {
            text: "Min Dist (m)"
            width: valueBoxWidth
        }

        WGLabel {
            text: "LOD List"
            Layout.fillWidth: true
            horizontalAlignment: Text.AlignHCenter
            font.bold: true
        }

        WGLabel {
            text: "Max Dist (m)"
            width: valueBoxWidth
        }

    }

    WGExpandingRowLayout {

        Layout.fillWidth: true
        spacing: defaultSpacing.rowSpacing

        WGColumnLayout {
            id: minColumn

            Repeater {
                // Stack of minimum distance number boxes
                model: lodList
                delegate: WGNumberBox {
                    value: model.minDist_
                    Layout.preferredWidth: valueBoxWidth
                    minimumValue: 0
                    maximumValue: index > 0 ? maxDistanceAbs : 0 //min distance on LOD 0 should always be 0

                    textColor: {
                        if(enabled && index == selectedLOD && !activeFocus)
                        {
                            palette.highlightColor
                        }
                        else if(enabled && !readOnly)
                        {
                            palette.textColor
                        }
                        else if(enabled && readOnly)
                        {
                            palette.neutralTextColor
                        }
                        else
                        {
                            palette.disabledTextColor
                        }
                    }


                    onValueChanged: {
                        if (!__changingMin && !__changingMax && index > 0)
                        {
                            //change this LOD's min distance and the LOD above's max distance
                            if (dragging_)
                            {
                                __changingMin = true
                                lodList.set(index,{"minDist_": value})
                                lodList.set(index-1,{"maxDist_": value})
                                checkValues(index, value)
                                checkLods()
                                __changingMin = false
                            }
                        }
                    }

                    onEditingFinished: {
                        __changingMin = true
                        lodList.set(index,{"minDist_": value})
                        lodList.set(index-1,{"maxDist_": value})
                        checkValues(index, value)
                        checkLods()
                        __changingMin = false
                    }

                    readOnly: index == 0 ? true : false

                    Connections {
                        target: __deletingLOD ? null : lodList.get(index)
                        onMinDist_Changed: {
                            value = minDist_
                        }
                    }
                }
            }
        }

        WGColumnLayout { // Area containing LOD slider bars

            WGTextBoxFrame {
                id: lodFrame
                Layout.fillWidth: true
                //(Number of Lods * rowHeight) + spacing between rows + top & bottom margins
                Layout.preferredHeight: (lodList.count * defaultSpacing.minimumRowHeight) + ((lodList.count - 1) * defaultSpacing.rowSpacing) + (defaultSpacing.standardMargin * 2)

                clip: true

                Rectangle {
                    id: cameraDistanceBar
                    color: palette.textColor
                    opacity: 0.3
                    width: defaultSpacing.standardBorderSize
                    anchors.top: parent.top
                    anchors.bottom: parent.bottom
                    z: 1
                    x: (currentDistance * __unitWidth) + defaultSpacing.standardMargin
                }

                GridLayout {
                    id: lodStack
                    anchors.fill: parent
                    anchors.margins: defaultSpacing.standardMargin
                    rowSpacing: defaultSpacing.rowSpacing
                    columnSpacing: 0

                    Repeater {
                        model: lodList
                        delegate: Rectangle {
                            id: colorBar

                            property bool highlighted: {
                                if (!__deletingLOD)
                                {
                                    lodList.get(index).selected_
                                } else
                                {
                                    false
                                }
                            }

                            Connections {
                                target: mainBody
                                onSelectedLODChanged: {
                                    if (target.selectedLOD == index)
                                    {
                                        highlighted = true
                                    } else
                                    {
                                        highlighted = false
                                    }
                                }
                            }

                            border.width: highlighted ? defaultSpacing.standardBorderSize : 0
                            border.color: highlighted ? palette.textColor : "transparent"

                            //3 repeating colours
                            color: {
                                var barColor = "transparent"
                                if (!__deletingLOD)
                                {
                                    var colorCount = index % 3
                                    if (colorCount == 0)
                                    {
                                        barColor = "#e23627"
                                    }
                                    else if (colorCount == 1)
                                    {
                                        barColor = "#7ac943"
                                    }
                                    else if (colorCount == 2)
                                    {
                                        barColor = "#3fa9f5"
                                    }
                                }
                                return barColor
                            }

                            opacity: model.maxDist_ === model.minDist_ ? 0.5 : 1
                            radius: defaultSpacing.halfRadius

                            Layout.preferredHeight: defaultSpacing.minimumRowHeight
                            Layout.preferredWidth: {
                                if(!__deletingLOD)
                                {
                                    if (lodList.get(index).maxDist_ == lodList.get(index).minDist_)
                                    {
                                        0
                                    }
                                    else
                                    {
                                        (lodList.get(index).maxDist_ - lodList.get(index).minDist_) * __unitWidth
                                    }
                                }
                                else
                                {
                                    0
                                }
                            }

                            Layout.fillWidth: true
                            Layout.row: index
                            Layout.column: index

                            //click to select bar
                            MouseArea {
                                anchors.centerIn: parent
                                width: parent.width - defaultSpacing.doubleMargin
                                height: parent.height
                                hoverEnabled: true
                                cursorShape: Qt.PointingHandCursor

                                onClicked: {
                                    selectedLOD = index
                                    lodList.set(index, {"selected_": true})
                                    parent.focus = true
                                }
                            }

                            //model name next to the bar. Click to change the model
                            Text {
                                objectName: "barName"
                                id: barName
                                color: state == "" || parent.highlighted ? palette.textColor : parent.color
                                text: __deletingLOD ? "" : lodList.get(index).text_
                                opacity: state == "" || parent.highlighted  ? 1 : 0.5
                                y: defaultSpacing.standardBorderSize
                                horizontalAlignment: Text.AlignLeft
                                x: defaultSpacing.standardMargin

                                font.bold: parent.highlighted ? true : false

                                states: [
                                    State {
                                        name: "RIGHT"
                                        when: ((upperBound * __unitWidth) - (lodList.get(index).maxDist_ * __unitWidth) > barName.paintedWidth)

                                        PropertyChanges {target: barName; x: parent.width + defaultSpacing.standardMargin}
                                        PropertyChanges {target: barName; horizontalAlignment: Text.AlignLeft}
                                        PropertyChanges {target: barName; visible: true}
                                    },
                                    State {
                                        name: "LEFT"
                                        when: ((lodList.get(index).minDist_ * __unitWidth > barName.paintedWidth))

                                        PropertyChanges {target: barName; x: -barName.paintedWidth - defaultSpacing.standardMargin}
                                        PropertyChanges {target: barName; horizontalAlignment: Text.AlignRight}
                                        PropertyChanges {target: barName; visible: true}
                                    }
                                ]

                                MouseArea {
                                    anchors.centerIn: parent
                                    height: parent.height
                                    width: parent.width - defaultSpacing.doubleBorderSize
                                    hoverEnabled: true

                                    cursorShape: Qt.PointingHandCursor

                                    onClicked: {
                                        selectedLOD = index
                                        lodList.set(index, {"selected_": true})
                                    }
                                }

                            }

                            //LOD number inside LOD bar, greys out and italics when LOD is zero width (hidden)
                            Text {
                                id: lodNumber
                                text: (__infinite == index) ? "∞" : index
                                color: model.maxDist_ === model.minDist_ ? palette.disabledTextColor : palette.textColor
                                anchors.centerIn: parent
                                horizontalAlignment: Text.AlignHCenter
                                font.pixelSize: (__infinite == index) ? 22 : 12
                                font.bold: true
                                font.italic: model.maxDist_ === model.minDist_ && __infinite != index ? true : false
                            }

                            //draggable handle on left side of LOD bar
                            MouseArea {
                                id: minDrag
                                height: parent.height
                                width: defaultSpacing.doubleMargin
                                anchors.verticalCenter: parent.verticalCenter
                                anchors.horizontalCenter: parent.left
                                hoverEnabled: true
                                cursorShape: Qt.SizeHorCursor

                                //no minimum handle on first LOD
                                visible: index > 0 ? true : false

                                z: 1

                                onMouseXChanged: {
                                    if (pressed)
                                    {
                                        if(index > 0)
                                        {
                                            //map mouse to inside LOD Frame Box
                                            var pos = mapToItem(lodFrame,mouse.x, mouse.y)
                                            var newVal = 0
                                            if (__infinite >= 0)
                                            {
                                                if (pos.x > 0 && pos.x < lodFrame.width)
                                                { //move value to position of mouse inside Frame Box
                                                    newVal = Math.round((pos.x / lodFrame.width) * upperBound)
                                                } else if (pos.x <= 0)
                                                { //set value to 0 if mouse to left of Frame Box
                                                    newVal = 0
                                                } else if (pos.x >= lodFrame.width)
                                                { //set value to max if mouse to right of Frame Box
                                                    newVal = upperBound
                                                }
                                            }
                                            else
                                            {
                                                if (pos.x > 0 && pos.x < (lodFrame.width - rightGapWidth))
                                                { //move value to position of mouse inside Frame Box and extra gap at max
                                                    newVal = Math.round((pos.x / lodFrame.width) * upperBound)
                                                }
                                                else if (pos.x <= 0)
                                                { //set value to 0 if mouse to left of Frame Box
                                                    newVal = 0
                                                }
                                                else if (pos.x >= (lodFrame.width - rightGapWidth))
                                                { //increase the upper bound and set value to upper bound

                                                    //TODO:: This should probably not be per tick but on some nice real time value
                                                    if (upperBound < maxDistanceAbs)
                                                    {
                                                        upperBound += 1
                                                    }
                                                    newVal = upperBound
                                                }
                                            }

                                            __changingMin = true

                                            //update this LODS min value and the previous LODS max value

                                            lodList.set((index - 1),{"maxDist_": newVal})
                                            lodList.set((index),{"minDist_": newVal})

                                            checkValues(index, newVal)
                                        }
                                    }
                                }

                                onPressed: {
                                    preventStealing = true
                                }

                                onReleased: {
                                    preventStealing = false

                                    checkLods()

                                    __changingMin = false
                                    __changingMax = false
                                }


                            }

                            //draggable handle on right side of LOD bar
                            MouseArea {
                                id: maxDrag
                                height: parent.height
                                width: defaultSpacing.doubleMargin
                                anchors.verticalCenter: parent.verticalCenter
                                anchors.horizontalCenter: parent.right

                                cursorShape: Qt.SizeHorCursor

                                //no minimum handle on last LOD if infinite = true
                                visible: {
                                    if (__infinite == -1)
                                    {
                                        true
                                    }
                                    else if (__infinite >= 0 && index < lodList.count - 1)
                                    {
                                        true
                                    }
                                    else
                                    {
                                        false
                                    }
                                }

                                z: 1

                                onMouseXChanged: {
                                    if (pressed)
                                    {
                                        //map mouse to inside LOD Frame Box
                                        var pos = mapToItem(lodFrame,mouse.x, mouse.y)
                                        var newVal = 0
                                        if (__infinite >= 0)
                                        {
                                            if (pos.x > 0 && pos.x < lodFrame.width)
                                            { //move value to position of mouse inside Frame Box
                                                newVal = Math.round((pos.x / lodFrame.width) * upperBound)
                                            }
                                            else if (pos.x <= 0)
                                            { //set value to 0 if mouse to left of Frame Box
                                                newVal = 0
                                            }
                                            else if (pos.x >= lodFrame.width)
                                            { //set value to max if mouse to right of Frame Box
                                                newVal = upperBound
                                            }
                                        }
                                        else
                                        {
                                            if (pos.x > 0 && pos.x < (lodFrame.width - rightGapWidth))
                                            { //move value to position of mouse inside Frame Box and extra gap at max
                                                newVal = Math.round((pos.x / lodFrame.width) * upperBound)
                                            }
                                            else if (pos.x <= 0)
                                            { //set value to 0 if mouse to left of Frame Box
                                                newVal = 0
                                            }
                                            else if (pos.x >= (lodFrame.width - rightGapWidth))
                                            { //increase the upper bound and set value to upper bound

                                                //TODO:: This should probably not be per tick but on some nice real time value
                                                if (upperBound < maxDistanceAbs)
                                                {
                                                    upperBound += 1
                                                }
                                                newVal = upperBound
                                            }
                                        }

                                        __changingMax = true

                                        //update this LODS max value and the next LODS min value
                                        if (index < lodList.count - 1)
                                        {
                                            lodList.set((index + 1),{"minDist_": newVal})
                                        }

                                        lodList.set((index),{"maxDist_": newVal})

                                        checkValues(index, newVal)
                                    }
                                }

                                onPressed: {
                                    preventStealing = true
                                }

                                onReleased: {
                                    preventStealing = false

                                    checkLods()

                                    __changingMin = false
                                    __changingMax = false
                                }
                            }
                        }
                    }

                    //invisible gap if slider != infinite
                    Rectangle {
                        id: finiteSpacer
                        color: "transparent"
                        Layout.preferredHeight: defaultSpacing.minimumRowHeight
                        Layout.preferredWidth: {
                            if (__infinite >= 0 && lodList.get(lodList.count - 1).maxDist_ >= rightGapWidth)
                            {
                                0
                            }
                            else if (__infinite >= 0 && lodList.get(lodList.count - 1).maxDist_ <= rightGapWidth)
                            {
                                (rightGapWidth - lodList.get(lodList.count - 1).maxDist_) * __unitWidth
                            }
                            else
                            {
                                rightGapWidth * __unitWidth
                            }
                        }
                        Layout.column: lodList.count
                        Layout.row: lodList.count - 1
                    }
                }
            }
        }

        WGColumnLayout {
            id: maxColumn

            Repeater {
                // Stack of maximum distance number boxes
                model: lodList
                delegate: WGNumberBox {
                    value: model.maxDist_
                    Layout.preferredWidth: valueBoxWidth
                    minimumValue: 0
                    maximumValue: __infinite >= 0 ? maxDistanceAbs : maxDistanceAbs - rightGapWidth

                    textColor: {
                        if (enabled && index == selectedLOD && !activeFocus)
                        {
                            palette.highlightColor
                        }
                        else if (enabled && !readOnly)
                        {
                            palette.textColor
                        }
                        else if (enabled && readOnly)
                        {
                            palette.neutralTextColor
                        }
                        else
                        {
                            palette.disabledTextColor
                        }
                    }

                    onValueChanged: {
                        if (!__changingMin && !__changingMax)
                        {
                            //change this LOD's max distance and the LOD before's max distance
                            if (dragging_)
                            {
                                __changingMax = true
                                lodList.set(index,{"maxDist_": value})

                                if (index < lodList.count - 1)
                                {
                                    lodList.set(index + 1,{"minDist_": value})
                                }

                                checkValues(index, value)
                                checkLods()
                                __changingMax = false
                            }
                        }
                    }

                    onEditingFinished: {
                        __changingMax = true
                        lodList.set(index,{"maxDist_": value})

                        if (index < lodList.count - 1)
                        {
                            lodList.set(index + 1,{"minDist_": value})
                        }

                        checkValues(index, value)
                        checkLods()
                        __changingMax = false
                    }

                    Connections {
                        target: __deletingLOD ? null : lodList.get(index)
                        onMaxDist_Changed: {
                            value = maxDist_
                        }
                    }
                }
            }
        }


        //Delete LOD buttons. Disabled if only one LOD left.
        WGColumnLayout {
            Repeater {
                model: lodList
                delegate: WGToolButton {
                    enabled: lodList.count > 1 && index == selectedLOD
                    iconSource: "icons/delete_16x16.png"
                    tooltip: "Delete Selected LOD"
                    onClicked: {
                        deleteLOD(index)
                    }
                }
            }
        }
    }

    WGSeparator{}

    //Labels for Distance slider & toggle options
    WGExpandingRowLayout {
        Layout.fillWidth: true
        spacing: defaultSpacing.rowSpacing

        WGLabel {
            text: "Dist (m)"
            Layout.preferredWidth: minColumn.width
        }

        WGExpandingRowLayout {
            Layout.maximumWidth: lodFrame.width
            Layout.minimumWidth: lodFrame.width


            WGButtonBar {
                objectName: "WGButtonBar"
                id: lodOptions
                Layout.fillWidth: true
                buttonList: [
                    WGPushButton {
                        objectName: "buttonOpen"
                        id: button_open
                        iconSource: "icons/open_16x16.png"
                        tooltip: "Open Model"
                        enabled: false
                        onClicked: chooseLODModelDialog.open()
                    },
                    WGPushButton {
                        objectName: "buttonUp"
                        id: button_up
                        iconSource: "icons/arrow2_up_16x16.png"
                        tooltip: "Move LOD Up"
                        enabled: false

                        onClicked: {
                            moveLOD(selectedLOD,-1,1)
                            selectedLOD -= 1
                        }
                    },
                    WGPushButton {
                        objectName: "buttonDown"
                        id: button_down
                        iconSource: "icons/arrow2_down_16x16.png"
                        tooltip: "Move LOD Down"
                        enabled: false
                        onClicked: {
                            moveLOD(selectedLOD,1,1)
                            selectedLOD += 1
                        }
                    },
                    WGPushButton {
                        objectName: "buttonHide"
                        id: button_hide
                        iconSource: checked ? "icons/show_16x16.png" : "icons/hide_16x16.png"
                        tooltip: checked ? "Unhide LOD" : "Hide LOD"
                        enabled: false
                        checkable: true
                        checked: false
                        onClicked: {
                            hideLOD(selectedLOD)
                        }
                    },
                    WGPushButton {
                        objectName: "buttonUseCamera"
                        id: button_use_camera
                        iconSource: "icons/camera_dist_16x16.png"
                        tooltip: "Use Current Camera Distance"
                        enabled: false
                        onClicked: { //ToDo Test if the lod is locked
                            lodList.set(selectedLOD, {"maxDist_": currentDistance})
                            checkValues(selectedLOD, lodList.get(selectedLOD).maxDist_)
                            checkLods()
                        }
                    },
                    WGPushButton {
                        objectName: "buttonLockSelectedLod"
                        id: button_lock_selected_lod
                        iconSource: "icons/lock_16x16.png"
                        tooltip: "Lock the selected LOD"
                        enabled: false
                        checkable: true
                        checked: false
                        onClicked: {
                            if (lodList.get(selectedLOD).locked_ == true)
                            {
                                lodList.set(selectedLOD, {"locked_": false})
                            }
                            else
                            {
                                lodList.set(selectedLOD, {"locked_": true})
                            }
                        }
                    }
                ]
            }


            WGPushButton {
                objectName: "buttonExtendLOD"
                iconSource: "icons/infinite_16x16.png"
                Layout.preferredWidth: implicitWidth
                tooltip: "Extend LOD to Infinity"
                checkable: true
                checked: __infinite >= 0
                onClicked: {
                    if(checked)
                    {
                        setInfinite()
                        checkLods()
                    }
                    else
                    {
                        __infinite = -1
                        checkLods()
                    }
                }
            }

            WGPushButton {
                objectName: "buttonAddNewLOD"
                iconSource: "icons/add_16x16.png"
                Layout.preferredWidth: implicitWidth
                tooltip: "Add New LOD"
                onClicked: {
                    __addingLOD = true
                    chooseLODModelDialog.open()
                }
            }
        }

        WGLabel {
            text: "Max Dist (m)"
            Layout.preferredWidth: maxColumn.width
        }
    }

    //Distance slider and values
    WGExpandingRowLayout {
        Layout.fillWidth: true
        spacing: defaultSpacing.rowSpacing

        WGNumberBox {
            objectName: "cameraDistance"
            //Camera distance
            id: currentDistanceNum
            Layout.preferredWidth: valueBoxWidth
            value: currentDistance
            minimumValue: 0
            maximumValue: upperBound

            onValueChanged: {
                currentDistance = value
            }

            Connections {
                target: mainBody
                onCurrentDistanceChanged: {
                    currentDistanceNum.value = target.currentDistance
                }
            }
        }

        //camera distance control
        WGSliderControl {
            objectName: "cameraDistanceSlider"
            id: distanceSlider
            Layout.fillWidth: true
            minimumValue: 0
            maximumValue: upperBound
            value: currentDistance
            decimals: 0

            //no value box because we want it on the left
            showValue: false

            onValueChanged: {
                currentDistance = value
            }

            Connections {
                target: mainBody
                onCurrentDistanceChanged: {
                    distanceSlider.value = target.currentDistance
                }
            }
        }

        //uneditable textbox that shows the upperBound
        WGNumberBox {
            id: upperBoundNum
            Layout.preferredWidth: valueBoxWidth
            value: upperBound
            minimumValue: upperBound
            maximumValue: upperBound
            readOnly: true

            Connections {
                target: mainBody
                onUpperBoundChanged: {
                    upperBoundNum.value = target.upperBound
                }
            }
        }

        //add LOD button
        WGToolButton {
            objectName: "buttonVirtualLODDistance"
            iconSource: checked ? "icons/camera_lock_16x16.png" : "icons/camera_16x16.png"
            checkable: true
            checked: false
            tooltip: "Virtual LOD Distance"
            onClicked: {
                __virtual = checked
            }
        }
    }

    FileDialog {
        objectName: "chooseLODModelDialog"
        id: chooseLODModelDialog
        title: "Choose a model"
        visible: false
        nameFilters: [ "Model files (*.model)" ]
        onAccepted: {
            if (__addingLOD)
            {
                addLOD(chooseLODModelDialog.fileUrl)
            } else
            {
                changeLOD(chooseLODModelDialog.fileUrl)
            }
        }
        onRejected:
        {
            console.log("Open Model Cancelled")
        }
    }

    /*! Deprecated */
        property alias lodList_: mainBody.lodList

        /*! Deprecated */
        property alias maxDistanceAbs_: mainBody.maxDistanceAbs

        /*! Deprecated */
        property alias valueBoxWidth_: mainBody.valueBoxWidth

        /*! Deprecated */
        property alias rightGapWidth_: mainBody.rightGapWidth

        /*! Deprecated */
        property alias currentDistance_: mainBody.currentDistance

        /*! Deprecated */
        property alias upperBound_: mainBody.upperBound

        /*! Deprecated */
        property alias deleting_: mainBody.__deletingLOD

        /*! Deprecated */
        property alias addLOD_: mainBody.__addingLOD

        /*! Deprecated */
        property alias selectedLOD_: mainBody.selectedLOD

        /*! Deprecated */
        property alias infinite_: mainBody.__infinite

        /*! Deprecated */
        property alias virtual_: mainBody.__virtual

        /*! Deprecated */
        property alias unitWidth_: mainBody.__unitWidth

        /*! Deprecated */
        property alias changingMin_: mainBody.__changingMin

        /*! Deprecated */
        property alias changingMax_: mainBody.__changingMax
}
