import QtQuick 2.5
import QtQuick.Controls 1.4
import QtQuick.Layouts 1.3

import WGControls 2.0
import WGControls.Views 2.0
import WGControls.Global 2.0

import QtQml.Models 2.2 as QtQmlModels

/*!
 \ingroup wgcontrols
 \brief
    Creates a tree of folders from folder model

*/

WGTreeView {
    id: folderTree
    objectName: "WGAssetTreeView"
    WGComponent { type: "WGAssetTreeView20" }
    columnDelegates : [foldersColumnDelegate]
    clamp: true
    focus: true

    /*! Whether to show a context menu on items */
    property bool useContextMenu: true

    /*!  This assetBrowser this view is connected to. Mostly used to update the iconSize and selected asset */
    property QtObject assetBrowser

    /*!  This contextMenu for the assets in this view */
    property QtObject contextMenu

    /*! Override this for determining whether an item in this view has children */
    property var getHasChildren: function(itemData) {
        if(itemData == null || typeof(itemData) == "undefined") {
            return false;
        }
        var isDirectory = itemData.isDirectory;
        if(typeof(isDirectory) != "undefined") {
            return isDirectory;
        }
        var hasChildren = itemData.hasChildren;
        return typeof(hasChildren) != "undefined" && hasChildren;
    }

    /*! Override this for determine what context object to set */
    property var getContextObject: function(rowIndex) {
		var selectedItemPaths = [];
		var selectedItems = assetBrowser.getSelectedFolderItems();
		for (var i = 0; i < selectedItems.length; ++i) {
			var selectedItem = selectedItems[i];
            selectedItemPaths.push(assetBrowser.getFullPath(selectedItem));
		}

		// Backwards compatibility, return a single item rather than a collection if theres only one
        if (selectedItemPaths.length == 1) {
            return selectedItemPaths[0];
        }
		return selectedItemPaths;
    }

    property Component foldersColumnDelegate: Item {
        id: folderIconHeaderContainer
        height: 16
        objectName: typeof(itemData.value) != "undefined" ? "TreeItem_" + itemData.value : "TreeItem"

        property var validItemData: typeof itemData != "undefined" && itemData != null
        property var isItemHidden: validItemData && typeof(itemData.isHidden) != "undefined" && itemData.isHidden
        property var hasChildren: validItemData && getHasChildren(itemData)
        property var toolTipText: (validItemData && typeof(itemData.tooltip) != "undefined") ? itemData.tooltip : ""

        WGToolTipBase {
            text: toolTipText
            tooltipArea: colMouseArea
        }

        WGImage{
            id: folderFileIcon
            objectName: typeof(itemData.value) != "undefined" ? "folderFileIcon_" + itemData.value : "folderFileIcon"
            anchors.verticalCenter: folderIconHeaderContainer.verticalCenter
            visible: true
            anchors.left: folderIconHeaderContainer.left
            width: assetBrowser.treeViewIconSize > 0 ? treeViewIconSize : sourceSize.width
            height: assetBrowser.treeViewIconSize > 0 ? treeViewIconSize : sourceSize.height
            source: assetBrowser.getIconSource(itemData, hasChildren,16)
            opacity: isItemHidden ? 0.5 : 1.0
        }

        Text {
            objectName: typeof(itemData.value) != "undefined" ? "Text_" + itemData.value : "Text"
            anchors.left: folderFileIcon.right
            color: palette.textColor
            opacity: isItemHidden ? 0.5 : 1.0
            clip: itemData != null && itemData.component != null
            text: assetBrowser.getItemValue(itemData)
            anchors.leftMargin: defaultSpacing.rowSpacing
            font.bold: hasChildren
            verticalAlignment: Text.AlignVCenter
            anchors.verticalCenter: folderIconHeaderContainer.verticalCenter
            elide: Text.ElideRight
        }
    }

    signal itemSelected(var item)

    onItemClicked: {
        if (mouse.button != Qt.RightButton) {
            return
        }

        if(useContextMenu && contextMenu != null) {
			//Temporary call to force the context - to be removed once the scenebrowser folder and content context menus have been separated
			selectionChanged(null, null);
            contextMenu.popup()
        }
    }

	onSelectionChanged: {
		var selectedIndex = view.selectionModel.isSelected(view.selectionModel.currentIndex) ? view.selectionModel.currentIndex : null;
		if (selectedIndex == null)
		{
			//current index was not selected, lets choose the previous selected index
			var selectedIndexes = view.selectionModel.selectedIndexes;
			if (!selectedIndexes.empty)
			{
				selectedIndex = selectedIndexes[selectedIndexes.length - 1];
			}
		}

		if (selectedIndex == null)
		{
            if (contextMenu != null) {
                contextMenu.contextObject = null;
            }

			itemSelected(null);
            return;
		}

		var selectedItem = view.extendedModel.indexToItem(selectedIndex);
        if (contextMenu != null) {
            contextMenu.contextObject = getContextObject(selectedIndex);
        }

        itemSelected(selectedItem);
	}

    function indexToItem(index) {
        if(index != null) {
            var proxyIndex = view.proxyModel.mapFromSource(index);
            var extendedIndex = view.extendedIndex(proxyIndex);
            return view.extendedModel.indexToItem(extendedIndex);
        }
        return null;
    }

	function getPath(index) {
        return assetBrowser.getFullPath(indexToItem(index));
    }

    function getIndex(path, parentIndex) {
        if(path == null) {
            return null;
        }
        var pathN = WGPath.normalisePath(path);

        var rowCount = parentIndex != null ? model.rowCount(parentIndex) : model.rowCount();
        for (var row = 0; row < rowCount; ++row) {
            var rowIndex = parentIndex != null ? model.index(row, 0, parentIndex) : model.index(row, 0);
            var rowPath = getPath(rowIndex);

            var rowPathN = WGPath.normalisePath(rowPath);
            if (pathN == rowPathN) {
                return rowIndex;
            }

            if (pathN.indexOf(rowPathN) == 0) {
                var index = getIndex(path, rowIndex);
                if (index != null) {
                    return index;
                }
            }
        }
        return null;
    }

    function clearSelection() {
		view.selectionModel.clearSelection();
        view.selectionModel.clearCurrentIndex();        
    }

    function selectPath(path) {
        var index = getIndex(path);
        if(index != null && selectIndex(index))
        {
            return true;
        }
        return false;
    }

    function selectIndex(index) {
		if(index == null) {
			clearSelection();
            return false;
        }

        var proxyIndex = view.proxyModel.mapFromSource(index);
        var extendedIndex = view.extendedIndex(proxyIndex);
		view.selectionModel.setCurrentIndex(extendedIndex, QtQmlModels.ItemSelectionModel.Clear | QtQmlModels.ItemSelectionModel.Select);
        return true;
    }
}
