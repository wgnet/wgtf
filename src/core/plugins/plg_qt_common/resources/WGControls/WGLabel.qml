import QtQuick 2.3
import QtQuick.Controls 1.2
import QtQuick.Layouts 1.1

/*!
 \brief A non-editable single line of text that can align to a panel wide width in defaultSpacing
 Will appear in the left column if placed in a WGFormLayout && formLabel == true

\code{.js}
WGLabel {
    text: "Example text"
}
\endcode
*/

Text {
    id: labelText
    objectName: "WGLabel"

    /*! This property right aligns the label and sets width to the largest label in the panel.
        The default value is false
    */
    property bool formLabel: false

    /*! This property ignores the panel wide label column width
        The default is false
    */
    property bool localForm: false

    property QtObject linkedFormObject: Item{}

    enabled: linkedFormObject.enabled


    /*
        Links the label to it's control object and then finds the copyable inside it.
        Only works with form labels.
        @param type:object parentObject The parent control object
    */
    /*! \internal */
    function selectLabelControl(parentObject){
        for (var i=0; i<parentObject.children.length; i++)
        {
            if (parentObject.children[i].label_ == labelText.text)
            {
                selectControlCopyable(parentObject.children[i])
                break;
            }
            else
            {
                selectLabelControl(parentObject.children[i])
            }
        }
    }

    function selectControlCopyable(parentObject){
        for (var i=0; i<parentObject.children.length; i++)
        {
            if (typeof parentObject.children[i].rootCopyable != "undefined")
            {
                formControlCopyable_ = parentObject.children[i]
            }
        }
    }

    Component.onCompleted: {
        if (formLabel && paintedWidth > defaultSpacing.labelColumnWidth && !localForm)
        {
            defaultSpacing.labelColumnWidth = paintedWidth;
        }

        if (formLabel)
        {
            selectLabelControl(labelText.parent)
        }
    }

    width: formLabel && !localForm ? defaultSpacing.labelColumnWidth: implicitWidth

    Layout.preferredWidth: formLabel && !localForm ? defaultSpacing.labelColumnWidth : implicitWidth

    color: enabled ? palette.textColor : palette.disabledTextColor

    renderType: globalSettings.wgNativeRendering ? Text.NativeRendering : Text.QtRendering

    smooth: true

    horizontalAlignment: formLabel ? Text.AlignRight : Text.AlignLeft

    MouseArea {
        anchors.fill: parent
        enabled: labelText.formLabel
        hoverEnabled: labelText.formLabel
        cursorShape: labelText.formLabel ? Qt.PointingHandCursor : Qt.ArrowCursor

        onClicked:{
            if ((formControlCopyable_ === null) || (!formControlCopyable_.enabled))
            {
                return;
            }
            if (!globalSettings.wgCopyableEnabled)
            {
                return;
            }

            if ((mouse.button == Qt.LeftButton) && (mouse.modifiers & Qt.ControlModifier))
            {
                if (formControlCopyable_.selected)
                {
                    formControlCopyable_.deSelect()
                }
                else
                {
                    formControlCopyable_.select()
                }
            }
            else if (mouse.button == Qt.LeftButton)
            {
                formControlCopyable_.rootCopyable.deSelect();
                formControlCopyable_.select()
            }
        }
    }

    /*! Deprecated */
    property alias formLabel_: labelText.formLabel

    /*! Deprecated */
    property alias localForm_: labelText.localForm
}
