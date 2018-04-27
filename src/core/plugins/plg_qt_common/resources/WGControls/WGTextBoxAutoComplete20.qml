import QtQuick 2.5
import QtQuick.Controls 1.4
import Qt.labs.controls 1.0 as Labs

import WGControls 2.0
import WGControls.Views 2.0

import "wg_utils.js" as WGUtils

/*!
 \brief WGTextBox with auto-complete function that matches an array of strings.

Example:
\code{.js}
WGTextBoxAutoComplete {
    autoCompleteData: ["list", "of", "auto", "complete", "values"]
}
\endcode
*/

WGTextBox {
    id: textBox
    objectName: "WGTextBoxAutoComplete"
    passActiveFocus: false

    WGComponent { type: "WGTextBoxAutoComplete20" }

    /*!
        An array of strings to use as auto-complete values.
    */
    property var autoCompleteData: []
    property var autoCompleteModel: null
    property var autoCompleteRole: "display"
    property bool showSuggestions: true
    property bool multiText: false

    property var suggestionModel: WGFastFilterProxy {
        sourceModel: internal.suggestionData
        filterRole: autoCompleteRole
    }

    signal suggestionSelected()

    onAutoCompleteDataChanged: {
        if (autoCompleteData.length > 0) {
            internal.autoCompleteModel.clear();
            for (var i = 0; i < autoCompleteData.length; ++i) {
                internal.autoCompleteModel.append({autoCompleteRole:autoCompleteData[i]});
            }
        }
    }

    QtObject {
        id: internal

        property bool allowComplete: false
        property bool allowSuggestions: false

        property var userText: ""
        property bool userModification: true

        property var autoCompleteModel: ListModel {}
        property var suggestionData: textBox.autoCompleteModel != null ? textBox.autoCompleteModel : internal.autoCompleteModel

        property var hasSelectedItem: false
        onUserTextChanged: {
            if(showSuggestions && !hasSelectedItem && userText != "") {
                suggestionModel.filterText = internal.userText;
            }
        }

        function getSuggestion(i){
            var index = suggestionModel.index(i, 0);
            return suggestionModel.data(index, suggestionModel.filterRoleId);
        }

        // Finds the first entry that starts with the input in the remaining suggestions
        function tryComplete (inputText) {
            inputText = suggestionModel.tryComplete(inputText);
            return inputText;
        }
    }

    Keys.onPressed: {
        internal.allowComplete = (event.key !== Qt.Key_Backspace && event.key !== Qt.Key_Delete);
        internal.allowSuggestions = (event.key !== Qt.Key_Escape);

        if (event.key === Qt.Key_Left) {
            if (textBox.selectionStart < textBox.text.length && textBox.selectionEnd == textBox.text.length &&
                textBox.text.substring(0, textBox.selectionStart) == internal.userText) {
                internal.userModification = false;
                textBox.text = internal.userText;
            }
        }
        else if (event.key === Qt.Key_Right) {
            if (textBox.selectionStart < textBox.text.length && textBox.selectionEnd == textBox.text.length &&
                textBox.text.substring(0, textBox.selectionStart) == internal.userText) {
                internal.userText = textBox.text.substr(0, internal.userText.length + 1);
                textBox.select(internal.userText.length, textBox.text.length);
                event.accepted = true;
            }
        }
        else if (event.key === Qt.Key_Down) {
            var row = suggestionList.currentRow + 1;
            if (row == suggestionList.count) {
                row = -1;
            }
            suggestionList.currentRow = row;
            internal.userModification = false;
            textBox.text = row < 0 ? internal.userText : suggestionList.view.extendedModel.item(row, 0, null)[autoCompleteRole];
            textBox.select(textBox.text.length, textBox.text.length);
        }
        else if (event.key === Qt.Key_Up) {
            var row = suggestionList.currentRow - 1;
            if (row < -1) {
                row = suggestionList.count - 1;
            }
            suggestionList.currentRow = row;
            internal.userModification = false;
            textBox.text = row < 0 ? internal.userText : suggestionList.view.extendedModel.item(row, 0, null)[autoCompleteRole];
            textBox.select(textBox.text.length, textBox.text.length);
        }
    }

    onActiveFocusChanged: {
        if (!activeFocus) {
            internal.allowSuggestions = false;
        }
    }

    onTextChanged: {
        if (!internal.userModification) {
            internal.userModification = true;
            return;
        }

        if(internal.userText != text) {
            internal.userText = text;
            if (internal.allowComplete && internal.userText.length > 0) {
                var completed = internal.tryComplete(internal.userText, multiText)
                if (completed.length > internal.userText.length) {
                    internal.userModification = false;
                    textBox.text = completed;
                    textBox.select(internal.userText.length, completed.length);
                }
            }
            if (internal.allowSuggestions) {
                suggestionList.view.selectionModel.clear();
            }
        }
    }

    Labs.Popup
    {
        id: suggestionPopup
        y: parent.height
        width: parent.width
        height: Math.min(200, suggestionList.contentHeight)
        implicitWidth: 0
        implicitHeight: 0

        visible: showSuggestions && internal.allowSuggestions && internal.userText.length > 0 && suggestionModel.rowCount() > 0
        closePolicy: Labs.Popup.OnPressOutsideParent

        contentItem: WGScrollView {
            anchors.fill: parent
            clip: true
            horizontalScrollBarPolicy: Qt.ScrollBarAlwaysOff

            WGListView {
                id: suggestionList
                model: suggestionModel
                columnRoles: [autoCompleteRole]

                columnDelegate: Text {
                    text: itemValue !== undefined ? itemValue : ""
                    color: palette.readonlyTextColor
                    verticalAlignment: Text.AlignVCenter
                    elide: Text.ElideLeft
                    height: defaultSpacing.minimumRowHeight
                    width: parent.width - defaultSpacing.rowSpacing
                    x: defaultSpacing.rowSpacing

                    WGToolTipBase {
                        text: itemValue !== undefined ? itemValue : ""
                        tooltipArea: colMouseArea
                    }
                }

                onItemPressed: {
                    internal.hasSelectedItem = true;
                    internal.userText = view.extendedModel.item(view.getRow(rowIndex), 0, null)[autoCompleteRole];
                    internal.userModification = false;
                    textBox.text = internal.userText;
                    textBox.select(textBox.text.length, textBox.text.length);
                    suggestionSelected()
                    internal.hasSelectedItem = false;
                }
            }
        }

        background: Rectangle {
            color: palette.mainWindowColor
            border.width: defaultSpacing.standardBorderSize
            border.color: palette.darkColor
        }
    }
}
