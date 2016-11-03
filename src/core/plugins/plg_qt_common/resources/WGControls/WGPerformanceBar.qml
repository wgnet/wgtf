import QtQuick 2.5
import QtQuick.Layouts 1.3
import WGControls 1.0
import WGControls.Layouts 1.0

/*!
 \ingroup wgcontrols
 \brief A visual, non-interactive analog bar that changes colour based on the value.

Example:
\code{.js}
WGExpandingRowLayout {
    WGNumberBox {
        width: 120
        value: 25
        minimumValue: 0
        maximumValue: 100

        Binding {
            target: perfBar
            property: "value"
            value: value
        }
    }

    WGPerformanceBar {
        id: perfBar
    }
}
\endcode
*/

WGExpandingRowLayout {
    id: mainFrame
    objectName: "WGPerformanceBar"
    WGComponent { type: "WGPerformanceBar" }

    /*! This property sets the implicitWidth of the text box that displays the percentage value
        The default value is derived from WGNumberbox
    */
    property int valueBoxWidth: valueBox.implicitWidth

    /*! This property sets the prefix string used in the textbox of the performance bar.
        The default value is an empty string
    */
    property string prefix: ""

    /*! This property sets the suffix string used in the textbox of the performance bar.
        The default value is \c %
    */
    property string suffix: "%"

    /*! This property toggles the visibility of the text box displayed in this control
        The default value is \c true
    */
    property bool showValue: true

    /*! This property defines the number of decimal places displayed in the textbox
        The default value is \c 0
    */
    property int decimals: 0

    /*! This property sets the minimum value or start point of the performance bar
        The default value is \c 0
    */
    property real minimumValue: 0

    /*! This property sets the maximum value or end point of the performance bar
        The default value is \c 100
    */
    property real maximumValue: 100

    /*! This property sets the incremental step size of the performance bar
        The default value is \c 1
    */
    property real stepSize: 1

    /*! This property sets the points at which the performance bar transitions between colours
        The default value is \c [50,75,100]
    */
    property var ranges: [50,75,100]

    /*! This property sets the colour transitions used by the performance bar
        The default value is \c ["#7ac943", "#ff931e", "#e23627"]
    */
    property var colors: ["#7ac943", "#ff931e", "#e23627"]

    /*! The current value of the performance bar
    */
    property real value: 0

    /*! This property is used to define the buttons label when used in a WGFormLayout
        The default value is an empty string
    */
    property string label: ""

    /*! \internal */
    property int __rangeIndex: 0 //holds current position in ranges_

    /*! \internal */
    property real __unitWidth: (barFrame.width - defaultSpacing.doubleBorderSize) / (maximumValue - minimumValue)


    /*! \internal */
    property bool __loaded: false

    // support copy&paste
    WGCopyable {
        id: copyableControl

        WGCopyController {
            id: copyableObject

            onDataCopied : {
                setValue( mainFrame.value )
            }

            onDataPasted : {
                // readonly control
                console.log("ReadOnly Control " + label);
                //mainFrame.value_ = data
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

    /*! \internal */
    function checkColor(){
        for (var i = 0; i < mainFrame.ranges.length; i++)
        {
            if (value <= mainFrame.ranges[i])
            {
                mainFrame.__rangeIndex = i
                i = mainFrame.ranges.length
            }
        }
    }

    Component.onCompleted: {
        __loaded = true
        checkColor();
        copyableControl.disableChildrenCopyable( mainFrame );
    }

    //If the value_ is less than a ranges_ index, set the colour to the same index.
    onValueChanged: {
        if (__loaded)
        {
            checkColor()
        }
    }

    WGTextBoxFrame{
        id: barFrame
        Layout.fillWidth: true
        Layout.preferredHeight: parent.height - defaultSpacing.doubleMargin

        Rectangle {
            id: bar
            radius: defaultSpacing.halfRadius
            height: parent.height - defaultSpacing.doubleBorderSize
            anchors.verticalCenter: parent.verticalCenter
            x: 1
            color: colors[__rangeIndex]

            width: (value - mainFrame.minimumValue) * __unitWidth
        }
    }

    WGNumberBox {
        id: valueBox
        visible: mainFrame.showValue
        Layout.preferredWidth: visible ? valueBoxWidth : 0
        prefix: mainFrame.prefix
        suffix: mainFrame.suffix
        decimals: mainFrame.decimals
        value: mainFrame.value

        hasArrows: false
        readOnly: true

        minimumValue: mainFrame.minimumValue
        maximumValue: mainFrame.maximumValue
        stepSize: mainFrame.stepSize
    }

    /*! Deprecated */
    property alias valueBoxWidth_: mainFrame.valueBoxWidth

    /*! Deprecated */
    property alias prefix_: mainFrame.prefix

    /*! Deprecated */
    property alias suffix_: mainFrame.suffix

    /*! Deprecated */
    property alias showValue_: mainFrame.showValue

    /*! Deprecated */
    property alias decimals_: mainFrame.decimals

    /*! Deprecated */
    property alias minimumValue_: mainFrame.minimumValue

    /*! Deprecated */
    property alias maximumValue_: mainFrame.maximumValue

    /*! Deprecated */
    property alias stepSize_: mainFrame.stepSize

    /*! Deprecated */
    property alias ranges_: mainFrame.ranges

    /*! Deprecated */
    property alias colors_: mainFrame.colors

    /*! Deprecated */
    property alias label_: mainFrame.label

    /*! Deprecated */
    property alias value_: mainFrame.value

    /*! Deprecated */
    property alias loaded_: mainFrame.__loaded

    /*! Deprecated */
    property alias rangeIndex_: mainFrame.__rangeIndex

    /*! Deprecated */
    property alias unitWidth_: mainFrame.__unitWidth
}
