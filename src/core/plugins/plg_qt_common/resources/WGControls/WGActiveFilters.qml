import QtQuick 2.3
import QtQuick.Dialogs 1.2
import QtQuick.Controls 1.2
import QtQuick.Layouts 1.0
import QtQuick.Controls.Private 1.0
import WGControls 1.0
import WGCopyableFunctions 1.0

/*!
 \brief A control used to represent the presence of a user definable filter.
 When the user inputs a search word a new active filter is created and displayed alongside the search bar.
 Individual filters and be toggled on/off or removed.

Example:
\code{.js}
WGActiveFilters {
    id: activeFilters
    dataModel: filtersModel
}
\endcode
*/

ColumnLayout {
    id: rootFrame
    objectName: "WGActiveFilters"

    // Public properties
    /*! This property holds the dataModel containing all filters */
    property var dataModel

    /*! This property holds the filter string
        The default value is an empty string
    */
    property var stringValue: __internalStringValue

    /*! This property makes the filter tags appear to the left of the search text instead of below it.
        When the search tags exceed half the width of the search field they are moved to a flow layout beneath the search text field
        The default value is true
    */
    property bool inlineTags: true

    // Locals for referencing interior fields
    /*! \internal */
    property var __internalStringValue: ""

    /*! \internal */
    property var __filterText: filterText

    //This property denotes what splitter character is used when generating the string value for filter components
    /*! \internal */
    property var __splitterChar: ","

    /*  This property holds the original inlineTags value.
        inLineTags can change and the original state must be kept. */
    /*! \internal */
    property bool __originalInlineTagSetting: false

    /*! \internal */
    property int __currentFilterWidth: 0

    /*! \internal */
    property int __filterTags: 0

    // This property indicates when we need to update the layout
    /*! \internal */
    property bool __changeLayout: false

    // This property indicates what the currently loaded filter is
    /*! \internal */
    property var __loadedFilterId: ""

    // This property holds the minimum width for binding purposes
    /* \internal */
    property var __minimumWidth: 200

    // This property holds the original minimumWidth value.
    /*! \internal */
    property int __originalMinimumWidth: __minimumWidth

    //------------------------------------------
    // Signals
    //------------------------------------------

    signal changeFilterWidth(int filterWidth, bool add)

    signal beforeAddFilter(var filter)

    //------------------------------------------
    // Functions
    //------------------------------------------

    // Handles the addition of a new filter to the active filters list
    /*! \internal */
    function addFilter( text ) {
        //remove extra whitespace at start and end and check string contains some characters
        var filter = {"display": text, "value": text, "active": true}
        beforeAddFilter( filter )
        if (filter.value != "")
        {
            rootFrame.dataModel.addFilterTerm(filter.display, filter.value, filter.active);
            // Get the newly added filter, sadly VariantList doesn't have easy access so iterate to it
            var filtersIter = iterator( rootFrame.dataModel.currentFilterTerms );
            var newFilter = filtersIter.current;
            while (filtersIter.moveNext()) {
                newFilter = filtersIter.current;
        }
        }
            filterText.text = "";
    }

    // Handles updating the string value when the active filters list model
    // has been changed (additions or removals)
    /*! \internal */
    function updateStringValue() {
        var combinedStr = "";
        var iteration = 0;
        var filtersIter = iterator( rootFrame.dataModel.currentFilterTerms );
        while (filtersIter.moveNext()) {
            if (filtersIter.current.active == true) {
            if (iteration != 0) {
                combinedStr += __splitterChar;
            }
                combinedStr += filtersIter.current.value;
                ++iteration;
            }
        }

        __internalStringValue = combinedStr;
    }

    // Handles saving an active filter
    /*! \internal */
    function saveActiveFilter( /*bool*/ overwrite ) {
        var filterName = rootFrame.dataModel.saveFilter( overwrite );
        if (filterName.length > 0) {
            rootFrame.__loadedFilterId = filterName;
        }
    }

    // Handles updating the width of the edit box to fit at the end of the flow
    // and be at least half the width of the parent
    // Also updates the minimum width to show the longest filter term
    function updateLayout(){
        // Update the minimum width to fit the longest term
        var minWidth = __originalMinimumWidth
        for(var i = 0; i< filterRepeater.count; ++i)
            {
            // Not sure why the item width doesn't include the close button so increase the size to include one
            minWidth = Math.max(minWidth, filterRepeater.itemAt(i).width
                                + clearCurrentFilterButton.width + activeFiltersLayout.spacing)
    }
        rootFrame.__minimumWidth = minWidth

        // Update the edit box width, the flow will position the edit box as needed
        var newWidth = textFrame.width - activeFiltersLayout.spacing
        if(filterRepeater.count > 0)
            {
            var lastItem = filterRepeater.itemAt(filterRepeater.count-1)
            var inlineWidth = textFrame.width - lastItem.x - lastItem.width - activeFiltersLayout.spacing*2
            if(inlineWidth > textFrame.width/2)
                {
                newWidth = inlineWidth
                }
            }
        newWidth = newWidth - clearCurrentFilterButton.width - activeFiltersLayout.rightPadding

        if(newWidth != filterText.width)
                {
            filterText.width = newWidth
            // Some updates cause a re-flow which could require another update so enable the timer
            __changeLayout = true
                }
                else
                {
            __changeLayout = false
                }
            }

    // Used when a layout change could occur to account for the changes in the flow
    // For example if the width changes another term may fit in the row above
    // The text box size will be wrong as its width would have been calculated using the moved term
    // So we need to re-calculate the width again after everything has been re-flowed
    /*! \internal */
    Timer{
        id: layoutUpdater
        interval: 0
        running: true
        onTriggered: {
            rootFrame.updateLayout()
        }
    }

    Component.onCompleted: {
        __originalInlineTagSetting = inlineTags
        __originalMinimumWidth = __minimumWidth
        }

    onBeforeAddFilter: {
        filter.display = filter.display.trim()
        filter.value = filter.value.trim()
    }

    //------------------------------------------
    // List View Models for Active Filters
    //------------------------------------------
    WGListModel {
        id: filtersModel
        source: rootFrame.dataModel.currentFilterTerms

        onRowsInserted: {
            updateStringValue();
        }

        onRowsRemoved: {
            updateStringValue();
        }

        ValueExtension {}

        ColumnExtension {}
        ComponentExtension {}
        TreeExtension {}
        ThumbnailExtension {}
        SelectionExtension {}
    }

    WGListModel {
        id: savedFiltersModel
        source: rootFrame.dataModel.savedFilters

        ValueExtension {}
    }

        //------------------------------------------
        // Top Row - Text Area and Buttons
        //------------------------------------------
        WGExpandingRowLayout {
            id: inputRow
        Layout.minimumWidth: rootFrame.__minimumWidth
            Layout.fillWidth: true
            Layout.alignment: Qt.AlignLeft | Qt.AlignTop

            WGPushButton {
                //Save filters and load previous filters
                id: btnListviewFilters
                objectName: "searchPushButton"
                iconSource: "icons/search_folder_16x16.png"

                tooltip: "Filter Options"

                menu: WGMenu {
                    id: activeFiltersMenu
                    title: "Filters"

                    MenuItem {
                        text: "Save New Filter"
                        onTriggered: {
                            // TODO - Refine saving to allow for naming of the filter
                            // JIRA - NGT-1484

                            if (rootFrame.__loadedFilterId.length > 0) {
                                // Prompt the user!
                                overwritePromptDialog.open()
                            }
                            else {
                                saveActiveFilter( false );
                            }
                        }
                    }

                    MenuItem {
                        text: "Clear Saved Filters"
                        onTriggered: {
                            rootFrame.dataModel.clearSavedFilters();
                        }
                    }

                    MenuSeparator { }

                    WGMenu {
                        id: savedFiltersMenu
                        title: "Saved Filters:"

                        Instantiator {
                            model: savedFiltersModel

                            delegate: MenuItem {
                                text: value.filterId + ": " + value.terms
                                onTriggered: {
                                    var result = rootFrame.dataModel.loadFilter(value.filterId);
                                    if (result) {
                                        rootFrame.__loadedFilterId = value.filterId;
                                    }
                                    else {
                                        rootFrame.__loadedFilterId = "";
                                    }
                                }
                            }

                            onObjectAdded: savedFiltersMenu.insertItem(index, object)
                            onObjectRemoved: savedFiltersMenu.removeItem(object)
                        }
                    }
                }
            }

            WGTextBoxFrame {
                id: textFrame
                color: palette.textBoxColor
                Layout.fillWidth: true
                Layout.alignment: Qt.AlignLeft | Qt.AlignTop
            Layout.preferredHeight: childrenRect.height

            onWidthChanged: { updateLayout() }

            Flow {
                id: activeFiltersLayout
                spacing: defaultSpacing.rowSpacing
                width: textFrame.width

                Repeater {
                    id: filterRepeater
                    model: filtersModel
                    onItemAdded: { updateLayout() }
                    onItemRemoved: { updateLayout() }

                    delegate: WGButtonBar {
                        property var myValue: value;
                        showSeparators: false
                        evenBoxes: false
                        buttonList: [
                            WGPushButton {
                                id: filterString
                                objectName: "filterStringButton"
                                text: myValue.display
                                checkable: true
                                checked: myValue.active
                                style: WGTagButtonStyle{}

                                Binding {
                                    target: myValue
                                    property: "active"
                                    value: filterString.checked
                }

                                Connections {
                                    target: myValue
                                    onActiveChanged: {
                                        updateStringValue();
                                    }
                }

                                MouseArea{
                                    id: tooltip
                                    anchors.fill: parent
                                    hoverEnabled: true
                                    acceptedButtons: Qt.NoButton
                                    propagateComposedEvents: true
                                    onExited: Tooltip.hideText()
                                    onCanceled: Tooltip.hideText()

                                    property string text: myValue.value

                                    Timer {
                                        id: timer
                                        interval: 500;
                                        running: tooltip.containsMouse && !tooltip.pressed && tooltip.text.length
                                        onTriggered: Tooltip.showText(tooltip, Qt.point(tooltip.mouseX, tooltip.mouseY), tooltip.text);
                                    }
                                }
                            },
                            WGToolButton {
                                id: closeButton
                                objectName: "closeButton"
                                iconSource: "icons/close_sml_16x16.png"

                                onClicked: {
                                    rootFrame.dataModel.removeFilterTerm(index);
                                }
                            }
                        ]
                    }
                }

                    WGTextBox {
                        id: filterText
                        objectName: "filterText"
                        Layout.fillWidth: true
                        Layout.preferredHeight: defaultSpacing.minimumRowHeight

                    onXChanged: updateLayout()

                        style: WGInvisTextBoxStyle{}

                        placeholderText: "Filter"
                        Layout.alignment: Qt.AlignLeft | Qt.AlignTop

                        Component.onCompleted: {
                            WGCopyableHelper.disableChildrenCopyable(filterText);
                        }

                        Keys.onReturnPressed: {
                            addFilter( text );
                        }

                        Keys.onEnterPressed: {
                            addFilter( text );
                        }
                    }

                    WGToolButton {
                        id: clearCurrentFilterButton
                        objectName: "clearFilterButton"
                        iconSource: "icons/close_sml_16x16.png"

                        tooltip: "Clear Filters"
                        Layout.alignment: Qt.AlignLeft | Qt.AlignTop

                        onClicked: {
                            rootFrame.dataModel.clearCurrentFilter();
                            rootFrame.__internalStringValue = "";
                            __currentFilterWidth = 0
                            __filterTags = 0
                        }
                    }
            } // activeFiltersLayout
        } // textFrame
        } // inputRow

    MessageDialog {
        id: overwritePromptDialog
        objectName: "overwrite_dialog"
        title: "Overwrite?"
        icon: StandardIcon.Question
        text: "This filter already exists. Would you like to overwrite it with the new terms?"
        standardButtons: StandardButton.Yes | StandardButton.No | StandardButton.Abort
        modality: Qt.WindowModal
        visible: false

        onYes: {
            saveActiveFilter( true );
                            }

        onNo: {
            saveActiveFilter( false );
                    }
                }

    /*! Deprecated */
    property alias internalStringValue: rootFrame.__internalStringValue

    /*! Deprecated */
    property alias filterText_: rootFrame.__filterText

    /*! Deprecated */
    property alias splitterChar: rootFrame.__splitterChar

    /*! Deprecated */
    property alias _originalInlineTagSetting: rootFrame.__originalInlineTagSetting

    /*! Deprecated */
    property alias _currentFilterWidth: rootFrame.__currentFilterWidth

    /*! Deprecated */
    property alias _filterTags: rootFrame.__filterTags

    /*! Deprecated */
    property alias _loadedFilterId: rootFrame.__loadedFilterId

    property alias placeHolderText: filterText.placeholderText

} // rootFrame
