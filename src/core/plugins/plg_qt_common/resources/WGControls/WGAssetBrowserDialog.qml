import QtQuick 2.5
import QtQuick.Layouts 1.3
import QtQuick.Dialogs 1.2

import WGControls 2.0
import WGControls.Layouts 2.0


/*!
 \ingroup wgcontrols
 \brief An AssetBrowser specific file dialog, 
 can be used as an individual dialog or through the global asset browser dialog
*/
WGFileDialog {
    id: mainDialog
    objectName: "WGAssetBrowserDialog"
    WGComponent { type: "WGAssetBrowserDialog" }

    title: typeof self != 'undefined' && self.title ? self.title : ""

    property alias model: dialogFrame.assetBrowserModel
    property alias nameFilters: dialogFrame.nameFilters
    property alias selectedNameFilter: dialogFrame.selectedNameFilter
    property alias assetPreview: dialogFrame.assetPreview
    property alias collapsedTreeNodeIconRole: dialogFrame.collapsedTreeNodeIconRole
    property alias expandedTreeNodeIconRole: dialogFrame.expandedTreeNodeIconRole
    property alias defaultCollapsedTreeNodeIconKey: dialogFrame.defaultCollapsedTreeNodeIconKey
    property alias defaultExpandedTreeNodeIconKey: dialogFrame.defaultExpandedTreeNodeIconKey
    property alias defaultLeafTreeNodeIconKey: dialogFrame.defaultLeafTreeNodeIconKey
	
    function open(dWidth, dHeight, selPath) {
        dialogFrame.init(dWidth, dHeight, selPath, folder.toString());
    }
    
    function close() {
        dialogFrame.close();
    }

    onOpened: {
        dialogFrame.acceptBase(false);
    }

    onClosed: {
        dialogFrame.closeBase();
    }

    Dialog {
        id: abInstance
        modality: mainDialog.modality
        title: mainDialog.title

        contentItem: WGAssetBrowserDialogFrame {
			id: dialogFrame

            close: function() {
                abInstance.close();
            }

            init: function(dWidth, dHeight, selPath, altFolder) {
                abInstance.width = dWidth;
                abInstance.height = dHeight;
                initBase(dWidth, dHeight, selPath, altFolder);
                abInstance.open();
            }

            accept: function(accepted) {
                acceptBase(accepted);
                if(accepted) {
                    mainDialog.accepted(fileUrl);   
                }
                else {
                    mainDialog.rejected();
                }
            }
		}

        property bool wasVisible
        Component.onCompleted: {
            wasVisible = visible;
        }

        onVisibleChanged: {
            if (visible == wasVisible) {
                return;
            }

            if (!visible) {
                mainDialog.closed();
            }
            else {
                mainDialog.opened();
            }

            wasVisible = visible;
        }
    }
}
