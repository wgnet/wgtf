import QtQuick 2.5
import QtQuick.Layouts 1.3

import WGControls 2.0
import WGControls.Layouts 2.0

/*!
 \ingroup wgcontrols
 \brief An AssetBrowser specific file dialog frame, 
 used by WGAssetBrowserDialog and custom qml dialogs
*/
Rectangle {
    id: dialogFrame
    objectName: "WGAssetBrowserDialogFrame"
    WGComponent { type: "WGAssetBrowserDialogFrame" }

	color: palette.mainWindowColor
	property var fileUrl: ""
	property bool hasAcceptedAsset: false

    property alias assetBrowserModel: assetBrowser.model
    property alias nameFilters: assetBrowser.nameFilters
    property alias selectedNameFilter: assetBrowser.selectedNameFilter
    property alias assetPreview: assetBrowser.assetPreview
    property alias collapsedTreeNodeIconRole: assetBrowser.collapsedTreeNodeIconRole
    property alias expandedTreeNodeIconRole: assetBrowser.expandedTreeNodeIconRole
    property alias defaultCollapsedTreeNodeIconKey: assetBrowser.defaultCollapsedTreeNodeIconKey
    property alias defaultExpandedTreeNodeIconKey: assetBrowser.defaultExpandedTreeNodeIconKey
    property alias defaultLeafTreeNodeIconKey: assetBrowser.defaultLeafTreeNodeIconKey
    
    /** Allows overriding when dialog is initialised */
    property var init: function(dWidth, dHeight, selPath, altFolder) {
        initBase(dWidth, dHeight, selPath, altFolder);
    }
    function initBase(dWidth, dHeight, selPath, altFolder) {
	    dialogFrame.width = dWidth;
	    dialogFrame.height = dHeight;
        dialogFrame.hasAcceptedAsset = false;
        dialogFrame.fileUrl = "";
		assetBrowser.clearFilters();
		assetBrowser.contentViewSearchText = "";
		assetBrowser.currentPath = "";
        if (!assetBrowser.selectPath(selPath)) {
            assetBrowser.selectFolder(altFolder);
        }
    }

    /** Allows overriding when dialog is closed */
    property var close: function() {
        closeBase();
    }
	function closeBase() {
        if(typeof dialog != 'undefined' && typeof dialog.close != 'undefined') {
            dialog.close(hasAcceptedAsset ? 0 : 1)
        }
    }

    /** Allows overriding when asset is accepted */
    property var accept: function(accepted) {
        acceptBase(accepted);
    }
	function acceptBase(accepted) {
		hasAcceptedAsset = accepted;
		if (accepted && typeof dialogAssetAccepted != "undefined") {
			dialogAssetAccepted(fileUrl)
		}
	}

	ColumnLayout {
		anchors.fill: parent
		anchors.margins: defaultSpacing.standardMargin

		WGAssetBrowser {
			id: assetBrowser
			objectName: "assetBrowser"
			model: typeof(dialogAssetModel) != "undefined" ? dialogAssetModel : null
			nameFilters: typeof(dialogNameFilters) != "undefined" ? dialogNameFilters : null
			selectedNameFilter: typeof(dialogNameFilter) != "undefined" ? dialogNameFilter : null
			programSelectionPath: typeof(dialogSelectedAsset) != "undefined" ? dialogSelectedAsset : ""
			Layout.fillHeight: true
			Layout.fillWidth: true

			onCurrentPathChanged: {
				fileUrl = currentPath
			}

			onAssetAccepted: {
				fileUrl = assetPath
				accept(true);
				close();
			}
			
			Component.onCompleted: {
                if (typeof dialogStartVisible != "undefined") {
                    var selPath = typeof dialogSelectedAsset != "undefined" ? dialogSelectedAsset : "";
                    init(900, 650, selPath, selPath);
                }
			}
		}

		WGExpandingRowLayout {
			Layout.preferredHeight: defaultSpacing.minimumRowHeight
			Layout.fillWidth: true

			WGLabel {
				text: "Selected File: "
			}

			WGTextBox {
				objectName: "fileSelectBox"
				Layout.preferredHeight: defaultSpacing.minimumRowHeight
				Layout.fillWidth: true
				readOnly: true
				text: fileUrl
			}

			WGPushButton {
				objectName: "openButton"
				text: "Open"
				Layout.preferredHeight: defaultSpacing.minimumRowHeight
				Layout.preferredWidth: 60
				onClicked: {
					if (assetBrowser.selectFolder(fileUrl)) {
						return;
					}
					accept(true);
					close();
				}
			}

			WGPushButton {
				objectName: "cancelButton"
				text: "Cancel"
				Layout.preferredHeight: defaultSpacing.minimumRowHeight
				Layout.preferredWidth: 60
				onClicked: {
                    accept(false);
					close();
				}
			}
		}
	}
}
