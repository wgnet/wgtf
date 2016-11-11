import QtQuick 2.5
import QtQuick.Controls 1.4
import Qt.labs.controls 1.0 as Labs

import WGControls 2.0
import WGControls.Views 2.0

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
    WGComponent { type: "WGTextBoxAutoComplete20" }

    /*!
        An array of strings to use as auto-complete values.
    */
    property var autoCompleteData: []
	property var autoCompleteModel: null
	property bool showSuggestions: true

	onAutoCompleteDataChanged: {
		if (autoCompleteData.length > 0) {
			internal.autoCompleteModel.clear();
			for (var i = 0; i < autoCompleteData.length; ++i) {
				internal.autoCompleteModel.append({"display":autoCompleteData[i]});
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

		// Finds first entry and shortest entry.
		function tryComplete (inputText) {
			var candidate = "";
			var shortestString = "";
			for (var i = 0 ; i < suggestionData.rowCount(); ++i) {
				var index = suggestionData.index(i, 0);
				var currentString = suggestionData.data(index);

				if (currentString.toLowerCase().indexOf(text.toLowerCase()) === 0) {
					if (candidate.length) { // Find smallest possible match
						var cmp = 0;

						// We try to complete the shortest string that matches our search
						if (currentString.length < candidate.length)
							candidate = currentString

						while (cmp < Math.min(currentString.length, shortestString.length)
							   && shortestString[cmp].toLowerCase() === currentString[cmp].toLowerCase())
							cmp++;
						shortestString = shortestString.substring(0, cmp);
					} else { // First match, select as current index and find other matches
						candidate = currentString;
						shortestString = currentString;
					}
				}
			}

			if (candidate.length)
				return text + candidate.substring(text.length, candidate.length);
			return text;
		}
	}

    Keys.onPressed: {
		internal.allowComplete = (event.key !== Qt.Key_Backspace && event.key !== Qt.Key_Delete);
		internal.allowSuggestions = (event.key !== Qt.Key_Escape);
		
		if (event.key === Qt.Key_Backspace) {
			if (textBox.selectionStart < textBox.text.length && textBox.selectionEnd == textBox.text.length && 
				textBox.text.substring(0, textBox.selectionStart) == internal.userText) {
				internal.userModification = false;
				textBox.text = internal.userText;
			}
		}
		else if (event.key === Qt.Key_Left) {
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
			textBox.text = row < 0 ? internal.userText : suggestionList.view.extendedModel.item(row, 0, null).display;
			textBox.select(textBox.text.length, textBox.text.length);
		}
		else if (event.key === Qt.Key_Up) {
			var row = suggestionList.currentRow - 1;
			if (row < -1) {
				row = suggestionList.count - 1;
			}
			suggestionList.currentRow = row;
			internal.userModification = false;
			textBox.text = row < 0 ? internal.userText : suggestionList.view.extendedModel.item(row, 0, null).display;
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

		internal.userText = text;
        if (internal.allowComplete && internal.userText.length > 0) {
            var completed = internal.tryComplete(internal.userText)
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

	Labs.Popup
    {
		id: suggestionPopup
		y: parent.height
        width: parent.width
		height: Math.min(200, suggestionList.contentHeight)
		implicitWidth: 0
		implicitHeight: 0

        visible: showSuggestions && internal.allowSuggestions && internal.userText.length > 0
		closePolicy: Labs.Popup.OnPressOutsideParent

		WGSortFilterProxy {
			id: suggestionModel
			sourceModel: internal.suggestionData

			filterObject: QtObject {
				property var filterText: internal.userText.toLowerCase()

				onFilterTextChanged: {
					suggestionModel.invalidateFilter();
				}

				function filterAcceptsRow(item) {
					var text = item.display.toLowerCase();
					if (text.length == filterText.length) {
						return false;
					}

					if (text.indexOf(filterText) == -1) {
						return false;
					}

					return true;
				}
			}
		}

        contentItem: WGScrollView {
            anchors.fill: parent
            clip: true
			horizontalScrollBarPolicy: Qt.ScrollBarAlwaysOff

            WGListView {
				id: suggestionList
                model: suggestionModel

				columnDelegate: Text {
					text: itemValue !== undefined ? itemValue : ""
					color: palette.textColor
					verticalAlignment: Text.AlignVCenter
					height: defaultSpacing.minimumRowHeight
					x: 4
				}

				onItemPressed: {
					internal.userText = view.extendedModel.item(view.getRow(rowIndex), 0, null).display;
					internal.userModification = false;
					textBox.text = internal.userText;
					textBox.select(textBox.text.length, textBox.text.length);
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
