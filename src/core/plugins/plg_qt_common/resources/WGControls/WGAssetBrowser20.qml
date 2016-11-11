import QtQuick 2.5
import QtQuick.Controls 1.4
import QtQuick.Layouts 1.3
import QtQuick.Window 2.2

import WGControls 2.0
import WGControls.Private 2.0
import WGControls.Views 2.0
import WGControls.Layouts 2.0

import QtQml.Models 2.2 as QtQmlModels

/*!
 \ingroup wgcontrols
 \brief A control used for display, browsing and interaction of assets on disc.

----------------------------------------------------------------------------------------------
 Preliminary Layout Designed but not Finalised! Icons and menus currently placeholders only.

 It is operating off data models for file folders and the listview. The layout is based upon:
        Search for Asset Browser Qt Prototype on the Wargaming Confluence
----------------------------------------------------------------------------------------------

Example:
\code{.js}
WGAssetBrowser {
    id: testAssetBrowserControl
    model: fileSystemModel
}
\endcode
*/

//ToDo: Better naming of ObjectName for the thumbnails and repeated elements within assetbrowser using itemData

Rectangle {
    id: root
    objectName: "WGAssetBrowser"
    WGComponent { type: "WGAssetBrowser20" }
    
    color: palette.mainWindowColor

	property var model
	property var nameFilters
	property var selectedNameFilter: ""
	property var currentPath: ""

	/*! This property determines the maximum number of history items tracked during asset tree navigation
        The default value is \c 10 */
    property int maxHistoryItems: 10

	function selectPath(path) {
		var pathN = internal.normalisePath(path);
		var folderPath = pathN.slice(0, pathN.lastIndexOf("\\") + 1);
		if (!folderTree.selectPath(folderPath)) {
			return false;
		}

		if (!assetList.selectPath(pathN)) {
			return false;
		}
		return true;
	}

	function selectFolder(path) {
		if (!folderTree.selectPath(path)) {
			return false;
		}

		assetList.selectIndex(null);
		return true;
	}

	signal assetAccepted(string assetPath)

	QtObject {
		id: internal
		property int __currentFolderHistoryIndex: 0
		property bool shouldTrackFolderHistory: true
		function normalisePath(path) {
			if (typeof(path) != "string") {
				return "";
			}
			function filterEmpty(str) { return str!= ""; }
			return path.replace(/[/]/g, "\\").split("\\").filter(filterEmpty).join("\\").toLowerCase();
		}
		// Handles a history menu item being clicked
		function historyMenuItemClicked(index) {
			// Make sure the index is valid
			if (folderHistory.count <= index) {
				return;
			}

			// Don't track the folder history while navigating said history
			shouldTrackFolderHistory = false;

			__currentFolderHistoryIndex = index;
			var path = folderHistory.get(__currentFolderHistoryIndex).path;
			root.selectFolder(path);
		}

		    // Tells the page to navigate the history forward or backward
			// depending on what button was clicked
			function onNavigate( isForward ) {
				// Don't track the folder history while we use the navigate buttons the history
				shouldTrackFolderHistory = false;

				if (isForward) {
					if (folderHistory.count <= __currentFolderHistoryIndex + 1) {
						return;
					}
					__currentFolderHistoryIndex += 1;
				}
				else {
					if(__currentFolderHistoryIndex <= -1) {
						return;
					}
					__currentFolderHistoryIndex -= 1;
				}
				var path = folderHistory.get(__currentFolderHistoryIndex).path;
				root.selectFolder(path);
			}
	}

	ListModel { id: folderHistory }

	Component.onCompleted: {
        var value = preference.assetViewOrientation;
        if (typeof value != "undefined")
        {
            assetSplitter.state = value;
            btnAssetBrowserOrientation.checked = assetSplitter.orientation ==  Qt.Vertical;
        }

        var vVisible = preference.leftFrameVisible;
        var vWidth = preference.leftFrameWidth;
        var vHeight = preference.leftFrameHeight;
        if ((typeof vWidth != "undefined") && (typeof vHeight != "undefined")&& (typeof vVisible != "undefined"))
        {
            folderView.visible = (vVisible == "true");
            btnAssetBrowserHideFolders.checked = !(vVisible == "true");
            folderView.width = vWidth;
            folderView.height = vHeight;
        }
    }
	

	WGSortFilterProxy {
		id: filteredModel
		sourceModel: model

		filterObject: QtObject {
            property var filterText: activeFilters.filterString
            property var filterTokens: []

            onFilterTextChanged: {
                filterTokens = [];
                if (!filterText.empty) {
                    filterTokens = filterText.split( activeFilters.splitterChar );
                }
                filteredModel.invalidateFilter();
            }

            function filterAcceptsRow(item) {
                var tokenCount = filterTokens.length;
                if (tokenCount == 0) {
                    return true;
                }

                if (item.fullPath === undefined) {
                    return true;
                }

                for (var i = 0; i < tokenCount; ++i) {
                    var filterToken = filterTokens[i];
                    if (item.fullPath.indexOf(filterToken) == -1) {
                        return false;
                    }
                }

                return true;
            }
        }
	}

	WGSortFilterProxy {
		id: folderModel
		sourceModel: filteredModel

		filterObject: QtObject {
			function filterAcceptsRow(item) {
				return item.isDirectory
			}
		}
	}

	WGSubProxy {
		id: contentsModel
	}

	WGSortFilterProxy {
		id: filteredContentsModel
		sourceModel: contentsModel

		filterObject: QtObject {
			property var nameFilter: selectedNameFilter
			property var nameRegExp: null

			onNameFilterChanged: {
				nameRegExp = null;
				if (!nameFilter.empty) {
					var begin = nameFilter.indexOf("(");
					var end = nameFilter.indexOf(")");
					if (begin != -1 && end != -1 && begin + 1 < end) {
						var patterns = nameFilter.substring(begin + 1, end);
						var patterns = patterns.split(" ").join("|");
						var patterns = patterns.split("*").join(".*");
						nameRegExp = new RegExp("^" + patterns + "$");
					}
				}
				filteredContentsModel.invalidateFilter();
			}

			function filterAcceptsRow(item) {
				if (nameRegExp != null && !item.isDirectory) {
					if (!nameRegExp.test(item.display)) {
						return false;
					}
				}
				return true;
			}
		}
	}

	Connections {
		target: folderTree.view.selectionModel
		onCurrentChanged: {
			var proxyIndex = folderTree.view.sourceIndex(current);
			var folderModelIndex = folderTree.view.proxyModel.mapToSource(proxyIndex);
			var filteredModelIndex = folderModel.mapToSource(folderModelIndex);

			breadcrumbControl.modelIndex = folderModelIndex;
			breadcrumbControl.path = folderTree.getPath(folderModelIndex);
			contentsModel.sourceParent = filteredModelIndex;
			if (internal.shouldTrackFolderHistory) {
				// Prune history as needed based on maximum length allowed
				if (folderHistory.count >= maxHistoryItems) {
					folderHistory.remove(0);
				}

				// Track the folder selection indices history
				folderHistory.append({"path" : folderTree.getPath(folderModelIndex)});
				internal.__currentFolderHistoryIndex = folderHistory.count - 1;
			}

			// Reset the flag to track the folder history
			internal.shouldTrackFolderHistory = true;
		}
	}

	Connections {
		target: assetGrid
		onCurrentIndexChanged: {
			var index = filteredContentsModel.index(assetGrid.currentIndex, 0);
			assetList.selectIndex(index);
		}
	}

	Connections {
		target: assetList.view.selectionModel
		onCurrentChanged: {
			var row = assetList.view.getRow(current);
			if (assetGrid.currentIndex != row) {
				assetGrid.currentIndex = row;
			}

			var item = assetList.view.extendedModel.indexToItem(current);
			root.currentPath = item != null ? item.fullPath : "";
		}
	}

	Connections {
		target: breadcrumbControl
		onBreadcrumbSelected: {
			folderTree.selectIndex(modelIndex);
		}

		onPathEntered: {
			if (!folderTree.selectPath(path)) {
				breadcrumbControl.path = folderTree.getPath(breadcrumbControl.modelIndex);
			}
		}
	}
	
	/*! This property determines the default size of the icons in the listview */
	property int iconSize: 64

	/*! This property determines the size of the label of each icon */
    property int iconLabelSize: iconSize > 32 ? 9 : 7
	
	/*!  This property indicates if the asset browser is showing a grid (true) or a list view (false) */
	readonly property bool showIcons: iconSize >= 32

    //--------------------------------------
    // View Parent Layout
    //--------------------------------------

    ColumnLayout {
        // Initial column layout with button/path bar at the top and then
        // the split two column panel underneath it.

        id: mainColumn
        objectName: "mainColumn"

        anchors.fill: parent
        anchors.margins: defaultSpacing.standardMargin

		WGExpandingRowLayout {
            // Contains both Row and column layout that internal controls are reparented to within checkAssetBrowserWidth
            id: resizeContainer
            Layout.fillWidth: true

            z: 1

            property bool singleLineLayout: true

			WGExpandingRowLayout {
				//contains all assetBrowserInfo in a single line
				id: assetBrowserInfoFirstLine
				Layout.fillWidth: true
				Layout.preferredHeight: defaultSpacing.minimumRowHeight + defaultSpacing.doubleBorderSize

				WGButtonBar {
					evenBoxes: false

					buttonList: [
						// Breadcrumbs and back/forward
						WGToolButton {
							id: btnAssetBrowserBack
							objectName: "backButton"
							iconSource: "icons/back_16x16.png"
							tooltip: "Back"
							enabled: (internal.__currentFolderHistoryIndex != 0)
							onClicked: {
								internal.onNavigate( false );
							}
						},
						WGToolButton {
							id: btnAssetBrowserForward
							objectName: "forwardButton"
							iconSource: "icons/fwd_16x16.png"
							tooltip: "Forward"
							enabled: (internal.__currentFolderHistoryIndex < folderHistory.count - 1)

							onClicked: {
								internal.onNavigate( true );
							}
						},
						WGToolButton {
							id: btnAssetBrowserRefresh
							objectName: "refreshButton"
							iconSource: "icons/loop_16x16.png"
							tooltip: "Refresh"
							width: 16


							onClicked: {
								onRefresh();
							}
						},
						WGToolButton {
							id: btnAssetBrowserHistory
							objectName: "historyButton"
							iconSource: "icons/arrow_down_small_16x16.png"
							tooltip: "History"
							width: 16

							showMenuIndicator: false

							menu: WGMenu {
								id: historyMenu

								Instantiator {
									model: folderHistory
									delegate: MenuItem {
										text: path
										onTriggered: {
											internal.historyMenuItemClicked(index);
										}
									}
									onObjectAdded: historyMenu.insertItem(index, object)
									onObjectRemoved: historyMenu.removeItem(object)
								}
							}
						}
					]
				}

				WGBreadcrumbs {
					id: breadcrumbControl
					model: folderModel
				}

				WGExpandingRowLayout {
					id: assetBrowserPreferencesContainer
					Layout.fillWidth: false

					WGPushButton {
						id: displayButton
						objectName: "displayButton"
						Layout.preferredWidth: 100
						checkable: true

						text: showIcons ? (iconSize + "px Icons") : "List View"

						Timer {
							id: fadeTimer
							running: false
							interval: 1000

							onTriggered: {
								displayButton.checked = false
							}
						}

						onActiveFocusChanged: {
							if(!activeFocus)
							{
								displayButton.checked = false
							}
						}

						// IconSize control pulldown
						Rectangle {
							id: sizeMenu
							anchors.left: displayButton.left
							anchors.top: displayButton.bottom
							visible: displayButton.checked
							height: 120
							width: 140


							color: palette.mainWindowColor
							border.width: defaultSpacing.standardBorderSize
							border.color: palette.darkColor

							MouseArea {
								// prevents items being selected behind drop down
								id: catchMouseArea
								anchors.fill: parent
								propagateComposedEvents: false

								hoverEnabled: true

								onEntered: {
									fadeTimer.stop()
								}

								onExited: {
										fadeTimer.restart()
								}
							}

							WGSlider {
								id: slider
								objectName: "slider"
								stepSize: 32
								minimumValue: 0
								maximumValue: 256
								width: 16
								height: menuItems.childrenRect.height
								orientation: Qt.Vertical
								anchors.top: sizeMenu.top
								anchors.left: sizeMenu.left
								anchors.margins: {left: 2; right: 2; top: 5; bottom: 5}
								rotation: 180

								WGSliderHandle {
									id: sliderHandle
									minimumValue: slider.minimumValue
									maximumValue: slider.maximumValue
									showBar: true

									onValueChanged: {
										root.iconSize = value
									}

									Binding {
										target: sliderHandle
										property: "value"
										value: root.iconSize
									}
								}
							}

							// Iconsize Pulldown Buttons
							Item {
								id: menuItems
								height: childrenRect.height
								width: 110
								anchors.top: sizeMenu.top
								anchors.left: slider.right
								anchors.margins: {left: 2; right: 2; top: 5; bottom: 5}

								WGPushButton {
									id: listViewButton
									objectName: "listViewButton"
									anchors.top: menuItems.top
									anchors.left: menuItems.left
									width: menuItems.width
									text: "List View"

									onHoveredChanged: {
										if (hovered) {
											fadeTimer.stop()
										}
										else {
											fadeTimer.restart()
										}
									}

									onClicked: {
										iconSize = 0
									}
								}
								WGPushButton {
									id: smallIconsButton
									objectName: "smallIconsButton"
									anchors.top: listViewButton.bottom
									anchors.left: menuItems.left
									width: menuItems.width
									text: "Small Icons"

									onHoveredChanged: {
										if (hovered) {
											fadeTimer.stop()
										}
										else {
											fadeTimer.restart()
										}
									}

									onClicked: {
										iconSize = 48
									}
								}
								WGPushButton {
									id: mediumIconsButton
									objectName: "mediumIconsButton"
									anchors.top: smallIconsButton.bottom
									anchors.left: menuItems.left
									width: menuItems.width
									text: "Medium Icons"

									onHoveredChanged: {
										if (hovered) {
											fadeTimer.stop()
										}
										else {
											fadeTimer.restart()
										}
									}

									onClicked: {
										iconSize = 96
									}
								}
								WGPushButton {
									id: largeIconsButton
									objectName: "largeIconsButton"
									anchors.top: mediumIconsButton.bottom
									anchors.left: menuItems.left
									width: menuItems.width
									text: "Large Icons"

									onHoveredChanged: {
										if (hovered) {
											fadeTimer.stop()
										}
										else {
											fadeTimer.restart()
										}
									}

									onClicked: {
										iconSize = 128
									}
								}
								WGPushButton {
									id: extraLargeIconsButton
									objectName: "extraLargeIconsButton"
									anchors.top: largeIconsButton.bottom
									anchors.left: menuItems.left
									width: menuItems.width
									text: "Extra Large Icons"

									onHoveredChanged: {
										if (hovered) {
											fadeTimer.stop()
										}
										else {
											fadeTimer.restart()
										}
									}

									onClicked: {
										iconSize = 256
									}
								}
							}
						}

						MouseArea {
							id: sliderCoverMouseArea
							objectName: "sliderCoverMouseArea"

							anchors.top: parent.top
							anchors.left: sizeMenu.left
							anchors.bottom: sizeMenu.bottom
							width: sizeMenu.width - menuItems.width
							propagateComposedEvents: true

							hoverEnabled: displayButton.checked

							acceptedButtons: Qt.NoButton

							onEntered: {
								fadeTimer.stop()
							}
							onExited: {
								fadeTimer.restart()
							}

							onWheel: {
								if (wheel.angleDelta.y > 0)
								{
									sliderHandle.range.decreaseSingleStep()
								}
								else
								{
									sliderHandle.range.increaseSingleStep()
								}
							}
						}
					}

					// Asset Browser View Options
					WGPushButton {
						id: btnAssetBrowserOrientation
						objectName: "btnAssetBrowserOrientation"
						iconSource: checked ? "icons/rows_16x16.png" : "icons/columns_16x16.png"
						checkable: true
						checked: false

						tooltip: "Horizontal/Vertical Toggle"

						onClicked: {
							if (checked) { //note: The click event changes the checked state before (checked) is tested
								assetSplitter.state = "VERTICAL"
							}
							else
							{
								assetSplitter.state = "HORIZONTAL"
							}
						}
					}

					WGPushButton {
						id: btnAssetBrowserHideFolders
						objectName: "btnAssetBrowserHideFolders"
						iconSource: checked ? "icons/folder_tree_off_16x16.png" : "icons/folder_tree_16x16.png"
						checkable: true
						checked: false

						tooltip: "Hide Folder List"

						onClicked: {
							if(checked){
								folderView.visible = false
							} else {
								folderView.visible = true
							}
						}
					}
				}
			}
		}

		WGExpandingRowLayout {
			WGActiveFilters {
				id: activeFilters
				objectName: "activeFilters"
				Layout.fillWidth: true
				Layout.minimumHeight: defaultSpacing.minimumRowHeight + defaultSpacing.doubleBorderSize
				autoCompleteModel: WGSortFilterProxy {
					dynamicSortFilter: true
					sortObject: QtObject {
						function lessThan(left, right) {
							return left.display.toLowerCase() < right.display.toLowerCase();
						}
					}

					Component.onCompleted: {
						sort(0, Qt.AscendingOrder);
					}
				}

				Timer {
					interval: 100
					running: true
					repeat: true

					property var data: ListModel {}
					property var index: null

					onTriggered: {
						var timeout = Date.now() + 10
						while (Date.now() < timeout) {
							index = nextIndex(index);
							if (index == null) {
								running = false;
								break;
							}

							data.append({"display":filteredModel.data(index)})
							if (activeFilters.autoCompleteModel.sourceModel != data) {
								activeFilters.autoCompleteModel.sourceModel = data;
							}
						}
					}

					function nextIndex(index) {
						if (index != null ? filteredModel.hasChildren(index) : filteredModel.hasChildren()) {
							return index != null ? filteredModel.index(0, 0, index) : filteredModel.index(0, 0);
						}

						while (index != null) {
							var parent = filteredModel.parent(index);
							if (parent == index) {
								return null;
							}
							var rowCount = filteredModel.rowCount(parent);
							for (var row = 1; row < rowCount; ++row) {
								if (filteredModel.index(row - 1, 0, parent) == index) {
									return filteredModel.index(row, 0, parent);
								}
							}
							index = parent;
						}

						return null;
					}
				}
			}

			WGLabel {
                text: "File Type:"
                visible: nameFilters != null
                Layout.alignment: Qt.AlignVCenter
            }

			WGDropDownBox {
                id: nameFiltersDropDown
                Layout.preferredWidth: 150
                Layout.alignment: Qt.AlignTop
                visible: nameFilters != null

                model: nameFilters
				textRole: Array.isArray(model) ? "" : "value"

				Component.onCompleted: {
					currentIndex = find(selectedNameFilter);
					selectedNameFilter = currentText;
				}

                onActivated: {
					selectedNameFilter = currentText;
                }
            }
		}

		WGContextMenu {
            id: folderContextMenu
            path: "WGAssetBrowser20FolderMenu"
        }

		WGContextMenu {
            id: assetContextMenu
            path: "WGAssetBrowser20AssetMenu"
        }

        // SplitView that breaks the panel up into two columns with draggable
        // handle. Haven't used this before but seems to work fine.

        SplitView {
            id: assetSplitter
            objectName: "assetSplitter"
            Layout.fillHeight: true
            Layout.fillWidth: true
            orientation: Qt.Horizontal

			Component.onDestruction: {
                //TODO: directly use Preference when supporting dynamically add property for GeneircObject
                addPreference(viewId, "assetViewOrientation", assetSplitter.state );
            }

			states: [
                State {
                    name: "VERTICAL"
                    PropertyChanges { target: assetSplitter; orientation: Qt.Vertical }
                    PropertyChanges { target: folderView; height: Math.min(200, Math.round(assetSplitter.height / 3)) }
                },
                State {
                    name: "HORIZONTAL"
                    PropertyChanges { target: assetSplitter; orientation: Qt.Horizontal }
                    PropertyChanges { target: folderView; width: Math.min(300, Math.round(assetSplitter.width / 3)) }
                }
            ]
			state: "HORIZONTAL"

            // TODO Maybe should be a separate WG Component
            handleDelegate: Item {

                // yes this is reversed. Blame the default SplitView for being stupid.
                property bool vertical: assetSplitter.orientation == Qt.Horizontal

                width: vertical ? defaultSpacing.separatorWidth + defaultSpacing.doubleBorderSize : assetSplitter.width
                height: vertical ? assetSplitter.height : defaultSpacing.separatorWidth + defaultSpacing.doubleBorderSize

                WGSeparator {
                    vertical: parent.vertical

                    anchors.centerIn: parent

                    width: vertical ? defaultSpacing.separatorWidth : parent.width
                    height: vertical ? parent.height : defaultSpacing.separatorWidth
                }
            }

			WGScrollView {
				id: folderView

				Component.onDestruction: {
                    //TODO: directly use Preference when supporting dynamically add property for GeneircObject
                    addPreference(viewId, "leftFrameVisible", folderView.visible ? "true" : "false" );
                    addPreference(viewId, "leftFrameWidth", folderView.width );
                    addPreference(viewId, "leftFrameHeight", folderView.height );
                }

				WGTreeView {
					id: folderTree
					model : folderModel
					columnDelegates : [foldersColumnDelegate]
					clamp: true
					focus: true

					property Component foldersColumnDelegate: Item {
						id: folderIconHeaderContainer
						height: folderFileIcon.height

						Image{
							id: folderFileIcon
							objectName: typeof(itemData.value) != "undefined" ? "folderFileIcon_" + itemData.value : "folderFileIcon"
							anchors.verticalCenter: folderIconHeaderContainer.verticalCenter
							visible: true
							anchors.left: folderIconHeaderContainer.left
							width: sourceSize.width
							height: sourceSize.heigth
							//TODO: Awaiting type support for icon customisation
							source: itemData.expanded ? "icons/folder_open_16x16.png" : "icons/folder_16x16.png"
						}

						Text {
							objectName: typeof(itemData.value) != "undefined" ? "Text_" + itemData.value : "Text"
							anchors.left: folderFileIcon.right
							color: palette.textColor
							clip: itemData != null && itemData.component != null
							text: itemValue
							anchors.leftMargin: defaultSpacing.rowSpacing
							font.bold: true
							verticalAlignment: Text.AlignVCenter
							anchors.verticalCenter: folderIconHeaderContainer.verticalCenter
							elide: Text.ElideRight
						}
					}

					onItemClicked: {
						if (mouse.button != Qt.RightButton) {
							return
						}

						var item = folderTree.view.extendedModel.indexToItem(rowIndex);
						var fullPath = item.fullPath;
                        folderContextMenu.contextObject = fullPath;
						folderContextMenu.popup()
					}

					function getPath(index) {
						var proxyIndex = view.proxyModel.mapFromSource(index);
						var extendedIndex = view.extendedIndex(proxyIndex);
						var item = view.extendedModel.indexToItem(extendedIndex);
						return item.fullPath;
					}

					function getDisplayName(index) {
						var proxyIndex = view.proxyModel.mapFromSource(index);
						var extendedIndex = view.extendedIndex(proxyIndex);
						var item = view.extendedModel.indexToItem(extendedIndex);
						return item.value;
					}

					function getIndex(path, parentIndex) {
						var rowCount = parentIndex != null ? model.rowCount(parentIndex) : model.rowCount();
						for (var row = 0; row < rowCount; ++row) {
							var rowIndex = parentIndex != null ? model.index(row, 0, parentIndex) : model.index(row, 0);
							var rowPath = getPath(rowIndex);

							var pathN = internal.normalisePath(path);
							var rowPathN = internal.normalisePath(rowPath);
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

					function selectPath(path) {
						var index = getIndex(path);
						if (index == null) {
							return false;
						}

						return selectIndex(index);
					}

					function selectIndex(index) {
						if (index == null) {
							view.selectionModel.clearSelection();
							view.selectionModel.clearCurrentIndex();
						}
						var proxyIndex = view.proxyModel.mapFromSource(index);
						var extendedIndex = view.extendedIndex(proxyIndex);
						view.selectionModel.setCurrentIndex(extendedIndex, QtQmlModels.ItemSelectionModel.Clear | QtQmlModels.ItemSelectionModel.Select);
						return true;
					}
				}
			}

			Item {
				Layout.fillHeight: true
				Layout.fillWidth: true

				WGScrollView {
					anchors.fill: parent
					visible: showIcons

					GridView {
						id: assetGrid
						visible: showIcons
						model: filteredContentsModel
						focus: true

						cellWidth: iconSize < width ? width / Math.floor(width / iconSize) : iconSize
						cellHeight: iconSize + 36

						snapMode: GridView.SnapToRow

						highlight: WGHighlightFrame {}
						highlightMoveDuration: 0

						delegate: Item {
							width: assetGrid.cellWidth
                            height: assetGrid.cellHeight

							ColumnLayout {
								objectName: typeof(iconLabel.text) != "undefined" ? iconLabel.text : "assetArea"
								spacing: 0
								anchors.fill: parent

								//TODO Replace this with proper thumbnail
								Item {
									Layout.preferredHeight: iconSize
									Layout.preferredWidth: iconSize
									Layout.alignment: Qt.AlignVCenter | Qt.AlignHCenter

									Image {
                                        id: icon_file
										anchors.fill: parent
										source: {
											if (isDirectory == true)
												return "icons/folder_128x128.png"
											/*else if ( thumbnail !== undefined )
												return thumbnail*/
											else
												return "icons/file_128x128.png"
										}
										/*Image {
											source: statusIcon !== undefined ? statusIcon : ""
											anchors.left: icon_file.left
											anchors.bottom: icon_file.bottom
											anchors.leftMargin: iconSize > 32 ? Math.round(iconSize / 12) : 0
											anchors.bottomMargin: iconSize > 32 ? Math.round(iconSize / 24) : 0
										}*/
									}
								}

								WGMultiLineText {
									id: iconLabel

									text: display
									horizontalAlignment: Text.AlignHCenter

									lineHeightMode: Text.FixedHeight
									lineHeight: iconLabelSize + 2

									y: -2

									Layout.preferredWidth: parent.width - defaultSpacing.rowSpacing * 2
									Layout.preferredHeight: defaultSpacing.minimumRowHeight * 2
									Layout.alignment: Qt.AlignTop | Qt.AlignHCenter

									maximumLineCount: {
										var lines = 2
										if (iconSize <= 32) {
											lines += 1
										}
										return lines
									}
									wrapMode: Text.WrapAnywhere

									font.pointSize: iconLabelSize

									elide: Text.ElideRight
								}
							}

							MouseArea {
                                anchors.fill: parent
                                cursorShape: Qt.PointingHandCursor

                                acceptedButtons: Qt.RightButton | Qt.LeftButton

                                onPressed: {
                                    assetGrid.currentIndex = index
                                }

								onClicked: {
									if (mouse.button != Qt.RightButton) {
										return;
									}

									assetContextMenu.contextObject = fullPath;
									assetContextMenu.popup();
								}

                                onDoubleClicked: {
                                    if(mouse.button != Qt.LeftButton){
										return;
									}

									if (isDirectory) {
										selectFolder(fullPath);
										return;
									}

									assetAccepted(fullPath);
                                }
                            }
						}
					}
				}

				WGScrollView {
					anchors.fill: parent
					visible: !showIcons

					WGListView {
						id: assetList
						model: filteredContentsModel
						columnDelegates: [contentDelegate]
						clamp: true
						focus: true

						property Component contentDelegate: RowLayout {
							Layout.fillWidth: true					

							Image {
								source: itemData.isDirectory ? "icons/folder_16x16.png" : "icons/file_16x16.png"
							}

							WGLabel {
								text: itemValue
							}
						}

						onItemClicked: {
							if (mouse.button != Qt.RightButton) {
								return;
							}

							var item = assetList.view.extendedModel.indexToItem(rowIndex);
							var fullPath = item.fullPath;
							assetContextMenu.contextObject = fullPath;
							assetContextMenu.popup();
						}

						onItemDoubleClicked: {
							var item = assetList.view.extendedModel.indexToItem(rowIndex);
							var isDirectory = item.isDirectory;
							var fullPath = item.fullPath;

							if (isDirectory) {
								selectFolder(fullPath);
								return;
							}

							assetAccepted(fullPath);
						}

						function getPath(index) {
							var proxyIndex = view.proxyModel.mapFromSource(index);
							var extendedIndex = view.extendedIndex(proxyIndex);
							var item = view.extendedModel.indexToItem(extendedIndex);
							return item.fullPath;
						}

						function getIndex(path) {
							var rowCount = model.rowCount();
							for (var row = 0; row < rowCount; ++row) {
								var rowIndex = model.index(row, 0);
								var rowPath = getPath(rowIndex);

								var pathN = internal.normalisePath(path);
								var rowPathN = internal.normalisePath(rowPath);
								if (pathN == rowPathN) {
									return rowIndex;
								}
							}
							return null;
						}

						function selectPath(path) {
							var index = getIndex(path);
							if (index == null) {
								return false;
							}

							return selectIndex(index);
						}

						function selectIndex(index) {
							if (index == null) {
								view.selectionModel.clearSelection();
								view.selectionModel.clearCurrentIndex();
							}
							var proxyIndex = view.proxyModel.mapFromSource(index);
							var extendedIndex = view.extendedIndex(proxyIndex);
							view.selectionModel.setCurrentIndex(extendedIndex, QtQmlModels.ItemSelectionModel.Clear | QtQmlModels.ItemSelectionModel.Select);
							return true;
						}
					}
				}
			}
        } //SplitView
    }
}
