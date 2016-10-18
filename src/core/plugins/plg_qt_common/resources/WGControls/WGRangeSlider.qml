import QtQuick 2.5
import QtQuick.Controls 1.4
import QtQuick.Controls.Private 1.0
import QtQuick.Layouts 1.3

import WGControls 1.0
import WGControls.Styles 1.0
import WGControls.Layouts 1.0
import WGControls.Private 1.0

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
    WGComponent { type: "WGRangeSlider" }

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

    property alias lowerTextBoxStyle: sliderLowerValue.textBoxStyle
    property alias lowerButtonFrame: sliderLowerValue.buttonFrame

    property alias upperTextBoxStyle: sliderUpperValue.textBoxStyle
    property alias upperButtonFrame: sliderUpperValue.buttonFrame

    implicitHeight: defaultSpacing.minimumRowHeight
    implicitWidth: defaultSpacing.standardMargin

    onLowerValueChanged: {
        setValueHelper(slider, "value", sliderFrame.lowerValue);
    }

    onUpperValueChanged: {
        setValueHelper(slider, "value", sliderFrame.upperValue);
    }

    // support copy&paste
    WGCopyable {
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
    }

    WGExpandingRowLayout {
        anchors.fill: parent


        WGNumberBox {
            objectName: "sliderLowerValue"
            id: sliderLowerValue

            Layout.preferredHeight: defaultSpacing.minimumRowHeight
            visible: showValue
            decimals: decimals
            Layout.preferredWidth: visible ? valueBoxWidth : 0

            prefix: sliderFrame.prefix
            suffix: sliderFrame.suffix

            value: sliderFrame.lowerValue

            minimumValue: sliderMinHandle.minimumValue
            maximumValue: sliderMinHandle.maximumValue

            stepSize: slider.stepSize

            onEditingFinished: {
                setValueHelper(sliderFrame, "value", value);
            }

            onValueChanged: {
                sliderFrame.lowerValue = value
            }


            Binding {
                target: sliderLowerValue
                property: "value"
                value: sliderFrame.lowerValue
            }
        }

        WGSlider {
            objectName: "slider"
            id: slider

            property bool showValue: true

            stepSize: 1.0

            activeFocusOnPress: true

            Layout.fillWidth: true

            Layout.preferredHeight: Math.round(sliderFrame.height)

            onPressedChanged:{
                if(!pressed)
                {
                    setValueHelper(sliderFrame, "value", value);
                }
            }

            WGSliderHandle {
                id: sliderMinHandle
                minimumValue: slider.minimumValue
                maximumValue: sliderMaxHandle.value
                showBar: false
                rangePartnerHandle: sliderMaxHandle
                value: sliderFrame.lowerValue
                maxHandle: false

                onValueChanged: {
                    sliderFrame.lowerValue = value
                }

                Binding {
                    target: sliderMinHandle
                    property: "value"
                    value: sliderFrame.lowerValue
                }
            }

            WGSliderHandle {
                id: sliderMaxHandle
                minimumValue: sliderMinHandle.value
                maximumValue: slider.maximumValue
                showBar: true
                barMinPos: sliderMinHandle.range.position
                rangePartnerHandle: sliderMinHandle
                value: sliderFrame.upperValue
                maxHandle: true

                onValueChanged: {
                    sliderFrame.upperValue = value
                }

                Binding {
                    target: sliderMaxHandle
                    property: "value"
                    value: sliderFrame.upperValue
                }
            }

            style : WGSliderStyle{

                handle: Item {
                    id: handleFrame
                    implicitWidth: defaultSpacing.minimumRowHeight - defaultSpacing.rowSpacing * 2
                    implicitHeight: defaultSpacing.minimumRowHeight - defaultSpacing.rowSpacing * 2

                    Loader {
                        sourceComponent: control.__handlePosList.children[buttonid].handleStyle

                        anchors.top: {
                            if(__horizontal)
                            {
                                !control.__handlePosList.children[buttonid].maxHandle ? parent.top : undefined
                            }
                            else
                            {
                                undefined
                            }
                        }
                        anchors.bottom: {
                            if(__horizontal)
                            {
                                control.__handlePosList.children[buttonid].maxHandle ? parent.bottom : undefined
                            }
                            else
                            {
                                undefined
                            }
                        }
                        anchors.left: {
                            if(!__horizontal)
                            {
                                !control.__handlePosList.children[buttonid].maxHandle ? parent.left : undefined
                            }
                            else
                            {
                                undefined
                            }
                        }
                        anchors.right: {
                            if(!__horizontal)
                            {
                                control.__handlePosList.children[buttonid].maxHandle ? parent.right : undefined
                            }
                            else
                            {
                                undefined
                            }
                        }

                        height:{
                            if(__horizontal)
                            {
                                control.__handlePosList.children[buttonid].__overlapping ? parent.height / 2 : parent.height
                            }
                            else
                            {
                                parent.implicitHeight
                            }
                        }

                        width: {
                            if(!__horizontal)
                            {
                                control.__handlePosList.children[buttonid].__overlapping ? parent.width / 2 : parent.width
                            }
                            else
                            {
                                parent.implicitWidth
                            }
                        }

                        Behavior on height{
                            enabled: __horizontal
                            NumberAnimation {
                                duration: 120
                                easing {
                                    type: Easing.OutCirc
                                    amplitude: 1.0
                                    period: 0.5
                                }
                            }
                        }

                        Behavior on width{
                            enabled: !__horizontal
                            NumberAnimation {
                                duration: 120
                                easing {
                                    type: Easing.OutCirc
                                    amplitude: 1.0
                                    period: 0.5
                                }
                            }
                        }
                    }
                }
            }
        }

        WGNumberBox {
            objectName: "sliderUpperValue"
            id: sliderUpperValue

            Layout.preferredHeight: defaultSpacing.minimumRowHeight
            visible: showValue
            decimals: decimals
            Layout.preferredWidth: visible ? valueBoxWidth : 0

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
                sliderFrame.upperValue = value
            }


            Binding {
                target: sliderUpperValue
                property: "value"
                value: sliderFrame.upperValue
            }
        }
    }

    /*! Deprecated */
    property alias label_: sliderFrame.label
}
