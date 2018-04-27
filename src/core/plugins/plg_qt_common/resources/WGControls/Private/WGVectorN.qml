import QtQuick 2.5
import QtQuick.Layouts 1.3

import WGControls 2.0
import WGControls.Layouts 2.0

/*!
 \ingroup wgcontrols
 \brief A generic vector control that can be used to represent any number of vectors.
 Use WGVector2, WGVector3, WGVector4, WGVectorRGB, and WGVectorRGBA instead of this control for
 preset values.
*/

FocusScope {
    id: mainLayout
    objectName: "WGVectorN"

    /*! This property holds each individual vectors label */
    property variant vectorLabels: []
    /*! This property holds the vector values */
    property variant vectorData: []

    //properties for controlling spinner behaviour
    property var stepSizes: [0.1, 0.1, 0.1, 0.1]
    property var maximumValues: [2147483647, 2147483647, 2147483647, 2147483647]
    property var minimumValues: [-2147483647, -2147483647, -2147483647, -2147483647]
    property var decimals: [3, 3, 3, 3]
    property var defaultValues: minimumValues

    property bool multipleValues: false
    property bool readOnly: false

    //binding changes in value back to data
    signal elementChanged (var value_, int index)

    onElementChanged: {
        vectorData[index] = value_
    }

    Text {
        id: textWidthHelper
        visible: false
        text: "999.999"
    }

    Text {
        id: labelWidthHelper
        visible: false
        text: vectorLabels.toString()
    }

    implicitHeight: mainLayout.width <= mainLayout.rowLayoutImplicitWidth ? defaultSpacing.minimumRowHeight * vectorData.length : defaultSpacing.minimumRowHeight

    property int rowLayoutImplicitWidth: (textWidthHelper.contentWidth + defaultSpacing.doubleMargin) * vectorData.length + labelWidthHelper.contentWidth

    implicitWidth: textWidthHelper.contentWidth + defaultSpacing.doubleMargin + labelWidthHelper.contentWidth

    GridLayout {
        id: grid
        anchors.fill: parent
        columnSpacing: 0
        rowSpacing: 0
        flow: mainLayout.width > mainLayout.rowLayoutImplicitWidth ? GridLayout.LeftToRight : GridLayout.TopToBottom
        columns: vectorData.length
        rows: vectorData.length
        Repeater {
            id: spinBoxRepeater
            model: vectorData.length
            delegate: Item {
                id: layoutN
                Layout.preferredHeight: defaultSpacing.minimumRowHeight
                Layout.fillWidth: true
                Layout.preferredWidth: numboxN.implicitWidth + labelN.width
                Layout.minimumWidth: textWidthHelper.contentWidth + labelN.width

                property var value_: vectorData[index]

                WGLabel{
                    id: labelN
                    anchors.left: parent.left
                    anchors.verticalCenter: parent.verticalCenter
                    width: contentWidth + defaultSpacing.doubleMargin
                    height: parent.height
                    text: vectorLabels[index]
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                }

                WGNumberBox {
                    id: numboxN
                    objectName: mainLayout.objectName + "_numbox_" + index
                    anchors.left: labelN.right
                    anchors.right: parent.right
                    height: parent.height

                    multipleValues: mainLayout.multipleValues
                    readOnly: mainLayout.readOnly

                    value: layoutN.value_

                    focus: true

                    onEditingFinished: {
                        mainLayout.elementChanged(value, index)
                    }

                    maximumValue: mainLayout.maximumValues[index]
                    minimumValue: mainLayout.minimumValues[index]
                    stepSize: mainLayout.stepSizes[index]
                    decimals: mainLayout.decimals[index]
                    defaultValue: mainLayout.defaultValues[index]
                }
            } //end delegate
        }
    }
}
