import QtQuick 2.3
import QtQuick.Controls 1.2
import QtQuick.Layouts 1.0
import WGControls 2.0
import WGControls.Views 2.0
import WGControls.Layouts 2.0

// WGPanel triggers auto-reloading when QML file is saved
WGPanel {
	objectName: "WGMacroView"
	WGComponent { type: "WGMacroView" }
    color: palette.MainWindowColor
    title: "Macros"
    layoutHints: { 'macros': 0.1 }
    property var topControlsHeight: 20

    RowLayout {
        id: statusBar
        anchors {left: parent.left; top: parent.top; right: parent.right}
        anchors.margins: {left: defaultSpacing.doubleMargin; right: defaultSpacing.doubleMargin;}
        height: defaultSpacing.minimumRowHeight + defaultSpacing.standardMargin

        WGLabel {
            text: "Macros: "
            font.bold: true
        }

        Item {
            //invisible spacer
            Layout.fillWidth: true
            Layout.preferredHeight: defaultSpacing.minimumRowHeight
        }

        WGPushButton {
            id: rearrangePanels
            text: !recording ? "Record" : "Stop"
            checkable: true
            checked: recording
			property bool recording: false
			enabled: false
            onClicked: {
                recording = !recording;
				if(!recording){
				// create a new macro based on records
				}
            }
        }
    }
	
    WGScrollView {
        id: scrollPanel
        anchors.top: statusBar.bottom
		anchors.left: parent.left
		anchors.right: parent.right
		anchors.bottom: parent.bottom
        WGListView {
            model: macroModel
			columnWidth: parent.width
			columnDelegates: [macroItemComponent]
            property Component macroItemComponent: WGCompoundCommandDelegate {
                id: macroDisplayObject
				width: columnWidth
            }
        }
    }
}
