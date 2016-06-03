import QtQuick 2.1
import QtQuick.Controls 1.0
import WGControls 1.0

Row
{
	property var source;

	WGTextBox {
		id: numeric
		text: source.numeric
		width: 100
		height: 20
		Binding {
			target: source
			property: "numeric"
			value: numeric.text
		}
	}

	WGTextBox {
		text: source.string
		width: 120
		height: 20
		onTextChanged: {
			source.string = text;
		}
	}

	WGPushButton {
		text: "Increment!"
		width: 100
		height: 20
		onClicked: {
			source.incrementNumeric(parseInt(incrementValue.text));
		}
	}

	WGTextBox {
		id: incrementValue
		width: 100
		height: 20
		text: "1"
	}
	
	Connections {
		target: source

		onIncrementNumericInvoked: {
			source.numericChanged(0);
		}
	}
}