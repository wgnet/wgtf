import QtQuick 2.5
import QtQuick.Controls 1.4

import WGControls 2.0

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
        Determines whether autoComplete is used at all.

        The default is true.
    */
    property bool useAutoComplete: true

    /*!
        Determines whether to search for values to auto-complete.

        False when the delete or backspace key is pressed.
        True when any other key is pressed.
    */
    property bool allowComplete: false

    /*!
        An array of strings to use as auto-complete values.
    */
    property var autoCompleteData: []

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


    // Finds first entry and shortest entry.
    function tryComplete (inputText) {
        var candidate = "";
        var shortestString = "";
        for (var i = 0 ; i < autoCompleteData.length ; ++i) {
            var currentString = autoCompleteData[i];

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

    Keys.onPressed: allowComplete = (event.key !== Qt.Key_Backspace && event.key !== Qt.Key_Delete);

    onTextChanged: {
        if (allowComplete && text.length > 0 && useAutoComplete) {
            var completed = textBox.tryComplete(text)
            if (completed.length > text.length) {
                var oldtext = textBox.text;
                textBox.text = completed;
                textBox.select(text.length, oldtext.length);
            }
        }
    }
}
