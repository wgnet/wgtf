import QtQuick 2.5
import QtQuick.Controls 1.4
import QtQuick.Layouts 1.3

import WGControls 2.0
import WGControls.Views 2.0
import WGControls.Global 2.0

import QtQml.Models 2.2 as QtQmlModels

WGIconView {
    id: assetGrid
    objectName: "WGAssetIconView"
    WGComponent { type: "WGAssetIconView20" }

    /*!  This assetBrowser this view is connected to. Mostly used to update the iconSize and selected asset */
    property QtObject assetBrowser

    /*!  This contextMenu for the assets in this view */
    property QtObject contextMenu

    /*! This property determines the default size of the icons in the gridview */
    iconSize: assetBrowser != null ? assetBrowser.iconSize : 64
    iconLabelSize: assetBrowser != null ? assetBrowser.iconLabelSize : iconSize > 32 ? 9 : 7

    /*! Override this for determining whether an item in this view has children */
    property var getHasChildren: function(itemData) {
        if(itemData == null || typeof(itemData) == "undefined") {
            return false;
        }
        var isDirectory = itemData.isDirectory;
        if(typeof(isDirectory) != "undefined") {
            return isDirectory;
        }
        var hasChildren = itemData.hasModelChildren;
        return typeof(hasChildren) != "undefined" && hasChildren;
    }

    /*! Override this for determine what context object to set */
    property var getContextObject: function(rowIndex) {
		var selectedItemPaths = [];
		var selectedItems = assetBrowser.getSelectedContentItems();
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

    signal itemSelected(var item)
	signal itemSelectedDoubleClicked(var item)

    focus: true
    cellWidth: (iconSize + defaultSpacing.doubleBorderSize) < width ? width / Math.floor(width / (iconSize + defaultSpacing.doubleBorderSize)) : iconSize + defaultSpacing.doubleBorderSize
    cellHeight: {
        if (assetBrowser != null) {
            assetBrowser.showLabels ? (iconSize + defaultSpacing.doubleBorderSize) + iconLabelSize * 4 : (iconSize + defaultSpacing.doubleBorderSize)
        }
        else {
            (iconSize + defaultSpacing.doubleBorderSize) + iconLabelSize * 4
        }
    }

    height: parent != null ? parent.height : 100
    width: parent != null ? parent.width : 100

    onItemClicked: {
        if (mouse.button != Qt.RightButton) {
            return;
        }

        if (contextMenu != null) {
			//Temporary call to force the context - to be removed once the scenebrowser folder and content context menus have been separated
			selectionChanged(null, null);
            contextMenu.popup();
        }
    }

    onItemDoubleClicked: {
		var item = view.extendedModel.indexToItem(rowIndex);
		var fullPath = assetBrowser.getFullPath(item);

		if(!assetBrowser.selectFolder(fullPath)) {
			// only process item requests if the folder has not changed
			assetBrowser.assetAccepted(fullPath);
			itemSelectedDoubleClicked(item);
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
			assetBrowser.previewAsset("");
            if (contextMenu != null) {
                contextMenu.contextObject = null;
            }

			itemSelected(null);
            return;
		}

		var selectedItem = view.extendedModel.indexToItem(selectedIndex);
		var fullPath = assetBrowser.getFullPath(selectedItem);

        assetBrowser.previewAsset(getHasChildren(selectedItem) ? "" : fullPath);
        if (contextMenu != null) {
            contextMenu.contextObject = getContextObject(selectedIndex);
        }

        itemSelected(selectedItem);
	}

    function indexToItem(index) {
		var modelIndex = null;
		if (typeof(index) == "number") {
			modelIndex = index == -1 ? null : model.index(index, 0);
        }
		else {
			modelIndex = index;
		}

        if(modelIndex != null) {
            var proxyIndex = view.proxyModel.mapFromSource(modelIndex);
            var extendedIndex = view.extendedIndex(proxyIndex);
            return view.extendedModel.indexToItem(extendedIndex);
        }
        return null;
    }

    function getPath(index) {
        return assetBrowser.getFullPath(indexToItem(index));
    }

    function getIndex(path) {
        if(path == null) {
            return null;
        }
        var rowCount = model.rowCount();
        for (var row = 0; row < rowCount; ++row) {
			var rowIndex = model.index(row, 0);
            var rowPath = getPath(rowIndex);

			var pathN = WGPath.normalisePath(path);
            var rowPathN = WGPath.normalisePath(rowPath);
            if (pathN == rowPathN) {
                return rowIndex;
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
		if (index == -1 || index == null) {
			clearSelection();
			return false;
		}

        var proxyIndex = view.proxyModel.mapFromSource(index);
        var extendedIndex = view.extendedIndex(proxyIndex);
		view.selectionModel.setCurrentIndex(extendedIndex, QtQmlModels.ItemSelectionModel.Clear | QtQmlModels.ItemSelectionModel.Select);
        return true;
    }

    getItemImageSource: function(itemData) {
        var imageData = "";
        if(assetBrowser == null) {
            return imageData;
        }
        if(assetBrowser.showThumbnails) {
            imageData = assetBrowser.getThumbnailSource(itemData);
        }
        var hasChildren = getHasChildren(itemData);
        if(imageData == "")
        {
            imageData = assetBrowser.getIconSource(itemData, hasChildren, assetBrowser.iconFileSize);
        }
        return imageData;
    }

    getItemText: function(itemData) {
        return assetBrowser != null ? assetBrowser.getItemValue(itemData) : ""
    }

    getItemToolTip: function(itemData) {
        return typeof itemData.display !== "undefined" ? itemData.display : ""
    }
}
