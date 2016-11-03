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

WGExpandingRowLayout {
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
        text: "9999.999"
    }

    Text {
        id: labelWidthHelper
        visible: false
        text: vectorLabels.toString()
    }

    spacing: 0
    implicitHeight: defaultSpacing.minimumRowHeight ? defaultSpacing.minimumRowHeight : 22
    implicitWidth: (textWidthHelper.contentWidth + defaultSpacing.doubleMargin) * vectorData.length + labelWidthHelper.contentWidth

    Repeater {
        id: spinBoxRepeater
        model: vectorData.length
        delegate: Item {
            id: layoutN
            Layout.preferredHeight: parent.height
            Layout.fillWidth: true
            Layout.minimumWidth: numboxN.implicitWidth + labelN.width

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
                anchors.left: labelN.right
                anchors.right: parent.right
                height: parent.height

                multipleValues: mainLayout.multipleValues
                readOnly: mainLayout.readOnly

                number: layoutN.value_

                onNumberChanged: {
                    mainLayout.elementChanged(number, index)
                }

                maximumValue: mainLayout.maximumValues[index]
                minimumValue: mainLayout.minimumValues[index]
                stepSize: mainLayout.stepSizes[index]
                decimals: mainLayout.decimals[index]
                defaultValue: mainLayout.defaultValues[index]
            }
        } //end delegate
    }//end repeater
}
