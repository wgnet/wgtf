import QtQuick 2.5
import QtQuick.Controls 1.4
import QtQuick.Layouts 1.3

import WGControls.Layouts 1.0
import WGControls.Private 1.0

/*!
    \ingroup wgcontrols
    \brief A Column layout where every child has a draggable handle on the left hand side.
    Non invasive drag handles only visible when unlocked and mouse over.
    The handle can be dragged to a new position, then the actual children are shuffled into place
    Every child MUST be able to use anchors: eg WGSubPanel, WGExpandingRowLayout
    Drop location is indicated by a ghost cell.

//WGBoolGridLayout etc. must be inside a WGColumnLayout or they will cause errors!!!!

Example:
\code{.js}
WGScrollPanel {

    childObject :
    WGDraggableColumn {

        WGSubPanel {
            text: "Form Layout"
            childObject :
                WGFormLayout {
                    id: topForm
                    localForm: true

                    WGLabel {
                        text: "Import Files"
                    }

                    WGPushButton {
                        label: "Import"
                        text: "..."
                    }
                }
            }
        }
}
\endcode
*/

WGColumnLayout {
    id: mainColumn
    objectName: "WGDraggableColumn"
    WGComponent { type: "WGDraggableColumn" }
    
    rows: children.length + 1
    columns: 1

    /*! This property contains the panel being dragged. */
    property QtObject draggedObject

    /*! This property contains the row index of the panel being dragged
        The default value is \c -1 */
    property int dragItemIndex: -1

    /*! This property contains the dragHandle underneath the dragged panel */
    property QtObject targetObject

    /*! This property contains the drop target index of dragged panel
        The default value is \c -1 */
    property int dropTarget: -1

    /*! This property contains the fake object at the end of the column so panels can be dragged to the bottom */
    property QtObject lastSpacer

    /*! This property indicates when a has a dragged object has been dragged over a space */
    property bool hovering: false

    /*! This property toggles the lock settings of the draggable state.
        When locked you cannot drag the column */
    property bool unlocked: !globalSettings.dragLocked

    //if something is being dragged, give the fake object a size
    onDragItemIndexChanged: dragItemIndex > -1 ? lastSpacer.lineSpaces_ = 5 : lastSpacer.lineSpaces_ = 0

    Component.onCompleted: {
        //create an invisible object to be the last space in the column
        var bottomRow = Qt.createComponent("../WGVerticalSpacer.qml");
        if (bottomRow.status === Component.Ready)
        {
            lastSpacer = bottomRow.createObject(mainColumn, {
                                       "Layout.alignment": Qt.AlignTop,
                                       "Layout.fillWidth": true,
                                       "lineSpaces_": 0
                                   });
        }

        var shuffleRow = 0
        for (var i = 0; i < children.length; i++)
        {
            //shuffle the draggable children up a row and generate a DragHandle behind them
            children[i].Layout.row = shuffleRow
            children[i].Layout.column = 0
            shuffleRow++
            var newHandle = Qt.createComponent("../Private/WGDragHandle.qml");
            if (newHandle.status === Component.Ready)
            {
                newHandle.createObject(mainColumn.children[i], {
                                           "index": i,
                                           "z": -1,
                                           "dragLayout": mainColumn,
                                       });
            }
            children[i].Layout.alignment = Qt.AlignBottom
        }

        columns = 2
        rows = children.length - 1
    }

    flow: GridLayout.TopToBottom

    /*! Deprecated */
    property alias hovering_: mainColumn.hovering

    /*! Deprecated */
    property alias unLocked_: mainColumn.unlocked
}
