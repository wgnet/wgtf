import QtQuick 2.5
import WGControls 2.0

WGEnum {
	id: enumControl
    objectName: "WGReflectedEnum"
	WGComponent { type: "WGReflectedEnum20" }

	property var object: null
	property var path: ""
	property var value

	QtObject {
		id: internal
		property var enumData: object != null ? object.getMetaObject(path, "Enum") : null
		property var valueType: ""
	}

	enumString: internal.enumData != null ? internal.enumData.enumString : ""
	enumGenerator: function() { return internal.enumData != null ? internal.enumData.generateEnum(object) : null; }

	Component.onCompleted: {
		internal.valueType = typeof value;
		if (internal.valueType == 'number') {
			valueInt = Qt.binding(function() { return value; });
		}
		if (internal.valueType == 'string') {
			valueStr = Qt.binding(function() { return value; });
		}
	}

	onValueIntChanged: {
		if (internal.valueType == 'number' && value != valueInt) {
			setValueHelper(enumControl, "value", valueInt);
		}
	}

	onValueStrChanged: {
		if (internal.valueType == 'string' && value != valueStr) {
			setValueHelper(enumControl, "value", valueStr);
		}
	}
}

