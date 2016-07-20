import QtQuick 2.5
import QtQuick.Controls 1.2
import QtQuick.Controls.Private 1.0
import QtQuick.Layouts 1.1
import WGControls 2.0

//TODO: Test orientation = vertical. Create vertical slider. Remove option here
//Resizing the slider could be smarter. Does not take into account content of spinner width

/*!
 \brief Slider with value spinbox.
 Purpose: Provide the user with a single value clamped between min and max value

Example:
\code{.js}
WGSliderControl {
    Layout.fillWidth: true
    minimumValue: 0
    maximumValue: 100
    stepSize: 1
    value: 40
}
\endcode
*/

Item {
    id: sliderFrame
    objectName: "WGSliderControl"

    /*! This property holds the maximum value of the slider.
        The default value is \c{1.0}.
    */
    property alias maximumValue: slider.maximumValue

    /*! This property holds the minimum value of the slider.
        The default value is \c{0.0}.
    */
    property alias minimumValue: slider.minimumValue

    /*!
        This property indicates the slider step size.

        A value of 0 indicates that the value of the slider operates in a
        continuous range between \l minimumValue and \l maximumValue.

        Any non 0 value indicates a discrete stepSize.

        The default value is \c{0.0}.
    */
    property alias stepSize: slider.stepSize

    /*!
        This property holds the layout orientation of the slider.
        The default value is \c Qt.Horizontal.
    */
    /* TODO: It is likely that this does not work. It should be tested and disabled
       A separate vertical slider should probably be made */
    property alias orientation: slider.orientation

    /*! This property defines what SliderStyle component will be used for the slider */
    property alias style: slider.style

    /*! This property defines what Slider Handle component will be used for the slider handle */
    property alias handleType: slider.handleType

    /*! This property defines what frame component will be used for the numberbox text box */
    property alias textBoxStyle: sliderValue.textBoxStyle

    /*! This property defines what frame component will be used for the numberbox buttons */
    property alias buttonFrame: sliderValue.buttonFrame

    /*! property indicates if the control represetnts multiple data values */
    property bool multipleValues: false

    /*! This property defines the value indicated by the control
        The default value is \c 0.0
    */
    property real value

    /*! This property defines the colour of the slider */
    property alias barColor: slider.barColor

    /*! This property defines whether the tickmarks are displayed or not */
    property alias tickmarksEnabled: slider.tickmarksEnabled

    /*! This property determines the prefix string displayed within the slider textbox.
        Typically used to display unit type.
        The default value is an empty string.
    */
    property string prefix: ""

    /*! This property determines the suffix string displayed within the slider textbox.
        Typically used to display unit type.
        The default value is an empty string.
    */
    property string suffix: ""


    /*! This property defines the number of decimal places displayed in the textbox
        The default value is \c 1
    */
    property int decimals: 1


    /*! This property determines whether a number box will be displayed next to the slider

      The default value is \c true
    */
    property bool showValue: true

    /*! This property determines whether a space will be made on the left of the slider
      so that it lines up with a range slider.

      The default value is \c false
    */
    property bool fakeLowerValue: false

    /*! This property can be used to give the number box(es) a set width.

      The default value is based on the implicit width of the valuebox
    */
    property int valueBoxWidth: sliderValue.implicitWidth

    /*! This property is used to define the buttons label when used in a WGFormLayout
        The default value is an empty string
    */

    /*!
        This property determines if the slider groove should have padding to fit inside the overall control size.

        This is useful to make sure the handles don't move outside the control boundaries but means the control values
        don't exactly line up with the control height/width in a linear fashion. (the value is always accurate)

        The default value is \ctrue
    */
    property alias handleClamp: slider.handleClamp

    /*! This property is used to define the slider's label when used in a WGFormLayout
        The default value is an empty string
    */
    property string label: ""

    /*! \internal */
    property alias __slider: slider

    /*! \internal */
    property bool __horizontal: orientation === Qt.Horizontal

    implicitHeight: defaultSpacing.minimumRowHeight
    implicitWidth: defaultSpacing.standardMargin

    onValueChanged: {
        setValueHelper(slider, "value", sliderFrame.value);
        setValueHelper(sliderValue, "value", sliderFrame.value);
        setValueHelper(slider.__handlePosList[0], "value", sliderFrame.value);
    }

    // support copy&paste
    WGCopyable {
        objectName: "copyableControl"
        id: copyableControl

        WGCopyController {
            id: copyableObject

            onDataCopied : {
                setValue( slider.value )
            }

            onDataPasted : {
                setValueHelper(sliderFrame, "value", data)
                if(sliderFrame.value != data)
                {
                    bPasted = false;
                }
            }
        }

        onSelectedChanged : {
            if(selected)
            {
                selectControl( copyableObject )
            }
            else
            {
                deselectControl( copyableObject )
            }
        }
    }

    Component.onCompleted: {
        copyableControl.disableChildrenCopyable( sliderFrame );
        setValueHelper(slider, "value", sliderFrame.value);
        setValueHelper(sliderValue, "value", sliderFrame.value);
    }
    RowLayout {
        anchors.fill: parent
        visible: __horizontal

        Item {
            id: horizLower
            visible: fakeLowerValue
            Layout.fillHeight: true
            Layout.preferredWidth: valueBoxWidth
        }
        Item {
            id: horizSlider
            Layout.fillHeight: true
            Layout.fillWidth: true
        }
        Item {
            id: horizUpper
            visible: showValue
            Layout.fillHeight: true
            Layout.preferredWidth: valueBoxWidth
        }
    }

    ColumnLayout {
        anchors.fill: parent
        visible: !__horizontal

        Item {
            id: vertLower
            visible: fakeLowerValue
            Layout.preferredHeight: defaultSpacing.minimumRowHeight
            Layout.preferredWidth: valueBoxWidth
            Layout.alignment: Qt.AlignVCenter | Qt.AlignHCenter
        }

        Item {
            id: vertSlider
            Layout.fillHeight: true
            Layout.fillWidth: true
        }

        Item {
            id: vertUpper
            visible: showValue
            Layout.preferredHeight: defaultSpacing.minimumRowHeight
            Layout.preferredWidth: valueBoxWidth
            Layout.alignment: Qt.AlignVCenter | Qt.AlignHCenter
        }
    }

    Rectangle {
        id: fakeValue
        color: "transparent"

        parent: __horizontal ? horizLower : vertLower

        width:  valueBoxWidth
        height:  defaultSpacing.minimumRowHeight
        visible: fakeLowerValue ? true : false
    }

    WGSlider {
        id: slider
        opacity: 1.0

        property bool showValue: true

        stepSize: 1.0

        parent: __horizontal ? horizSlider : vertSlider

        anchors.fill: parent

        activeFocusOnPress: true

        Layout.preferredHeight: __horizontal ? Math.round(sliderFrame.height) : -1

        value: sliderFrame.value;

        multipleValues: sliderFrame.multipleValues

        onValueChanged: {
            if ( __handleMoving ) {
                setValueHelper(sliderFrame, "value", value);
            }
        }

        onChangeValue: {
            setValueHelper(sliderFrame, "value", value);
        }

        style : WGSliderStyle{

        }

        states: [
            State {
                name: ""
                when: sliderFrame.width < sliderValue.Layout.preferredWidth + sliderHandle.width
                PropertyChanges {target: slider; opacity: 0}
                PropertyChanges {target: sliderLayout; spacing: 0}
                PropertyChanges {target: slider; visible: false}
            },
            State {
                name: "HIDDENSLIDER"
                when: sliderFrame.width >= sliderValue.Layout.preferredWidth + sliderHandle.width
                PropertyChanges {target: slider; opacity: 1}
                PropertyChanges {target: sliderLayout; spacing: defaultSpacing.rowSpacing}
                PropertyChanges {target: slider; visible: true}
            }
        ]

        transitions: [
            Transition {
                from: ""
                to: "HIDDENSLIDER"
                NumberAnimation { properties: "opacity"; duration: 200 }
            },
            Transition {
                from: "HIDDENSLIDER"
                to: ""
                NumberAnimation { properties: "opacity"; duration: 200 }
            }
        ]
    }

    WGNumberBox {
        objectName: "NumberBox"
        id: sliderValue
        parent: __horizontal ? horizUpper : vertUpper

        width:  valueBoxWidth
        height:  defaultSpacing.minimumRowHeight

        Layout.preferredHeight: defaultSpacing.minimumRowHeight
        visible: showValue
        decimals: sliderFrame.decimals

        prefix: sliderFrame.prefix
        suffix: sliderFrame.suffix

        value: sliderFrame.value
        multipleValues: sliderFrame.multipleValues

        minimumValue: sliderFrame.minimumValue
        maximumValue: sliderFrame.maximumValue

        stepSize: slider.stepSize

        //Keyboard enter key input
        onEditingFinished: {
            setValueHelper(sliderFrame, "value", value);
        }

        onValueChanged: {
            setValueHelper(sliderFrame, "value", value);
        }
    }

    /*! Deprecated */
    property alias label_: sliderFrame.label

    /*! Deprecated */
    property bool timeObject: false
}
