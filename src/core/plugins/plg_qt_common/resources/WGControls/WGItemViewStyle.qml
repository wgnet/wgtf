import QtQuick 2.4
import QtQuick.Controls 1.2
import QtQml.Models 2.2
import WGControls 2.0

Item {
	property Component rowBackground: Item {}
	property Component rowHeader: Item {}
	property Component rowFooter: Item {}

	property Component columnBackground: Item {}
	property Component columnHeader: Item {}
	property Component columnFooter: Item {}

	property Component columnHandle: Rectangle {
        color: palette.darkColor
	}
}