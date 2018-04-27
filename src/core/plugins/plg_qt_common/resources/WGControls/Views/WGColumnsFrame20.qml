import QtQuick 2.5
import WGControls 2.0
import WGControls.Private 2.0

/** \brief Controls behaviour for resizable column widths and spacing.
Adds the mouse handles between columns, and provides column widths for the views to lock on to.
\ingroup wgcontrols */
Item {
    id: columnsFrame
    objectName: "WGColumnsFrame"
    WGComponent { type: "WGColumnsFrame20" }

    x: itemView.view.contentItem.x + itemView.view.originX
    y: 0
    width: childrenRect.width
    height: itemView.view.height

    property var itemView: null

    /** Monitors if the columnHandle has been dragged.*/
    property bool columnsHandleTouched: false

    /** Monitors if the windowWidth is modified.*/
    property bool windowWidthChanged: false

    /** The number of columns of the view.*/
    property var columnCount: itemView.columnCount()
    /** The minimum a column can be reduced to. */
    property var minimumColumnWidth: 5
    /** The default user set initial column width of the view.*/
    property var initialColumnWidth: itemView.initialColumnWidth
    /** The default user set initial column widths of the view, per index.*/
    property var initialColumnWidths: itemView.initialColumnWidths
    /** The ideal column widths to accomadate contents of the view, per index.*/
    property var implicitColumnWidths: []
    property var minImplicitColumnWidths: []
    property var maxImplicitColumnWidths: []
    /** The actual column widths of the view, per index.*/
    property var columnWidths: []
    /** Size of the gap between columns.*/
    property real columnSpacing: itemView.columnSpacing
    property var totalWidth: 0
    property var visibleRange: clamp ? Qt.vector2d(0, itemView.view.width) : Qt.vector2d(itemView.view.contentX, itemView.view.contentX + itemView.view.width)
    property var visibleOffset: 0
    property var visibleColumns: Qt.vector2d(0, -1)
    /** Clamp (fix) width of the view to the containing component and adjust contents when width resized.*/
    property bool clamp: false
    /** Style component to draw the column sizing handle.*/
    property Component handleDelegate : Item {}

    Component.onCompleted: {
        if (itemView.clamp)
        {
            clamp = Qt.binding(function() { return itemView.clamp })
        }
    }

    Connections {
        target: itemView
        onClampChanged: {
            clamp = Qt.binding(function() { return itemView.clamp })
            internal.updateVisibleColumns();
            internal.jostleColumnWidths(true);
        }
    }

    onColumnCountChanged: {
        if (clamp) {
            internal.updateVisibleColumns();
            return;
        }

        var widths = columnWidths;
        for (var i = 0; i < columnCount; ++i) {
            var column = getColumnIndex(i);
            widths[column] = getColumnWidth(i);
        }
        columnWidths = widths;
    }

    function resetImplicitColumnWidths() {
        for (var i = 0; i <implicitColumnWidths.length; i++) {
            implicitColumnWidths[i] = minimumColumnWidth
        }
    }

    function getColumnIndex(index) {
        return itemView.sourceColumn(index);
    }

    function getColumnWidth(index) {
        var column = getColumnIndex(index);
        if (column < columnWidths.length) {
            var width = columnWidths[column];
            if (typeof width != 'undefined') {
                return width;
            }
        }
        return getInitialColumnWidth(index);
    }

    function getInitialColumnWidth(index) {
        var column = getColumnIndex(index);
        if (column < initialColumnWidths.length) {
            var width = initialColumnWidths[column];
            if (typeof width != 'undefined') {
                return width;
            }
        }
        return initialColumnWidth;
    }

    function getImplicitColumnWidth(index) {
        var column = getColumnIndex(index);
        if (column < implicitColumnWidths.length) {
            var width = implicitColumnWidths[column];
            if (typeof width != 'undefined') {
                return width;
            }
        }
        return 0;
    }

    function getMinImplicitColumnWidth(index) {
        var column = getColumnIndex(index);
        if (column < minImplicitColumnWidths.length) {
            var width = minImplicitColumnWidths[column];
            if (typeof width != 'undefined') {
                return width;
            }
        }
        return 0;
    }

    function getMaxImplicitColumnWidth(index) {
        var column = getColumnIndex(index);
        if (column < maxImplicitColumnWidths.length) {
            var width = maxImplicitColumnWidths[column];
            if (typeof width != 'undefined') {
                return width;
            }
        }
        return -1;
    }

    function setColumnWidth(index, width) {
        if (clamp) {
            return;
        }

        var currentWidth = getColumnWidth(index);
        if (currentWidth == width) {
            return;
        }

        var column = getColumnIndex(index);
        var widths = columnWidths;
        widths[column] = width;
        columnWidths = widths;
    }

    function updateImplicitColumnWidth(index, width,  minWidth, maxWidth) {        
        var column = getColumnIndex(index);
        var widthChanged = false;

        var currentWidth = getImplicitColumnWidth(index);
        if (width > currentWidth) {
            implicitColumnWidths[column] = width;
            widthChanged = true;
        }

        var currentMinWidth = getMinImplicitColumnWidth(index);
        if (minWidth > currentMinWidth) {
            minImplicitColumnWidths[column] = minWidth;
            widthChanged = true;
        }

        var currentMaxWidth = getMaxImplicitColumnWidth(index);
        if (maxWidth > currentMaxWidth) {
            maxImplicitColumnWidths[column] = maxWidth;
            widthChanged = true;
        }

        if (widthChanged) {
            internal.jostleColumnWidths();
        }
    }

    onColumnWidthsChanged: {
        internal.updateVisibleColumns();
    }

    onVisibleRangeChanged: {
        windowWidthChanged = true
        columnsHandleTouched = false

        var availableWidth = visibleRange.y - visibleRange.x;

        if (internal.availableWidth != availableWidth) {
            internal.availableWidth = availableWidth;
            internal.jostleColumnWidths();
        }

        internal.updateVisibleColumns();
    }

    Row {
        x: visibleOffset
        width: childrenRect.width
        height: childrenRect.height

        Repeater {
            id: handles
            model: WGRangeProxy {
                sourceModel: itemView.headerRowModel

                property var range: visibleColumns
                onRangeChanged: {
                    if (range.y >= range.x) {
                        setRange(range.x, 0, range.y, 0);
                    }
                }
            }

            onItemAdded: internal.jostleColumnWidths(true);
            onItemRemoved: internal.jostleColumnWidths(true);

            Item {
                id: column

                width: handle.x + handle.width
                height: columnsFrame.height

                property var columnIndex: visibleColumns.x + index

                onColumnIndexChanged: {
                    if (columnIndex < 0) {
                        return;
                    }
                    handle.x = getColumnWidth(columnIndex);
                }

                Connections {
                    target: itemView.extendedModel
                    onLayoutChanged: {
                        column.columnIndexChanged();
                    }
                }

                Rectangle {
                    id: handle
                    objectName: "WGColumnsFrameHandle_" + columnIndex
                    visible: !clamp || columnIndex < columnCount - 1
                    x: getColumnWidth(columnIndex)
                    width: columnSpacing
                    height: columnsFrame.height
                    color: "transparent"

                    Loader {
                        anchors.fill: parent
                        sourceComponent: handleDelegate
                    }

                    MouseArea {
                        id: handleMouseArea
                        anchors.verticalCenter: parent.verticalCenter
                        width: handle.width + 1
                        height: handle.height
                        cursorShape: Qt.SplitHCursor
                        drag.target: handle
                        drag.threshold: 0
                        drag.minimumX: minimumColumnWidth
                        drag.axis: Drag.XAxis

                        /* A workaround to prevent changing properties from calling jostle*/
                        onPressed: {
                            columnsFrame.columnsHandleTouched = true
                        }

                        onDoubleClicked: {
                            if (clamp) {
                                var leftIdealWidth = getImplicitColumnWidth(columnIndex);
                                var rightIdealWidth = getImplicitColumnWidth(columnIndex + 1);
                                var leftWidthWithIdealRight = getColumnWidth(columnIndex) - rightIdealWidth + getColumnWidth(columnIndex + 1);
                                var desiredWidth = handle.x !== leftIdealWidth ? leftIdealWidth : leftWidthWithIdealRight;
                                internal.divideWidthAroundHandle(columnIndex, desiredWidth);
                            }
                            else {
                                handle.x = Math.max(drag.minimumX, getImplicitColumnWidth(columnIndex));
                            }
                        }
                    }

                    onXChanged: {
                        if (!clamp) {
                            setColumnWidth(columnIndex, x);
                        }
                        else if (handleMouseArea.pressedButtons & Qt.LeftButton) {
                            internal.divideWidthAroundHandle(columnIndex, x);
                        }
                    }
                }

                Column {
                    id: columnDebugLogging
                    visible: internal.showDebugLog
                    width: handle.x
                    height: parent.height
                    spacing: 1

                    Item {
                        width: 10
                        height: 30
                    }

                    Loader {
                        property color colour: "green"
                        property string label: "old width"
                        property var value: columnIndex < internal.previousWidths.length ? internal.previousWidths[columnIndex] : 0
                        anchors.horizontalCenter: parent.horizontalCenter
                        enabled: internal.showDebugLog
                        sourceComponent: internal.debugOutputComponent
                    }

                    Loader {
                        property color colour: "blue"
                        property string label: "new width"
                        property var value: getColumnWidth(columnIndex)
                        anchors.horizontalCenter: parent.horizontalCenter
                        enabled: internal.showDebugLog
                        sourceComponent: internal.debugOutputComponent
                    }

                    Loader {
                        property color colour: "magenta"
                        property string label: "implicit"
                        property var value: getImplicitColumnWidth(columnIndex)
                        anchors.horizontalCenter: parent.horizontalCenter
                        enabled: internal.showDebugLog
                        sourceComponent: internal.debugOutputComponent
                    }

                    Loader {
                        property color colour: "orange"
                        property string label: "priority"
                        property var value: columnIndex < internal.priorityReserves.length ? internal.priorityReserves[columnIndex] : 0
                        anchors.horizontalCenter: parent.horizontalCenter
                        enabled: internal.showDebugLog
                        sourceComponent: internal.debugOutputComponent
                    }

                    Loader {
                        property color colour: "red"
                        property string label: "adjusted"
                        property var value: columnIndex < internal.lastAdjustments.length ? internal.lastAdjustments[columnIndex] : 0
                        anchors.horizontalCenter: parent.horizontalCenter
                        enabled: internal.showDebugLog
                        sourceComponent: internal.debugOutputComponent
                    }
                }
            }
        }
    }

    Column {
        id: frameDebugLogging
        visible: internal.showDebugLog
        width: parent.width
        y: parent.height - childrenRect.height
        spacing: 1

        Item {
            width: 10
            height: 30
        }

        Loader {
            property color colour: "green"
            property string label: "availableWidth"
            property var value: internal.availableWidth
            anchors.horizontalCenter: parent.horizontalCenter
            enabled: internal.showDebugLog
            sourceComponent: internal.debugOutputComponent
        }

        Loader {
            property color colour: "blue"
            property string label: "width"
            property var value: itemView.view.width
            anchors.horizontalCenter: parent.horizontalCenter
            enabled: internal.showDebugLog
            sourceComponent: internal.debugOutputComponent
        }
    }

    QtObject {
        id: internal
        property bool showDebugLog: false
        property bool updatingVisibleColumns: false
        property var availableWidth: 0
        property var implicitWidths: []
        property var priorityReserves: []
        property var previousWidths: []
        property var lastAdjustments: []

        property Component debugOutputComponent: Rectangle {
            width: Math.max(textLabel.implicitWidth, textOutput.implicitWidth) + 4
            height: textLabel.implicitHeight + textOutput.implicitHeight + 4
            radius: 3
            border.width: 1
            border.color: palette.darkestShade
            color: palette.midLightColor

            Column {
                anchors.horizontalCenter: parent.horizontalCenter
                anchors.verticalCenter: parent.verticalCenter

                Text {
                    id: textLabel
                    anchors.horizontalCenter: parent.horizontalCenter
                    horizontalAlignment: Text.AlightHCenter
                    color: colour
                    text: label.length > 0 ? label : ""
                }

                Text {
                    id: textOutput
                    anchors.horizontalCenter: parent.horizontalCenter
                    horizontalAlignment: Text.AlightHCenter
                    color: colour
                    text: typeof value === "undefined" ? "loading" : value
                }
            }
        }

        function updateVisibleColumns() {
            if (columnCount == 0) {
                return;
            }

            if (clamp) {
                var columns = Qt.vector2d(0, columnCount - 1);

                if (columns != visibleColumns) {
                    visibleColumns = columns;
                }

                if (totalWidth != availableWidth) {
                    totalWidth = availableWidth;
                }

                return;
            }

            if (updatingVisibleColumns) {
                return;
            }

            var visibleStart = visibleRange.x;
            var visibleEnd = visibleRange.y;
            var first = 0;
            var last = -1;
            var width = 0;
            for (var i = 0; i < columnCount; ++i) {
                var columnWidth = getColumnWidth(i) + columnSpacing;
                if (visibleStart >= columnWidth && first == i) {
                    ++first
                    visibleStart -= columnWidth;
                }
                if (visibleEnd > 0) {
                    ++last;
                    visibleEnd -= columnWidth;
                }
                width += columnWidth;
            }

            var deferUpdate = false;

            if (totalWidth != width) {
                updatingVisibleColumns = last >= first;
                totalWidth = width;
                deferUpdate = updatingVisibleColumns;
                updatingVisibleColumns = false;
            }

            if (last < first) {
                return;
            }

            var offset = visibleRange.x - visibleStart;
            if (visibleOffset != offset) {
                visibleOffset = offset;
            }

            var columns = Qt.vector2d(first, last);
            if (visibleColumns != columns) {
                visibleColumns = columns;
            }

            if (deferUpdate) {
                updateVisibleColumns();
            }
        }

        /** Let columns fight for space in the available width, based on priority and ideal widths.*/
        function jostleColumnWidths(resetWidths) {
            if (columnsFrame.columnsHandleTouched || !columnsFrame.windowWidthChanged) {
                return;
            }

            if (!clamp || handles.count === 0 || handles.count != columnCount) {
                return;
            }

            var currentWidths = columnWidths.slice();
            var previousImplicitWidths = internal.implicitWidths.slice();
            var currentImplicitWidths = implicitColumnWidths.slice();

            // Reset column widths and recalculate as a different amount of columns need a different distribution.
            if (resetWidths) {
                currentWidths = [];
                previousImplicitWidths = [];
            }

            for (var i = 0; i < currentWidths.length; ++i) {
                currentWidths[i] = isNaN(currentWidths[i]) ? (isNaN(initialColumnWidths[i]) ? initialColumnWidth : initialColumnWidths[i]) : currentWidths[i];
            }

            previousWidths = currentWidths.slice();

            for (var i = 0; i < previousImplicitWidths.length; ++i) {
                previousImplicitWidths[i] = isNaN(previousImplicitWidths[i]) ? 0 : previousImplicitWidths[i];
            }

            for (var i = 0; i < currentImplicitWidths.length; ++i) {
                currentImplicitWidths[i] = isNaN(currentImplicitWidths[i]) ? (isNaN(implicitColumnWidths[i]) ? 0 : implicitColumnWidths[i]) : currentImplicitWidths[i];
            }

            for (var i = 0; i < columnCount; ++i) {
                var column = getColumnIndex(i);
                if (isNaN(currentWidths[column])) {
                    currentWidths[column] = columnsFrame.getInitialColumnWidth(i);
                }
                if (isNaN(previousImplicitWidths[column])) {
                    previousImplicitWidths[column] = 0;
                }
                if (isNaN(currentImplicitWidths[column])) {
                    currentImplicitWidths[column] = 0;
                }
            }

            internal.implicitWidths = currentImplicitWidths.slice();

            var currentWidth = 0;
            var implicitWidth = 0;

            for (var i = 0; i < columnCount; ++i) {
                var column = getColumnIndex(i);
                currentWidth += currentWidths[column];
                implicitWidth += currentImplicitWidths[column];
                lastAdjustments[column] = 0;
            }

            var spacersWidth = columnSpacing * (columnCount - 1);
            var newWidth = Math.max(internal.availableWidth - spacersWidth, 0);
            var delta = newWidth - currentWidth

            // if not resizing, rebalance according to changes in implicit widths.
            if (delta === 0) {
                var implicitDeltas = [];
                var implicitChanged = false;

                for (var i = 0; i < columnCount; ++i) {
                    var column = getColumnIndex(i);
                    var currentReserve = Math.max(currentImplicitWidths[column] - currentWidths[column], 0);
                    var previousReserve = Math.max(previousImplicitWidths[column] - currentWidths[column], 0);
                    var implicitDelta = currentImplicitWidths[column] - previousImplicitWidths[column];

                    if (currentReserve === 0 && previousReserve === 0) {
                        implicitDelta = 0;
                    }
                    else if (implicitDelta !== 0) {
                        implicitDelta = currentReserve - previousReserve;
                    }

                    implicitChanged |= implicitDelta !== 0;
                    implicitDeltas.push(implicitDelta);
                }

                if (!implicitChanged) {
                    return;
                }

                for (var i = 0; i < columnCount; ++i) {
                    var column = getColumnIndex(i);
                    currentWidths[column] += implicitDeltas[i];
                    currentWidth += implicitDeltas[i];
                    delta -= implicitDeltas[i];
                }
            }

            for (var i = 0; i < columnCount; ++i) {
                var column = getColumnIndex(i);
                var minColumnWidth = getMinImplicitColumnWidth(i);
                var maxColumnWidth = getMaxImplicitColumnWidth(i);
                var currentWidthDelta = 0;
                if (currentWidths[column] < minColumnWidth) {
                    currentWidthDelta = minColumnWidth - currentWidths[column];
                }
                else if (maxColumnWidth != -1 && currentWidths[column] > maxColumnWidth) {
                    currentWidthDelta = maxColumnWidth - currentWidths[column];
                }
                currentWidths[column] += currentWidthDelta;
                currentWidth += currentWidthDelta;
                delta -= currentWidthDelta;
            }

            priorityReserves = [];
            var attractiveColumns = [];
            var attractiveDelta = 0;
            var repulsiveDelta = 0;

            // decide which columns are attractive to resize, and which ones are not.
            for (var i = 0; i < columnCount; ++i) {
                var column = getColumnIndex(i);
                var reserve = currentImplicitWidths[column] - currentWidths[column];
                var attractive = delta * reserve > 0 && reserve !== 0 || delta === 0 && reserve < 0;
                var repulsive = delta * reserve < 0 || delta === 0 && reserve > 0;

                if (attractive) {
                    attractiveColumns.push(i);
                    attractiveDelta += reserve;
                }
                else if (repulsive) {
                    repulsiveDelta += reserve;
                }

                priorityReserves.push(reserve);
            }

            priorityReservesChanged();

            // size attractive (priority) columns first, proportionately.
            if (attractiveDelta !== 0) {
                var adjustedWidth = currentWidth + attractiveDelta;
                var priorityColumns = [];
                var currentAttractiveDelta = attractiveDelta;
                var usableDelta = attractiveDelta < 0 ?
                    Math.max(delta, attractiveDelta) : Math.min(delta, attractiveDelta);

                for (var i = 0; i < attractiveColumns.length; ++i) {
                    var index = attractiveColumns[i];
                    var column = getColumnIndex(index);
                    var columnDelta = usableDelta * priorityReserves[index] / currentAttractiveDelta;
                    columnDelta = (columnDelta < 0) ? Math.ceil(columnDelta) : Math.floor(columnDelta);
                    priorityReserves[index] -= columnDelta;
                    attractiveDelta -= columnDelta;
                    delta -= columnDelta;
                    currentWidths[column] += columnDelta;

                    if (priorityReserves[index] !== 0) {
                        var position = 0;
                        var insertValue = -priorityReserves[index] /
                            (currentWidths[column] - priorityReserves[index]);

                        for (; position < priorityColumns.length; ++position) {
                            var compareIndex = priorityColumns[position];
                            var compareColumn = getColumnIndex(compareIndex);
                            var compareValue = -priorityReserves[compareIndex] /
                                (currentWidths[compareColumn] - priorityReserves[compareIndex]);

                            if (compareValue < insertValue) {
                                break;
                            }
                        }

                        priorityColumns.splice(position, 0, index);
                    }
                }

                // make sure we end up with exact amounts.
                while (priorityColumns.length > 0 && attractiveDelta !== 0 && delta !== 0) {
                    var index = priorityColumns.shift();
                    var column = getColumnIndex(index);
                    var columnDelta = attractiveDelta < 0 ? -1 : 1;
                    priorityReserves[index] -= columnDelta;
                    attractiveDelta -= columnDelta;
                    delta -= columnDelta;
                    currentWidths[column] += columnDelta;
                }
            }

            // size everything proportionately with what's left if the attractive columns were not enough.
            if (delta !== 0) {
                var totalSafeDelta = 0;
                var safeColumnDeltas = [];
                var unboundedColumnIndices = [];
                for (var i = 0; i < columnCount; ++i) {
                    var column = getColumnIndex(i);
                    safeColumnDeltas[column] = 0;
                    if (delta > 0) {
                        var maxImplicitWidth = getMaxImplicitColumnWidth(i);
                        if (maxImplicitWidth == -1) {
                            unboundedColumnIndices.push(column);
                        }
                        else if (columnWidths[column] < maxImplicitWidth) {
                            safeColumnDeltas[column] = maxImplicitWidth - columnWidths[column];
                        }
                    }
                    else if (delta < 0) {
                        var minImplicitWidth = getMinImplicitColumnWidth(i);
                        if (columnWidths[column] > minImplicitWidth) {
                            safeColumnDeltas[column] = minImplicitWidth - columnWidths[column];
                        }
                    }
                    totalSafeDelta += safeColumnDeltas[column];
                }

                var unboundDelta = 0;
                var safeDelta = 0;
                var unsafeDelta = delta;
                if (delta > 0) {
                    if (unboundedColumnIndices.length > 0) {
                        unboundDelta = delta;
                        unsafeDelta = 0;
                    }
                    else {
                        safeDelta = Math.min(delta, totalSafeDelta);
                        unsafeDelta -= safeDelta;
                    }
                }
                else {
                    safeDelta = Math.max(delta, totalSafeDelta);
                    unsafeDelta -= safeDelta;
                }

                currentWidth = newWidth - delta;
                var factor = unsafeDelta / currentWidth;
                var priorityColumns = [];

                for (var i = 0; i < columnCount; ++i) {
                    var column = getColumnIndex(i);
                    var columnDelta = 0;

                    if (currentWidth === 0) {
                        columnDelta = newWidth / columnCount;
                    }
                    else {
                        columnDelta = factor * currentWidths[column];
                    }

                    if (safeDelta != 0) {
                        columnDelta += safeDelta * safeColumnDeltas[column] / totalSafeDelta;
                    }

                    if (unboundedColumnIndices.indexOf(column) != -1) {
                        columnDelta += unboundDelta / unboundedColumnIndices.length;
                    }

                    if (columnDelta < 0) {
                        columnDelta = Math.ceil(columnDelta);
                        columnDelta = Math.max(columnDelta, 0 - currentWidths[column]);
                    }
                    else {
                        columnDelta = Math.floor(columnDelta);
                    }

                    priorityReserves[i] -= columnDelta;
                    repulsiveDelta -= columnDelta;
                    delta -= columnDelta;
                    currentWidths[column] += columnDelta;

                    if (currentWidths[column] > 0) {
                        var position = 0;
                        var insertValue = priorityReserves[i] /
                            (currentWidths[column] + priorityReserves[i]);

                        for (; position < priorityColumns.length; ++position) {
                            var compareIndex = priorityColumns[position];
                            var compareColumn = getColumnIndex(compareIndex);
                            var compareValue = priorityReserves[compareIndex] /
                                (currentWidths[compareColumn] + priorityReserves[compareIndex]);

                            if (Math.abs(compareValue) > Math.abs(insertValue)) {
                                break;
                            }
                        }

                        priorityColumns.splice(position, 0, i);
                    }
                }

                // make sure we end up with exact amounts.
                while (priorityColumns.length > 0 && delta !== 0) {
                    var index = priorityColumns.shift();
                    var column = getColumnIndex(index)
                    var columnDelta = delta < 0 ? -1 : 1;
                    priorityReserves[index] -= columnDelta;
                    repulsiveDelta -= columnDelta;
                    delta -= columnDelta;
                    currentWidths[column] += columnDelta;
                }
            }

            // change the columns frame handles.
            for (var i = 0; i < columnCount; ++i) {
                var column = getColumnIndex(i)
                if (handles.itemAt(i) !== null) {
                    var handle = handles.itemAt(i).children[0];
                    handle.x = currentWidths[column];
                }
            }

            var adjustments = [];

            for (var i = 0; i < columnCount; ++i) {
                var column = getColumnIndex(i)
                adjustments.push(currentWidths[column] - previousWidths[column]);
            }

            lastAdjustments = adjustments.slice();
            columnWidths = currentWidths.slice();
        }

        /** Split a fixed width between two columns. As the one increases, the other decreases.*/
        function divideWidthAroundHandle(index, desiredWidth) {            
            var leftColumn = getColumnIndex(index);
            var rightColumn = getColumnIndex(index + 1 );

            var currentWidths = columnWidths.slice();
            var availableWidth = currentWidths[leftColumn] + currentWidths[rightColumn] - minimumColumnWidth;
            var newWidth = Math.max(minimumColumnWidth, Math.min(desiredWidth, availableWidth));
            var delta = newWidth - currentWidths[leftColumn];

            previousWidths = currentWidths.slice();
            currentWidths[leftColumn] += delta;
            currentWidths[rightColumn] -= delta;

            var adjustments = lastAdjustments.slice();
            adjustments[leftColumn] = delta;
            adjustments[rightColumn] = -delta;
            lastAdjustments = adjustments.slice();

            var leftHandleItem = handles.itemAt(index)
            var rightHandleItem = handles.itemAt(index + 1)

            if (leftHandleItem !== null) {
                var handle = leftHandleItem.children[0];
                handle.x = currentWidths[leftColumn];
            }

            if (rightHandleItem !== null) {
                handle = rightHandleItem.children[0];
                handle.x = currentWidths[rightColumn];
            }

            columnWidths = currentWidths.slice();
        }
    }
}
