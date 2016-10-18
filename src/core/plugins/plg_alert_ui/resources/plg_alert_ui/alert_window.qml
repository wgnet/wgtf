import QtQuick 2.3
import QtQuick.Controls 1.2
import QtQuick.Layouts 1.0
import QtQuick.Window 2.1

import WGControls 2.0
import WGControls.Views 2.0

Rectangle {
	property var title: "Alerts"

    id: main
    WGComponent { type: "alert_window" }

	color: palette.mainWindowColor

    /// Remove the alert message at the index passed in
    /// See alert_models.cpp ( AlertPageModel::removeAlert )

    function removeAlertAt( index ){
        currentSelectedRowIndex = index;
        removeAlert;
    }

	WGListModel {
		id : alertModel
		source : alerts

		ValueExtension {}
	}

    // Could have something like a WGFrame here but as this will probably be
	// some kind of invisible 'floating' window we don't really need it.

    WGListView {
		// ListView needs to fill the parent Rectangle with some margins around the entire view
                        anchors.fill: parent
       anchors.margins: defaultSpacing.standardMargin
		model: alertModel
		interactive: true

		// Replacing the default delegate of the WGListView with our custom Alert Frame
		// Made this a new control so it can be edited independently of this window and also be used elsewhere
		delegate: WGAlertFrame {
			// The alert needs to know its width with anchors but we'll let it decide its own height.
			anchors.left: parent.left
			anchors.right: parent.right
                }
            }
        }


