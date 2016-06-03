import QtQuick 2.3
import QtQuick.Controls 1.2
import QtQuick.Layouts 1.1
import WGControls 1.0

//TODO: Test with various configurations of depthColourisation and flatColourisation
//TODO: Requires extensive testing with indentation and leafNodeIndentation

/*!
 \brief
    Creates a TreeView of data with branches and leaf nodes.
    The TreeView loads WGTreeItems and passes it a columnDelegates list of contents for each row.
    If a columnDelegate is not defined the defaultColumnDelegate will be used.

Example:
\code{.js}
    WGTreeView {
        id: testTreeView
        anchors.top: searchBox.bottom
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.bottom: parent.bottom
        model: testModel
        rightMargin: 8 // leaves just enought space for conventional slider
        columnDelegates: [defaultColumnDelegate, propertyDelegate]
        selectionExtension: treeModelSelection
        childRowMargin: 2
        columnSpacing: 4
        lineSeparator: false

        flatColourisation: false
        depthColourisation: 5

        property Component propertyDelegate: Loader {
            clip: true
            sourceComponent: itemData != null ? itemData.component : null
        }
    }
\endcode
*/

Item {
    id: treeView
    objectName: typeof(itemData) != "undefined" ? itemData.indexPath : "WGTreeView"

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

    /*! This property will add space to the right of each column element.
        The default value is \c 1
    */
    property real columnSpacing: 2

    /*! This property determines the margin around the selection highlight.
        A value of \c 0 will cause the highlights to fill the same space as the frame that takes up the entire row.
        The default value is \c 0
    */
    property real selectionMargin: 0

    /*! This property determines the space to the left and right of the expand/contract icon.
        The default value is \c 3
    */
    property real expandIconMargin: 3

    /*! This property determines the font height of the expand icon (triangle).
        The default value is \c 16 */
    readonly property real expandIconSize: 16

    /*! This property adds vertical spacing under each parent object.
        The default value is \c 0
    */
    property real childListMargin: 0

    /*! This property adds spacing beneath each control.
        Increases row height.
        The default value is \c 0
    */
    property real footerSpacing: 0

    /*! This property adds top and bottom margin spacing for branch nodes
        Modifies row height
        The default value is \c 0
    */
    property real headerRowMargin: 0

    /*! This property adds top and bottom margin spacing for leaf nodes
        Modifies row height
        The default value is \c 0
    */
    property real childRowMargin: 0

    /*! This property sets the minimum row height.
        Unlike Row Margin changes this change will alter the space available for controls within each row.
        If controls automatically fill the row height their size will change.
        The default value is \c defaultSpacing.minimumRowHeight
    */
    property real minimumRowHeight: defaultSpacing.minimumRowHeight

    /*! This property determines the indentation of all nodes (child and branch), excluding the root node
        When depthColourisation is used, indentation is set to \c 0 by default as the entire row is indented instead.
        The default value is \c 12
    */
    property int indentation: 12

    property var selectionExtension: null
    property var treeExtension: null

    /*! This property holds the list of columns that are displayed within each row
    */
    property var columnDelegates: []

    /*! This property causes the first column to resize based on the largest label width
        when a row item is expanded or contracted.
        The default value is \c true if the column handle is visible */
    property bool autoUpdateLabelWidths: false

    /*!  This property enables the vertical scrollbar (both flickable and conventional).
        Mouse wheel scrolling is unaffected by this setting.
        The default value is \c true.
    */
    property bool enableVerticalScrollBar: true


    /*! This property adds a horizontal line separator between rows.
        The default value is \c true.
    */
    property bool lineSeparator: true

    /*! Specifies the way the background is coloured, can be one of the constants:
        noBackgroundColour
        uniformRowBackgroundColours
        alternatingRowBackgroundColours
        incrementalGroupBackgroundColours */
    property int backgroundColourMode: noBackgroundColour

    /*! Colour mode with no background */
    readonly property int noBackgroundColour: 0
    /*! Colour mode with a sigle background colour */
    readonly property int uniformRowBackgroundColours: 1
    /*! Colour mode with a sigle background colour */
    readonly property int alternatingRowBackgroundColours: 2
    /*! Colour mode with a sigle background colour */
    readonly property int incrementalGroupBackgroundColours: 3

    /*! Number of shades to use for incremental colours per level until starting over using the first shade */
    property int backgroundColourIncrements: 3

    readonly property color backgroundColour: "transparent"
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
            objectName: headerText
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
            objectName: footerText
            anchors.left: parent.left
            anchors.top: parent.top
            anchors.bottom: parent.bottom
            anchors.margins: 4
            verticalAlignment: Text.AlignVCenter
            color: palette.textColor
            text: footerText
        }
    }

    /*! This property contains the number of columns */
    property int columnCount: 0

    Component.onCompleted: updateColumnCount()

    Connections {
        target: typeof(model) === "undefined" ? null : model

        onModelReset: {
            updateColumnCount();
        }

        onHeaderDataChanged: headerDataChanged(first, last);
    }

    /*! This property contains the column widths */
    property var columnWidths: []

    /*! This property contains the initial column widths */
    property var initialColumnWidths: []

    /*! This property determines if the column sizing handles are shown */
    property bool showColumnsFrame: true

    property var depthLevelGroups: []
    //property var maximumColumnText: []

    readonly property real minimumScrollbarWidth:
        enableVerticalScrollBar ? verticalScrollBar.collapsedWidth + defaultSpacing.standardBorderSize : 0

    readonly property real maximumScrollbarWidth:
        enableVerticalScrollBar ? verticalScrollBar.expandedWidth + defaultSpacing.standardBorderSize : 0

    readonly property real rowMargins: leftMargin + rightMargin + minimumScrollbarWidth

    readonly property real minimumRowWidth: width - rowMargins

    readonly property real initialColumnsFrameWidth:
        minimumRowWidth + (showColumnsFrame ? minimumScrollbarWidth - maximumScrollbarWidth : 0)

    property real expandIconWidth: 0

    /*! This property allow users to explicitly set tree view root node default expansion status.
        The default value is \c true */
    property bool rootExpanded: true

    /*! This signal is emitted when the row is clicked.
    */
    signal rowClicked(var mouse, var modelIndex)

    /*! This signal is emitted when the row is double clicked.
    */
    signal rowDoubleClicked(var mouse, var modelIndex)

    function updateColumnCount()
    {
        if (typeof(model) === "undefined" || model === null)
        {
            columnCount = 0;
        }
        else
        {
            columnCount = model.columnCount();
        }

        headerDataChanged(0, columnCount - 1);
    }

    function calculateMaxTextWidth(column)
    {
        return calculateMaxTextWidthHelper(rootItem, 0, column);
    }

    // searches through all the TreeViews children in a column for visible text objects
    // gets their paintedWidths and calculates a new maxTextWidth
    function calculateMaxTextWidthHelper(parentObject, currentDepth, column){
        var maxTextWidth = 0;

        // for loop checks all the children
        for (var i=0; i<parentObject.children.length; i++)
        {
            var childObject = parentObject.children[i]
            var checkColumn = column
            var checkDepth = currentDepth

            // if the child has a columnIndex set column to it
            if (typeof childObject.columnIndex != "undefined")
            {
                checkColumn = childObject.columnIndex
            }

            // if the child is visible keep going
            if (childObject.visible)
            {
                //if the child has a depth value... remember it so we can add more indentation
                if (typeof childObject.depth != "undefined")
                {
                    checkDepth = childObject.depth
                }

                // if it has a painted width, turn off elide,
                // check if its painted width + depth indentation is the longest
                // then update and reset elide
                if (typeof childObject.__treeLabel != "undefined")
                {
                    var childElide = Text.ElideNone

                    if (childObject.elide != Text.ElideNone)
                    {
                        childElide = childObject.elide
                        childObject.elide = Text.ElideNone
                    }

                    var indent = column > 0 ? 0 : indentation * checkDepth + expandIconWidth;
                    var testWidth = childObject.paintedWidth + indent
                    maxTextWidth = Math.max(maxTextWidth, testWidth);

                    if(childElide != childObject.elide)
                    {
                        childObject.elide = childElide
                    }
                }
                // if the column is the same as the checked column
                // rerun this function with the child object
                if (checkColumn == column)
                {
                    maxTextWidth = Math.max(maxTextWidth, calculateMaxTextWidthHelper(childObject, checkDepth, column));
                }
            }
        }

        return maxTextWidth;
    }

    function columnWidthFunction(index)
    {
        var columnWidths = treeView.columnWidths;
        var columnWidth = 0;

        if (columnWidths.length === 0)
        {
            var treeWidth = rootItem.width;
            columnWidth = Math.ceil(treeWidth / columnCount - treeView.columnSpacing);
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

    function addDepthLevel(depth)
    {
        if (depth >= depthLevelGroups.length)
        {
            depthLevelGroups.push(1);
        }

        ++depthLevelGroups[depth];
    }

    function removeDepthLevel(depth)
    {
        if (--depthLevelGroups[depth] == 0)
        {
            depthLevelGroups.pop();
        }
    }

    function setExpandIconWidth(width)
    {
        expandIconWidth = width
    }

    /*! This Component is used by the property columnDelegate if no other column delegate is defined
    */
    property Component defaultColumnDelegate: Text {
        property bool __treeLabel: true
        color: palette.textColor
        clip: itemData != null && itemData.component != null
        text: itemData != null ? itemData.display : ""
        font.bold: itemData != null && itemData.hasChildren
        verticalAlignment: Text.AlignVCenter
        elide: Text.ElideRight
    }

    WGTreeItem {
        id: rootItem
        model: treeView.model
        clip: true
        x: treeView.leftMargin
        y: treeView.topMargin + headerHeight
        width: Math.max(columnsFrame.width, treeView.minimumRowWidth) + treeView.rowMargins
        height: treeView.height - y - footerHeight

        property real headerHeight: headerItemLoader.status === Loader.Ready ? treeView.headerItem.height : 0
        property real footerHeight: footerItemLoader.status === Loader.Ready ? treeView.footerItem.height : 0
        property bool scrollable: contentHeight > height

        onContentHeightChanged: {
            if (autoUpdateLabelWidths)
            {
                columnsFrame.resizeColumnToIdealSize(0)
            }
        }
    }

    property alias headerItem: headerItemLoader.item

    Loader {
        id: headerItemLoader
        x: leftMargin
        anchors.top: treeView.top
        width: rootItem.width
        active: showColumnHeaders
        sourceComponent: header
    }

    property Component header: showColumnHeaders ? headerComponent : null

    property Component headerComponent: WGHeaderRow {
        topMargin: treeView.topMargin
        columnCount: treeView.columnCount
        columnWidthFunction: treeView.columnWidthFunction
        backgroundColour: headerBackgroundColour
        columnDelegate: columnHeaderDelegate
        model: treeView.model
        minimumRowHeight: treeView.minimumRowHeight
        spacing: columnSpacing
        visible: showColumnHeaders
        width: treeView.width - treeView.rightMargin - treeView.leftMargin
    }

    property alias footerItem: footerItemLoader.item

    Loader {
        id: footerItemLoader
        x: leftMargin
        anchors.bottom: treeView.bottom
        width: rootItem.width
        active: showColumnFooters
        sourceComponent: footer
    }

    property Component footer: showColumnFooters ? footerComponent : null

    property Component footerComponent: WGHeaderRow {
        bottomMargin: treeView.bottomMargin
        columnCount: treeView.columnCount
        columnWidthFunction: treeView.columnWidthFunction
        backgroundColour: footerBackgroundColour
        columnDelegate: columnFooterDelegate
        model: treeView.model
        minimumRowHeight: treeView.minimumRowHeight
        spacing: columnSpacing
        visible: showColumnFooters
        width: treeView.width - treeView.rightMargin - treeView.leftMargin
    }

    WGColumnsFrame {
        id: columnsFrame
        columnCount: treeView.columnCount
        y: treeView.topMargin
        x: treeView.leftMargin
        height: treeView.height - treeView.topMargin - treeView.bottomMargin
        width: initialColumnsFrameWidth
        handleWidth: treeView.columnSpacing
        drawHandles: showColumnsFrame && treeView.columnSpacing > 1
        resizableColumns: showColumnsFrame
        initialColumnWidths: treeView.initialColumnWidths
        defaultInitialColumnWidth: treeView.columnCount === 0 ? 0 : initialColumnsFrameWidth / treeView.columnCount - handleWidth
        idealColumnSizeFunction: calculateMaxTextWidth
        firstColumnIndentation: expandIconWidth + (depthLevelGroups.length - 1) * indentation

        onColumnsChanged: {
            treeView.columnWidths = columnWidths;
        }
    }

    WGScrollBar {
        id: verticalScrollBar
        width: defaultSpacing.rightMargin
        anchors.top: treeView.top
        anchors.right: treeView.right
        anchors.bottom: treeView.bottom
        anchors.topMargin: treeView.topMargin
        anchors.bottomMargin: treeView.bottomMargin
        anchors.rightMargin: treeView.rightMargin
        orientation: Qt.Vertical
        position: rootItem.visibleArea.yPosition
        pageSize: rootItem.visibleArea.heightRatio
        scrollFlickable: rootItem
        visible: rootItem.scrollable && enableVerticalScrollBar
    }
    property alias verticalScrollBar: verticalScrollBar
}
