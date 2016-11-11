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
    property var filterCount: filterList.count

    ListModel {
        id: savedFiltersList
    }
    property var currentFilter: ""

    property bool editableTags: true
    property var currentFilterText: ""

	property var autoCompleteData: []
	property var autoCompleteModel: null

    //------------------------------------------
    // Functions
    //------------------------------------------

    function filterExists(filterText) {
        filterText = filterText.trim();
        for (var i = 0; i < filterList.count; ++i) {
            var filter = filterList.get(i);
            if (filter.value == filterText) {
                return true;
            }
        }
        return false;
    }

    function addFilter(filterText) {
        if (filterExists(filterText)) {
            return;
        }

        filterText = filterText.trim();
        if (filterText == "") {
            return;
        }
        filterList.append({"display": filterText, "value": filterText, "active": true});
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
        filterString = tmp;
    }

    function saveFilter() {
        if (currentFilter == "") {
            currentFilter = "Filter" + (savedFiltersList.count + 1);
            saveCurrentFilter();
            return;
        }

        // Prompt the user!
        overwritePromptDialog.open()
    }

    function saveCurrentFilter() {
        var currentFilterValue = ""
        for (var i = 0; i < filterList.count; ++i) {
            var filter = filterList.get(i);
            currentFilterValue += filter.display + ";";
            currentFilterValue += filter.value + ";";
            currentFilterValue += filter.active + ";";
        }

        for (var i = 0; i < savedFiltersList.count; ++i) {
            var savedFilter = savedFiltersList.get(i)
            if (savedFilter.display == currentFilter) {
                savedFilter.value = currentFilterValue;
                return;
            }
        }

        savedFiltersList.append({"display": currentFilter, "value": currentFilterValue});
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
                        saveFilter();
                    }
                }

                MenuItem {
                    text: "Clear Saved Filters"
                    onTriggered: {
                        savedFiltersList.clear();
                        currentFilter = "";
                    }
                }

                MenuSeparator { }

                WGMenu {
                    id: savedFiltersMenu
                    title: "Saved Filters"

                    Instantiator {
                        model: savedFiltersList

                        delegate: MenuItem {
                            text: display
                            onTriggered: {
                                filterList.clear();
                                var tokens = value.split(";");
                                for (var i = 0; i < tokens.length - 1; i += 3) {
                                    filterList.append({"display": tokens[i], "value": tokens[i + 1], "active": tokens[i + 2] != 'false'});
                                }
                                currentFilter = display;
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
                        height: defaultSpacing.minimumRowHeight
                        anchors.fill: parent
                        anchors.rightMargin: clearCurrentFilterButton.width
						autoCompleteData: rootFrame.autoCompleteData
						autoCompleteModel: rootFrame.autoCompleteModel

                        style: WGInvisTextBoxStyle{}

                        placeholderText: "Filter"

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
                            }
                            else
                            {
                                inputRow.termEditing = -1
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

                        tooltip: "Clear All Filters"

                        onClicked: {
                            filterList.clear();
                            currentFilter = "";
                            updateFilterString();
                        }
                    }
                }
            } // Flow
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
            saveCurrentFilter();
        }

        onNo: {
            currentFilter = "Filter" + (savedFiltersList.count + 1);
            saveCurrentFilter();
        }

    }
} // rootFrame
