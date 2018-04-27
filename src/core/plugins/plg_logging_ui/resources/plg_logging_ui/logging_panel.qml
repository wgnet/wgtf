import QtQuick 2.3
import QtQuick.Controls 1.4
import QtQuick.Layouts 1.0
import WGControls 1.0
 
WGLoggingPanel {
	text: loggingModel.text
	layoutHints: { 'bottom': 0.1 }

	WGComponent { type: "logging_panel" }
	onClearText: loggingModel.clearText();
}