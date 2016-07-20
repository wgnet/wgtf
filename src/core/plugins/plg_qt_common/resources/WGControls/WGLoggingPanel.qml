import QtQuick 2.3
import QtQuick.Controls 1.4
import QtQuick.Layouts 1.0
import WGControls 1.0

/*!
 \brief Output window for logging messages from ILoggingSystem

Example:
\code{.js}
WGLoggingPanel {
    dataModel: myImplementation
}
\endcode
*/
 
WGPanel {
	id: loggingPanel
	title: "Log"
	color: palette.MainWindowColor

	/*! This alias references an implementation of an ILoggingModel.
	*/
	property var dataModel
	
	TextArea {
		id: logTextArea
		anchors.fill: parent
		readOnly: true
		wrapMode: Text.NoWrap
		textFormat: Text.AutoText

		text: dataModel.text
	}
}