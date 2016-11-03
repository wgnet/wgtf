import QtQuick 2.5
import QtQuick.Dialogs 1.2
import QtQuick.Controls 1.4
import QtQuick.Layouts 1.3
import QtQuick.Controls.Private 1.0
import Qt.labs.controls 1.0 as Labs

import WGControls 1.0
import WGControls.Styles 1.0
import WGCopyableFunctions 1.0
import WGControls.Layouts 1.0

// TODO: Make Active Filters use all 2.0 plugins
import WGControls.Views 2.0

/*!
 \ingroup wgcontrols
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

\todo
  1. Have an option for search filter to be at top or bottom of active filters control when inlinetags false
  2. Once the filter text has gone onto a new line it will always stay on its own line.. it should continue the
  inline behviour onto the second line if there is > __minimumFilterTextWidth space
  3. Add backspace entered into an empty filterText field to remove filter tags. Only if empty!
  4. Put filterText into an item so that the clearCurrentFilterButton does not obuscure large text entries
*/

ColumnLayout {
    id: rootFrame
    objectName: "WGActiveFilters"
    WGComponent { type: "WGActiveFilters" }

    // Public properties
    /*! This property holds the dataModel containing all filters */
    property var dataModel

    /*! This property holds the filter string
        The default value is an empty string
    */
    property var stringValue: __internalStringValue

    /*! This property makes the filter tags appear to the left of the search text instead of below it.
        When false the filterText will always be place on its own line
        When the filterText width falls below __minimumFilterTextWidth it will be placed on a new line
        The default value is true
    */
    property bool inlineTags: true

    /*! The string list for autocomplete */
    // TODO: This needs to be populated with possible matches
    property var autoCompleteData: []

    /*! Have the filter text automatically complete based on the autoCompleteData

        The default is true if autoCompleteData contains any strings. */
    property bool useAutoComplete: autoCompleteData.length > 0

    /*! Show a dropdown menu of suggestions based on autoCompleteData

        The default is true if useAutoComplete is true. */
    property bool showSuggestions: useAutoComplete

    /*! The searchType for autoComplete.

        Valid entries are:
        Qt.MatchExactly
        Qt.CaseSensitive
        Qt.MatchFixedString

        The default is Qt.MatchFixedString. */
    property int searchType: Qt.MatchFixedString

    /*! Allows filter tags to be edited after they have been created.

        The default value is true
    */
    property bool editableTags: true

    // current filter term to use for autocomplete
    /*! \internal */
    property string currentFilterText: ""

    // Locals for referencing interior fields
    /*! \internal */
    property var __internalStringValue: ""

    /*! \internal */
    property var __filterText: filterText

    //This property denotes what splitter character is used when generating the string value for filter components
    /*! \internal */
    property var __splitterChar: ","

    /*! \internal */
    property alias __filterTags: filterRepeater.count

    // This property indicates what the currently loaded filter is
    /*! \internal */
    property var __loadedFilterId: ""

    //This property holds the mimimum width the filterText will remain on the same line as filter tags
    /*! \internal */
    property int __minimumFilterTextWidth: 100

    // This property holds the original minimumWidth value.
    /*! \internal */
    property int __originalMinimumWidth: __minimumWidth

    // This property holds the minimum width for binding purposes
    /* \internal */
    property var __minimumWidth: 100

    //------------------------------------------
    // Signals
    //------------------------------------------

    signal changeFilterWidth(int filterWidth, bool add)

    signal beforeAddFilter(var filter)

    signal updateAutoComplete()

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

    function termExists( newTerm ) {
        var filtersIter = iterator( rootFrame.dataModel.currentFilterTerms );
        var termMatches = false
        while (filtersIter.moveNext()) {
            if (filtersIter.current.active == true) {
                if (newTerm == filtersIter.current.value)
                {
                    termMatches = true
                }
            }
        }
        return termMatches
    }

    // Handles saving an active filter
    /*! \internal */
    function saveActiveFilter( /*bool*/ overwrite ) {
        var filterName = rootFrame.dataModel.saveFilter( overwrite );
        if (filterName.length > 0) {
            rootFrame.__loadedFilterId = filterName;
        }
    }

    // If the filterText.width is less than half the remaining textFrame.width it will be put on a new line
    function updateLayout(){
        // Update the minimum width to fit the longest term
        var minWidth = __originalMinimumWidth
        for(var i = 0; i< filterRepeater.count; ++i)
        {
            minWidth = Math.max(minWidth, filterRepeater.itemAt(i).width)
        }

        rootFrame.__minimumWidth = Math.max(__minimumFilterTextWidth, minWidth)

        if (inlineTags)
        {
            if(filterRepeater.count > 0)
            {
                var currentLineWidth = 0
                var longestLine = 0

                for (var i=0; i < filterRepeater.count; i++) {
                    if (currentLineWidth + filterRepeater.itemAt(i).width + activeFiltersLayout.spacing > textFrame.width)
                    {
                        longestLine = Math.max(longestLine, currentLineWidth)
                        currentLineWidth = filterRepeater.itemAt(i).width + activeFiltersLayout.spacing
                    }
                    else
                    {
                        currentLineWidth += filterRepeater.itemAt(i).width + activeFiltersLayout.spacing
                        longestLine = Math.max(longestLine, currentLineWidth)
                    }
                }

                var remainingWidth = textFrame.width - currentLineWidth

                if (remainingWidth + activeFiltersLayout.spacing < __minimumFilterTextWidth)
                {
                    filterEditFrame.width = textFrame.width
                }
                else
                {
                    filterEditFrame.width = remainingWidth
                }
            }
            else
            {
                filterEditFrame.width = textFrame.width
            }
        }
        else //inlineTags == false
        {
            filterEditFrame.width = textFrame.width
        }
    }

    function find (text, searchType) {
        for (var i = 0 ; i < autoCompleteData.length ; ++i) {
            var currentString = autoCompleteData[i]
            if (searchType === Qt.MatchExactly) {
                if (text === currentString)
                    return i;
            } else if (searchType === Qt.CaseSensitive) {
                if (currentString.indexOf(text) === 0)
                    return i;
            } else if (searchType === Qt.MatchFixedString) {
                if (currentString.toLowerCase().indexOf(text.toLowerCase()) === 0
                        && currentString.length === text.length)
                    return i;
            } else if (currentString.toLowerCase().indexOf(text.toLowerCase()) === 0) {
                return i
            }
        }
        return -1;
    }

    Component.onCompleted: {
        __originalMinimumWidth = __minimumWidth
        updateAutoComplete()
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
            Layout.minimumWidth: rootFrame.__minimumWidth + btnListviewFilters.width + spacing
            Layout.fillWidth: true
            Layout.alignment: Qt.AlignLeft | Qt.AlignTop

            property int termEditing: -1

            signal clickAutoMenu( string term )

            onTermEditingChanged: {
                if (termEditing == -1)
                {
                    filterText.focus = true
                }
            }

            WGPushButton {
                //Save filters and load previous filters
                id: btnListviewFilters
                objectName: "searchPushButton"
                iconSource: "icons/search_folder_16x16.png"

                tooltip: "Filter Options"

                Layout.alignment: Qt.AlignHCenter | Qt.AlignTop

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
                Layout.minimumWidth: rootFrame.__minimumWidth

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

                        delegate: WGButtonFrame {
                            id: tagFrame
                        property var myValue: value;

                            objectName: "filterTagFrame_" + myValue.display

                            height: defaultSpacing.minimumRowHeight

                            width: defaultSpacing.doubleMargin + closeButton.width + Math.max(tagWidth.width + defaultSpacing.doubleMargin, tagEditTextWidth.width + defaultSpacing.doubleMargin)

                            onWidthChanged: {
                                if(inputRow.termEditing == index)
                                {
                                    updateLayout()
                                }
                            }

                            TextMetrics {
                                id: tagWidth
                                text: myValue.display
                            }

                            TextMetrics {
                                id: tagEditTextWidth
                                text: tagEdit.text
                            }

                            Row {
                                Item { height: parent.height; width: defaultSpacing.standardMargin;}
                                spacing: 0

                                WGToolButton {
                                    id: closeButton
                                    objectName: "closeButton_" + filterString.text
                                    iconSource: hovered || activeFocus ? "icons/delete_sml_16x16.png" : "icons/close_sml_16x16.png"

                                    y: pressed ? 1 : 0

                                    radius: 0

                                    height: parent.height
                                    width: 10

                                    onClicked: {
                                        filterText.focus = true
                                        rootFrame.currentFilterText = ""
                                        rootFrame.dataModel.removeFilterTerm(index);
                                    }

                                    style: WGToolButtonStyle {
                                        background: Item{}
                                    }
                                }

                                WGPushButton {
                                    id: filterString
                                    objectName: "filterStringButton_" + text
                                    visible: inputRow.termEditing != index
                                    text: myValue.display
                                    checkable: true
                                    checked: myValue.active
                                    style: WGTagButtonStyle{
                                        __textColor: palette.disabledTextColor
                                    }

                                    radius: 0

                                    width: tagWidth.width + defaultSpacing.doubleMargin

                                    property bool doubleCheck: false

                                    tooltip: myValue.value

                                    Connections {
                                        target: myValue
                                        onActiveChanged: {
                                            updateStringValue();
                                        }
                                    }

                                    onClicked: {
                                        if(!doubleCheck)
                                        {
                                            filterString.doubleCheck = true
                                        }
                                        else if (editableTags)
                                        {
                                            inputRow.termEditing = index
                                            tagEdit.focus = true
                                            tagEdit.selectAll()
                                            filterString.doubleCheck = false
                                        }
                                    }

                                    Timer {
                                        //Double click check timer
                                        running: filterString.doubleCheck == true
                                        interval: 200
                                        onTriggered: {
                                            myValue.active = !myValue.active
                                            filterString.doubleCheck = false
                                        }
                                    }
                                }

                                // editable tag text box
                                WGTextBoxAutoComplete {
                                    id: tagEdit
                                    objectName: "tagEdit_" + text
                                    visible: inputRow.termEditing == index
                                    text: myValue.display

                                    useAutoComplete: rootFrame.useAutoComplete

                                    autoCompleteData: rootFrame.autoCompleteData

                                    width: Math.max(tagWidth.width + defaultSpacing.doubleMargin, tagEditTextWidth.width + defaultSpacing.doubleMargin)
                                    height: defaultSpacing.minimumRowHeight - defaultSpacing.doubleBorderSize

                                    anchors.verticalCenter: parent.verticalCenter

                                    onTextChanged: {
                                        if(visible)
                                        {
                                            currentFilterText = text
                                            if (text != "")
                                            {
                                                inputRow.termEditing = index
                                            }
                                        }
                                    }

                                    onEditingFinished: {
                                        text = text.trim()

                                        if(text == "")
                                        {
                                            //Delete tag if string is empty
                                            //TODO: Clearing an empty tag this way does not seem to set focus back to the edit box properly
                                            filterText.focus = true
                                            inputRow.termEditing = -1
                                            rootFrame.currentFilterText = ""
                                            rootFrame.dataModel.removeFilterTerm(index);
                                        }
                                        else if (termExists(text))
                                        {
                                            //reset tag to model data if tag already exists
                                            text = myValue.display
                                            filterText.focus = true
                                            inputRow.termEditing = -1
                                            rootFrame.currentFilterText = ""
                                            updateLayout()
                                        }
                                        else
                                        {
                                            //change data
                                            myValue.value = text
                                            myValue.display = text
                                            updateStringValue();
                                            filterText.focus = true
                                            inputRow.termEditing = -1
                                            rootFrame.currentFilterText = ""
                                            updateLayout()
                                        }
                                    }

                                    Keys.onUpPressed: {
                                        if( useAutoComplete && inputRow.termEditing == index)
                                        {
                                            autoCompleteList.currentIndex = Math.max(0, autoCompleteList.currentIndex - 1)
                                            text = autoCompleteData[autoCompleteList.currentIndex]
                                        }
                                        else
                                        {
                                            event.accepted = false
                                        }
                                    }

                                    Keys.onDownPressed: {
                                        if( useAutoComplete && inputRow.termEditing == index)
                                        {
                                            autoCompleteList.currentIndex = Math.min(autoCompleteData.length - 1, autoCompleteList.currentIndex + 1)
                                            text = autoCompleteData[autoCompleteList.currentIndex]
                                        }
                                        else
                                        {
                                            event.accepted = false
                                        }
                                    }

                                    Connections {
                                        target: inputRow
                                        onClickAutoMenu: {
                                            if (index == inputRow.termEditing)
                                            {
                                                tagEdit.text = term
                                                inputRow.termEditing = -1
                                            }
                                        }
                                    }
                                }

                                Item { height: parent.height; width: defaultSpacing.standardMargin;}
                            }
                        }
                    }
                Item {
                    id: filterEditFrame
                    implicitWidth: __minimumFilterTextWidth
                    implicitHeight: defaultSpacing.minimumRowHeight
                    onXChanged: updateLayout()

                    WGTextBoxAutoComplete {
                        id: filterText
                        objectName: "filterText"
                        height: defaultSpacing.minimumRowHeight
                        anchors.fill: parent
                        anchors.rightMargin: clearCurrentFilterButton.width

                        style: WGInvisTextBoxStyle{}

                        placeholderText: "Filter"

                        autoCompleteData: rootFrame.autoCompleteData

                        useAutoComplete: rootFrame.useAutoComplete

                        onActiveFocusChanged: {
                            if(activeFocus)
                            {
                                inputRow.termEditing = rootFrame.__filterTags
                            }
                        }

                        onTextChanged: {
                            currentFilterText = text
                            if (text != "")
                            {
                                inputRow.termEditing = rootFrame.__filterTags
                            }
                            else
                            {
                                inputRow.termEditing = -1
                            }
                        }

                        Component.onCompleted: {
                            WGCopyableHelper.disableChildrenCopyable(filterText);
                        }

                        Keys.onReturnPressed: {
                            addFilter( text );
                            inputRow.termEditing = -1
                            rootFrame.currentFilterText = ""
                        }

                        Keys.onEnterPressed: {
                            addFilter( text );
                            inputRow.termEditing = -1
                            rootFrame.currentFilterText = ""
                        }

                        Keys.onTabPressed: {
                            addFilter( text );
                            inputRow.termEditing = -1
                            rootFrame.currentFilterText = ""
                        }

                        Keys.onUpPressed: {
                            if( useAutoComplete && inputRow.termEditing == rootFrame.__filterTags)
                            {
                                autoCompleteList.currentIndex = Math.max(0, autoCompleteList.currentIndex - 1)
                                text = autoCompleteData[autoCompleteList.currentIndex]
                            }
                            else
                            {
                                event.accepted = false
                            }
                        }

                        Keys.onDownPressed: {
                            if( useAutoComplete && inputRow.termEditing == rootFrame.__filterTags)
                            {
                                autoCompleteList.currentIndex = Math.min(autoCompleteData.length - 1, autoCompleteList.currentIndex + 1)
                                text = autoCompleteData[autoCompleteList.currentIndex]
                            }
                            else
                            {
                                event.accepted = false
                            }
                        }

                        Keys.onPressed: {
                            //Delete last tag if no text is entered and backspace/delete is not held down
                            if (event.key == Qt.Key_Backspace || event.key == Qt.Key_Delete && inputRow.termEditing == rootFrame.__filterTags)
                            {
                                if(filterText.text == "" && __filterTags > 0 && !event.isAutoRepeat)
                                {
                                    rootFrame.dataModel.removeFilterTerm(__filterTags - 1);
                                }
                            }
                        }
                    }

                    WGToolButton {
                        id: clearCurrentFilterButton
                        objectName: "clearFilterButton"
                        iconSource: "icons/close_sml_16x16.png"
                        anchors.right: parent.right
                        anchors.top: parent.top

                        tooltip: "Clear All Filters"

                        onClicked: {
                            filterText.text = ""
                            rootFrame.currentFilterText = ""
                            rootFrame.dataModel.clearCurrentFilter();
                            rootFrame.__internalStringValue = "";
                        }
                    }
                }
            } // Flow

            Labs.Popup
            {
                //auto complete suggestion drop down menu
                id: autoCompleteMenu
                width: parent.width
                height: 200
                y: parent.height

                visible: (inputRow.termEditing >= 0 && showSuggestions && __filterText.activeFocus)

                padding: 0
                margins: 0

                ListModel {
                    id: autoCompleteListModel
                }

                /*TODO:
                    This is a pretty big hack to refresh the model of strings... but no other way to have
                    the dropdown repopulate if the array of strings changes.
                */
                Connections {
                    target: rootFrame
                    onUpdateAutoComplete: {
                        autoCompleteListModel.clear()
                        for(var i = 0; i < rootFrame.autoCompleteData.length; i++)
                        {
                            autoCompleteListModel.append({"term": rootFrame.autoCompleteData[i]})
                        }
                    }
                }

                contentItem: WGScrollView {
                    anchors.top: parent.top
                    anchors.left: parent.left
                    anchors.right: parent.right
                    implicitHeight: Math.min(200, autoCompleteData.length * defaultSpacing.minimumRowHeight)
                    clip: true

                    ListView {
                        id: autoCompleteList

                        anchors.fill: parent

                        model: autoCompleteListModel

                        highlightResizeDuration: 0
                        highlightMoveDuration: 0

                        delegate: Item {
                            width: autoCompleteMenu.width
                            height: defaultSpacing.minimumRowHeight
                            Text {
                                anchors.left: parent.left
                                anchors.leftMargin: defaultSpacing.standardMargin
                                anchors.verticalCenter: parent.verticalCenter
                                text: model.term
                                color: palette.TextColor
                            }
                            MouseArea {
                                anchors.fill: parent
                                hoverEnabled: true
                                onEntered: {
                                    autoCompleteList.currentIndex = index
                                }
                                onClicked: {
                                    if(inputRow.termEditing == rootFrame.__filterTags)
                                    {
                                        addFilter( autoCompleteData[index] );
                                        inputRow.termEditing = -1
                                        rootFrame.currentFilterText = ""
                                    }
                                    else if (inputRow.termEditing >= 0)
                                    {
                                        inputRow.clickAutoMenu( autoCompleteData[index] )
                                    }
                                }
                            }
                        }

                        highlight: WGHighlightFrame {
                            width: autoCompleteList.width
                            height: defaultSpacing.minimumRowHeight
                        }

                        Connections {
                            target: rootFrame
                            onCurrentFilterTextChanged: {
                                autoCompleteList.currentIndex = find(currentFilterText, searchType)
                            }
                        }
                    } // List View
                } //Scroll View

                background: Rectangle {              // background
                    color: palette.mainWindowColor
                    border.width: defaultSpacing.standardBorderSize
                    border.color: palette.darkColor
                }
            } // Labs Popup
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
    property int _currentFilterWidth

    /*! Deprecated */
    property alias _filterTags: rootFrame.__filterTags

    /*! Deprecated */
    property alias _loadedFilterId: rootFrame.__loadedFilterId

    property alias placeHolderText: filterText.placeholderText

} // rootFrame
