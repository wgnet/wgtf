import QtQuick 2.3
import QtQuick.Controls 1.2
import QtQuick.Layouts 1.1
import WGControls 2.0
import WGControls.Layouts 2.0

WGPanel {
	id: viewport
    color: "#191919"
    layoutHints: { 'viewport': 0.1 }

	Text {
		text: "Viewport: " + viewport.title
		font.bold: true
		color: "#636363"
		anchors.centerIn: parent
	}
}
