import QtQuick 2.0

/*!
 \brief Controls behaviour for resizable column widths and spacing.
 Adds the mouse handles between columns.
 */
Row {
    id: columnsFrame
    width: childrenRect.width

    property var columnWidths: []
    property var implicitColumnWidths: []
    property real columnSpacing: 0
    property real availableWidth: 0
    property bool clamp: true

	property Component handleDelegate : Item {}

    onAvailableWidthChanged: {
        internal.jostleColumnWidths();
    }

    Component.onCompleted: {
        internal.initialColumnWidths = columnWidths
        internal.loaded = true;
        internal.jostleColumnWidths();
    }

    Repeater {
        id: handles
        model: internal.initialColumnWidths

        Item {
            width: handle.x + handle.width
            height: columnsFrame.height

            Item {
                id: handle
                visible: !clamp || index < columnWidths.length - 1
                x: internal.initialColumnWidths[index]
                width: columnSpacing
                height: columnsFrame.height

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
                    drag.minimumX: 1
                    drag.axis: Drag.XAxis
                    
                    onDoubleClicked: {
                        if (clamp) {
                            internal.divideWidthAroundHandle(index, implicitColumnWidths[index]);
                        }
                        else {
                            handle.x = Math.max(drag.minimumX, implicitColumnWidths[index]);
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
        }
    }

    QtObject {
        id: internal
        property var initialColumnWidths: []
        property bool loaded: false

        function jostleColumnWidths(adjustedIndex, adjustedColumnWidth) {
            if (!clamp || !internal.loaded) {
                return;
            }
            
            var columnCount = columnWidths.length

            if (columnCount < 1 || handles.count === 0) {
                return;
            }

            var currentWidths = columnWidths;
            var currentWidth = 0;
            var implicitWidths = implicitColumnWidths;
            var implicitWidth = 0;

            while (implicitWidths.length < currentWidths.length) {
                implicitWidths.push(0);
            }

            for (var i = 0; i < columnCount; ++i) {
                implicitWidths[i] = isNaN(implicitWidths[i]) ? 0 : implicitWidths[i];

                currentWidth += currentWidths[i];
                implicitWidth += implicitWidths[i];
            }

            var spacersWidth = columnSpacing * (columnCount - 1);
            var newWidth = Math.max(availableWidth - spacersWidth, 0);
            var delta = newWidth - currentWidth

            if (delta === 0) {
                return;
            }

            var priorityReserves = [];
            var attractiveColumns = [];
            var attractiveDelta = 0;
            var repulsiveDelta = 0;
        
            // decide which columns are attractive to resize, and which ones are not.
            for (var i = 0; i < columnCount; ++i) {
                var reserve = implicitWidths[i] - currentWidths[i];
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

            columnWidths = currentWidths;
        }

        function divideWidthAroundHandle(index, desiredWidth) {
            var minimumWidth = 1;
            var currentWidths = columnWidths;
            var availableWidth = currentWidths[index] + currentWidths[index + 1] - minimumWidth;
            var newWidth = Math.min(desiredWidth, availableWidth);
            var newWidth = Math.max(minimumWidth, newWidth);
            var delta = newWidth - currentWidths[index];

            currentWidths[index] = newWidth;
            currentWidths[index + 1] =  currentWidths[index + 1] - delta;

            var handle = handles.itemAt(index).children[0];
            handle.x = currentWidths[index];
            handle = handles.itemAt(index + 1).children[0];
            handle.x = currentWidths[index + 1];

            columnWidths = currentWidths;
        }
    }
}
