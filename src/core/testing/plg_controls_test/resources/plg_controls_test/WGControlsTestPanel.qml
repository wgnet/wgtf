import QtQuick 2.3
import QtQuick.Controls 1.2
import QtQuick.Layouts 1.0
import WGControls 1.0

/**
 *	Sample panel with all the available controls and styles.
 *	Used for control testing
 */
Rectangle {
    id: mainWindow

    visible: true
    property var title: qsTr("QML Controls Test Window")

    color: palette.mainWindowColor
    function getColor(vectorColor) {
        return Qt.rgba(vectorColor.x / 255, vectorColor.y / 255, vectorColor.z / 255, vectorColor.w / 255);
    }
    property var colorArray: []
    property var positionArray: []

    WGListModel {
        id : colorModel
        source : colorSource
        ValueExtension {}
        ColumnExtension {}
    }
    WGListModel {
        id : positionModel
        source : positionSource
        ValueExtension {}
        ColumnExtension {}
    }

    signal colorsUpdated()

    Component.onCompleted: {
        var colorCount = colorModel.rowCount(null);
        console.log("===color count===" + colorCount )
        if(colorCount == 0)
            colorArray = [Qt.rgba(0.5,0,0,1), Qt.rgba(1,0,0,1), Qt.rgba(1,1,0,1)]
        else
        {
            var colors = [];
            for(var i = 0; i < colorCount; ++i)
            {
                var modelIndex = colorModel.index( i );
                console.log("===modelIndex ===" + modelIndex )
                var vec4 = colorModel.data( modelIndex, "Value" );
                console.log("===vec4 ===" + vec4 )
                var color = getColor(vec4);


                console.log("===color===" + color )
                colors[i] = color;
            }
            colorArray = colors;
        }

        var positionCount = positionModel.rowCount(null);
        if(positionCount == 0)
            positionArray = [20, 40, 60]
        else
        {
            var positions = [];
            for(var i = 0; i < positionCount; ++i)
            {
                var modelIndex = positionModel.index( i );
                var position = positionModel.data( modelIndex, "Value" );
                positions[i] = position;
            }
            positionArray = positions;
        }
        mainWindow.colorsUpdated()
    }

    WGBusyIndicator {
        id: busyIndicator1
        z:100
        anchors.centerIn: parent
        running: false
    }

    WGScrollPanel {

        childObject :
        WGDraggableColumn {

            WGSubPanel {
                text: "Form Layout"
                headerObject :
                    WGExpandingRowLayout {
                        Rectangle {
                            color: "transparent"
                            Layout.fillWidth: true
                        }

                        WGPushButton {
                            text: checked ? "Turn Panel Dragging Off" : "Turn Panel Dragging On"
                            checkable: true
                            onClicked: {
                                globalSettings.dragLocked = !globalSettings.dragLocked
                                console.log(globalSettings.dragLocked)
                            }
                        }
                    }
                childObject :
                    WGFormLayout {
                        id: topForm
                        localForm: true

                        WGColumnLayout {
                            Layout.fillWidth: true
                            WGMultiLineText {
                                Layout.fillWidth: true
                                text: "This panel demonstrates the current QML controls that have been created.
Note: A lot of the functionality such as panels, draggable columns, form layouts etc. are QML implementations only and while ok for small panels, are probably not ideal as an application wide solution.

It is recommended to look at the generic_app_test to view other controls such as the Asset Browser, Active Filters, reflected properties etc."
                            }
                        }

                        WGLabel {
                            text: "Import Files"
                        }

                        WGExpandingRowLayout {
                            Layout.fillWidth: true
                            label: "Filename:"

                            Column {
                                Layout.fillWidth: true
                                Layout.minimumHeight: openImage.height

                                WGTextBox {
                                  id: filenameBox
                                  anchors{left: parent.left; right:parent.right;}
                                }

                                WGExpandingRowLayout {
                                    anchors{left: parent.left; right:parent.right;}
                                    height: defaultSpacing.minimumRowHeight

                                    WGPushButton {
                                        text: "Open"
                                        Layout.fillWidth: true
                                        onClicked:{
                                            openImage.clicked();
                                        }
                                    }

                                    WGPushButton {
                                        text: "Import"
                                        Layout.fillWidth: true
                                        onClicked:{
                                            openImage.clicked();
                                        }
                                    }
                                }

                                    WGVerticalSpacer{

                                        anchors{left: parent.left; right:parent.right;}
                                    }
                                }

                            WGThumbnailButton {
                                id: openImage
                                anchors.top: parent.top
                                defaultText: "Click to Load an Image"
                                Layout.preferredWidth: implicitWidth
                            }

                        }

                        WGFrame {
                            dark: false
                            shade: true

                            label: "File Options:"

                            childObject:
                            WGColumnLayout {

                                WGCheckBox {
                                    text: "File Option 1"
                                }
                                WGCheckBox {
                                    text: "File Option 2"
                                }
                                WGCheckBox {
                                    enabled: false
                                    text: "Disabled File Option"
                                }
                            }
                        }

                        WGProgressControl {
                            Layout.fillWidth: true
                            indeterminate: true
                            text: "File Importing..."
                        }

                    }


                }


            WGExpandingRowLayout {
                Layout.fillWidth: true

                WGTextBox {
                    placeholderText: "Placeholder Text"
                    Layout.fillWidth: true
                }

                WGTextBox {
                    text: "Read Only Text Box"
                    readOnly: true
                    Layout.fillWidth: true
                }

                WGTextBox {
                    text: "Disabled Text Box"
                    enabled: false
                    Layout.fillWidth: true
                }
            }


            WGGroupBox {
                text: "Group Box"
                toggleable : true
                childObject :
                WGBoolGridLayout {
                    ExclusiveGroup { id: radioPanelGroup }
                    WGRadioButton {
                        text: "Radio Button"
                        exclusiveGroup: radioPanelGroup
                        checked: true
                    }
                    WGRadioButton {
                        text: "Radio Button"
                        exclusiveGroup: radioPanelGroup
                    }
                    WGRadioButton {
                        text: "Radio Button with a long name"
                        exclusiveGroup: radioPanelGroup
                    }
                }
            }
            WGSubPanel {
                text: "Color Sliders"
                childObject :
                    WGColumnLayout {

                    WGColorSlider {
                        Layout.fillWidth: true
                        minimumValue: 0
                        maximumValue: 255
                        stepSize: 1
                        colorData: [Qt.rgba(0,0,0,1), Qt.rgba(1,1,1,1)]
                        positionData: [0, 255]
                        value: 128
                        linkColorsToHandles: false
                    }

                    WGColorSlider {
                        Layout.fillWidth: true
                        minimumValue: 0
                        maximumValue: 255
                        stepSize: 1
                        colorData: [Qt.rgba(1,0,0,0), Qt.rgba(1,0,0,1)]
                        positionData: [0, 255]
                        value: 255
                        linkColorsToHandles: false
                    }

                    WGColorSlider {
                        Layout.fillWidth: true
                        minimumValue: 0
                        maximumValue: 255
                        stepSize: 1
                        colorData: [Qt.rgba(1,0,0,1), Qt.rgba(1,1,0,1), Qt.rgba(0,1,0,1), Qt.rgba(0,0,1,1), Qt.rgba(1,0,1,1)]
                        positionData: [0, 64, 128, 192, 255]
                        value: 128
                        linkColorsToHandles: false
                    }

                    WGSeparator {
                        vertical: false

                    }

                    WGLabel {
                        Layout.fillWidth: true
                        text: "Gradient/Ramp Slider."
                        font.bold: true
                    }

                    WGMultiLineText {
                        Layout.fillWidth: true
                        text: "Shift+Click to add handles.\nCtrl+Click to delete handles.\nDouble click to change color"
                    }

                    WGColorSlider {
                        id: rampSlider
                        Layout.fillWidth: true
                        minimumValue: 0
                        maximumValue: 100
                        stepSize: 0.1
                        colorData: mainWindow.colorArray
                        positionData: mainWindow.positionArray
                        linkColorsToHandles: true
                        handleStyle: WGColorSliderArrowHandle{}
                        Item {
                            parent: rampSlider
                            Connections {
                                target: mainWindow
                                onColorsUpdated: {
                                    rampSlider.updateData()
                                }
                            }
                        }
                    }
                }
            }


            WGSubPanel {
                text: "Sub Panel"
                childObject :
                    WGColumnLayout {

                        WGExpandingRowLayout {

                            WGPushButton {
                                text: "Auto-Size Button"
                                iconSource: "icons/save_16x16.png"
                            }

                            WGPushButton {
                                iconSource: "icons/close_16x16.png"
                            }

                        }

                        WGPushButton {
                            text: "Auto-Size Button"
                        }

                        WGExpandingRowLayout {
                            ExclusiveGroup { id: toolbarGroup }
                            WGToolButton {
                                checkable: true
                                checked: true
                                exclusiveGroup: toolbarGroup
                                iconSource: "icons/pause_16x16.png"
                            }
                            WGToolButton {
                                checkable: true
                                exclusiveGroup: toolbarGroup
                                iconSource: "icons/play_16x16.png"
                            }
                            WGSeparator {
                                vertical: true
                            }

                            WGToolButton {
                                checkable: true
                                iconSource: "icons/loop_16x16.png"
                            }
                        }

                        WGInternalPanel {
                            text: "Scrolling Panel of Buttons"
                            clipContents: true
                            expanded: false
                            childObject :
                                WGSubScrollPanel {
                                    implicitHeight: 100
                                    childObject:
                                        WGFormLayout {
                                        localForm: true
                                            WGLabel{
                                                text: "Internal scrollbars possible, but not recommended"
                                            }

                                            Repeater {
                                                model: 10
                                                WGPushButton {
                                                    text: "Button"
                                                    label: "Repeated Label: "
                                                }
                                            }
                                        }
                            }
                        }

                        WGFrame {
                            childObject :
                            WGColumnLayout {
                                WGProgressControl {
                                    Layout.fillWidth: true
                                    maximumValue: 100
                                    fakeProgress: true
                                    text: "Progress Bar Complete..."
                                    units: "%"
                                }
                            }
                        }
                    }
            }

            WGTabView {
                id: tabviewexample
                anchors.left: parent.left
                anchors.right: parent.right
                tabPosition: Qt.TopEdge // Qt.BottomEdge
                Layout.preferredHeight: 180

                Tab {
                    title: "Test one"
                    WGScrollPanel{
                    childObject:
                        WGColumnLayout {
                            WGInternalPanel{
                                text: "Panel within a tab"
                                clipContents: true
                                expanded: true
                                childObject :
                                WGColumnLayout{
                                    WGMultiLineText {
                                        Layout.fillWidth: true
                                        text: "These tabs are QML only and cannot be detached, undocked etc. Use Dock Widgets for this."
                                    }

                                    WGTextBox {
                                        Layout.preferredWidth: 150
                                        placeholderText: "Text Field"
                                        text: "Im in a tab"
                                    }

                                    WGExpandingRowLayout {
                                        Layout.fillWidth: true
                                        Layout.preferredHeight: defaultSpacing.minimumRowHeight

                                        WGNumberBox {
                                            id: perfValue
                                            width: 120
                                            value: 25
                                            minimumValue: 0
                                            maximumValue: 100
                                        }

                                        WGPerformanceBar {
                                            id: perfBar
                                            Layout.fillWidth: true
                                            minimumValue: perfValue.minimumValue
                                            maximumValue: perfValue.maximumValue
                                            value: perfValue.value
                                        }
                                    }

                                    WGCheckBox {
                                        width: 200
                                        text: "Tri-State Checkbox 1"
                                        partiallyCheckedEnabled: true
                                    }
                                }
                            }
                        }
                    }
                }
                Tab {
                    title: "Test two"
                    anchors.left: parent.left
                    anchors.right: parent.right

                    WGSubScrollPanel{
                        anchors.rightMargin: defaultSpacing.rightMargin

                        childObject :
                        WGColumnLayout {
                            anchors.left: parent.left
                            anchors.right: parent.right

                            WGExpandingRowLayout {
                                Layout.fillWidth: true
                                WGTextBox {
                                    placeholderText: "Text Field"
                                    text: "Text Value"
                                    Layout.fillWidth: true
                                }
                                WGTextBox {
                                    text: "Read Only Text Box"
                                    readOnly: true
                                    Layout.fillWidth: true
                                }
                                WGTextBox {
                                    width: 150
                                    text: "Disabled Text Box"
                                    Layout.fillWidth: true
                                }
                            }
                            WGCheckBox {
                                width: 200
                                text: "Tri-State Checkbox 1"
                                partiallyCheckedEnabled: true
                            }
                            WGBoolGridLayout {
                                ExclusiveGroup { id: radioGroup2 }
                                WGRadioButton {
                                    text: "Radio 1"
                                    exclusiveGroup: radioGroup2
                                    checked: true
                                }
                                WGRadioButton {
                                    text: "Radio 2"
                                    exclusiveGroup: radioGroup2
                                }
                                WGRadioButton {
                                    text: "Radio 3"
                                    exclusiveGroup: radioGroup2
                                    enabled: false
                                }
                            }
                            WGNumberBox {
                                Layout.preferredWidth: 120
                                value: 25
                                minimumValue: 0
                                maximumValue: 100
                            }
                        }
                    }
                }

                Tab {
                    title: "test very large title three"
                    anchors.left: parent.left
                    anchors.right: parent.right

                    WGSubScrollPanel{
                        anchors.rightMargin: defaultSpacing.rightMargin

                        childObject :
                        WGColumnLayout {
                            anchors.left: parent.left
                            anchors.right: parent.right

                            WGNumberBox {
                                id: perfValue2
                                Layout.preferredWidth: 120
                                value: 25
                                minimumValue: 0
                                maximumValue: 100
                            }

                            WGPerformanceBar {
                                Layout.fillWidth: true
                                id: perfBar2
                                minimumValue: perfValue2.minimumValue
                                maximumValue: perfValue2.maximumValue
                                value: perfValue2.value
                            }

                            WGCheckBox {
                                Layout.preferredWidth: 200
                                text: "Tri-State Checkbox 1"
                                partiallyCheckedEnabled: true
                            }
                            WGCheckBox {
                                Layout.preferredWidth: 200
                                text: "Tri-State Checkbox 2"
                                partiallyCheckedEnabled: true
                            }
                            WGCheckBox {
                                Layout.preferredWidth: 200
                                text: "Tri-State Checkbox 3"
                                partiallyCheckedEnabled: true
                            }
                            WGCheckBox {
                                Layout.preferredWidth: 200
                                text: "Tri-State Checkbox 4"
                                partiallyCheckedEnabled: true
                            }
                        }
                    }
                }
            }

            Rectangle{
                color: "transparent"
                height: defaultSpacing.rowSpacing
            }

            WGColumnLayout {

                WGExpandingRowLayout {
                    Layout.fillWidth: true
                    Layout.preferredHeight: defaultSpacing.minimumRowHeight
                    WGTextBox {
                        Layout.preferredWidth: 150
                        placeholderText: "Text Field"
                        text: "Text Value"
                    }
                    WGTextBox {
                        Layout.preferredWidth: 150
                        text: "Read Only Text Box"
                        readOnly: true
                        Layout.fillWidth: true
                    }
                    WGTextBox {
                        Layout.preferredWidth: 150
                        text: "Disabled Text Box"
                        enabled: false
                    }
                }

                WGNumberBox {
                    id: perfValue3
                    Layout.preferredWidth: 120
                    value: 25
                    minimumValue: 0
                    maximumValue: 100
                }

                WGPerformanceBar {
                    id: perfBar3
                    Layout.fillWidth: true

                    minimumValue: perfValue3.minimumValue
                    maximumValue: perfValue3.maximumValue
                    value: perfValue3.value
                }

                WGCheckBox {
                    Layout.preferredWidth: 200
                    text: "Tri-State Checkbox"
                    partiallyCheckedEnabled: true
                }

                WGBoolGridLayout {
                    ExclusiveGroup { id: radioGroup }
                    WGRadioButton {
                        text: "Radio 1"
                        exclusiveGroup: radioGroup
                        checked: true
                    }
                    WGRadioButton {
                        text: "Radio 2"
                        exclusiveGroup: radioGroup
                    }
                    WGRadioButton {
                        text: "Radio 3"
                        exclusiveGroup: radioGroup
                        enabled: false
                    }
                }
            }

            WGFormLayout {

                WGSplitTextFrame {
                    boxList: [
                        WGNumberBox{
                            value: 192
                            minimumValue: 0
                            maximumValue: 255
                        },
                        WGNumberBox{
                            value: 168
                            minimumValue: 0
                            maximumValue: 255
                        },
                        WGNumberBox{
                            value: 0
                            minimumValue: 0
                            maximumValue: 255
                        },
                        WGNumberBox{
                            value: 1
                            minimumValue: 0
                            maximumValue: 255
                        }
                    ]
                    decimalSeparator: true
                    height: 24
                    Layout.preferredWidth: 130
                    label: "IP Address:"
                }
                WGSplitTextFrame {
                    boxList: [
                        WGNumberBox{
                            value: 60
                            minimumValue: 0
                            maximumValue: 255
                        },
                        WGNumberBox{
                            value: 128
                            minimumValue: 0
                            maximumValue: 255
                        },
                        WGNumberBox{
                            value: 55
                            minimumValue: 0
                            maximumValue: 255
                        }
                    ]
                    height: 24
                    Layout.preferredWidth: 130
                    label: "RGB:"
                }

                WGSplitTextFrame {
                    boxList: [
                        WGNumberBox{
                            value: 7
                            minimumValue: 0
                            maximumValue: 100
                            width: 36
                        },
                        WGTextBox{
                            text: "Greendale"
                            width: 80
                        },
                        WGTextBox{
                            text: "Drive"
                            width: 80
                        }
                    ]
                    evenBoxes: false
                    height: 24
                    label: "Address:"
                }

                WGSliderControl {
                    Layout.fillWidth: true
                    minimumValue: 0
                    maximumValue: 100
                    stepSize: 1
                    value: 40
                    label: "Single Slider:"
                }

                WGRangeSlider {
                    Layout.fillWidth: true
                    minimumValue: 0
                    maximumValue: 100
                    lowerValue: 25
                    upperValue: 25
                    stepSize: 1
                    enabled: true
                    label: "Range Slider:"
                }

                WGSlider {
                    Layout.fillWidth: true
                    Layout.preferredHeight: defaultSpacing.minimumRowHeight
                    minimumValue: 0
                    maximumValue: 100
                    stepSize: 1
                    enabled: true
                    property string label: "Multi Handle Slider:"

                    WGSliderHandle {
                        minimumValue: 0
                        maximumValue: 100
                        value: 0
                        showBar: false
                    }
                    WGSliderHandle {
                        minimumValue: 0
                        maximumValue: 100
                        value: 25
                        showBar: false
                    }
                    WGSliderHandle {
                        minimumValue: 0
                        maximumValue: 100
                        value: 50
                        showBar: false
                    }
                    WGSliderHandle {
                        minimumValue: 0
                        maximumValue: 100
                        value: 75
                        showBar: false
                    }
                    WGSliderHandle {
                        minimumValue: 0
                        maximumValue: 100
                        value: 100
                        showBar: false
                    }
                }
            }


            WGSubPanel {
                text: "LOD Control"
                childObject :
                    WGColumnLayout {
                        WGLodSlider {
                            Layout.fillWidth: true
                        }
                }
            }


            WGColumnLayout {

                WGExpandingRowLayout {
                    WGPushButton {
                        id: busyIndicatorButton
                        width: 150
                        checkable: true
                        text: "Busy Indicator"
                        iconSource: "icons/close_16x16.png"

                        onCheckedChanged: {
                            if (busyIndicatorButton.checked == true){
                                busyIndicator1.running = true
                            }
                            else {
                                busyIndicator1.running = false
                            }
                        }
                    }

                    WGPushButton {
                        width: 150
                        checkable: true
                        enabled: false
                        text: "Disabled Button"
                    }
                }

                WGExpandingRowLayout {

                    WGPushButton {
                        width: 150
                        text: "Icon Button"
                        iconSource: "icons/save_16x16.png"
                    }

                    WGPushButton {
                        width: 150
                        checkable: true
                        text: "Icon Button"
                        iconSource: "icons/close_16x16.png"
                        enabled: false
                    }
                }

                WGExpandingRowLayout {
                    WGToolButton {
                        iconSource: "icons/save_16x16.png"
                    }
                    WGToolButton {
                        iconSource: "icons/paste_16x16.png"
                        enabled: false
                    }
                    WGToolButton {
                        iconSource: "icons/close_16x16.png"
                    }
                }

                WGDropDownBox {
                    id: dropDown

                    model: ListModel {
                        ListElement { text: "Option 1" }
                        ListElement { text: "Option 2" }
                        ListElement { text: "Option 3 Has a Really long Name" }
                        ListElement { text: "Option 4" }
                        ListElement { text: "Option 5" }
                    }
                }

                WGButtonBar {
                    Layout.fillWidth: true
                    buttonList: [
                            WGPushButton {
                                text: "Button 1"
                            },
                            WGPushButton {
                                text: "Button 1"
                            },
                            WGPushButton {
                                text: "Button 1"
                            },
                            WGPushButton {
                                text: "Button 4"
                            }
                    ]
                }

                WGExpandingRowLayout {

                    WGButtonBar {
                        width: 120
                        ExclusiveGroup {
                            id: xyzGroup
                        }

                        buttonList: [
                            WGToolButton {
                                text: "X"
                                checked: true
                                checkable: true
                                exclusiveGroup: xyzGroup
                            },
                            WGToolButton {
                                text: "Y"
                                checkable: true
                                exclusiveGroup: xyzGroup
                            },
                            WGToolButton {
                                text: "Z"
                                checkable: true
                                exclusiveGroup: xyzGroup
                            }
                        ]

                    }

                    WGButtonBar {
                        width: 120
                        buttonList: [
                            WGToolButton {
                                iconSource: "icons/pause_16x16.png"
                            },
                            WGToolButton {
                                iconSource: "icons/play_16x16.png"
                            },
                            WGToolButton {
                                iconSource: "icons/loop_16x16.png"
                            }
                        ]
                    }

                }

                WGSeparator {
                    Layout.fillWidth: true
                    vertical: false
                }
            }
        }
    }
}

