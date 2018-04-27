import QtQuick 2.5
import QtQuick.Controls 1.4

import WGControls 2.0
import WGControls.Private 2.0

WGReflectedEnum {
    id: enumComponent
    objectName: itemData == null || typeof itemData.indexPath == "undefined" ? "enum_component" : itemData.indexPath
    anchors.left: parent ? parent.left : undefined
    anchors.right: parent ? parent.right : undefined
    enabled: itemData != null &&
        (typeof itemData.enabled == "undefined" || itemData.enabled) &&
        (typeof itemData.readOnly == "undefined" || !itemData.readOnly) &&
        (typeof readOnlyComponent == "undefined" || !readOnlyComponent)
    multipleValues: itemData != null && typeof itemData.value == "undefined" && itemData.multipleValues

    property var objectHierarchy: itemData != null ? itemData.objectHierarchy : null
    object: {
        var objects = objectHierarchy ? objectHierarchy : null;
        return (objects != null) && objects.count() > 0 ? objects.value(0) : null
    }
    path: {
        var fullPath = itemData != null ? itemData.path : "";
        return fullPath.substr(fullPath.lastIndexOf('.') + 1);
    }
    value: itemData != null ? itemData.value : null

    onValueChanged: {
        if(itemData == null)
            return;
        if(!itemData.multipleValues) {
            itemData.value = value;
        } else {
            beginUndoFrame();
            itemData.value = value;
            endUndoFrame();
        }
    }

    function convertToColor(data)
    {
        if(typeof data != 'string')
        {
            if(isValidColor(data))
            {
                return Qt.rgba(data.x, data.y, data.z, data.w != undefined ? data.w : 1);
            }
        }
        return data;
    }

    readonly property bool isModelValid: typeof model != "undefined"  && model != null && enumComponent.itemCount() > 0 && currentIndex >= 0

    delegate: WGDropDownDelegate {
        id: listDelegate
        objectName: "DropDownDelegate_" + index + "_" + text
        property string image: enumComponent.imageRole ? (Array.isArray(enumComponent.model) ? modelData[enumComponent.imageRole] : model[enumComponent.imageRole]) : ""
        parentControl: enumComponent
        width: Math.max(parentControl.labelMaxWidth + (parentControl.imageRole ? enumComponent.height : 0) + (showRowIndicator ? enumComponent.height : 0) + (defaultSpacing.doubleMargin * 2), enumComponent.width)
        text: enumComponent.convertToColor(enumComponent.textRole ? (Array.isArray(enumComponent.model) ? modelData[enumComponent.textRole] : model[enumComponent.textRole]) : modelData)

        checkable: true
        autoExclusive: true
        checked: enumComponent.currentIndex === index
        highlighted: enumComponent.highlightedIndex === index
        pressed: highlighted && enumComponent.pressed

        onTextChanged: {
            updateLabelWidths()
        }

        function updateLabelWidths() {
            if (listDelegate.text != "")
            {
                var oldString = enumComponent.maxTextString.text
                var oldWidth = enumComponent.maxTextString.width
                enumComponent.maxTextString.text = listDelegate.text
                enumComponent.maxTextString.text = enumComponent.maxTextString.width > oldWidth ? listDelegate.text : oldString
            }
        }

        //! [label]
        label: Rectangle {
            id: label
            width: parentControl.labelMaxWidth + (parentControl.imageRole ? delegateImage.width : 0) + (parentControl.showRowIndicator ? indicator.width : 0)
            height: parentControl.height
            x: listDelegate.mirrored ? listDelegate.width - width - listDelegate.rightPadding : listDelegate.leftPadding
            anchors.verticalCenter: parent.verticalCenter
            color: "transparent"

            Rectangle {
                id: delegateColor
                objectName: "DelegateColor"
                anchors.verticalCenter: parent.verticalCenter
                anchors.left: parent.left
                width: indicator.x - listDelegate.rightPadding
                height: parentControl.imageMaxHeight
                visible: isValidColor(listDelegate.text)
                color: isValidColor(listDelegate.text) ? listDelegate.text : "transparent"
            }

            Item {
                id: delegateImage
                objectName: "DelegateImage"
                anchors.verticalCenter: parent.verticalCenter
                height: parentControl.imageMaxHeight
                width: height

                visible: listDelegate.image && !delegateColor.visible

                Loader {
                    anchors.centerIn: parent
                    sourceComponent: imageDelegate
                }
            }

            WGLabel {
                id: delegateLabel
                anchors.left: delegateImage.visible ? delegateImage.right : parent.left
                anchors.right: parent.right
                anchors.verticalCenter: parent.verticalCenter

                anchors.leftMargin: delegateImage.visible ? defaultSpacing.standardMargin : 0

                text: listDelegate.text
                elide: Text.ElideRight
                visible: listDelegate.text && !delegateColor.visible
                horizontalAlignment: Text.AlignLeft
                verticalAlignment: Text.AlignVCenter
            }
        }
        //! [label]

    }


    contentItemDelegateComponent: Item {
        Rectangle {
            id: contentColor
            objectName: "ContentColor"
            anchors.verticalCenter: parent.verticalCenter
            anchors.left: parent.left
            width: parent.width - indicatorWidth - enumComponent.rightPadding
            height: enumComponent.imageMaxHeight
            visible: isModelValid && isValidColor(enumComponent.getItem(enumComponent.currentIndex).value)
            color: isModelValid && isValidColor(enumComponent.getItem(enumComponent.currentIndex).value) ? enumComponent.convertToColor(enumComponent.getItem(enumComponent.currentIndex).value): "transparent"
        }

        Item {
            id: contentImage
            objectName: "ContentImage"
            anchors.verticalCenter: parent.verticalCenter
            height: enumComponent.imageMaxHeight
            width: height
            visible: enumComponent.imageRole && !contentColor.visible

            Loader {
                anchors.centerIn: parent
                sourceComponent: enumComponent.multipleValues ? multiImageDelegate : imageDelegate
            }
        }


        WGLabel {
            anchors.left: contentImage.visible ? contentImage.right : parent.left
            anchors.right: parent.right
            anchors.verticalCenter: parent.verticalCenter

            anchors.leftMargin: contentImage.visible ? defaultSpacing.standardMargin : 0

            text: enumComponent.multipleValues ? __multipleValuesString : enumComponent.currentText
            font.italic: enumComponent.multipleValues ? true : false
            elide: Text.ElideRight
            horizontalAlignment: Text.AlignLeft
            verticalAlignment: Text.AlignVCenter
            visible: !contentColor.visible
        }
    }
}
