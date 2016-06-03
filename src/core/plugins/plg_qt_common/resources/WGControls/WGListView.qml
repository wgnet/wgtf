import QtQuick 2.3
import QtQuick.Controls 1.2
import QtQuick.Layouts 1.0
import WGControls 1.0

/*!
 \brief WGListView displays data from a model defined by its delegate.
 The WGListView is contructed of rows of WGListViewRowDelegates which in turn hold WGListViewColumnDelegates.

Example:
\code{.js}

WGListView {
    id: history
    model: historyModel
    anchors.fill: parent
    anchors.margins: defaultSpacing.standardMargin
    selectionExtension: root.historySelectionExtension
    columnDelegates: [columnDelegate]

    Component {
        id: columnDelegate

        Loader {
            source: "WGTimelineEntryDelegate.qml"
        }
    }

    onCurrentIndexChanged: {
        historySelection.data = currentIndex
    }

    onRowDoubleClicked: {
        history.currentIndex = historyModel.indexRow(modelIndex);
    }
}

\endcode
*/

Item {
    id: listView
    objectName: "WGListView"

    /*! This property holds the dataModel information that will be displayed in the tree view
    */
    property var model

    /*! This property determines the gap between the left edge of the entire control and the list contents.
        The default value is \c 2
    */
    property real leftMargin: 2

    /*! This property determines the gap between the right edge of the entire control and the list contents.
        The default value of \c 8 is given to allow for enough room for the vertical scrollbar
        The default value is \c 8
    */
    property real rightMargin: 2

    /*! This property determines the gap between the top edge of the entire control and the list contents.
        The default value is \c 2
    */
    property real topMargin: 2

    /*! This property determines the gap between the bottom edge of the entire control and the list contents.
        The default value is \c 2
    */
    property real bottomMargin: 2

    /*! This property adds vertical spacing under each sibling object in the list.
        This will not add spacing between a child and its parent.
        See the childListMargin: for spacing between parents and children.
        The default value is \c 0
    */
    property real spacing: 0

    /*! This property holds the spacing between column items
        The default value is \c 1
    */
    property real columnSpacing: 2

    /*! This property holds the spacing between rows
        The default value is set by defaultSpacing.minimumRowHeight
    */
    property real minimumRowHeight: defaultSpacing.minimumRowHeight

    property alias currentIndex: list.currentIndex

    //TODO: Document this. Should this be internal?
    /*! This property holds multi select state information */
    property var selectionExtension: null
    property var treeExtension: null

    /*! This property holds a list of items to be passed to the WGListViewRowDelegate
        The default value is an empty list
    */
    property var columnDelegates: []

    /*! This property holds a list of indexes to adapt from the model's columns
        to the view's columns.
        e.g. if the input model has 1 column, but columnSequence is [0,0,0]
             then the view can have 3 columns that lookup column 0 in the model.
        The default value is an empty list
    */
    property var columnSequence: []

    onColumnSequenceChanged: {
        updateColumnCount();
    }

    /*! This property defines the anchors.margins used by the selection highlight
        The default value is \c 0
    */
    property real selectionMargin: 0

    /*! This property toggles addition of a WGScrollBar to the list view
        The default value is \c true
    */
    property bool enableVerticalScrollBar: true

    /*! Specifies the way the background is coloured, can be one of the constants:
        noBackgroundColour
        uniformRowBackgroundColours
        alternatingRowBackgroundColours */
    property int backgroundColourMode: noBackgroundColour

    /*! Colour mode with no background */
    readonly property int noBackgroundColour: 0
    /*! Colour mode with a sigle background colour */
    readonly property int uniformRowBackgroundColours: 1
    /*! Colour mode with a sigle background colour */
    readonly property int alternatingRowBackgroundColours: 2

    readonly property color backgroundColour: palette.midDarkColor
    readonly property color alternateBackgroundColour:
        backgroundColourMode === uniformRowBackgroundColours ? backgroundColour
        : Qt.darker(palette.midLightColor,1.2)

    property bool showColumnHeaders: false
    property bool showColumnFooters: false

    property Component columnHeaderDelegate: defaultColumnHeaderDelegate
    property Component columnFooterDelegate: defaultColumnFooterDelegate

    property color headerBackgroundColour: palette.midDarkColor
    property color footerBackgroundColour: palette.midDarkColor

    property Component defaultColumnHeaderDelegate: Item {
        signal dataChanged;

        property var headerTextVariant: getData("headerText");
        property string headerText:
            headerTextVariant !== null && typeof(headerTextVariant) === "string" ? headerTextVariant : ""

        onDataChanged:headerTextVariant = getData("headerText");

        Text {
            id: textBox
            anchors.left: parent.left
            anchors.top: parent.top
            anchors.bottom: parent.bottom
            anchors.margins: 4
            verticalAlignment: Text.AlignVCenter
            color: palette.textColor
            text: headerText
        }
    }

    property Component defaultColumnFooterDelegate: Item {
        signal dataChanged;

        property var footerTextVariant: getData("footerText");
        property string footerText:
            footerTextVariant !== null && typeof(footerTextVariant) === "string" ? footerTextVariant : ""

        onDataChanged:footerTextVariant = getData("footerText");

        Text {
            id: textBoxFooter
            anchors.left: parent.left
            anchors.top: parent.top
            anchors.bottom: parent.bottom
            anchors.margins: 4
            verticalAlignment: Text.AlignVCenter
            color: palette.textColor
            text: footerText
        }
    }

    property var defaultColumnDelegate: Component {
        Item {
            Layout.fillWidth: true
            Layout.preferredHeight: minimumRowHeight

            Text {
                id: value
                clip: true
                anchors.left: parent.left
                anchors.top: parent.top
                anchors.bottom: parent.bottom
                anchors.margins: 4
                verticalAlignment: Text.AlignVCenter
                text: typeof itemData.value === "string" ? itemData.value : typeof itemData.value
                color: palette.textColor
            }
        }
    }

    /*! This property contains the number of columns */
    property int columnCount: 0

    Component.onCompleted: updateColumnCount()

    Connections {
        target: typeof(model) === "undefined" ? null : model

        onModelReset: {
            updateColumnCount();

            if (showColumnsFrame)
            {
                columnsFrame.initialColumnWidths = listView.columnWidths;
            }
        }

        onHeaderDataChanged: headerDataChanged(first, last);
    }

    /*! This property contains the column widths */
    property var columnWidths: []

    /*! This property contains the initial column widths */
    property var initialColumnWidths: []

    /*! This property determines if the column sizing handles are shown */
    property bool showColumnsFrame: true

    readonly property real minimumScrollbarWidth:
        enableVerticalScrollBar ? verticalScrollBar.collapsedWidth + defaultSpacing.standardBorderSize : 0

    readonly property real maximumScrollbarWidth:
        enableVerticalScrollBar ? verticalScrollBar.expandedWidth + defaultSpacing.standardBorderSize : 0

    readonly property real minimumRowWidth: width - leftMargin - rightMargin - minimumScrollbarWidth

    readonly property real initialColumnsFrameWidth:
        minimumRowWidth + (showColumnsFrame ? minimumScrollbarWidth - maximumScrollbarWidth : 0)

    function updateColumnCount()
    {
        if (typeof(model) === "undefined" || model === null)
        {
            columnCount = 0;
        }
        else
        {
            if ( columnSequence.length !== 0 )
            {
                columnCount = columnSequence.length;
            }
            else
            {
                columnCount = model.columnCount();
            }
        }

        headerDataChanged(0, columnCount - 1);

        //TODO(aidan): check if there is a better way to do this NGT-2101
        list.model = null;
        list.model = listView.model;
    }

    function calculateMaxTextWidth(index)
    {
        var maxTextWidth = 0;

        for (var i = 0; i < listView.children.length; ++i)
        {
            var childObject = listView.children[i]
            maxTextWidth = Math.max(maxTextWidth, childObject.calculateMaxTextWidth(index));
        }

        return maxTextWidth;
    }

    function setCurrentIndex( modelIndexToSet ) {
        selectionExtension.currentIndex = modelIndexToSet

        // Make sure the listView has active focus, otherwise the listView's keyboard event handles won't work
        listView.forceActiveFocus()
    }

    function keyboardScroll( /* bool */ isUpward, /* bool */ calculateRows ) {

        var currentRow = listView.model.indexRow(selectionExtension.currentIndex);
        listView.positionViewAtIndex( currentRow, ListView.Contain );
    }

    function columnWidthFunction(index)
    {
        var columnWidths = listView.columnWidths;
        var columnWidth = 0;

        if (columnWidths.length === 0)
        {
            var listWidth = listView.width - listView.leftMargin - listView.rightMargin;
            columnWidth = Math.ceil(listWidth / columnCount - listView.columnSpacing);
        }
        else if (index < columnWidths.length)
        {
            columnWidth = columnWidths[index];
        }

        return Math.max(0, columnWidth);
    }

    function headerDataChanged(fromColumn, toColumn)
    {
        if (headerItemLoader.status === Loader.Ready)
        {
            headerItem.dataChanged(fromColumn, toColumn)
        }

        if (footerItemLoader.status === Loader.Ready)
        {
            footerItem.dataChanged(fromColumn, toColumn)
        }
    }

    Keys.onUpPressed: {
        // Handle the up key pressed event
        if (selectionExtension.moveUp()) {
            keyboardScroll(true, true);
        }
    }

    Keys.onDownPressed: {
        // Handle the down key pressed event
        if (selectionExtension.moveDown()) {
            keyboardScroll(false, true);
        }
    }

    Keys.onReturnPressed: {
        returnPressed();
    }

    /*! This signal is sent when the row is clicked.
    */
    signal rowClicked(var mouse, var modelIndex)

    /*! This signal is sent when the row is double clicked.
    */
    signal rowDoubleClicked(var mouse, var modelIndex)

    /*! This signal is sent when the Retern Key is pressed.
    */
    signal returnPressed()

    ListView {
        id: list
        model: listView.model
        clip: true
        spacing: listView.spacing
        x: listView.leftMargin
        y: listView.topMargin + headerHeight
        width: listView.width - x - listView.rightMargin
        height: listView.height - y - listView.bottomMargin - footerHeight

        property real headerHeight: headerItemLoader.status === Loader.Ready ? listView.headerItem.height : 0
        property real footerHeight: footerItemLoader.status === Loader.Ready ? listView.footerItem.height : 0
        property bool scrollable: contentHeight > height

        delegate: WGListViewRowDelegate {
            anchors.left: parent.left
            width: Math.max(columnsFrame.width, minimumRowWidth)
            defaultColumnDelegate: listView.defaultColumnDelegate
            columnDelegates: listView.columnDelegates
            columnSequence: listView.columnSequence
            columnWidths: listView.columnWidths
            columnSpacing: listView.columnSpacing
            selectionExtension: listView.selectionExtension
            modelIndex: listView.model.index(rowIndex, 0)
            showBackgroundColour: backgroundColourMode !== noBackgroundColour
            backgroundColour: listView.backgroundColour
            alternateBackgroundColour: listView.alternateBackgroundColour
            hasActiveFocusDelegate: listView.activeFocus

            onClicked: {
                var modelIndex = listView.model.index(rowIndex, 0);
                listView.rowClicked(mouse, modelIndex);

                // Update the selectionExtension's currentIndex
                setCurrentIndex( modelIndex )
            }

            onDoubleClicked: {
                var modelIndex = listView.model.index(rowIndex, 0);
                listView.rowDoubleClicked(mouse, modelIndex);

                // Update the selectionExtension's currentIndex
                setCurrentIndex( modelIndex )
            }
        }
    }

    property alias headerItem: headerItemLoader.item

    Loader {
        id: headerItemLoader
        x: leftMargin
        anchors.top: listView.top
        width: listView.width
        active: showColumnHeaders
        sourceComponent: header
    }

    property Component header: !showColumnHeaders ? null : headerComponent

    property Component headerComponent: WGHeaderRow {
        topMargin: listView.topMargin
        columnCount: listView.columnCount
        columnWidthFunction: listView.columnWidthFunction
        backgroundColour: headerBackgroundColour
        columnDelegate: columnHeaderDelegate
        model: listView.model
        minimumRowHeight: listView.minimumRowHeight
        spacing: columnSpacing
        visible: showColumnHeaders
        width: listView.width - listView.rightMargin - listView.leftMargin
    }

    property alias footerItem: footerItemLoader.item

    Loader {
        id: footerItemLoader
        x: leftMargin
        anchors.bottom: listView.bottom
        width: listView.width
        active: showColumnFooters
        sourceComponent: footer
    }

    property Component footer: !showColumnFooters ? null : footerComponent

    property Component footerComponent: WGHeaderRow {
        bottomMargin: listView.bottomMargin
        columnCount: listView.columnCount
        columnWidthFunction: listView.columnWidthFunction
        backgroundColour: footerBackgroundColour
        columnDelegate: columnFooterDelegate
        model: listView.model
        minimumRowHeight: listView.minimumRowHeight
        spacing: columnSpacing
        visible: showColumnFooters
        width: listView.width - listView.rightMargin - listView.leftMargin
    }

    WGColumnsFrame {
        id: columnsFrame
        columnCount: listView.columnCount
        y: listView.topMargin
        x: listView.leftMargin
        height: listView.height - listView.topMargin - listView.bottomMargin
        width: initialColumnsFrameWidth
        handleWidth: listView.columnSpacing
        drawHandles: showColumnsFrame && listView.columnSpacing > 1
        resizableColumns: showColumnsFrame
        initialColumnWidths: listView.initialColumnWidths
        defaultInitialColumnWidth: listView.columnCount === 0 ? 0 : initialColumnsFrameWidth / listView.columnCount - handleWidth
        idealColumnSizeFunction: calculateMaxTextWidth

        onColumnsChanged: {
            listView.columnWidths = columnWidths;
        }
    }

    WGScrollBar {
        objectName: "verticalScrollBar"
        id: verticalScrollBar
        width: defaultSpacing.rightMargin
        anchors.top: listView.top
        anchors.right: listView.right
        anchors.bottom: listView.bottom
        anchors.topMargin: listView.topMargin
        anchors.bottomMargin: listView.bottomMargin
        anchors.rightMargin: listView.rightMargin
        orientation: Qt.Vertical
        position: list.visibleArea.yPosition
        pageSize: list.visibleArea.heightRatio
        scrollFlickable: list
        visible: list.scrollable && enableVerticalScrollBar
    }
    property alias verticalScrollBar : verticalScrollBar
}
