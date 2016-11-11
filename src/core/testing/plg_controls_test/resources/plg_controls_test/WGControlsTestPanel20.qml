import QtQuick 2.3
import QtQuick.Controls 1.2
import QtQuick.Layouts 1.0
import QtGraphicalEffects 1.0

import WGControls 2.0
import WGControls.Private 2.0
import WGControls.Overlay 2.0
import WGControls.Layouts 2.0
import WGControls.Styles 2.0
import WGControls.Views 2.0

/**
 *	Sample panel with all the available controls and styles.
 *	Used for control testing
 */
Rectangle {
    id: mainWindow

    visible: true
    property var title: qsTr("QML Controls Test Window")

    color: palette.mainWindowColor

    WGBusyIndicator {
        id: busyIndicator1
        z:100
        duration: 3000
        running: false
        busyMessage: "WGBusyIndicator. This will disappear in 3 seconds."
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
                        value: 128
                        gradient: Gradient {
                            GradientStop {
                                position: 0
                                color: "#000000"
                            }
                            GradientStop {
                                position: 1
                                color: "#FFFFFF"
                            }
                        }
                    }

                    WGColorSlider {
                        Layout.fillWidth: true
                        minimumValue: 0
                        maximumValue: 255
                        stepSize: 1
                        value: 128
                        gradient: Gradient {
                            GradientStop {
                                position: 0
                                color: "#00FF0000"
                            }
                            GradientStop {
                                position: 1
                                color: "#FFFF0000"
                            }
                        }
                    }

                    WGColorSlider {
                        Layout.fillWidth: true

                        value: 128

                        gradient: Gradient {
                            GradientStop {
                                position: 0.0
                                color: "#FF0000"
                            }
                            GradientStop {
                                position: 0.2
                                color: "#FFFF00"
                            }
                            GradientStop {
                                position: 0.4
                                color: "#00FF00"
                            }
                            GradientStop {
                                position: 0.6
                                color: "#00FFFF"
                            }
                            GradientStop {
                                position: 0.8
                                color: "#0000FF"
                            }
                            GradientStop {
                                position: 1.0
                                color: "#FF00FF"
                            }
                        }
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

                    WGGradientSlider {
                        id: rampSlider
                        Layout.fillWidth: true

                        WGGradientSliderHandle {
                            value: 0.2
                            color: "#800000"
                        }
                        WGGradientSliderHandle {
                            value: 0.4
                            color: "#FF0000"
                        }
                        WGGradientSliderHandle {
                            value: 0.6
                            color: "#FFFF00"
                        }
                        WGGradientSliderHandle {
                            value: 0.8
                            color: "#FFFFFF"
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
                Layout.fillWidth: true

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
                        text: "Busy Indicator"
                        iconSource: "icons/close_16x16.png"

                        onClicked: busyIndicator1.running = !busyIndicator1.running
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

                WGMultiLineText {
                    Layout.fillWidth: true
                    text: "This section will tests all the readOnly, enabled combinations of WGControls
Write/Enabled, ReadOnly/Enabled, Write/Disabled, ReadOnly/Disabled | MultipleValues"
                }

                WGExpandingRowLayout {
                    WGMultiLineText {
                        text: "WGButtonBar"
                    }

                    WGButtonBar {
                        implicitWidth: 400
                        buttonList: [
                            WGToolButton {
                                iconSource: "icons/pause_16x16.png"
                                text: "NA/enabled"
                                enabled: true
                            },
                            WGToolButton {
                                text: "NA/enabled"
                                iconSource: "icons/play_16x16.png"
                                enabled: true
                            },
                            WGToolButton {
                                text: "NA/disabled"
                                iconSource: "icons/loop_16x16.png"
                                enabled: false
                            },
                            WGToolButton {
                                text: "NA/disabled"
                                iconSource: "icons/loop_16x16.png"
                                enabled: false
                            }
                        ]
                    }

                    Item {
                        Layout.fillWidth: true
                    }
                }

                WGExpandingRowLayout {
                    WGMultiLineText {
                        text: "WGCheckBox"
                    }
                    WGCheckBox {
                        text: "NA/enabled"
                        enabled: true
                    }
                    WGCheckBox {
                        text: "NA/enabled"
                        enabled: true
                    }
                    WGCheckBox {
                        text: "NA/disabled"
                        enabled: false
                    }
                    WGCheckBox {
                        text: "NA/disabled"
                        enabled: false
                    }

                    WGSeparator {
                        vertical: true
                    }

                    WGCheckBox {
                        text: "NA/enabled - Multiple"
                        enabled: true
                        multipleValues: true
                    }

                    Item {
                        Layout.fillWidth: true
                    }

                }

                WGExpandingRowLayout {
                    WGMultiLineText {
                        text: "WGColorButton"
                    }
                    WGColorButton {
                        //text: "NA/enabled"
                        enabled: true
                    }
                    WGColorButton {
                        //text: "NA/enabled"
                        enabled: true
                    }
                    WGColorButton {
                        //text: "NA/disabled"
                        enabled: false
                    }
                    WGColorButton {
                        //text: "NA/disabled"
                        enabled: false
                    }

                    WGSeparator {
                        vertical: true
                    }

                    WGColorButton {
                        enabled: true
                        multipleValues: true
                    }

                    Item {
                        Layout.fillWidth: true
                    }

                }

                WGExpandingRowLayout {
                    WGMultiLineText {
                        text: "WGColorSlider20"
                    }
                    WGColorSlider {
                        enabled: true

                        Layout.fillWidth: true
                        minimumValue: 0
                        maximumValue: 255
                        stepSize: 1
                        value: 128
                        gradient: Gradient {
                            GradientStop {
                                position: 0
                                color: "#000000"
                            }
                            GradientStop {
                                position: 1
                                color: "#FFFFFF"
                            }
                        }
                    }
                    WGColorSlider {
                        enabled: true

                        Layout.fillWidth: true
                        minimumValue: 0
                        maximumValue: 255
                        stepSize: 1
                        value: 128
                        gradient: Gradient {
                            GradientStop {
                                position: 0
                                color: "#000000"
                            }
                            GradientStop {
                                position: 1
                                color: "#FFFFFF"
                            }
                        }
                    }
                    WGColorSlider {
                        enabled: false

                        Layout.fillWidth: true
                        minimumValue: 0
                        maximumValue: 255
                        stepSize: 1
                        value: 128
                        gradient: Gradient {
                            GradientStop {
                                position: 0
                                color: "#000000"
                            }
                            GradientStop {
                                position: 1
                                color: "#FFFFFF"
                            }
                        }
                    }
                    WGColorSlider {
                        enabled: false

                        Layout.fillWidth: true
                        minimumValue: 0
                        maximumValue: 255
                        stepSize: 1
                        value: 128
                        gradient: Gradient {
                            GradientStop {
                                position: 0
                                color: "#000000"
                            }
                            GradientStop {
                                position: 1
                                color: "#FFFFFF"
                            }
                        }
                    }

                    Item {
                        Layout.fillWidth: true
                    }

                }

                WGExpandingRowLayout {
                    WGMultiLineText {
                        text: "WGFileSelectBox"
                    }
                    WGFileSelectBox {
                        text: "write/enabled"
                        readOnly: false
                        enabled: true
                    }
                    WGFileSelectBox {
                        text: "readOnly/enabled"
                        readOnly: true
                        enabled: true
                    }
                    WGFileSelectBox {
                        text: "write/disabled"
                        readOnly: false
                        enabled: false
                    }
                    WGFileSelectBox {
                        text: "readOnly/disabled"
                        readOnly: true
                        enabled: false
                    }

                    WGSeparator {
                        vertical: true
                    }

                    WGFileSelectBox {
                        enabled: true
                        multipleValues: true
                    }

                    Item {
                        Layout.fillWidth: true
                    }

                }

                WGExpandingRowLayout {
                    WGMultiLineText {
                        text: "WGFrame"
                    }
                    WGFrame {
                        text: "NA/enabled"
                        enabled: true

                        dark: false
                        shade: true

                        label: "File Options:"

                        childObject:
                            WGColumnLayout {

                            TextField {
                                text: "NA/enabled"
                            }
                        }
                    }
                    WGFrame {
                        text: "NA/enabled"
                        enabled: true

                        dark: false
                        shade: true

                        label: "File Options:"

                        childObject:
                            WGColumnLayout {

                            TextField {
                                text: "NA/enabled"
                            }
                        }
                    }
                    WGFrame {
                        text: "NA/disabled"
                        enabled: false

                        dark: false
                        shade: true

                        label: "File Options:"

                        childObject:
                            WGColumnLayout {

                            TextField {
                                text: "NA/disabled"
                            }
                        }
                    }
                    WGFrame {
                        text: "NA/disabled"
                        enabled: false

                        dark: false
                        shade: true

                        label: "File Options:"

                        childObject:
                            WGColumnLayout {

                            TextField {
                                text: "NA/disabled"
                            }
                        }
                    }

                    Item {
                        Layout.fillWidth: true
                    }

                }

                WGExpandingRowLayout {
                    WGMultiLineText {
                        text: "WGGradientSlider20"
                    }
                    WGGradientSlider {
                        //text: "NA/enabled"
                        enabled: true

                        Layout.fillWidth: true

                        WGGradientSliderHandle {
                            value: 0.25
                            color: "red"
                        }
                        WGGradientSliderHandle {
                            value: 0.5
                            color: "yellow"
                        }
                        WGGradientSliderHandle {
                            value: 0.75
                            color: "white"
                        }
                    }
                    WGGradientSlider {
                        //text: "NA/enabled"
                        enabled: true

                        Layout.fillWidth: true

                        WGGradientSliderHandle {
                            value: 0.25
                            color: "red"
                        }
                        WGGradientSliderHandle {
                            value: 0.5
                            color: "yellow"
                        }
                        WGGradientSliderHandle {
                            value: 0.75
                            color: "white"
                        }
                    }
                    WGGradientSlider {
                        //text: "NA/disabled"
                        enabled: false

                        Layout.fillWidth: true

                        WGGradientSliderHandle {
                            value: 0.25
                            color: "red"
                        }
                        WGGradientSliderHandle {
                            value: 0.5
                            color: "yellow"
                        }
                        WGGradientSliderHandle {
                            value: 0.75
                            color: "white"
                        }
                    }
                    WGGradientSlider {
                        //text: "NA/disabled"
                        enabled: false

                        Layout.fillWidth: true

                        WGGradientSliderHandle {
                            value: 0.25
                            color: "red"
                        }
                        WGGradientSliderHandle {
                            value: 0.5
                            color: "yellow"
                        }
                        WGGradientSliderHandle {
                            value: 0.75
                            color: "white"
                        }
                    }

                    Item {
                        Layout.fillWidth: true
                    }

                }

                WGExpandingRowLayout {
                    WGMultiLineText {
                        text: "WGGradientSlider20 Repeater Handles"
                    }
                    WGGradientSlider {

                        Layout.fillWidth: true

                        Repeater {
                            model: 3
                            WGGradientSliderHandle {
                                value: 0.25 * index + 0.25
                                color: Qt.rgba(Math.random(),Math.random(),Math.random(),1)
                            }
                        }
                    }
                }

                WGExpandingRowLayout {
                    WGMultiLineText {
                        text: "WGGradientSlider20 Created Handles"
                    }
                    WGGradientSlider {

                        Layout.fillWidth: true

                        Component.onCompleted: {
                            createColorHandle(.25,handleStyle,__handlePosList.length, "red")
                            createColorHandle(.5,handleStyle,__handlePosList.length, "yellow")
                            createColorHandle(.75,handleStyle,__handlePosList.length, "white")
                        }
                    }
                }

                WGExpandingRowLayout {
                    Layout.minimumHeight: 150
                    Layout.fillWidth: true
                    spacing: 50

                    WGMultiLineText {
                        text: "Vertical Sliders"
                    }

                    WGSliderControl {
                        enabled: true

                        Layout.fillHeight: true
                        Layout.preferredWidth: defaultSpacing.minimumRowHeight
                        orientation: Qt.Vertical

                        minimumValue: 0
                        maximumValue: 100
                        stepSize: 1
                        value: 40
                    }

                    WGRangeSlider {
                        Layout.fillHeight: true
                        Layout.preferredWidth: defaultSpacing.minimumRowHeight
                        orientation: Qt.Vertical

                        minimumValue: 0
                        maximumValue: 100
                        lowerValue: 25
                        upperValue: 25
                        stepSize: 1
                        enabled: true
                    }


                    WGColorSlider {
                        enabled: true

                        Layout.fillHeight: true
                        Layout.preferredWidth: defaultSpacing.minimumRowHeight
                        orientation: Qt.Vertical

                        minimumValue: 0
                        maximumValue: 255
                        stepSize: 1
                        value: 128
                        gradient: Gradient {
                            GradientStop {
                                position: 0
                                color: "#000000"
                            }
                            GradientStop {
                                position: 1
                                color: "#FFFFFF"
                            }
                        }
                    }


                    WGGradientSlider {
                        enabled: true

                        Layout.fillHeight: true
                        Layout.preferredWidth: defaultSpacing.minimumRowHeight
                        orientation: Qt.Vertical

                        WGGradientSliderHandle {
                            value: 0.25
                            color: "red"
                        }
                        WGGradientSliderHandle {
                            value: 0.5
                            color: "yellow"
                        }
                        WGGradientSliderHandle {
                            value: 0.75
                            color: "white"
                        }
                    }
                }

                WGExpandingRowLayout {
                    WGMultiLineText {
                        text: "WGGroupBox"
                    }
                    WGGroupBox {
                        //text: "NA/enabled"
                        enabled: true

                        text: "WGGroupBox toggle"

                        toggleable: true
                        childObject:
                            WGColumnLayout {

                            TextField {
                                text: "NA/enabled"
                            }
                        }
                    }
                    WGGroupBox {
                        //text: "NA/enabled"
                        enabled: true

                        text: "WGGroupBox toggle"

                        toggleable: true
                        childObject:
                            WGColumnLayout {

                            TextField {
                                text: "NA/enabled"
                            }
                        }
                    }
                    WGGroupBox {
                        //text: "NA/disabled"
                        enabled: false

                        text: "WGGroupBox toggle"

                        toggleable: true
                        childObject:
                            WGColumnLayout {

                            TextField {
                                text: "NA/disabled"
                            }
                        }
                    }
                    WGGroupBox {
                        //text: "NA/disabled"
                        enabled: false

                        text: "WGGroupBox toggle"

                        toggleable: true
                        childObject:
                            WGColumnLayout {

                            TextField {
                                text: "NA/disabled"
                            }
                        }
                    }

                    Item {
                        Layout.fillWidth: true
                    }
                }

                WGExpandingRowLayout {
                    WGMultiLineText {
                        text: "WGMultiLineText"
                    }
                    WGMultiLineText {
                        text: "   NA/enabled   "
                        enabled: true
                    }
                    WGMultiLineText {
                        text: "   NA/enabled   "
                        enabled: true
                    }
                    WGMultiLineText {
                        text: "   NA/disabled   "
                        enabled: false
                    }
                    WGMultiLineText {
                        text: "   NA/disabled   "
                        enabled: false
                    }

                    Item {
                        Layout.fillWidth: true
                    }

                }

                WGExpandingRowLayout {
                    WGMultiLineText {
                        text: "WGNumberBox"
                    }
                    WGNumberBox {
                        enabled: true
                        readOnly: false

                        value: 25
                        minimumValue: 0
                        maximumValue: 100
                    }
                    WGNumberBox {
                        enabled: true
                        readOnly: true

                        value: 25
                        minimumValue: 0
                        maximumValue: 100
                    }
                    WGNumberBox {
                        enabled: false
                        readOnly: false

                        value: 25
                        minimumValue: 0
                        maximumValue: 100
                    }
                    WGNumberBox {
                        enabled: false
                        readOnly: true

                        value: 25
                        minimumValue: 0
                        maximumValue: 100
                    }

                    WGSeparator {
                        vertical: true
                    }

                    WGNumberBox {
                        enabled: true
                        readOnly: false
                        multipleValues: true

                        value: 75
                        minimumValue: 0
                        maximumValue: 100
                    }

                    WGNumberBox {
                        enabled: true
                        readOnly: false

                        valueIsInfinite: false
                        showInfiniteButton: true

                        value: 25
                        minimumValue: 0
                        maximumValue: 100
                    }


                    WGNumberBox {
                        enabled: true
                        readOnly: false

                        valueIsInfinite: true
                        showInfiniteButton: true

                        value: 25
                        minimumValue: 0
                        maximumValue: 100
                    }

                    WGNumberBox {
                        enabled: true
                        readOnly: false
                        multipleValues: true
                        showInfiniteButton: true

                        value: 75
                        minimumValue: 0
                        maximumValue: 100
                    }

                    Item {
                        Layout.fillWidth: true
                    }

                }

                WGExpandingRowLayout {
                    WGMultiLineText {
                        text: "WGTimeBox"
                    }
                    WGTimeBox {
                        enabled: true
                        readOnly: false

                        time: 1024
                    }
                    WGTimeBox {
                        enabled: true
                        readOnly: true

                        time: 1024
                    }
                    WGTimeBox {
                        enabled: false
                        readOnly: false

                        time: 1024
                    }
                    WGTimeBox {
                        enabled: false
                        readOnly: true

                        time: 1024
                    }

                    WGSeparator {
                        vertical: true
                    }

                    WGTimeBox {
                        enabled: true
                        readOnly: false
                        multipleValues: true

                        time: 1024
                    }

                    Item {
                        Layout.fillWidth: true
                    }

                }


                WGExpandingRowLayout {
                    WGMultiLineText {
                        text: "WGSliderControl20"
                    }
                    WGSliderControl {
                        enabled: true

                        Layout.fillWidth: true
                        minimumValue: 0
                        maximumValue: 100
                        stepSize: 1
                        value: 40
                    }
                    WGSliderControl {
                        enabled: true

                        Layout.fillWidth: true
                        minimumValue: 0
                        maximumValue: 100
                        stepSize: 1
                        value: 40
                    }
                    WGSliderControl {
                        enabled: false

                        Layout.fillWidth: true
                        minimumValue: 0
                        maximumValue: 100
                        stepSize: 1
                        value: 40
                    }
                    WGSliderControl {
                        enabled: false

                        Layout.fillWidth: true
                        minimumValue: 0
                        maximumValue: 100
                        stepSize: 1
                        value: 40
                    }

                    WGSeparator {
                        vertical: true
                    }

                    WGSliderControl {
                        enabled: true
                        multipleValues: true

                        Layout.fillWidth: true
                        minimumValue: 0
                        maximumValue: 100
                        stepSize: 1
                        value: 40

                    }

                    Item {
                        Layout.fillWidth: true
                    }

                }


                WGExpandingRowLayout {
                    WGMultiLineText {
                        text: "WGDial20"
                    }
                    WGDial {
                        Layout.preferredHeight: defaultSpacing.minimumRowHeight * 3
                        Layout.preferredWidth: defaultSpacing.minimumRowHeight * 3

                        value: 180

                        showValue: true
                        clockwise: true
                        zeroValue: 0
                        snapToClick: false
                        loopAtBounds: true

                        decimals: 0
                        stepSize: 1

                        tickInterval: 45
                        minorTickInterval: 15
                    }
                    WGDial {
                        Layout.preferredHeight: defaultSpacing.minimumRowHeight * 3
                        Layout.preferredWidth: defaultSpacing.minimumRowHeight * 3

                        value: 180

                        enabled: false

                        showValue: true
                        clockwise: true
                        zeroValue: 0
                        snapToClick: false
                        loopAtBounds: true

                        decimals: 0
                        stepSize: 1

                        tickInterval: 45
                        minorTickInterval: 15
                    }

                    WGDial {
                        Layout.preferredHeight: defaultSpacing.minimumRowHeight * 3
                        Layout.preferredWidth: defaultSpacing.minimumRowHeight * 3

                        value: 0


                        minimumValue: -180
                        maximumValue: 180

                        showValue: true
                        clockwise: true
                        zeroValue: 180

                        loopAtBounds: false

                        decimals: 0
                        stepSize: 1

                        tickInterval: 45
                        minorTickInterval: 15
                    }


                    WGDial {
                        Layout.preferredHeight: defaultSpacing.minimumRowHeight * 3
                        Layout.preferredWidth: defaultSpacing.minimumRowHeight * 3

                        value: 180

                        showValue: true
                        clockwise: false
                        zeroValue: 0
                        snapToClick: false
                        loopAtBounds: true

                        decimals: 0
                        stepSize: 1

                        tickInterval: 45
                        minorTickInterval: 15
                    }

                    WGDial {
                        Layout.preferredHeight: defaultSpacing.minimumRowHeight * 3
                        Layout.preferredWidth: defaultSpacing.minimumRowHeight * 3

                        value: 180

                        showValue: true
                        clockwise: true
                        zeroValue: 0
                        loopAtBounds: false

                        decimals: 0
                        stepSize: 1

                        tickInterval: 45
                        minorTickInterval: 15
                    }

                    WGDial {
                        Layout.preferredHeight: defaultSpacing.minimumRowHeight * 3
                        Layout.preferredWidth: defaultSpacing.minimumRowHeight * 3

                        value: 180

                        multipleValues: true

                        showValue: true
                        clockwise: true
                        zeroValue: 0
                        snapToClick: false

                        decimals: 0
                        stepSize: 1

                        tickInterval: 45
                        minorTickInterval: 15
                    }


                    WGDial {
                        Layout.preferredHeight: defaultSpacing.minimumRowHeight * 3
                        Layout.preferredWidth: defaultSpacing.minimumRowHeight * 3

                        value: 180

                        snapToClick: true

                        decimals: 0
                        stepSize: 1

                        tickInterval: 10
                        minorTickInterval: 0
                    }

                    WGDial {
                        Layout.preferredHeight: defaultSpacing.minimumRowHeight * 3
                        Layout.preferredWidth: defaultSpacing.minimumRowHeight * 3

                        minimumValue: 0
                        maximumValue: Math.PI * 2

                        decimals: 3

                        unitString: "rad"

                        value: Math.PI

                        showValue: true

                        tickInterval: 0
                        minorTickInterval: 0
                    }

                    WGDial {
                        Layout.preferredHeight: defaultSpacing.minimumRowHeight * 3
                        Layout.preferredWidth: defaultSpacing.minimumRowHeight * 3

                        minimumValue: 0
                        maximumValue: Math.PI * 2

                        value: Math.PI

                        loopAtBounds: false

                        decimals: 3

                        showValue: true
                        unitString: "rad"

                        tickInterval: 0
                        minorTickInterval: 0
                    }

                    WGDial {
                        Layout.preferredHeight: defaultSpacing.minimumRowHeight * 3
                        Layout.preferredWidth: defaultSpacing.minimumRowHeight * 3

                        value: 180


                        minimumValue: -90
                        maximumValue: 270

                        showValue: true
                        clockwise: true

                        loopAtBounds: false

                        decimals: 0
                        stepSize: 1

                        tickInterval: 45
                        minorTickInterval: 15
                    }


                    Item {
                        Layout.preferredHeight: defaultSpacing.minimumRowHeight * 3
                        Layout.preferredWidth: defaultSpacing.minimumRowHeight * 3

                        WGDial {
                            id: tapDial
                            height: defaultSpacing.minimumRowHeight * 3
                            width: defaultSpacing.minimumRowHeight * 3

                            value: 5


                            minimumValue: 0
                            maximumValue: 11

                            showValue: true
                            clockwise: true
                            unitString: ""

                            zeroValue: 0

                            loopAtBounds: false

                            decimals: 0
                            stepSize: 1

                            tickInterval: 36
                            minorTickInterval: 0
                        }

                        Colorize {
                            id: spinalTap
                            anchors.fill: tapDial
                            source: tapDial
                            hue: 0.0
                            saturation: 1
                            lightness: 0
                            visible: tapDial.value == 11
                        }

                        Timer {
                            interval: 10
                            repeat: true
                            running: tapDial.value == 11
                            onTriggered: {
                                spinalTap.hue += 0.01
                                if (spinalTap.hue > 1) spinalTap.hue = 0
                            }
                        }
                    }

                    WGDial {
                        Layout.preferredHeight: defaultSpacing.minimumRowHeight * 3
                        Layout.preferredWidth: defaultSpacing.minimumRowHeight * 3

                        value: 0

                        minimumValue: -180
                        maximumValue: 180

                        showValue: true
                        zeroValue: 180

                        clockwise: false

                        loopAtBounds: false

                        decimals: 0
                        stepSize: 1

                        tickInterval: 45
                        minorTickInterval: 15
                    }

                    WGDial {
                        Layout.preferredHeight: defaultSpacing.minimumRowHeight * 3
                        Layout.preferredWidth: defaultSpacing.minimumRowHeight * 3

                        value: 0

                        minimumValue: -180
                        maximumValue: 180

                        showValue: true
                        zeroValue: 270

                        loopAtBounds: false

                        decimals: 0
                        stepSize: 1

                        tickInterval: 45
                        minorTickInterval: 15
                    }

                    WGDial {
                        Layout.preferredHeight: defaultSpacing.minimumRowHeight * 3
                        Layout.preferredWidth: defaultSpacing.minimumRowHeight * 3

                        value: 180

                        showValue: true
                        clockwise: true
                        zeroValue: 180
                        snapToClick: false
                        loopAtBounds: true

                        decimals: 0
                        stepSize: 1

                        tickInterval: 45
                        minorTickInterval: 15
                    }

                    WGDial {
                        Layout.preferredHeight: defaultSpacing.minimumRowHeight * 3
                        Layout.preferredWidth: defaultSpacing.minimumRowHeight * 3

                        value: 180

                        showValue: true
                        clockwise: true
                        zeroValue: 270
                        snapToClick: false
                        loopAtBounds: true

                        decimals: 0
                        stepSize: 1

                        tickInterval: 45
                        minorTickInterval: 15
                    }



                    WGDial {
                        Layout.preferredHeight: defaultSpacing.minimumRowHeight * 3
                        Layout.preferredWidth: defaultSpacing.minimumRowHeight * 3

                        value: 180

                        showValue: true
                        clockwise: false
                        zeroValue: 180
                        snapToClick: false
                        loopAtBounds: true

                        decimals: 0
                        stepSize: 1

                        tickInterval: 45
                        minorTickInterval: 15
                    }

                    WGDial {
                        Layout.preferredHeight: defaultSpacing.minimumRowHeight * 3
                        Layout.preferredWidth: defaultSpacing.minimumRowHeight * 3

                        value: 180

                        showValue: true
                        clockwise: false
                        zeroValue: 270
                        snapToClick: false
                        loopAtBounds: true

                        decimals: 0
                        stepSize: 1

                        tickInterval: 45
                        minorTickInterval: 15
                    }

                    Item {
                        Layout.fillWidth: true
                    }

                }


                WGExpandingRowLayout {
                    WGMultiLineText {
                        text: "WGSpinBox"
                    }
                    WGSpinBox {
                        enabled: true
                        readOnly: false

                        Layout.fillWidth: true
                        minimumValue: 0
                        maximumValue: 100
                        stepSize: 1
                        value: 40
                    }
                    WGSpinBox {
                        enabled: true
                        readOnly: true

                        Layout.fillWidth: true
                        minimumValue: 0
                        maximumValue: 100
                        stepSize: 1
                        value: 40
                    }
                    WGSpinBox {
                        enabled: false
                        readOnly: false

                        Layout.fillWidth: true
                        minimumValue: 0
                        maximumValue: 100
                        stepSize: 1
                        value: 40
                    }
                    WGSpinBox {
                        enabled: false
                        readOnly: true

                        Layout.fillWidth: true
                        minimumValue: 0
                        maximumValue: 100
                        stepSize: 1
                        value: 40
                    }

                    Item {
                        Layout.fillWidth: true
                    }

                }


                WGExpandingRowLayout {
                    WGMultiLineText {
                        text: "WGSpinBox"
                    }
                    WGSpinBox {
                        enabled: true
                        readOnly: false

                        Layout.fillWidth: true
                        minimumValue: 0
                        maximumValue: 100
                        stepSize: 1
                        value: 40
                    }
                    WGSpinBox {
                        enabled: true
                        readOnly: true

                        Layout.fillWidth: true
                        minimumValue: 0
                        maximumValue: 100
                        stepSize: 1
                        value: 40
                    }
                    WGSpinBox {
                        enabled: false
                        readOnly: false

                        Layout.fillWidth: true
                        minimumValue: 0
                        maximumValue: 100
                        stepSize: 1
                        value: 40
                    }
                    WGSpinBox {
                        enabled: false
                        readOnly: true

                        Layout.fillWidth: true
                        minimumValue: 0
                        maximumValue: 100
                        stepSize: 1
                        value: 40
                    }

                    Item {
                        Layout.fillWidth: true
                    }

                }

                WGExpandingRowLayout {
                    WGMultiLineText {
                        text: "WGTextBox"
                    }
                    WGTextBox {
                        enabled: true
                        readOnly: false

                        placeholderText: "write/enabled"
                        Layout.fillWidth: true
                    }
                    WGTextBox {
                        enabled: true
                        readOnly: true

                        placeholderText: "readOnly/enabled"
                        Layout.fillWidth: true
                    }
                    WGTextBox {
                        enabled: false
                        readOnly: false

                        placeholderText: "write/disabled"
                        Layout.fillWidth: true
                    }
                    WGTextBox {
                        enabled: false
                        readOnly: true

                        placeholderText: "readOnly/disabled"
                        Layout.fillWidth: true
                    }

                    Item {
                        Layout.fillWidth: true
                    }

                }


                WGExpandingRowLayout {
                    WGMultiLineText {
                        text: "WGThumbnailButton"
                    }
                    WGThumbnailButton {
                        enabled: true

                        source: "icons/diffuse1.png"
                        defaultText: "NA/enabled"
                    }
                    WGThumbnailButton {
                        enabled: true

                        source: "icons/diffuse1.png"
                        defaultText: "NA/enabled"
                    }
                    WGThumbnailButton {
                        enabled: false

                        source: "icons/diffuse1.png"
                        defaultText: "NA/disabled"
                    }
                    WGThumbnailButton {
                        enabled: false

                        source: "icons/diffuse1.png"
                        defaultText: "NA/disabled"
                    }
                }

                WGExpandingRowLayout {

                    WGMultiLineText {
                        text: "WGThumbnail"
                    }
                    WGThumbnail {
                        enabled: true

                        source: "icons/diffuse1.png"
                    }
                    WGThumbnail {
                        enabled: true

                        source: "icons/diffuse1.png"
                    }
                    WGThumbnail {
                        enabled: false

                        source: "icons/diffuse1.png"
                    }
                    WGThumbnail {
                        enabled: false

                        source: "icons/diffuse1.png"
                    }

                    WGSeparator {
                        vertical: true
                    }

                    WGThumbnail {
                        enabled: true
                        source: ""
                    }

                    WGThumbnail {
                        enabled: true
                        multipleValues: true
                        source: ""
                    }

                    Item {
                        Layout.fillWidth: true
                    }

                }


                WGExpandingRowLayout {
                    WGMultiLineText {
                        text: "WGToolButton"
                    }
                    ExclusiveGroup { id: toolbarGroup2 }
                    WGToolButton {
                        enabled: true
                        text: "enabled, checked"

                        checkable: true
                        checked: true
                        exclusiveGroup: toolbarGroup2
                        iconSource: "icons/pause_16x16.png"
                    }
                    WGToolButton {
                        enabled: true
                        text: "enabled, unchecked"

                        checkable: true
                        checked: false
                        exclusiveGroup: toolbarGroup2
                        iconSource: "icons/pause_16x16.png"
                    }
                    WGToolButton {
                        enabled: false
                        text: "disabled, checked"

                        checkable: true
                        checked: true
                        exclusiveGroup: toolbarGroup2
                        iconSource: "icons/pause_16x16.png"
                    }
                    WGToolButton {
                        enabled: false
                        text: "disabled, unchecked"

                        checkable: true
                        checked: false
                        exclusiveGroup: toolbarGroup2
                        iconSource: "icons/pause_16x16.png"
                    }
                }


                WGExpandingRowLayout {
                    WGMultiLineText {
                        text: "WGDropDownBox20"
                    }
                    WGDropDownBox {
                        enabled: true

                        textRole: "label"
                        imageRole: "icon"

                        model: ListModel {}

                        Component.onCompleted: {
                            model.append ({"label": "enabled", "icon": Qt.resolvedUrl("icons/diffuse1.png")})
                            model.append ({"label": "Option 2", "icon": Qt.resolvedUrl("icons/diffuse2.png")})
                            model.append ({"label": "Option 3", "icon": Qt.resolvedUrl("icons/diffuse3.png")})
                        }
                    }
                    WGDropDownBox {
                        enabled: true
                        textRole: "label"
                        imageRole: "icon"

                        model: ListModel {}

                        Component.onCompleted: {
                            model.append ({"label": "enabled", "icon": Qt.resolvedUrl("icons/diffuse1.png")})
                            model.append ({"label": "Option 2", "icon": Qt.resolvedUrl("icons/diffuse2.png")})
                            model.append ({"label": "Option 3", "icon": Qt.resolvedUrl("icons/diffuse3.png")})
                        }
                    }
                    WGDropDownBox {
                        enabled: false

                        textRole: "label"
                        imageRole: "icon"

                        model: ListModel {}

                        Component.onCompleted: {
                            model.append ({"label": "enabled", "icon": Qt.resolvedUrl("icons/diffuse1.png")})
                            model.append ({"label": "Option 2", "icon": Qt.resolvedUrl("icons/diffuse2.png")})
                            model.append ({"label": "Option 3", "icon": Qt.resolvedUrl("icons/diffuse3.png")})
                        }
                    }
                    WGDropDownBox {
                        enabled: false

                        textRole: "label"
                        imageRole: "icon"

                        model: ListModel {}

                        Component.onCompleted: {
                            model.append ({"label": "disabled", "icon": Qt.resolvedUrl("icons/diffuse1.png")})
                            model.append ({"label": "Option 2", "icon": Qt.resolvedUrl("icons/diffuse2.png")})
                            model.append ({"label": "Option 3", "icon": Qt.resolvedUrl("icons/diffuse3.png")})
                        }
                    }

                    WGSeparator {
                        vertical: true
                    }

                    WGDropDownBox {
                        enabled: true
                        multipleValues: true

                        textRole: "label"
                        imageRole: "icon"

                        model: ListModel {}

                        Component.onCompleted: {
                            model.append ({"label": "enabled", "icon": Qt.resolvedUrl("icons/diffuse1.png")})
                            model.append ({"label": "Option 2", "icon": Qt.resolvedUrl("icons/diffuse2.png")})
                            model.append ({"label": "Option 3", "icon": Qt.resolvedUrl("icons/diffuse3.png")})
                        }
                    }

                    WGSeparator {
                        vertical: true
                    }

                    //No image role testing variation
                    WGDropDownBox {
                        enabled: true

                        textRole: "label"
                        //imageRole: "icon"

                        model: ListModel {}

                        Component.onCompleted: {
                            model.append ({"label": "enabled", "icon": Qt.resolvedUrl("icons/diffuse1.png")})
                            model.append ({"label": "Option 2", "icon": Qt.resolvedUrl("icons/diffuse2.png")})
                            model.append ({"label": "Option 3", "icon": Qt.resolvedUrl("icons/diffuse3.png")})
                        }
                    }

                }

                WGExpandingRowLayout {
                    WGMultiLineText {
                        text: "WGImageDropDownBox20"
                    }
                    WGImageDropDown {
                        enabled: true

                        textRole: "label"
                        imageRole: "icon"

                        model: ListModel {}

                        Component.onCompleted: {
                            model.append ({"label": "enabled", "icon": Qt.resolvedUrl("icons/diffuse1.png")})
                            model.append ({"label": "Option 2", "icon": Qt.resolvedUrl("icons/diffuse2.png")})
                            model.append ({"label": "Option 3", "icon": Qt.resolvedUrl("icons/diffuse3.png")})
                        }
                    }
                    WGImageDropDown {
                        enabled: true

                        textRole: "label"
                        imageRole: "icon"

                        model: ListModel {}

                        Component.onCompleted: {
                            model.append ({"label": "enabled", "icon": Qt.resolvedUrl("icons/diffuse1.png")})
                            model.append ({"label": "Option 2", "icon": Qt.resolvedUrl("icons/diffuse2.png")})
                            model.append ({"label": "Option 3", "icon": Qt.resolvedUrl("icons/diffuse3.png")})
                        }
                    }
                    WGImageDropDown {
                        enabled: false

                        textRole: "label"
                        imageRole: "icon"

                        model: ListModel {}

                        Component.onCompleted: {
                            model.append ({"label": "disabled", "icon": Qt.resolvedUrl("icons/diffuse1.png")})
                            model.append ({"label": "Option 2", "icon": Qt.resolvedUrl("icons/diffuse2.png")})
                            model.append ({"label": "Option 3", "icon": Qt.resolvedUrl("icons/diffuse3.png")})
                        }
                    }
                    WGImageDropDown {
                        enabled: false

                        textRole: "label"
                        imageRole: "icon"

                        model: ListModel {}

                        Component.onCompleted: {
                            model.append ({"label": "disabled", "icon": Qt.resolvedUrl("icons/diffuse1.png")})
                            model.append ({"label": "Option 2", "icon": Qt.resolvedUrl("icons/diffuse2.png")})
                            model.append ({"label": "Option 3", "icon": Qt.resolvedUrl("icons/diffuse3.png")})
                        }
                    }

                    WGSeparator {
                        vertical: true
                    }

                    WGImageDropDown {
                        enabled: true
                        multipleValues: true

                        textRole: "label"
                        imageRole: "icon"

                        model: ListModel {}

                        Component.onCompleted: {
                            model.append ({"label": "enabled", "icon": Qt.resolvedUrl("icons/diffuse1.png")})
                            model.append ({"label": "Option 2", "icon": Qt.resolvedUrl("icons/diffuse2.png")})
                            model.append ({"label": "Option 3", "icon": Qt.resolvedUrl("icons/diffuse3.png")})
                        }
                    }
                }

                WGExpandingRowLayout {
                    WGMultiLineText {
                        text: "WGImageDropDownBox20 - big"
                    }
                    WGImageDropDown {
                        enabled: true

                        implicitHeight: 100

                        textRole: "label"
                        imageRole: "icon"

                        model: ListModel {}

                        Component.onCompleted: {
                            model.append ({"label": "enabled", "icon": Qt.resolvedUrl("icons/diffuse1.png")})
                            model.append ({"label": "Option 2", "icon": Qt.resolvedUrl("icons/diffuse2.png")})
                            model.append ({"label": "Option 3", "icon": Qt.resolvedUrl("icons/diffuse3.png")})
                        }
                    }
                    WGImageDropDown {
                        enabled: true

                        implicitHeight: 100

                        textRole: "label"
                        imageRole: "icon"

                        model: ListModel {}

                        Component.onCompleted: {
                            model.append ({"label": "enabled", "icon": Qt.resolvedUrl("icons/diffuse1.png")})
                            model.append ({"label": "Option 2", "icon": Qt.resolvedUrl("icons/diffuse2.png")})
                            model.append ({"label": "Option 3", "icon": Qt.resolvedUrl("icons/diffuse3.png")})
                        }
                    }
                    WGImageDropDown {
                        enabled: false

                        implicitHeight: 100

                        textRole: "label"
                        imageRole: "icon"

                        model: ListModel {}

                        Component.onCompleted: {
                            model.append ({"label": "disabled", "icon": Qt.resolvedUrl("icons/diffuse1.png")})
                            model.append ({"label": "Option 2", "icon": Qt.resolvedUrl("icons/diffuse2.png")})
                            model.append ({"label": "Option 3", "icon": Qt.resolvedUrl("icons/diffuse3.png")})
                        }
                    }
                    WGImageDropDown {
                        enabled: false

                        implicitHeight: 100

                        textRole: "label"
                        imageRole: "icon"

                        model: ListModel {}

                        Component.onCompleted: {
                            model.append ({"label": "disabled", "icon": Qt.resolvedUrl("icons/diffuse1.png")})
                            model.append ({"label": "Option 2", "icon": Qt.resolvedUrl("icons/diffuse2.png")})
                            model.append ({"label": "Option 3", "icon": Qt.resolvedUrl("icons/diffuse3.png")})
                        }
                    }

                    WGSeparator {
                        vertical: true
                    }

                    WGImageDropDown {
                        enabled: true
                        multipleValues: true

                        implicitHeight: 100

                        textRole: "label"
                        imageRole: "icon"

                        model: ListModel {}

                        Component.onCompleted: {
                            model.append ({"label": "enabled", "icon": Qt.resolvedUrl("icons/diffuse1.png")})
                            model.append ({"label": "Option 2", "icon": Qt.resolvedUrl("icons/diffuse2.png")})
                            model.append ({"label": "Option 3", "icon": Qt.resolvedUrl("icons/diffuse3.png")})
                        }
                    }
                }

                WGSeparator {}

                Image {
                    //Overlay Controls

                    Layout.preferredWidth: 768
                    Layout.preferredHeight: 256

                    source: "icons/diffuse1.png"

                    fillMode: Image.Tile

                    ColumnLayout {
                        anchors.fill: parent
                        anchors.margins: 20

                        WGOverlayLabel {
                            text: "Overlay Controls"
                        }

                        WGExpandingRowLayout {
                            WGMultiLineText {
                                text: "WGOverlayDropDownBox"
                            }
                            WGOverlayDropDownBox {
                                enabled: true
                                model: ListModel {
                                    ListElement { text: "Enabled 1" }
                                    ListElement { text: "Option 2" }
                                    ListElement { text: "Option 3 Has a Really long Name" }
                                    ListElement { text: "Option 4" }
                                    ListElement { text: "Option 5" }
                                }
                            }
                            WGOverlayDropDownBox {
                                enabled: false
                                model: ListModel {
                                    ListElement { text: "Disabled 1" }
                                    ListElement { text: "Option 2" }
                                    ListElement { text: "Option 3 Has a Really long Name" }
                                    ListElement { text: "Option 4" }
                                    ListElement { text: "Option 5" }
                                }
                            }
                        }

                        WGExpandingRowLayout {
                            WGMultiLineText {
                                text: "WGOverlaySlider"
                            }
                            WGOverlaySlider {
                                enabled: true
                                Layout.fillWidth: true
                                Layout.preferredHeight: defaultSpacing.minimumRowHeight
                                minimumValue: 0
                                maximumValue: 100
                                stepSize: 1
                                value: 40
                            }

                            WGOverlaySlider {
                                enabled: false
                                Layout.fillWidth: true
                                Layout.preferredHeight: defaultSpacing.minimumRowHeight
                                minimumValue: 0
                                maximumValue: 100
                                stepSize: 1
                                value: 40
                            }
                        }

                        WGExpandingRowLayout {
                            Layout.fillWidth: true

                            WGMultiLineText {
                                text: "WGPushButton - WGOverlayButtonStyle"
                            }

                            WGPushButton {
                                id: testheight
                                text: "Overlay Button"
                                iconSource: "icons/loop_16x16.png"
                                style: WGOverlayButtonStyle{}

                                onClicked: {
                                    console.log("My height is " + testheight.height)
                                }
                            }


                            WGToolButton {
                                iconSource: "icons/close_16x16.png"
                                style: WGOverlayButtonStyle{}
                            }

                            WGSeparator {
                                vertical: true
                                height: 22
                            }

                            WGPushButton {
                                enabled: false
                                text: "Overlay Button - disabled"
                                iconSource: "icons/loop_16x16.png"
                                style: WGOverlayButtonStyle{}
                            }


                            WGToolButton {
                                enabled: false
                                iconSource: "icons/close_16x16.png"
                                style: WGOverlayButtonStyle{}
                            }


                            Item {
                                Layout.fillWidth: true
                            }
                        }
                    }
                }
            }
        }
    }
}

