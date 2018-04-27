import QtQuick 2.5
import QtQuick.Layouts 1.3
import QtQuick.Controls 1.4
import WGControls.Layouts 2.0
import WGControls.Global 2.0
import WGControls.Styles 1.0
import WGControls 2.0

/*!
 \ingroup wgcontrols
 \todo brief
*/
FocusScope {
    id: fileComponent
    objectName: "WGFileSelectBox"
    WGComponent { type: "WGFileSelectBox20" }

    /*! Determines whether text in the url is readOnly and can only be copied.
      Setting readOnly to true also allows the dialog box to be opened on double click.

      The default value is true.
    */
    property alias readOnly: textField.readOnly

    /*! The text in the filename box.

      The default value points to fileUrl
    */
    property alias text: textField.text

    /*! property indicates if the control represetnts multiple data values */
    property bool multipleValues: false

    /*! the property containing the string to be displayed when multiple values are represented by the control
    */
    property string __multipleValuesString: "Multiple Values"

    property var model: null

    /*! The folder the fileDialog will open to by default.
      The default value is empty
    */
    property url folder

    /*! The path for customizing the context menu
    */
    property var fileSelectBoxContextMenuPath: "FileSelectBoxMenu"

    /*! The path of the file which was selected by the user.

        Note: This property is set only if exactly one file was selected. In all other cases, it will be empty.
    */
    property url fileUrl

    /*! The list of file paths which were selected by the user.
    */
    property var fileUrls

    /*! Whether the list of file path(s) which were selected by the user is considered valid
    */
    property var isValidUrl: true

    /*! The height of the dialog box displayed.

      The default value is 600
    */
    property int dialogHeight: 600

    /*! The width of the dialog box displayed.

      The default value is 800
    */
    property int dialogWidth: 800

    readonly property bool isCompact: fileComponent.width/3 < defaultSpacing.minimumRowHeight*4

    /*! Whether the dialog should be shown modal with respect to the window containing the dialog's parent Item,
        modal with respect to the whole application, or non-modal.
        The default value is Qt.ApplicationModal
    */
    property var modality: Qt.ApplicationModal

    /*! This array contains a list of filename Filters in the format:
      \code{.js}
      description (*.extension1 *.extension2 ...)
      \endcode
      All files can be selected using (*)
      Default value includes the following filters:

      \code{.js}
      "All files (*)",
      "Image files (*.jpg *.png *.bmp *.dds)",
      "Model files (*.model *.primitives *.visual)",
      "Script files (*.txt *.xml *.py)",
      "Audio files (*.fsb *.fev)",
      \endcode
    */
    property var nameFilters: {
        [
            "All files (*)",
            "Image files (*.jpg *.png *.bmp *.dds)",
            "Model files (*.model *.primitives *.visual)",
            "Script files (*.txt *.xml *.py)",
            "Audio files (*.fsb *.fev)",
        ]
    }

    /*! This currently selected filename filter
      The default value is All Files (*)
    */
    property string selectedNameFilter: nameFilters[0]

    property string title: "Select a File"

    /*! internal */
    property var __dialogInstance: null

    // Signals
    signal fileChosen(var selectedFile)
    signal fileRejected()

    implicitHeight: defaultSpacing.minimumRowHeight
    implicitWidth: textField.implicitWidth + clearButton.implicitWidth +
                   findInAssetBrowserButton.implicitWidth + openAssetButton.implicitWidth + openButton.implicitWidth

    /*! This function opens the desired dialog box.
    */
    function openDialog() {
        if(fileComponent.readOnly) {
            return;
        }

        if (fileComponent.model != null) {
            __dialogInstance = WGDialogs.assetBrowserDialog
            __dialogInstance.model = fileComponent.model
        }
        else {
            __dialogInstance = WGDialogs.nativeFileDialog
        }
        __dialogInstance.modality = fileComponent.modality
        __dialogInstance.nameFilters = fileComponent.nameFilters
        __dialogInstance.selectedNameFilter = fileComponent.selectedNameFilter
        __dialogInstance.title = fileComponent.title

        __dialogInstance.open(dialogWidth, dialogHeight, textField.text)
    }

    /*! This function closes the desired dialog box depending on whether useAssetBrowser == true or not.
    */
    function closeDialog() {
        __dialogInstance.close()
    }

    Connections {
        target: __dialogInstance
        ignoreUnknownSignals: true

        onAccepted: {
            fileChosen(selectedValue)
        }

        onRejected: {
            fileRejected()
        }

        onClosed: {
            __dialogInstance.model = null
            __dialogInstance = null
        }
    }

    RowLayout {
        anchors.fill: parent
        spacing: 0

        WGFileUrl {
            id: textField
            objectName: "textField"
            Layout.fillWidth: true
            Layout.preferredHeight: defaultSpacing.minimumRowHeight

            textColor: fileComponent.isValidUrl ? palette.textColor : palette.highlightTextColor
            style: WGTextBoxStyle {
                frameHighlightColor: fileComponent.isValidUrl ? "transparent" : palette.warningHighlight
            }

            text: fileComponent.multipleValues ? fileComponent.__multipleValuesString : fileUrl
            readOnly: !fileComponent.readOnly

            focus: true

            onEditAccepted: fileChosen(text)

            DropArea {
                anchors.fill:parent
                keys: ["text/uri-list"]
                enabled: !fileComponent.readOnly
                onDropped: {
                    if (drop.hasUrls)
                    {
                        var fileName = drop.urls[0].toString();
                        var fileExtensionIndex = fileName.lastIndexOf('.') + 1;

                        if ( fileExtensionIndex !== -1 )
                        {
                            var fileType = fileName.substring(fileExtensionIndex);

                            for ( var i in nameFilters )
                            {
                                var filter = nameFilters[i];

                                if ( filter.indexOf('(') !== -1 )
                                {
                                    filter = filter.substring(filter.indexOf('(') + 1, filter.indexOf(')'));
                                }

                                if ( filter.indexOf( "*." + fileType ) !== -1 || filter === '*' )
                                {
                                    fileChosen(fileName);
                                    drop.accept();
                                    return;
                                }
                            }
                        }
                    }
                }
            }

            MouseArea {
                //mousearea to allow double clicking to change the file if the text is readonly
                anchors.fill: parent
                propagateComposedEvents: true
                hoverEnabled: true

                enabled: textField.readOnly

                cursorShape: Qt.IBeamCursor

                //Difficult to make discrete text selection and open via double-click work at the same time without rewriting TextField.
                //This is a compromise so that the filename can still be copied to clipboard

                onClicked: {
                    textField.selectAll()
                    textField.forceActiveFocus()
                }

                onDoubleClicked: {
                    openDialog()
                }
            }
        }

        Item
        {
            id: spacer
            width: 3
        }

        WGPushButton {
            id: clearButton
            objectName: "clearButton"
            Layout.preferredHeight: defaultSpacing.minimumRowHeight
            Layout.preferredWidth: defaultSpacing.minimumRowHeight
            iconSource: "icons/close_sml_16x16.png"
            visible: !textField.readOnly && !isCompact
            enabled: visible && textField.text.length
            tooltip: "Clear"
            onClicked:{
                textField.selectAll();
                textField.remove(0, textField.text.length)
                textField.editAccepted()
            }
            // Menu item to be added to the context menu when the component finishes loading
            MenuItem
            {
                id: clearAction
                text: clearButton.tooltip
                enabled: clearButton.enabled
                onTriggered: clearButton.clicked()
            }
        }

        // This button is hidden by default and is enabled if an action matching this button's action ID is enabled
        WGPushButton {
            id: findInAssetBrowserButton
            objectName: "findInAssetBrowserButton"
            visible: action && !isCompact
            enabled: textField.text.length && fileComponent.isValidUrl
            tooltip: action ? action.text : ""
            // Sadly there is no easy way to use the icon of the associated action
            iconSource: "icons/search_folder_16x16.png"
            Layout.preferredHeight: defaultSpacing.minimumRowHeight
            Layout.preferredWidth: defaultSpacing.minimumRowHeight
            property var action

            onClicked: {
                textField.focus = true
                action.triggered()
            }
        }

        // This button is hidden by default and is enabled if an action matching this button's action ID is enabled
        WGPushButton {
            id: openAssetButton
            objectName: "openAssetButton"
            visible: action && !isCompact
            enabled: textField.text.length && fileComponent.isValidUrl
            tooltip: action ? action.text : ""
            // Sadly there is no easy way to use the icon of the associated action
            iconSource: "icons/folder_open_16x16.png"
            Layout.preferredHeight: defaultSpacing.minimumRowHeight
            Layout.preferredWidth: defaultSpacing.minimumRowHeight
            property var action

            onClicked: {
                textField.focus = true
                action.triggered()
            }
        }

        WGPushButton {
            id: openButton
            objectName: "openButton"
            visible: !fileComponent.readOnly
            enabled: !fileComponent.readOnly
            iconSource: "icons/open_16x16.png"
            tooltip: "Browse..."
            Layout.preferredHeight: defaultSpacing.minimumRowHeight
            Layout.preferredWidth: defaultSpacing.minimumRowHeight

            onClicked: {
                openDialog()
            }
        }

        Component.onCompleted:{
            textField.contextMenu.path = fileSelectBoxContextMenuPath
            textField.contextMenu.insertItem(0, clearAction)

            findInAssetBrowserButton.action = textField.contextMenu.find("FileSelectFindInBrowser");
            openAssetButton.action = textField.contextMenu.find("FileSelectOpenAsset")
        }

        Connections{
            target: textField
            onActiveFocusChanged: {
                if(textField.activeFocus)
                {
                    textField.contextMenu.contextObject = Qt.binding(function() { return textField.text; })
                }
            }
        }
    }
}

