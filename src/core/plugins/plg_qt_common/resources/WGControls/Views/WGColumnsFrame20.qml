import QtQuick 2.5
import WGControls.Private 2.0

/** \brief Controls behaviour for resizable column widths and spacing.
Adds the mouse handles between columns, and provides column widths for the views to lock on to.
\ingroup wgcontrols */
Row {
    id: columnsFrame
    objectName: "WGColumnsFrame"
    WGComponent { type: "WGColumnsFrame20" }
    
    width: childrenRect.width

    /** The number of columns of the view.*/
    property var columnCount: 0
    /** The minimum a column can be reduced to. */
    property var minimumColumnWidth: 1
    /** The default user set initial column width of the view.*/
    property var initialColumnWidth: 0
    /** The default user set initial column widths of the view, per index.*/
    property var initialColumnWidths: []
    /** The ideal column widths to accomadate contents of the view, per index.*/
    property var implicitColumnWidths: []
    /** The actual column widths of the view, per index.*/
    property var columnWidths: []
    /** Size of the gap between columns.*/
    property real columnSpacing: 0
    /** The visible width available to fit all columns into, when clamping to the containing component.*/
    property real availableWidth: 0
    /** Clamp (fix) width of the view to the containing component and adjust contents when width resized.*/
    property bool clamp: true
    /** Style component to draw the column sizing handle.*/
    property Component handleDelegate : Item {}

    function getInitialColumnWidth(index) {
        if (index < initialColumnWidths.length) {
            return initialColumnWidths[index];
        }
        return initialColumnWidth;
    }

    function getImplicitColumnWidth(index) {
        if (index < implicitColumnWidths.length) {
            return implicitColumnWidths[index];
        }
        return 0;
    }

    onAvailableWidthChanged: {
        internal.jostleColumnWidths();
    }

    onImplicitColumnWidthsChanged: {
        internal.jostleColumnWidths();
    }

    Component.onCompleted: {
        internal.loaded = true;
        internal.jostleColumnWidths();
    }

    Repeater {
        id: handles
        model: columnCount

        Item {
            width: handle.x + handle.width
            height: columnsFrame.height
            
            Rectangle {
                id: handle
                objectName: "WGColumnsFrameHandle_" + index
                visible: !clamp || index < columnCount - 1
                x: getInitialColumnWidth(index)
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

                    onDoubleClicked: {
                        if (clamp) {
                            internal.divideWidthAroundHandle(index, getImplicitColumnWidth(index));
                        }
                        else {
                            handle.x = Math.max(drag.minimumX, getImplicitColumnWidth(index));
                        }
                    }
                }

                onXChanged: {
                    if (!clamp) {
                        var widths = columnWidths
                        widths[index] = x
                        columnWidths = widths
                    }
                    else if (handleMouseArea.pressedButtons & Qt.LeftButton) {
                        internal.divideWidthAroundHandle(index, x);
                    }
                }
            }
            
            Column {
                id: debugLogging
                visible: internal.showDebugLog
                width: handle.x
                height: parent.height
                spacing: 1

                function lookupArrayValue(array, index) {
                    return typeof(array) == "undefined" || index >= array.length ? 0 : array[index];
                }

                Item {
                    width: 10
                    height: 30
                }
                
                Loader {
                    property color colour: "green"
                    property string label: "old width"
                    property var value: index < internal.previousWidths.length ? internal.previousWidths[index] : 0
                    anchors.horizontalCenter: parent.horizontalCenter
                    enabled: internal.showDebugLog
                    sourceComponent: internal.debugOutputComponent
                }

                Loader {
                    property color colour: "blue"
                    property string label: "new width"
                    property var value: columnWidths[index]
                    anchors.horizontalCenter: parent.horizontalCenter
                    enabled: internal.showDebugLog
                    sourceComponent: internal.debugOutputComponent
                }

                Loader {
                    property color colour: "magenta"
                    property string label: "implicit"
                    property var value: getImplicitColumnWidth(index)
                    anchors.horizontalCenter: parent.horizontalCenter
                    enabled: internal.showDebugLog
                    sourceComponent: internal.debugOutputComponent
                }

                Loader {
                    property color colour: "orange"
                    property string label: "priority"
                    property var value: index < internal.priorityReserves.length ? internal.priorityReserves[index] : 0
                    anchors.horizontalCenter: parent.horizontalCenter
                    enabled: internal.showDebugLog
                    sourceComponent: internal.debugOutputComponent
                }

                Loader {
                    property color colour: "red"
                    property string label: "adjusted"
                    property var value: index < internal.lastAdjustments.length ? internal.lastAdjustments[index] : 0
                    anchors.horizontalCenter: parent.horizontalCenter
                    enabled: internal.showDebugLog
                    sourceComponent: internal.debugOutputComponent
                }
            }
        }
    }

    QtObject {
        id: internal
        property bool showDebugLog: false
        property bool loaded: false
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
                    text: value
                }
            }
        }

        /** Let columns fight for space in the available width, based on priority and ideal widths.*/
        function jostleColumnWidths() {
            if (!clamp || !internal.loaded || columnCount < 1 || handles.count === 0) {
                return;
            }

            previousWidths = columnWidths.slice();
            var currentWidths = columnWidths.slice();
            var previousImplicitWidths = internal.implicitWidths.slice();
            var currentImplicitWidths = implicitColumnWidths.slice();

            for (var i = 0; i < currentWidths.length; ++i) {
                currentWidths[i] = isNaN(currentWidths[i]) ? 0 : currentWidths[i];
            }

            for (var i = 0; i < previousImplicitWidths.length; ++i) {
                previousImplicitWidths[i] = isNaN(previousImplicitWidths[i]) ? 0 : previousImplicitWidths[i];
            }

            for (var i = 0; i < currentImplicitWidths.length; ++i) {
                currentImplicitWidths[i] = isNaN(currentImplicitWidths[i]) ? 0 : currentImplicitWidths[i];
            }

            while (currentWidths.length < columnCount) {
                currentWidths.push(0);
            }

            while (previousImplicitWidths.length < columnCount) {
                previousImplicitWidths.push(0);
            }

            while (currentImplicitWidths.length < columnCount) {
                currentImplicitWidths.push(0);
            }

            while (lastAdjustments.length < columnCount) {
                lastAdjustments.push(0);
            }

            internal.implicitWidths = currentImplicitWidths.slice();

            var currentWidth = 0;
            var implicitWidth = 0;

            for (var i = 0; i < columnCount; ++i) {
                currentWidth += currentWidths[i];
                implicitWidth += currentImplicitWidths[i];
                lastAdjustments[i] = 0;
            }

            var spacersWidth = columnSpacing * (columnCount - 1);
            var newWidth = Math.max(availableWidth - spacersWidth, 0);
            var delta = newWidth - currentWidth

            // if not resizing, rebalance according to changes in implicit widths.
            if (delta === 0) {
                var implicitDeltas = [];
                var implicitChanged = false;

                for (var i = 0; i < implicitColumnWidths.length; ++i) {
                    var currentReserve = Math.max(currentImplicitWidths[i] - currentWidths[i], 0);
                    var previousReserve = Math.max(previousImplicitWidths[i] - currentWidths[i], 0);
                    var implicitDelta = currentImplicitWidths[i] - previousImplicitWidths[i];

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
                    currentWidths[i] += implicitDeltas[i];
                    currentWidth += implicitDeltas[i];
                    delta -= implicitDeltas[i];
                }
            }

            priorityReserves = [];
            var attractiveColumns = [];
            var attractiveDelta = 0;
            var repulsiveDelta = 0;

            // decide which columns are attractive to resize, and which ones are not.
            for (var i = 0; i < columnCount; ++i) {
                var reserve = currentImplicitWidths[i] - currentWidths[i];
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
                    var columnDelta = usableDelta * priorityReserves[index] / currentAttractiveDelta;
                    columnDelta = (columnDelta < 0) ? Math.ceil(columnDelta) : Math.floor(columnDelta);
                    priorityReserves[index] -= columnDelta;
                    attractiveDelta -= columnDelta;
                    delta -= columnDelta;
                    currentWidths[index] += columnDelta;

                    if (priorityReserves[index] !== 0) {
                        var position = 0;
                        var insertValue = -priorityReserves[index] /
                            (currentWidths[index] - priorityReserves[index]);

                        for (; position < priorityColumns.length; ++position) {
                            var compareIndex = priorityColumns[position];
                            var compareValue = -priorityReserves[compareIndex] /
                                (currentWidths[compareIndex] - priorityReserves[compareIndex]);

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
                    var columnDelta = attractiveDelta < 0 ? -1 : 1;
                    priorityReserves[index] -= columnDelta;
                    attractiveDelta -= columnDelta;
                    delta -= columnDelta;
                    currentWidths[index] += columnDelta;
                }
            }

            // size everything proportionately with what's left if the attractive columns were not enough.
            if (delta !== 0) {
                currentWidth = newWidth - delta;
                var factor = delta / currentWidth;
                var priorityColumns = [];

                for (var i = 0; i < columnCount; ++i) {
                    var columnDelta = factor * currentWidths[i];

                    if (columnDelta < 0) {
                        columnDelta = Math.ceil(columnDelta);
                        columnDelta = Math.max(columnDelta, 0 - columnWidths[i]);
                    }
                    else {
                        columnDelta = Math.floor(columnDelta);
                    }

                    priorityReserves[i] -= columnDelta;
                    repulsiveDelta -= columnDelta;
                    delta -= columnDelta;
                    currentWidths[i] += columnDelta;

                    if (currentWidths[i] > 0) {
                        var position = 0;
                        var insertValue = priorityReserves[i] /
                            (currentWidths[i] + priorityReserves[i]);

                        for (; position < priorityColumns.length; ++position) {
                            var index = priorityColumns[position];
                            var compareValue = priorityReserves[index] /
                                (currentWidths[index] + priorityReserves[index]);

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
                    var columnDelta = delta < 0 ? -1 : 1;
                    priorityReserves[index] -= columnDelta;
                    repulsiveDelta -= columnDelta;
                    delta -= columnDelta;
                    currentWidths[index] += columnDelta;
                }
            }

            // change the columns frame handles.
            for (var i = 0; i < columnCount; ++i) {
                var handle = handles.itemAt(i).children[0];
                handle.x = currentWidths[i];
            }

            var adjustments = [];

            for (var i = 0; i < columnCount; ++i) {
                adjustments.push(currentWidths[i] - previousWidths[i]);
            }

            lastAdjustments = adjustments.slice();
            columnWidths = currentWidths.slice();
        }

        /** Split a fixed width between two columns. As the one increases, the other decreases.*/
        function divideWidthAroundHandle(index, desiredWidth) {
            var currentWidths = columnWidths.slice();
            var availableWidth = currentWidths[index] + currentWidths[index + 1] - minimumColumnWidth;
            var newWidth = Math.min(desiredWidth, availableWidth);
            var newWidth = Math.max(minimumColumnWidth, newWidth);
            var delta = newWidth - currentWidths[index];

            previousWidths = currentWidths.slice();
            currentWidths[index] = newWidth;
            currentWidths[index + 1] -= delta;

            var adjustments = lastAdjustments.slice();
            adjustments[index] = delta;
            adjustments[index + 1] = -delta;
            lastAdjustments = adjustments.slice();

            var handle = handles.itemAt(index).children[0];
            handle.x = currentWidths[index];
            handle = handles.itemAt(index + 1).children[0];
            handle.x = currentWidths[index + 1];

            columnWidths = currentWidths.slice();
        }
    }
}
