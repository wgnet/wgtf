import QtQuick 2.4
import QtQuick.Window 2.2
import QtQuick.Controls 1.2
import QtQuick.Layouts 1.0

import WGControls 1.0

// Toolbar
RowLayout {
    id: toolbar

    property bool editEnabled : true
	property bool timeScaleEnabled : true
    property real time: 0
    property real value: 0
    property string title: ""

    property alias timeScale: editTimeScale.number
    property alias valueScale: editValueScale.number

	signal toggleX;
	signal toggleY;
	signal toggleZ;
	signal toggleW;
    signal unscaledTimeChanged( real time );
    signal unscaledValueChanged( real value );

	WGLabel{
		text: "Time:"
	}
	WGNumberBox {
        id: editTime
        objectName: "editTime"
        enabled: toolbar.editEnabled
		maximumValue: 4294967295
		minimumValue: 0
		stepSize: 0.01
        decimals: 3
        onNumberChanged: unscaledTimeChanged(number / toolbar.timeScale)
        number: toolbar.time * toolbar.timeScale
	}
	WGLabel{
		text: "Value:"
	}
	WGNumberBox {
        id: editValue
        objectName: "editValue"
        enabled: toolbar.editEnabled
		// Removing the upper limit for the max value that is editable. 
		maximumValue: 2147483647
		minimumValue: -2147483647
		stepSize: 0.01
        decimals: 3
        onNumberChanged: unscaledValueChanged(number / toolbar.valueScale)
        number: toolbar.value * toolbar.valueScale
	}
	WGLabel{
		text: "Time Scale:"
	}
	WGNumberBox {
        id: editTimeScale
        objectName: "editTimeScale"
		enabled: toolbar.timeScaleEnabled
		maximumValue: 4294967295
		minimumValue: 0
		stepSize: 0.01
		decimals: 3
	}
	WGLabel{
		text: "Value Scale:"
	}
	WGNumberBox {
        id: editValueScale
        objectName: "editValueScale"
		maximumValue: 4294967295
		minimumValue: 0
		stepSize: 0.01
		decimals: 3
	}

    WGToolButton {
        objectName: "X"
        iconSource: "images/x.png"
		visible: curveRepeater.count > 0
		checkable: true
		checked: curveEnabled(0)
        onClicked: {
            toggleX()
        }
    }
    WGToolButton {
        objectName: "Y"
        iconSource: "images/y.png"
		visible: curveRepeater.count > 1
		checkable: true
		checked: curveEnabled(1)
        onClicked: {
            toggleY()
        }
    }
    WGToolButton {
        objectName: "Z"
        iconSource: "images/z.png"
		visible: curveRepeater.count > 2
		checkable: true
		checked: curveEnabled(2)
        onClicked: {
            toggleZ()
        }
    }
    WGToolButton {
        objectName: "W"
        iconSource: "images/w.png"
		visible: curveRepeater.count > 3
		checkable: true
		checked: curveEnabled(3)
        onClicked: {
            toggleW()
        }
    }
    // Parent our sub-title with a rectangle filling up the remaining space, otherwise our text doesn't show
    Rectangle{
        id: toolbarTitleWrapper
        color: palette.mainWindowColor
        anchors.top: parent.top
        anchors.bottom: parent.bottom
        Layout.fillWidth: true

        WGLabel {
            id: subTitleLabel
            horizontalAlignment: Text.AlignRight
            text: toolbar.title
            anchors.fill: parent
        }
    }

	// Add a space to act as a margin
	WGLabel { text: " " }
}
