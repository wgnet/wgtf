import QtQuick 2.5
import QtQuick.Controls 1.4
import QtQuick.Controls.Private 1.0
import QtQuick.Controls.Styles 1.4
import WGControls 2.0
import WGControls.Styles 2.0

/*!
 \ingroup wgcontrols
 \brief A QML object that accepts and returns data based on a conversion function.
    Used to convert numeric data such as sliders or number boxes into a different format for user editing
    without affecting the underlying data.

    Example:
    \code{.js}
    WGNumberBox {
        id: degNum
        value: degConv.value
        stepSize: degConv.stepSize
        decimals: degConv.decimals
        minimumValue: degConv.minimumValue
        maximumValue: degConv.maximumValue

        WGDataConversion {
            id: degConv
            dataTarget: myNumberBoxDataObject
            //converts 0 - 1 to degrees
            dataToValue: function(val) {
                return val * 360
            }
            valueToData: function(val) {
                return val / 360
            }
        }

        onEditingFinished: {
            degConv.changeValue(degNum.value)
        }
    }
    \endcode
*/

Item {
    id: dataConversion
    objectName: "WGDataConversion"
    WGComponent { type: "WGDataConversion20" }

    /*! A helper object to set the conversion values.

        If this target has the same format of data as a Slider or NumberBox and has an onChangeValue signal
        then only the dataToValue and valueToData functions need to be set.
    */
    property QtObject dataTarget: fakeData

    /*! The data value the conversion will be performed on.

        By default this is the result of dataToValue(dataTarget.value) or 0 if it is undefined.
    */
    property var value: typeof dataTarget.value != "undefined" ? dataToValue(dataTarget.value) : fakeData.value

    /*! The converted minimumValue of the data.

        By default this is the result of dataToValue(dataTarget.minimumValue) or 0 if it is undefined.
    */
    property var minimumValue: typeof dataTarget.minimumValue != "undefined" ? dataToValue(dataTarget.minimumValue) : fakeData.minimumValue

    /*! The converted maximumValue of the data.

        By default this is the result of dataToValue(dataTarget.maximumValue) or 1 if it is undefined.
    */
    property var maximumValue: typeof dataTarget.maximumValue != "undefined" ? dataToValue(dataTarget.maximumValue) : fakeData.maximumValue

    /*! The stepSize of the data.

        By default this is dataTarget.stepSize or 0.1 if it is undefined.
    */
    property var stepSize: typeof dataTarget.stepSize != "undefined" ? dataTarget.stepSize : fakeData.stepSize

    /*! The number of decimal places of the data. This is NOT the value used for round() in onValueChanged.

        By default this is dataTarget.decimals or 1 if it is undefined.
    */
    property int decimals: typeof dataTarget.decimals != "undefined" ? dataTarget.decimals : fakeData.decimals

    /*! The prefix of the data.

        By default this is dataTarget.suffix or a blank string if it is undefined.
    */
    property string prefix: typeof dataTarget.prefix != "undefined" ? dataTarget.prefix : fakeData.prefix

    /*! The suffix of the data.

        By default this is dataTarget.suffix or a blank string if it is undefined.
    */
    property string suffix: typeof dataTarget.suffix != "undefined" ? dataTarget.suffix : fakeData.suffix

    /*! Performs a conversion on the initial data (val) and returns the converted value

        By default this returns the input. Change this to perform a conversion function.
    */
    property var dataToValue: function(val) {
        return val
    }

    /*! Perform a conversion on a value (val) and returns it in the format expected by the data

        By default this returns the input. This should probably be the inverse of dataToValue.
    */
    property var valueToData: function(val) {
        return val
    }

    /*! This should be sent by the control editing the data when it changes value and/or the user edits a value.
    */
    signal changeValue(var val)

    /*! If true and the dataTarget has a changeValue signal, the data will be automatically updated.

        The default value is true if dataTarget is set and not "undefined"
    */
    property bool autoChangeData: typeof dataTarget.changeValue != "undefined" && dataTarget != fakeData

    /*! If dataTarget has a round function this will compare the rounded values of the data onChangeValue
        passing on the signal if autoChangeData is true.
    */
    property var round: function(num) {
        if (typeof dataTarget.round != "undefined" && dataTarget != fakeData)
        {
            return dataTarget.round(num)
        }
        else
        {
            return num
        }
    }

    onChangeValue: {
        if (autoChangeData)
        {
            if (round(dataTarget.value) != round(valueToData(val)))
            {
                dataTarget.changeValue(valueToData(val))
            }
        }
    }

    // some placeholder data to stop undefined errors when loading or if the values are not required.
    Item {
        id: fakeData
        property var value: 0
        property var minimumValue: 0
        property var maximumValue: 1
        property var stepSize: 0.1
        property int decimals: 1
        property string prefix: ""
        property string suffix: ""
    }
}
