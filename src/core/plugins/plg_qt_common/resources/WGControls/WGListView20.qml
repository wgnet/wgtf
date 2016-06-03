import QtQuick 2.4
import QtQuick.Controls 1.2
import QtQml.Models 2.2
import WGControls 2.0
import "wg_view_selection.js" as WGViewSelection


/*!
 \brief WGListView displays data from a model defined by its delegate.
 The WGListView is contructed from a WGListViewBase which creates rows and columns.

Example:
\code{.js}

ScrollView {
    anchors.top: lastControl.bottom
    anchors.left: parent.left
    anchors.right: parent.right
    anchors.bottom: parent.bottom

    WGListView {
        id: example
        model: sourceModel
        columnWidth: 50
        columnSpacing: 1
        headerDelegates: [exampleHeaderDelegate]
        footerDelegates: [exampleFooterDelegate]
        headerDelegate: exampleHeaderDelegate
        footerDelegate: exampleFooterDelegate
        columnDelegates: [columnDelegate, exampleDelegate]
        roles: ["value", "headerText", "footerText"]
        model: sourceModel

        Component {
            id: exampleHeaderDelegate

            Text {
                id: textBoxHeader
                color: palette.textColor
                text: headerData.headerText
                height: 24
            }
        }

        Component {
            id: exampleFooterDelegate

            Text {
                id: textBoxFooter
                color: palette.textColor
                text: headerData.footerText
                height: 24
            }
        }

        Component {
            id: exampleDelegate

            Text {
                id: textItem

                visible: typeof itemData.value === "string"
                text: typeof itemData.value === "string" ? itemData.value : ""
                color: palette.textColor
            }
        }
    }
}

\endcode
*/
WGListViewBase {
    id: listView

    property alias roles: itemView.roles

    /*! The default component to be used for columns that are not specified
        by columnDelegates.
    */
    property alias columnDelegate: itemView.columnDelegate

    /*! A list of components to be used for each column.
        Item 0 for column 0, item 1 for column 1 etc.
        If a column is not in the list, then it will default to columnDelegate.
        The default value is an empty list.
    */
    property alias columnDelegates: itemView.columnDelegates

    /*! This property holds a list of indexes to adapt from the model's columns
        to the view's columns.
        e.g. if the input model has 1 column, but columnSequence is [0,0,0]
             then the view can have 3 columns that lookup column 0 in the model.
        The default value is an empty list
    */
    property alias columnSequence: itemView.columnSequence
    property alias columnWidth: itemView.columnWidth
    property alias columnWidths: itemView.columnWidths
    property alias columnSpacing: itemView.columnSpacing

    property alias internalModel: listView.model

    /*! This property holds the data model information that will be displayed
        in the view.
    */
    property alias model: itemView.model

    /*! A list of components to be used for each header/footer column.
        Item 0 for column 0, item 1 for column 1 etc.
        If a column is not in the list, then it will default to headerDelegate/footerDelegate.
        The default value is an empty list.
    */
    property alias headerDelegates: itemView.headerDelegates
    property alias footerDelegates: itemView.footerDelegates
    /*! The default component to be used for header/footer columns that are not specified
        by headerDelegates/footerDelegates.
    */
    property alias headerDelegate: itemView.headerDelegate
    property alias footerDelegate: itemView.footerDelegate

    /*! This component is used for showing a sort indicator 
        on header column.
    */
    property alias sortIndicator: itemView.sortIndicator

    property var extensions: []


    /*! Move the keyboard highlight up/left.
     */
    function moveKeyHighlightPrevious(event) {
        var newIndex = listExtension.getPreviousIndex(itemView.selectionModel.currentIndex);
        WGViewSelection.updateKeyboardSelection(event, newIndex, itemView, listExtension);
    }
    
    /*! Move the keyboard highlight down/right.
     */
    function moveKeyHighlightNext(event) {
        var newIndex = listExtension.getNextIndex(itemView.selectionModel.currentIndex);
        WGViewSelection.updateKeyboardSelection(event, newIndex, itemView, listExtension);
    }

    clip: true
    view: itemView
    internalModel: itemView.extendedModel
    keyboardHighlightModelIndex: itemView.currentIndex

    Keys.onUpPressed: {
        if (orientation == ListView.Vertical) {
            moveKeyHighlightPrevious(event);
        }
    }
    Keys.onDownPressed: {
        if (orientation == ListView.Vertical) {
            moveKeyHighlightNext(event);
        }
    }
    Keys.onLeftPressed: {
        if (orientation == ListView.Horizontal) {
            moveKeyHighlightPrevious(event);
        }
    }
    Keys.onRightPressed: {
        if (orientation == ListView.Horizontal) {
            moveKeyHighlightNext(event);
        }
    }

    // Data holder for various C++ extensions.
    // Pass it down to children
    WGItemViewCommon {
        id: itemView

        // WGControls.ListExtension C++
        ListExtension {
            id: listExtension
        }

        property var listExtensions: listView.extensions.concat(commonExtensions.concat([listExtension]))
        extensions: listExtensions

        Connections {
            target: listView
            onItemPressed: WGViewSelection.itemPressed(mouse, itemView, listExtension, rowIndex)
        }
    }
}

