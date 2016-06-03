import QtQuick 2.3
import QtQuick.Dialogs 1.2
import QtQuick.Controls 1.2
import QtQuick.Layouts 1.0
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

Item {
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

    // This property holds the flip state between filter tags being drawn inline or on a new line
    /*! \internal */
    property bool __changeLayout: false

    // This property indicates what the currently loaded filter is
    /*! \internal */
    property var __loadedFilterId: ""

    //------------------------------------------
    // Signals
    //------------------------------------------

    signal changeFilterWidth(int filterWidth, bool add)

    //------------------------------------------
    // Functions
    //------------------------------------------

    // Handles the addition of a new filter to the active filters list
    /*! \internal */
    function addFilter( text ) {
        //remove extra whitespace at start and end and check string contains some characters
        text = text.trim()
        if (text != "")
        {
            rootFrame.dataModel.addFilterTerm(text);
            filterText.text = "";
        }
        else
        {
            filterText.text = "";
        }
    }

    // Handles updating the string value when the active filters list model
    // has been changed (additions or removals)
    /*! \internal */
    function updateStringValue() {
        var combinedStr = "";
        var iteration = 0;
        var filtersIter = iterator( rootFrame.dataModel.currentFilterTerms );
        while (filtersIter.moveNext()) {
            if (iteration != 0) {
                combinedStr += __splitterChar;
            }

            if (filtersIter.current.active == true) {
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

    // Moves active filters to and from a flow layout when containter is resized
    /*! \internal */
    function checkActiveFilterSize(){
        if (__originalInlineTagSetting && inlineTags)
        {
            if (__currentFilterWidth > textFrame.width / 2)
            {
                __filterTags = 0 // all filter tags are rebuilt when inlineTags changes, must reset value
                __currentFilterWidth = 0
                inlineTags = false // move to flow layout
            }
        }
        if (__originalInlineTagSetting && !inlineTags)
        {
            if (__currentFilterWidth < textFrame.width / 2)
            {
                __filterTags = 0
                __currentFilterWidth = 0
                inlineTags = true
            }
        }
    }

    Component.onCompleted: {
        __originalInlineTagSetting = inlineTags
    }

    onChangeFilterWidth: {
        if(add)
        {
            __currentFilterWidth += filterWidth
            __filterTags += 1
            if (__originalInlineTagSetting && inlineTags)
            {
                // are the filters taking up more than half the space?
                if (__currentFilterWidth > (textFrame.width / 2))
                {
                    __filterTags = 0 // all filter tags are rebuilt when inlineTags changes, must reset value
                    __currentFilterWidth = 0
                    inlineTags = false
                }
            }
        }
        else // active filter being removed
        {
            __currentFilterWidth -= filterWidth
            __filterTags -= 1
            if (__originalInlineTagSetting && !inlineTags)
            {
                if (__currentFilterWidth > (textFrame.width / 2))
                {
                    inlineTags = false
                }
                else
                {
                    __filterTags = 0
                    __currentFilterWidth = 0
                    inlineTags = true
                }
            }
        }
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
    // Main Layout
    //------------------------------------------

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


    ColumnLayout {
        id: mainRowLayout
        anchors {left: parent.left; top: parent.top; right: parent.right}

        //------------------------------------------
        // Top Row - Text Area and Buttons
        //------------------------------------------
        WGExpandingRowLayout {
            id: inputRow
            Layout.fillWidth: true
            Layout.preferredHeight: childrenRect.height
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
                        text: "Save New Filter..."
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
                Layout.preferredHeight: childrenRect.height + defaultSpacing.standardBorderSize
                Layout.maximumHeight: childrenRect.height + defaultSpacing.standardBorderSize
                Layout.alignment: Qt.AlignLeft | Qt.AlignTop

                onWidthChanged: {
                    checkActiveFilterSize()
                }

                // can only be a single row

                WGExpandingRowLayout {
                    id: inputLine
                    anchors {left: parent.left; top: parent.top; right: parent.right}
                    height: defaultSpacing.minimumRowHeight

                    Loader {
                        id: activeFiltersInlineRect
                        visible: __filterTags > 0 && inlineTags
                        Layout.preferredWidth: __currentFilterWidth + (defaultSpacing.rowSpacing * __filterTags) + defaultSpacing.rowSpacing
                        Layout.alignment: Qt.AlignLeft | Qt.AlignTop
                        sourceComponent: inlineTags ? filterTagList : null
                    } // activeFiltersLayoutRect

                    WGTextBox {
                        id: filterText
                        objectName: "filterText"
                        Layout.fillWidth: true
                        Layout.preferredHeight: defaultSpacing.minimumRowHeight

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

                }
            }
        } // inputRow

        //------------------------------------------
        // Bottom Area with Filter Entries
        //------------------------------------------
        Loader {
            id: activeFiltersBelowLoader
            visible: !inlineTags && __filterTags > 0
            Layout.fillWidth: true
            sourceComponent: inlineTags ? null : filterTagList
        }

        Rectangle {
            id: spacer
            visible: !inlineTags && __filterTags > 0
            Layout.fillWidth: true
            Layout.minimumHeight: defaultSpacing.doubleBorderSize
            color: "transparent"
        }

        Component {
            id: filterTagList
            Flow {
                id: activeFiltersLayout
                spacing: defaultSpacing.rowSpacing

                Repeater {
                    id: filterRepeater
                    model: filtersModel
                    onItemAdded: {
                        changeFilterWidth(item.width, true)
                    }
                    onItemRemoved: {
                        changeFilterWidth(item.width, false)
                    }

                    delegate: WGButtonBar {
                        property var myValue: value;
                        showSeparators: false
                        evenBoxes: false
                        buttonList: [
                            WGPushButton {
                                id: filterString
                                objectName: "filterStringButton"
                                text: myValue.value
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
            } // activeFiltersBelowRect
        } //filterTags
    } // mainRowLayout

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
    property alias _changeLayout: rootFrame.__changeLayout

    /*! Deprecated */
    property alias _loadedFilterId: rootFrame.__loadedFilterId

} // rootFrame
