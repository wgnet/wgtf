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
    Creates a list of assets with a small preview icon/thumbnail

Example:
\code{.js}
    WGAssetListView {
        anchors.fill: parent
        assetBrowser: myAssetBrowser
        contextMenu: myAssetBrowser.contextMenu

        model: myContentsModel
    }
\endcode

  \todo - Add more asset info: filesize, pixel dimensions,

*/

WGListView {
    id: assetList
    objectName: "WGAssetListView"
    WGComponent { type: "WGAssetListView20" }

	property var proxyModel: WGColumnLayoutProxy {
        id: columnLayoutProxy
        columnSequence: [0, 0, 0, 0]
    }

    property alias assetModel: assetList.model
    property alias model: columnLayoutProxy.sourceModel
	property alias columnSequence: columnLayoutProxy.columnSequence

	assetModel: proxyModel

    /*! Whether to show a context menu on items */
    property bool useContextMenu: true

    /*!  This assetBrowser this view is connected to. Mostly used to update the selected asset */
    property QtObject assetBrowser

    /*!  This contextMenu for the assets in this view */
    property QtObject contextMenu

    /*! Override this for determining whether an item in this view has children */
    property var getHasChildren: function(itemData, hasModelChildren) {
        if(itemData == null || typeof(itemData) == "undefined") {
            return false;
        }
        var isDirectory = itemData.isDirectory;
        if(typeof(isDirectory) != "undefined") {
            return isDirectory;
        }
        return typeof(hasModelChildren) != "undefined" && hasModelChildren;
    }

    /*! Override this for determine what context object to set */
    property var getContextObject: function(rowIndex) {
        var selectedItemPaths = [];
		var selectedItems = assetBrowser.getSelectedContentItems();
		for (var i = 0; i < selectedItems.length; ++i) {
			var selectedItem = selectedItems[i];
            var selectedPath = assetBrowser.getFullPath(selectedItem);
            selectedItemPaths.push(selectedPath);
		}

		// Backwards compatibility, return a single item rather than a collection if theres only one
        if (selectedItemPaths.length == 1) {
            return selectedItemPaths[0];
        }
		return selectedItemPaths;
    }

    property Component contentDelegate: RowLayout {
        anchors.left: parent ? parent.left : undefined
        anchors.right: parent ? parent.right : undefined

        property var validItemData: typeof itemData != "undefined" && itemData != null
        property var hasChildren: validItemData && getHasChildren(itemData, hasModelChildren)
        property var isItemHidden: validItemData && typeof(itemData.isHidden) != "undefined" && itemData.isHidden
        property var toolTipText: (validItemData && typeof(itemData.tooltip) != "undefined") ? itemData.tooltip : ""

        WGToolTipBase {
            text: toolTipText
            tooltipArea: colMouseArea
        }

        WGImage {
            id: iconLoader
            asynchronous: true
            source: assetBrowser.getIconSource(itemData, hasChildren,16)
            Layout.preferredHeight: 16
            Layout.preferredWidth: 16
            Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
        }

        Text {
            text: assetBrowser.getItemValue(itemData)
            Layout.fillWidth: true
            color: enabled ? palette.textColor : palette.disabledTextColor
            opacity: isItemHidden ? 0.5 : 1.0
            font.bold: hasChildren
        }
    }

	property Component dateDelegate: Text {
		text: {
			if (itemData == null || typeof(itemData) == "undefined") {
				return "";
			}

			if (itemData.modified == 0) {
				return "";
			}

			var d = new Date((itemData.modified - 116444736000000000) / 10000); //set to epoch
			return d.toLocaleString(Qt.locale(), Locale.ShortFormat)
		}
		color: enabled ? palette.textColor : palette.disabledTextColor
	}

	property Component typeDelegate: Text {
		text: {
			if (itemData == null || typeof(itemData) == "undefined") {
				return "";
			}

			if (itemData.isDirectory) {
				return "File Folder";
			}

			var asset = assetBrowser.getItemValue(itemData)
			var ext = asset.lastIndexOf(".");
			return asset.substr(ext + 1);
		}
		color: enabled ? palette.textColor : palette.disabledTextColor
	}

	property Component sizeDelegate: Text {
		text: {
			if (itemData == null || typeof(itemData) == "undefined") {
				return "";
			}

			if (itemData.isDirectory) {
				return "";
			}

			return Math.ceil(itemData.size / 1024) + " KB"
		}
		color: enabled ? palette.textColor : palette.disabledTextColor
        width: columnWidth
		horizontalAlignment: Text.AlignRight
	}

    signal itemSelected(var item)
	signal itemSelectedDoubleClicked(var item)

	sortObject: QtObject {
        function lessThan(left, right) {
			if (left.isDirectory && !right.isDirectory) {
				return true;
			}
			else if (!left.isDirectory && right.isDirectory) {
				return false;
			}

			var sortColumn = left.column;
			switch (sortColumn) {
				case 1:
					return left.modified < right.modified
				case 2:
					if (left.isDirectory) {
						return false;
					}
					var leftAsset = assetBrowser.getItemValue(left)
					var rightAsset = assetBrowser.getItemValue(right)
					var leftExt = leftAsset.lastIndexOf(".");
					var rightExt = rightAsset.lastIndexOf(".");
					return leftAsset.substr(leftExt + 1) < rightAsset.substr(rightExt + 1);
				case 3:
					if (left.isDirectory) {
						return false;
					}
					return left.size / 1024 < right.size / 1024;
				default:
					return assetBrowser.getItemValue(left) < assetBrowser.getItemValue(right)
			}
        }
    }

    columnDelegates: [contentDelegate, dateDelegate, typeDelegate, sizeDelegate]
	headerDelegate: Text {
		text: {
			switch (headerColumnIndex) {
				case 1:
					return "Date modified";
				case 2:
					return "Type";
				case 3:
					return "Size";
				default:
					return "Name";
			}
		}
		color: enabled ? palette.textColor : palette.disabledTextColor
		font.bold: true
	}
    clamp: true
    focus: true

    height: parent != null ? parent.height : 100
    width: parent != null ? parent.width : 100

    onItemClicked: {
        if (mouse.button != Qt.RightButton) {
            return;
        }

        if (useContextMenu && contextMenu != null) {
			//Temporary call to force the context - to be removed once the scenebrowser folder and content context menus have been separated
			//selectionChanged(null, null);
            contextMenu.popup();
            mouse.accepted = true;
        }
    }

    onItemDoubleClicked: {
		var item = assetList.view.extendedModel.indexToItem(rowIndex);
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
            var columnProxyIndex = proxyModel.mapFromSource(modelIndex);
            var proxyIndex = view.proxyModel.mapFromSource(columnProxyIndex);
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
        var pathN = WGPath.normalisePath(path);

        var rowCount = model.rowCount();
        for (var row = 0; row < rowCount; ++row) {
			var rowIndex = model.index(row, 0);
            var rowPath = getPath(rowIndex);

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

        var columnProxyIndex = proxyModel.mapFromSource(index);
        var proxyIndex = view.proxyModel.mapFromSource(columnProxyIndex);
        var extendedIndex = view.extendedIndex(proxyIndex);
		view.selectionModel.setCurrentIndex(extendedIndex, QtQmlModels.ItemSelectionModel.Clear | QtQmlModels.ItemSelectionModel.Select);

		//Temporary call to force the context - to be removed once the scenebrowser folder and content context menus have been separated
		selectionChanged(null, null);
        return true;
    }
}
