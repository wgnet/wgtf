import QtQuick 2.5
import QtQuick.Controls 1.4
import QtQuick.Controls.Private 1.0
import QtQuick.Layouts 1.3

import WGControls 2.0
import WGControls.Private 2.0

/*!
 \ingroup wgcontrols
 \brief Slider with two handles that encompasses a range of values.

\code{.js}
WGRangeSlider {
    Layout.fillWidth: true
    minimumValue: 0
    maximumValue: 100
    lowerValue: 25
    upperValue: 75
    stepSize: 1
    enabled: true
}
\endcode
*/

Item {
    id: sliderFrame
    objectName: "WGSliderControl"
    WGComponent { type: "WGRangeSlider20" }

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

    /*! This property defines what sliderstyle styling component to use for this control */
    property alias style: slider.style

    /*! This property defines what Slider Handle component will be used for the slider handle */
    property alias handleType: slider.handleType

    /*! This property defines what frame component will be used for the numberbox text box */
    property alias textBoxStyle: sliderUpperValue.textBoxStyle

    /*! This property defines what frame component will be used for the numberbox buttons */
    property alias buttonFrame: sliderUpperValue.buttonFrame

    /*! This property defines the lower value indicated by the control
        The default value is \c 0.0
    */

    property real lowerValue

    /*! This property defines the upper value indicated by the control
        The default value is \c 0.0
    */

    property real upperValue

    /*! This property defines the colour of the slider */
    property alias barColor: slider.barColor

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

    /*! This property is used to define the sliders label when used in a WGFormLayout
        The default value is an empty string
    */
    property string label: ""

    /*! This property is used to define the width of the number box.
        The default value is the implicit width.
    */
    property int valueBoxWidth: sliderUpperValue.implicitWidth

    /*! This property determines if the number box will be visible or not.
        The default value is true.
    */
    property bool showValue: true

    /*! This property is used to leave an empty space in the case where you want a regular slider
        to line up with a range slider.
        The default value is false.
    */
    property bool fakeLowerValue: false

    /*!
        This property determines if the slider groove should have padding to fit inside the overall control size.

        This is useful to make sure the handles don't move outside the control boundaries but means the control values
        don't exactly line up with the control height/width in a linear fashion. (the value is always accurate)

        The default value is \ctrue
    */
    property alias handleClamp: slider.handleClamp

    /*! \internal */
    property bool __horizontal: orientation === Qt.Horizontal

    implicitHeight: defaultSpacing.minimumRowHeight
    implicitWidth: defaultSpacing.standardMargin

    onLowerValueChanged: {
        setValueHelper(slider, "value", sliderFrame.lowerValue);
    }

    onUpperValueChanged: {
        setValueHelper(slider, "value", sliderFrame.upperValue);
    }

    Component.onCompleted: {
        setValueHelper(slider, "value", sliderFrame.value);
    }

    RowLayout {
        anchors.fill: parent
        visible: __horizontal

        Item {
            id: horizLower
            visible: showValue
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
            id: vertUpper
            visible: showValue
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
            id: vertLower
            visible: showValue
            Layout.preferredHeight: defaultSpacing.minimumRowHeight
            Layout.preferredWidth: valueBoxWidth
            Layout.alignment: Qt.AlignVCenter | Qt.AlignHCenter
        }
    }

    WGNumberBox {
        objectName: "sliderLowerValue"
        id: sliderLowerValue

        parent: __horizontal ? horizLower : vertLower

        width:  valueBoxWidth
        height:  defaultSpacing.minimumRowHeight

        visible: showValue
        decimals: sliderFrame.decimals

        prefix: sliderFrame.prefix
        suffix: sliderFrame.suffix

        value: sliderFrame.lowerValue

        minimumValue: sliderMinHandle.minimumValue
        maximumValue: sliderMinHandle.maximumValue

        stepSize: slider.stepSize
        buttonFrame: sliderFrame.buttonFrame
        textBoxStyle: sliderFrame.textBoxStyle

        onEditingFinished: {
            setValueHelper(sliderFrame, "value", value);
        }

        onValueChanged: {
            setValueHelper(sliderFrame, "lowerValue", value)
        }
    }

    WGSlider {
        objectName: "slider"
        id: slider

        property bool showValue: true

        stepSize: 1.0

        activeFocusOnPress: true

        parent: __horizontal ? horizSlider : vertSlider

        anchors.fill: parent

        onPressedChanged:{
            if(!pressed)
            {
                setValueHelper(sliderFrame, "value", value);
            }
        }


        WGRangeSliderHandle {
            id: sliderMinHandle
            minimumValue: slider.minimumValue
            maximumValue: sliderMaxHandle.value
            showBar: false
            rangePartnerHandle: sliderMaxHandle
            value: sliderFrame.lowerValue
            maxHandle: false

            onValueChanged: {
                setValueHelper(sliderFrame, "lowerValue", value)
            }
        }

        WGRangeSliderHandle {
            id: sliderMaxHandle
            minimumValue: sliderMinHandle.value
            maximumValue: slider.maximumValue
            showBar: true
            barMinPos: Math.round((((sliderMinHandle.value - slider.minimumValue) / (slider.maximumValue - slider.minimumValue)) * parentSlider.__clampedLength) + slider.__visualMinPos)
            rangePartnerHandle: sliderMinHandle
            value: sliderFrame.upperValue
            maxHandle: true

            onValueChanged: {
                setValueHelper(sliderFrame, "upperValue", value)
            }
        }
    }

    WGNumberBox {
        objectName: "sliderUpperValue"
        id: sliderUpperValue

        parent: __horizontal ? horizUpper : vertUpper

        width:  valueBoxWidth
        height:  defaultSpacing.minimumRowHeight

        visible: showValue
        decimals: sliderFrame.decimals

        prefix: sliderFrame.prefix
        suffix: sliderFrame.suffix

        value: sliderFrame.upperValue

        minimumValue: sliderMaxHandle.minimumValue

        maximumValue: sliderMaxHandle.maximumValue

        stepSize: slider.stepSize

        onEditingFinished: {
            setValueHelper(sliderFrame, "value", value);
        }

        onValueChanged: {
            setValueHelper(sliderFrame, "upperValue", value)
        }
    }

    /*! Deprecated */
    property alias label_: sliderFrame.label
}
