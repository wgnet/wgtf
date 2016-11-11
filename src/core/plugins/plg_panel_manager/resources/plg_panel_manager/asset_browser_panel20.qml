import QtQuick 2.3
import QtQuick.Controls 1.2
import QtQuick.Layouts 1.0
import QtQuick.Window 2.1
import WGControls 2.0
import WGControls.Views 2.0

WGPanel {
	id: mainFrame
    WGComponent { type: "asset_browser_panel20" }

	title: "Asset Browser2.0"
	layoutHints: { 'assetbrowser': 1.0, 'bottom': 0.5 }

	color: palette.mainWindowColor	
	Layout.fillWidth: true
	Layout.fillHeight: true

	Keys.forwardTo: [assetBrowserControl]

	property var __nameFilters: nameFilters
	property var __iconSize: iconSize
	property var __assetAccepted: assetAccepted

	WGAssetBrowser {
		id: assetBrowserControl
        anchors.fill: parent
        anchors.margins: defaultSpacing.standardMargin

		model: assetModel
		nameFilters: __nameFilters
		iconSize: __iconSize
		
		onAssetAccepted: {
			__assetAccepted(assetPath);
		}
	}	
}
