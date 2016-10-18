import QtQuick 2.5
import WGControls 1.0

/*!
    \ingroup wgcontrols
    \brief An item that acts as a selectable border for copy/paste type functionality
    Not intended for use as a control on its own

Example:
\code{.js}
// support copy&paste inside WGSliderControl
WGCopyable {
    id: copyableControl

    WGCopyController {
        id: copyableObject

        onDataCopied : {
            setValue( slider.value )
        }

        onDataPasted : {
            slider.value = data
            if(slider.value != data)
            {
                pasted = false;
            }
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
\endcode
*/

Rectangle {
    id: copyable
    objectName: "WGCopyable"
    WGComponent { type: "WGCopyable" }

    //----------------------------------
    // Property Declarations
    //----------------------------------

    /*! This property holds the selected state of this object.
        The default value is \c false
    */
    property bool selected: false

    /*! This property holds the number of child objects containing a WGCopyable
        The default value is \c 0
    */
    property int childrenCount: 0

    /*! This property contains the number of child objects containing a WGCopyable that are selected
        The default value is \c 0
    */
    property int selectedChildrenCount: 0

    /*! This property searches for the root copyable control.
    */
    property QtObject rootCopyable: parentCopyable === null ? copyable : parentCopyable.rootCopyable

    /*! This property is a reference to the imediate parent copyable control. It is set by a copyable above this object.
        The default value is \c null
    */
    property QtObject parentCopyable: null

    //----------------------------------
    // Signals
    //----------------------------------

    /*! children copyables listen for this and 'can' deselect themselves if all have been selected. */
    signal selectChildren()
    /*! children copyables listen for this and 'can' deselect themselves if all have been selected. */
    signal deSelectChildren()

    //----------------------------------
    // Functions
    //----------------------------------

    Component.onDestruction: {
        copyable.selected = false;
    }

    //TODO: This seems a little dependent on whether or not the lowest parent copyable runs this first... seems a bit risky.
    /*! This function Recursively finds a copyable child and sets this object as its parent if it doesn't have one. */
    function setParentCopyable(parentObject) {
        if ((parentObject === null))
        {
            return;
        }
        var found = false;
        for (var i=0; i<parentObject.children.length; i++)
        {
            if (typeof parentObject.children[i].parentCopyable !== "undefined")
            {
                if (parentObject.children[i].enabled)
                {
                    if (parentObject.children[i] !== copyable)
                    {
                        if (parentObject.children[i].parentCopyable === null)
                        {
                            parentObject.children[i].parentCopyable = copyable
                            copyable.childrenCount++;
                            found = true;
                        }
                    }
                }
            }
        }
        if (!found)
        {
            for (var i=0; i<parentObject.children.length; i++)
            {
                if (typeof parentObject.children[i].parentCopyable !== "undefined")
                {
                    continue;
                }
                else
                {
                    setParentCopyable(parentObject.children[i])
                }
            }
        }
    }

    /*! This function recursively finds copyable children and disables them, as this object
        will be copied as a whole instead of copying its children separately
    */
    function disableChildrenCopyable(parentObject) {
        if ((parentObject === null))
        {
            return;
        }
        for (var i=0; i<parentObject.children.length; i++)
        {
            if (typeof parentObject.children[i].parentCopyable !== "undefined")
            {
                if (parentObject.children[i].enabled)
                {
                    if (parentObject.children[i] !== copyable)
                    {
                        parentObject.children[i].enabled = false;
                         parentObject.children[i].visible = false;
                    }
                }
            }
            else
            {
                disableChildrenCopyable( parentObject.children[i] );
            }
        }
    }

    /*! This function selects this object and toggles all selected */
    function select(){
        copyable.selected = true
        selectChildren(copyable)
    }

    /*! This function deselects this object and its children */
    function deSelect(){
        //deselects this object
        copyable.selected = false
        //deselects all children
        deSelectChildren(copyable)
    }

    onSelectedChanged: {
        if (parentCopyable === null)
        {
            return;
        }

        if (!selected)
        {
            parentCopyable.selected = false;
            parentCopyable.selectedChildrenCount--;
        }
        else
        {
            parentCopyable.selectedChildrenCount++;
            if (parentCopyable.selectedChildrenCount === parentCopyable.childrenCount)
            {
                parentCopyable.selected = true;
            }
        }
    }

    //----------------------------------
    // Object Properties
    //----------------------------------

    anchors.fill: visible ? parent : undefined
    anchors.leftMargin: -1
    anchors.rightMargin: -1

    //for some reason without this the control is 1 pixel too low.
    anchors.topMargin: -2

    radius: defaultSpacing.halfRadius

    color: "transparent"
    border.width: defaultSpacing.standardBorderSize
    border.color: "transparent"

    enabled: parent.enabled
    visible: parent.visible

    Connections {
        target: parentCopyable
        onSelectChildren: {
            select();
        }
        //listen for parent being deselected (only if all children are selected) and pass that through to this objects children
        onDeSelectChildren: {
            deSelect()
        }
    }

    //----------------------------------
    // Child Objects
    //----------------------------------

    //Click area over the control when Ctrl key is held.
    MouseArea {
        id: copySelect
        anchors.fill: copyable.visible ? parent : undefined
        enabled: copyable.enabled && copyable.visible && globalSettings.wgCopyableEnabled

        hoverEnabled: enabled
        cursorShape: hoverEnabled ? Qt.PointingHandCursor : Qt.ArrowCursor

        preventStealing: true

        onClicked: {
            if ((mouse.button == Qt.LeftButton) && (mouse.modifiers & Qt.ControlModifier))
            {
                if (copyable.selected)
                {
                        copyable.deSelect()
                }
                else
                {
                        copyable.select()
                }
            }
            else if (mouse.button == Qt.LeftButton)
            {
                copyable.rootCopyable.deSelect();
                copyable.select();
            }
        }
    }

    //----------------------------------
    // States
    //----------------------------------

    states: [
        State {
            //clicked state
            name: "SELECTED"
            when: copyable.selected && copyable.enabled && copyable.visible && globalSettings.wgCopyableEnabled
            PropertyChanges {
                target: copyable
                border.color: "transparent"
                color: palette.highlightShade
            }
        },
        State {
            //border when copy key held (Ctrl)
            name: "COPYACTIVE"
            when: !copySelect.containsMouse && !copyable.selected
                  && copyable.enabled && copyable.visible && globalSettings.wgCopyableEnabled
                  && globalSettings.wgCopyableEnabled
            PropertyChanges {
                target: copyable
                color: "transparent"
                border.color: palette.lighterShade
            }
        },
        State {
            //border when mouseovered
            name: "HOVERED"
            when: copySelect.containsMouse && !copyable.selected
                  && copyable.enabled && copyable.visible && globalSettings.wgCopyableEnabled
                  && globalSettings.wgCopyableEnabled
            PropertyChanges {
                target: copyable
                color: "transparent"
                border.color: palette.highlightShade
            }
        }
    ]
}
