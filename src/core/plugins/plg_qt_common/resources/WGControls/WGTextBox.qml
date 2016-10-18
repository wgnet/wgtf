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

TextField {
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

    /*! This property determines if the context menu for this control contains the "Find In AssetBrowser" option
        The default value is \c false
    */
    property bool assetBrowserContextMenu : false

    /*! This property determines if the text is selected when the user presses Enter or Return.
        The default value is \c true
    */
    property bool selectTextOnAccepted: true

    property bool multipleValues: false

    /*! This alias holds the width of the text entered into the textbox.
      */
    property alias contentWidth: contentLengthHelper.contentWidth

    /*! This property is used by the setValueHelper function which requires documenting */
    property string oldText

    /*! This signal is emitted when text field loses focus and text changes is accepted */
    signal editAccepted();

    /*! This signal is emitted when text field editing is cancelled */
    signal editCancelled();

    Keys.onPressed: {
        if (activeFocus)
        {
            if(event.key == Qt.Key_Enter || event.key == Qt.Key_Return)
            {
                textBox.focus = true;
                if (selectTextOnAccepted) {
                    selectAll();
                }
                editAccepted()
            }
            else if (event.key == Qt.Key_Escape)
            {
                setValueHelper( textBox, "text", oldText );
                editCancelled();
                textBox.focus = true;
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
            deselect()
            if (acceptableInput && (text !== oldText))
            {
                editAccepted();
            }
        }
    }

    activeFocusOnTab: readOnly ? false : true

    verticalAlignment: TextInput.AlignVCenter

    // provide default heights
    implicitHeight: defaultSpacing.minimumRowHeight
    implicitWidth: contentLengthHelper.contentWidth + defaultSpacing.doubleMargin

    /*! This property denotes if the control's text should be scaled appropriately as it is resized */
    smooth: true

    //Placeholder text in italics
    font.italic: text == "" ? true : false

    onEditAccepted: {
        contentLengthHelper.text = textBox.text + "MM"
    }

    Text {
        id: contentLengthHelper
        visible: false
        Component.onCompleted: {
            contentLengthHelper.text = textBox.text + "MM"
        }
    }

    style: WGTextBoxStyle {
    }

    // support copy&paste
    WGCopyable {
        id: copyableControl

        WGCopyController {
            id: copyableObject

            onDataCopied : {
                setValue( textBox.text )
            }

            onDataPasted : {
                textBox.text = data
            }
        }

        onSelectedChanged : {
            if(selected)
            {
                selectControl( copyableObject )
            }
            else
            {
                deselectControl( copyableObject )
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
                var highlightedText = selectedText
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

        MenuItem {
            objectName: "FindInAssetBrowser"
            text: "Find In AssetBrowser"
            shortcut: "Ctrl+?"
            visible: assetBrowserContextMenu == true ? true : false
            onTriggered: {
                console.log("Not yet implemented")
            }
        }
    }

    /*! Deprecated */
    property alias label_: textBox.label

}
