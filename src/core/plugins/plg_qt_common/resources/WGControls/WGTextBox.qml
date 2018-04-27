import QtQuick 2.5
import QtQuick.Controls 1.4
import WGControls 1.0
import WGControls.Styles 1.0

/*!
 \ingroup wgcontrols
 \brief Text entry field mostly intended for strings

Example:
\code{.js}
WGTextBox {
    placeholderText: "Placeholder Text"
    Layout.fillWidth: true
}
\endcode
*/

FocusScope {
    id: textBox
    objectName: "WGTextBox"
    WGComponent { type: "WGTextBox" }

    /*! This property is used to define the buttons label when used in a WGFormLayout
        The default value is an empty string
    */
    property string label: ""

    /*! This property determines if the textBox has a context menu
        The default value is \c true
    */
    property bool useContextMenu : true

    /*! This property determines if the text is selected when the user presses Enter or Return.
        The default value is \c true
    */
    property bool selectTextOnAccepted: false

    property bool multipleValues: false

    property string multipleValuesDisplayString: "Multiple Values"

    /*! This alias holds the width of the text entered into the textbox.
      */
    property alias contentWidth: contentLengthHelper.contentWidth

    /*! This property is used by the setValueHelper function which requires documenting */
    property string oldText

    /*! This signal is emitted when text field loses focus and text changes is accepted */
    signal editAccepted();

    /*! This signal is emitted when text field editing is cancelled */
    signal editCancelled();

    /*! This function recalculates the width of the text box based on its contents */
    function recalculateWidth(){
        contentLengthHelper.text = textBox.text + "MM"
    }

    property bool editInProgress: textField.focus && !focusHolder.focus

    property bool passActiveFocus: true

    // provide default heights
    implicitHeight: defaultSpacing.minimumRowHeight
    implicitWidth: contentLengthHelper.contentWidth + defaultSpacing.doubleMargin

    readonly property alias acceptableInput: textField.acceptableInput
    property alias activeFocusOnPress : textField.activeFocusOnPress
    readonly property alias canPaste: textField.canPaste
    readonly property alias canUndo: textField.canUndo
    readonly property alias canRedo: textField.canRedo
    property alias cursorPosition: textField.cursorPosition
    readonly property alias cursorRectangle : textField.cursorRectangle
    readonly property alias displayText: textField.displayText
    property alias echoMode: textField.echoMode
    readonly property alias effectiveHorizontalAlignment: textField.effectiveHorizontalAlignment
    property alias font: textField.font
    property alias horizontalAlignment: textField.horizontalAlignment
    readonly property alias hovered: mouseAreaContextMenu.containsMouse
    property alias inputMask : textField.inputMask
    readonly property alias inputMethodComposing : textField.inputMethodComposing
    property alias inputMethodHints: textField.inputMethodHints
    readonly property alias length: textField.length
    property alias maximumLength: textField.maximumLength
    property alias menu: textField.menu
    property alias contextMenu: contextMenu
    property alias placeholderText: textField.placeholderText
    property alias readOnly: textField.readOnly
    property alias selectByMouse : textField.selectByMouse
    readonly property alias selectedText: textField.selectedText
    readonly property alias selectionEnd: textField.selectionEnd
    readonly property alias selectionStart: textField.selectionStart
    property alias style: textField.style
    property alias text: textField.text
    property alias textColor: textField.textColor
    property alias validator: textField.validator
    property alias verticalAlignment: textField.verticalAlignment
    property alias focusHolder: focusHolder

    signal editingFinished()
    signal accepted()

    function copy() { textField.copy() }
    function cut() { textField.cut() }
    function deselect() { textField.deselect() }
    function getText(start, end) { return textField.getText(start, end) }
    function insert(pos, text) { textField.insert(pos, text) }
    function isRightToLeft(start, end) { return textField.isRightToLeft(start, end) }
    function paste() { textField.paste() }
    function redo() { textField.redo() }
    function remove(start, end) { textField.remove(start, end) }
    function select(start, end) { textField.select(start, end) }
    function selectAll() { textField.selectAll() }
    function selectWord() { textField.selectWord() }
    function undo() { textField.undo() }

    onEditAccepted: {
        recalculateWidth()
    }

    onEditCancelled: {
        recalculateWidth()
    }

    function stopEditing() {
        editCancelled();
        passActiveFocus = false
        focusHolder.focus = true
    }

    Text {
        id: contentLengthHelper
        visible: false
        Component.onCompleted: {
            recalculateWidth()
        }
    }

    Item {
        id: focusHolder
        focus: true
        activeFocusOnTab: focus

        Keys.forwardTo: textBox

        Keys.onPressed: {
            if(event.key == Qt.Key_Enter || event.key == Qt.Key_Return)
            {
                textField.focus = true
            }
        }

        onActiveFocusChanged: {
            if (activeFocus)
            {
                //if the user tabs to this it should be editable, but not if they've just interacted with it
                if (passActiveFocus)
                {
                    textField.focus = true
                }
            }
            else
            {
                passActiveFocus = true
            }
        }
    }

    TextField {
        anchors.fill: parent
        id: textField

        focus: false

        verticalAlignment: TextInput.AlignVCenter

        /*! This property denotes if the control's text should be scaled appropriately as it is resized */
        smooth: parent.smooth

        //Placeholder text in italics
        font.italic: text == "" ? true : false

        property bool editFocus: textBox.activeFocus
        activeFocusOnTab: focus

        style: WGTextBoxStyle {
        }

        onEditingFinished: {
            textBox.editingFinished()
        }
        onAccepted: {
            textBox.editAccepted()
        }

        Keys.forwardTo: focusHolder

        Keys.onPressed: {
            if (activeFocus)
            {
                if(event.key == Qt.Key_Enter || event.key == Qt.Key_Return)
                {
                    editAccepted()
                    if (selectTextOnAccepted) {
                        selectAll();
                    }
                    else
                    {
                        passActiveFocus = false
                        focusHolder.focus = true
                    }
                    event.accepted = true
                }
                else if (event.key == Qt.Key_Escape)
                {
                    setValueHelper( textBox, "text", oldText );
                    stopEditing();
                    event.accepted = true
                }
                else if (event.modifiers & Qt.ControlModifier)
                {
                    if(event.key == Qt.Key_Z || event.key == Qt.Key_Y)
                    {
                        setValueHelper( textBox, "text", oldText );
                        stopEditing();
                        event.accepted = true
                    }
                }
            }
        }

        onActiveFocusChanged: {
            if (activeFocus)
            {
                selectAll()
                setValueHelper( textBox, "oldText", text );
            }
            else
            {
                if (text != oldText)
                {
                    editAccepted()
                }
                deselect()
            }
        }
    }

    MouseArea {
        id: mouseAreaContextMenu
        acceptedButtons: Qt.RightButton

        propagateComposedEvents: true
        preventStealing: false
        anchors.fill: parent
        hoverEnabled: true

        cursorShape: Qt.IBeamCursor
        onClicked:{
            if (textBox.useContextMenu)
            {
                textBox.forceActiveFocus()
                contextMenu.popup()
            }
        }
    }

    // Some context menu items may be data driven.
    // I have added a visibility switch to contextMenu
    WGMenu {
        id: contextMenu
        objectName: "Menu"
        title: "Edit"

        MenuItem {
            objectName: "Cut"
            text: "Cut"
            shortcut: "Ctrl+X"
            enabled: readOnly == true ? false : true
            onTriggered: {
                cut()
            }
        }

        MenuItem {
            objectName: "Copy"
            text: "Copy"
            shortcut: "Ctrl+C"
            onTriggered: {
                copy()
            }
        }

        MenuItem {
            objectName: "Paste"
            text: "Paste"
            shortcut: "Ctrl+V"
            enabled: canPaste == true ? true : false
            onTriggered: {
                paste()
            }
        }

        MenuSeparator { }

        MenuItem {
            objectName: "SelectAll"
            text: "Select All"
            shortcut: "Ctrl+A"
            onTriggered: {
                selectAll()
            }
        }

        MenuSeparator { }
    }

    /*! Deprecated */
    property alias label_: textBox.label

    /*! Deprecated */
    property bool assetBrowserContextMenu : false
}
