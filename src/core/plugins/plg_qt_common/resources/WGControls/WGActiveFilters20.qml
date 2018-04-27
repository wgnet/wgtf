import QtQuick 2.5
import QtQuick.Dialogs 1.2
import QtQuick.Controls 1.4
import QtQuick.Layouts 1.3
import QtQuick.Controls.Private 1.0
import Qt.labs.controls 1.0 as Labs

import WGControls 2.0
import WGControls.Styles 2.0
import WGControls.Layouts 2.0
import WGControls.Views 2.0
import WGControls.Global 2.0

/*!
 \ingroup wgcontrols
 \brief A control used to represent the presence of a user definable filter.
  When the user inputs a search word a new active filter is created and displayed alongside the search bar.
  Individual filters and be toggled on/off or removed.

Example:
\code{.js}
WGActiveFilters {
    id: activeFilters
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
    WGComponent { type: "WGActiveFilters20" }

    property var splitterChar: ","
    property var dynamic : false;
    property var dynSearchMinLength: 3
    property var dynamicSearchInProgress : filterStringDynamic.length > 0;

    ListModel {
        id: filterList

        onRowsInserted: {
            updateFilterString();
        }

        onRowsRemoved: {
            updateFilterString();
        }
    }
    property var filterString: ""
    property var filterStringDynamic: ""
    property var filterCount: filterList.count

    ListModel {
        id: recentFiltersList
    }

    ListModel {
        id: defaultFiltersList
    }

    property bool editableTags: true
    property var currentFilterText: ""

    property var autoCompleteData: []
    property var autoCompleteModel: null
    property var autoCompleteRole: "display"

    property var nameFilters: ["Filter file (*.filter)", "All files (*)"]
    property var defaultNameFilter: nameFilters[0]
    property var defaultFiltersFolder: WGPath.getApplicationFolder("filters")

    //------------------------------------------
    // Signals
    //------------------------------------------

    signal beforeAddFilter(var filter)

    //------------------------------------------
    // Functions
    //------------------------------------------

    // Check active filter for the filter token value
    function filterExists(filterText) {
        for (var i = 0; i < filterList.count; ++i) {
            var filter = filterList.get(i);
            if (filter.value == filterText) {
                return true;
            }
        }
        return false;
    }

    function clearFilters() {
        filterList.clear();
    }

    // Add a single filter token to the active filter
    function addFilter(filterText, labelText, isActive) {
        filterText = filterText.trim();

        if (filterText == "")         return;
        if (filterExists(filterText)) return;

        var filter = {"display": typeof labelText !== 'undefined' ? labelText : filterText,
                      "value":   filterText,
                      "active":  typeof isActive  !== 'undefined' ? isActive : true};

        beforeAddFilter(filter);
        filterList.append(filter);
        filterStringDynamic = "";
    }

    function updateFilterString() {
        var tmp = "";
        for (var i = 0; i < filterList.count; ++i) {
            var filter = filterList.get(i);
            if (!filter.active) {
                continue;
            }
            if (tmp != "") {
                tmp += splitterChar;
            }
            tmp += filter.value;
        }
        if(dynamic)
        {
            var newFilterString = tmp;
            newFilterString.trim();
            if(newFilterString != "")
            {
                newFilterString += splitterChar;
            }
            newFilterString += filterStringDynamic;
            filterString = newFilterString;
        }
        else
        {
            filterString = tmp;
        }
    }

    // Check if the supplied filter exists in our saved/recent lists yet.
    function getFilterIndexInRecentsList(filterPath) {
        for (var i = 0; i < recentFiltersList.count; i += 1) {
            var savedFilterPath = recentFiltersList.get(i).path;
            if (savedFilterPath == filterPath) return i;
        }

        return -1;
    }

    function serialiseCurrentFilter() {
        // Serialise to format (display name is same as token value if not specified)
        // <FilterTokenDisplayName>;<FilterTokenValue>;<FilterIsEnabled>
        // Rock;Rock;True;Tree;Tree;False

        var result = ""
        for (var i = 0; i < filterList.count; ++i) {
            var filter = filterList.get(i);
            result += filter.display + ";";
            result += filter.value + ";";
            result += filter.active + ";";

        }
        return result;
    }

    // If the filterText.width is less than half the remaining textFrame.width it will be put on a new line
    function updateLayout() {
        if(filterRepeater.count > 0)
        {
            var currentLineWidth = 0

            for (var i=0; i < filterRepeater.count; i++) {
                if (currentLineWidth + filterRepeater.itemAt(i).width + activeFiltersLayout.spacing > textFrame.width)
                {
                    currentLineWidth = filterRepeater.itemAt(i).width + activeFiltersLayout.spacing
                }
                else
                {
                    currentLineWidth += filterRepeater.itemAt(i).width + activeFiltersLayout.spacing
                }
            }

            var remainingWidth = textFrame.width - currentLineWidth

            if (remainingWidth + activeFiltersLayout.spacing < 100)
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

    function deserialiseFilterString(serialisedFilter) {
        var tokens = serialisedFilter.split(";");

        var newFiltersLoaded = [];
        for (var i = 0; i < tokens.length - 1; i += 3) {
            newFiltersLoaded.push({"display": tokens[i],
                                   "value":   tokens[i + 1],
                                   "active":  tokens[i + 2] != 'false'});
        }

        return newFiltersLoaded;
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
                    text: "Open Filter"
                    onTriggered: {
                        openOrSaveFilterDialog.selectExisting     = true;
                        openOrSaveFilterDialog.title              = "Open Filter"
                        openOrSaveFilterDialog.appendToFilterList = false;
                        openOrSaveFilterDialog.open();
                    }
                }

                MenuItem {
                    text: "Append Filter"
                    onTriggered: {
                        openOrSaveFilterDialog.selectExisting     = true;
                        openOrSaveFilterDialog.title              = "Append Filter"
                        openOrSaveFilterDialog.appendToFilterList = true;
                        openOrSaveFilterDialog.open();
                    }
                }

                MenuItem {
                    text: "Save New Filter As"
                    enabled: filterList.count != 0
                    onTriggered: {
                        openOrSaveFilterDialog.selectExisting = false;
                        openOrSaveFilterDialog.title = "Save New Filter As"
                        openOrSaveFilterDialog.open();
                    }
                }

                MenuSeparator { }

                WGMenu {
                    id: defaultFiltersMenu
                    title: "Default Filters"

                    Component.onCompleted: {
                        updateDefaultFiltersListing();
                    }

                    Instantiator {
                        model: defaultFiltersList
                        delegate: MenuItem {
                            text: WGPath.getFileNameWithoutExtension(path)
                            onTriggered: {
                                if(!WGPath.fileExists(path)) {
                                    WGLogger.logError("Filter " + path + " does not exist");
                                    updateDefaultFiltersListing();
                                }
                                else {
                                    loadFilterFromPath(path, true /*replaceFilterList*/);
                                }
                            }
                        }

                        onObjectAdded:   defaultFiltersMenu.insertItem(index, object)
                        onObjectRemoved: defaultFiltersMenu.removeItem(object)
                    }
                }

                WGMenu {
                    id: recentFiltersMenu
                    title: "Recent Filters"

                    Instantiator {
                        model: recentFiltersList

                        delegate: MenuItem {
                            text: path
                            onTriggered: {
                                if(!WGPath.fileExists(text)) {
                                    WGLogger.logError("Filter " + path + " does not exist");
                                }
                                else {
                                    loadFilterFromPath(text, true /*replaceFilterList*/);
                                }
                                updateRecentFiltersListing(filterPath);
                            }
                        }

                        onObjectAdded:   recentFiltersMenu.insertItem(index, object)
                        onObjectRemoved: recentFiltersMenu.removeItem(object)
                    }
                }

                MenuItem {
                    text: "Clear Recent Filters List"
                    enabled: recentFiltersList.count != 0
                    onTriggered: {
                        clearFiltersPromptDialog.open()
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
                    model: filterList

                    onItemAdded: { updateLayout() }
                    onItemRemoved: { updateLayout() }

                    delegate: WGButtonFrame {
                        id: tagFrame

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
                            text: display
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
                                objectName: "closeButton_" + filterButton.text
                                iconSource: hovered || activeFocus ? "icons/delete_sml_16x16.png" : "icons/close_sml_16x16.png"

                                y: pressed ? 1 : 0

                                radius: 0

                                height: parent.height
                                width: 10

                                onClicked: {
                                    filterText.focus = true
                                    currentFilterText = ""
                                    filterList.remove(index, 1);
                                }

                                style: WGToolButtonStyle {
                                    background: Item{}
                                }
                            }

                            WGPushButton {
                                id: filterButton
                                visible: inputRow.termEditing != index
                                text: display
                                checkable: true
                                checked: active
                                style: WGTagButtonStyle{
                                    __textColor: palette.disabledTextColor
                                }

                                radius: 0

                                width: tagWidth.width + defaultSpacing.doubleMargin

                                property bool doubleCheck: false

                                tooltip: value

                                onClicked: {
                                    if(!doubleCheck)
                                    {
                                        filterButton.doubleCheck = true
                                    }
                                    else if (editableTags)
                                    {
                                        inputRow.termEditing = index
                                        tagEdit.focus = true
                                        tagEdit.selectAll()
                                        filterButton.doubleCheck = false
                                    }
                                }

                                Timer {
                                    //Double click check timer
                                    running: filterButton.doubleCheck == true
                                    interval: 200
                                    onTriggered: {
                                        model.active = !model.active
                                        filterButton.doubleCheck = false
                                        updateFilterString();
                                    }
                                }
                            }

                            // editable tag text box
                            WGTextBoxAutoComplete {
                                id: tagEdit
                                objectName: "tagEdit_" + text
                                visible: inputRow.termEditing == index
                                text: display
                                autoCompleteData: rootFrame.autoCompleteData
                                autoCompleteModel: rootFrame.autoCompleteModel
                                autoCompleteRole: rootFrame.autoCompleteRole

                                width: Math.max(tagWidth.width + defaultSpacing.doubleMargin, tagEditTextWidth.width + defaultSpacing.doubleMargin)
                                height: defaultSpacing.minimumRowHeight - defaultSpacing.doubleBorderSize

                                anchors.verticalCenter: parent.verticalCenter
                                passActiveFocus: true

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
                                        currentFilterText = ""
                                        filterList.remove(index, 1);
                                    }
                                    else if (filterExists(text))
                                    {
                                        //reset tag to model data if tag already exists
                                        text = display
                                        filterText.focus = true
                                        inputRow.termEditing = -1
                                        currentFilterText = ""
                                        updateLayout()
                                    }
                                    else
                                    {
                                        //change data
                                        value = text
                                        display = text
                                        updateFilterString();
                                        filterText.focus = true
                                        inputRow.termEditing = -1
                                        currentFilterText = ""
                                        updateLayout()
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
                    implicitWidth: 100
                    implicitHeight: defaultSpacing.minimumRowHeight
                    onXChanged: updateLayout()

                    WGTextBoxAutoComplete {
                        id: filterText
                        objectName: "filterText"
                        showSuggestions : !rootFrame.dynamic
                        height: defaultSpacing.minimumRowHeight
                        anchors.fill: parent
                        anchors.rightMargin: clearCurrentFilterButton.width
                        autoCompleteData: rootFrame.autoCompleteData
                        autoCompleteModel: rootFrame.autoCompleteModel
                        autoCompleteRole: rootFrame.autoCompleteRole
                        passActiveFocus: true

                        style: WGInvisTextBoxStyle{}

                        placeholderText: "Filter"

                        onSuggestionSelected: {
                            addFilter( text );
                            text = "";
                            inputRow.termEditing = -1;
                            currentFilterText = "";
                        }

                        onActiveFocusChanged: {
                            if(activeFocus)
                            {
                                inputRow.termEditing = filterCount
                            }
                        }

                        onTextChanged: {
                            currentFilterText = text
                            if (text != "")
                            {
                                inputRow.termEditing = filterCount
                                var oldStringDynamic = filterStringDynamic;
                                if(dynamic)
                                {
                                    if( text.length < dynSearchMinLength )
                                    {
                                        filterStringDynamic = "";
                                    }
                                    else
                                    {
                                        filterStringDynamic = text;
                                    }
                                    if( oldStringDynamic == filterStringDynamic )
                                    {
                                        return;
                                    }
                                    updateFilterString();
                                }
                            }
                            else
                            {
                                inputRow.termEditing = -1
                                if(dynamic)
                                {
                                    filterStringDynamic = "";
                                    var oldStringDynamic = filterStringDynamic;
                                    if( oldStringDynamic == filterStringDynamic )
                                    {
                                        return;
                                    }
                                    updateFilterString();
                                }
                            }
                        }

                        Keys.onReturnPressed: {
                            addFilter( text );
                            text = "";
                            inputRow.termEditing = -1
                            currentFilterText = ""
                        }

                        Keys.onEnterPressed: {
                            addFilter( text );
                            text = "";
                            inputRow.termEditing = -1
                            currentFilterText = ""
                        }

                        Keys.onTabPressed: {
                            if(text != "")
                            {
                                addFilter( text );
                                text = "";
                                inputRow.termEditing = -1
                                currentFilterText = ""
                            }
                            else
                            {
                                event.accepted = false
                            }
                        }

                        Keys.onPressed: {
                            //Delete last tag if no text is entered and backspace/delete is not held down
                            if (event.key == Qt.Key_Backspace || event.key == Qt.Key_Delete && inputRow.termEditing == filterCount)
                            {
                                if(filterText.text == "" && filterCount > 0 && !event.isAutoRepeat)
                                {
                                    filterList.remove(filterCount - 1, 1);
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
                        visible: filterText.text != "" || filterRepeater.model.count != 0

                        tooltip: "Clear All Filters"

                        onClicked: {
                            clearFilters();
                            updateFilterString();
                            filterText.text = ""
                        }
                    }
                }
            } // Flow
        } // textFrame
    } // inputRow

    function loadFilterFromPath(path, appendToFilterList) {
        var filterSerialised = readStringFromFile(path);
        var filterVal        = deserialiseFilterString(filterSerialised);

        // Add loaded filter tokens for display and filtering
        if (!appendToFilterList) {
            clearFilters();
            filterList.append(filterVal);
        } else {
            for (var i = 0; i < filterVal.length; i += 1) {
                var filter = filterVal[i];
                if (!filterExists(filter.value)) filterList.append(filter);
            }
        }
    }

    function updateDefaultFiltersListing() {
        defaultFiltersList.clear();
        var folder = WGPath.toDisplayPath(rootFrame.defaultFiltersFolder);
        var files = WGPath.getFilesInFolder(folder, rootFrame.defaultNameFilter);
        for (var i = 0; i < files.length; ++i) {
            defaultFiltersList.append({"path": folder + "/" + files[i]});
        }
    }

    function updateRecentFiltersListing(newFilterPath) {
        var index      = getFilterIndexInRecentsList(newFilterPath);
        if (index == 0) {
            // Already most recent entry, don't modify list
        } else {
            if (index != -1) recentFiltersList.remove(index, 1)

            var entry = {"path": newFilterPath};
            recentFiltersList.append(entry);

            var maxFilters = 5;
            if (recentFiltersList.count > maxFilters) {
                recentFiltersList.remove(maxFilters, recentFiltersList.count - maxFilters);
            }
        }
    }

    FileDialog {
        id: openOrSaveFilterDialog
        folder: WGPath.pathToUrl(rootFrame.defaultFiltersFolder)
        nameFilters: rootFrame.nameFilters
        selectedNameFilter: rootFrame.defaultNameFilter

        property bool appendToFilterList: false

        onAccepted: {
            var absPath = WGPath.urlToPath(fileUrls);
            if (selectExisting) {
                loadFilterFromPath(absPath, appendToFilterList);
            } else {
                var filterSerialised = serialiseCurrentFilter();
                writeStringToFile(filterSerialised, absPath);
                updateDefaultFiltersListing();
            }

            updateRecentFiltersListing(absPath);
            folder = WGPath.pathToUrl(WGPath.getParentPath(absPath))
        }
    }

    MessageDialog {
        id: clearFiltersPromptDialog
        objectName: "clearFilters_dialog"
        title: "Clear Recent Filters?"
        icon: StandardIcon.Question
        text: "Are you sure you want to clear all recent filters?"
        standardButtons: StandardButton.Yes | StandardButton.No
        modality: Qt.ApplicationModal
        visible: false

        property int overwriteIndex: -1

        onYes: {
            recentFiltersList.clear();
        }
    }

    property alias placeHolderText: filterText.placeholderText
} // rootFrame
