import QtQuick 2.5
import QtQuick.Controls 1.2
import QtQuick.Controls.Private 1.0
import QtQuick.Layouts 1.1

import WGControls 1.0

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

    /*! This property defines what sliderstyle styling component to use for this control */
    property alias style: slider.style

    /*! This property defines the value indicated by the control
        The default value is \c 0.0
    */
    property real value

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

    property alias textBoxStyle: sliderValue.textBoxStyle
    property alias buttonFrame: sliderValue.buttonFrame

    implicitHeight: defaultSpacing.minimumRowHeight
    implicitWidth: defaultSpacing.standardMargin

    onValueChanged: {
        setValueHelper(slider, "value", sliderFrame.value);
        setValueHelper(sliderValue, "value", sliderFrame.value);
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

    WGExpandingRowLayout {
        id: sliderLayout
        anchors.fill: parent

        Rectangle {
            id: fakeValue
            color: "transparent"
            Layout.preferredWidth: fakeLowerValue ? valueBoxWidth : 0
            Layout.preferredHeight: defaultSpacing.minimumRowHeight
            visible: fakeLowerValue ? true : false
        }

        WGSlider {
            id: slider
            opacity: 1.0

            property bool showValue: true

            stepSize: 1.0

            Layout.fillWidth: __horizontal ? true : false
            Layout.fillHeight: __horizontal ? false : true

            activeFocusOnPress: true

            Layout.preferredHeight: __horizontal ? Math.round(sliderFrame.height) : -1

            WGSliderHandle {
                id: sliderHandle
                minimumValue: slider.minimumValue
                maximumValue: slider.maximumValue
                showBar: true

                value: sliderFrame.value;

                onValueChanged: {
                    if ( slider.__handleMoving) {
                        setValueHelper(sliderFrame, "value", value);
                    }
                }
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
            Layout.preferredWidth: visible ? valueBoxWidth : 0

            Layout.minimumWidth: visible ? valueBoxWidth : 0

            Layout.preferredHeight: defaultSpacing.minimumRowHeight
            visible: showValue
            decimals: sliderFrame.decimals

            prefix: sliderFrame.prefix
            suffix: sliderFrame.suffix

            value: sliderFrame.value

            minimumValue: sliderFrame.minimumValue
            maximumValue: sliderFrame.maximumValue

            stepSize: slider.stepSize

            //Keyboard enter key input
            onEditingFinished: {
                setValueHelper(sliderFrame, "value", value);
            }


            onValueChanged: {
                setValueHelper(sliderHandle, "value", value);
            }

        }
    }
    /*! Deprecated */
    property alias label_: sliderFrame.label

    /*! Deprecated */
    property bool timeObject: false
}
