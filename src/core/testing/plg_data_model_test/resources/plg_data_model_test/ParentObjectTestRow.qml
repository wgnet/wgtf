import QtQuick 2.1
import QtQuick.Controls 1.0
import QtQuick.Layouts 1.3
import WGControls 2.0

RowLayout {
    Text {
        text: "Number in object"
    }

    WGNumberBox {
        id: numberBox1
        objectName: "numberBox1"
        value: parentObject.number
        minimumValue: -10
        maximumValue: 10
        stepSize: 1
        decimals: 0
        readOnly: false
        enabled: true
        multipleValues: false

        Connections {
            target: parentObject
            onNumberChanged: numberBox1.value = parentObject.number
        }

        onEditingFinished: {
            parentObject.number = numberBox1.value;
        }
    }

    Text {
        text: "Number in object in object"
    }

    WGNumberBox {
        id: numberBox2
        objectName: "numberBox2"
        value: parentObject.child.number
        minimumValue: -10
        maximumValue: 10
        stepSize: 1
        decimals: 0
        readOnly: false
        enabled: true
        multipleValues: false

        onEditingFinished: {
            parentObject.child.number = numberBox2.value;
        }
    }
}
