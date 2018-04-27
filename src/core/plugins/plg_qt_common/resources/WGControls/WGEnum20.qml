import QtQuick 2.5
import WGControls 2.0

WGDropDownBox {
    id: dropDownBox
    objectName: "WGEnum"
    WGComponent { type: "WGEnum20" }

    textRole: "value"

    property var valueInt
    property var valueStr

    property var enumString: null
    property var enumGenerator: null

    property var storedModel: model

    QtObject {
        id: internal
        property var stringModel: ListModel {}
        property var intToStr: {}
        property var strToIndex: {}
        property var ignoreChange: false
    }

    Component.onCompleted: {
        internal.ignoreChange = true
        updateModel();
        internal.ignoreChange = false
        updateCurrentIndex();
    }

    onActivated: {
        if (activeFocus && currentIndex == -1 && valueInt != -1) {
            currentIndex = valueInt;
        }
    }

    onCurrentIndexChanged: {
        if (currentIndex < 0 || internal.ignoreChange || model == null) {
            return;
        }

        var item = getItem(currentIndex);
        if (valueInt != item.key) {
            setValueHelper(dropDownBox, "valueInt", item.key);
        }
        if (valueStr != item.value) {
            setValueHelper(dropDownBox, "valueStr", item.value);
        }
    }

    onValueIntChanged: {
        if (model == null) {
            return;
        }
        var value = null;
        if (valueInt in internal.intToStr) {
            value = internal.intToStr[valueInt];
        }
        if (value != null && valueStr != value) {
            setValueHelper(dropDownBox, "valueStr", value);
        }
    }

    onValueStrChanged: {
        updateCurrentIndex();
    }

    // just for the purpose of dynamically generating dropdown list when users click on the dropdownbox
    onPressedChanged: {
        if( pressed && !popup.visible )
        {
            // When the property has MetaInvalidatesObject we need to avoid changes because it will cause the UI to refresh
            // causing the dropdown to not show, so we suppress change notifications
            internal.ignoreChange = true
            var oldCurrentIndex = currentIndex
            updateModel()
            currentIndex = oldCurrentIndex
            internal.ignoreChange = false
        }
    }

    function itemCount() {
        if (model == null)
        {
            return 0;
        }

        return model == internal.stringModel ? model.count : model.count();
    }

    function getItem(index) {
        if (model == null)
        {
            return null;
        }

        return model == internal.stringModel ? model.get(index) : model.item(index);
    }

    function updateModel() {
        model = null;
        if (enumString && enumString.length > 0) {
            internal.stringModel.clear();
            var index = 0;
            var strings = enumString.split("|");
            for (var i = 0; i < strings.length; ++i) {
                var tokens = strings[i].split("=");
                var enumValue = tokens[0];
                var enumKey = index;
                if (tokens.length > 1) {
                    enumKey = tokens[1];
                    index = parseInt(enumKey);
                    if (isNaN(index)) {
                        index = i;
                    }
                }
                internal.stringModel.insert(i, {"key": enumKey.toString(), "value": enumValue});
                ++index;
            }
            model = internal.stringModel;
        }
        else if (enumGenerator) {
            model = enumGenerator();
        }

        if (model == null) {
            return;
        }

        internal.intToStr = {};
        internal.strToIndex = {};

        var count = itemCount();
        for (var i = 0; i < count; ++i) {
            var item = getItem(i);
            var itemValueInt = item.key;
            var itemValueStr = item.value;
            internal.intToStr[itemValueInt] = itemValueStr;
            internal.strToIndex[itemValueStr] = i;
        }
    }

    function updateCurrentIndex() {
        if (typeof model == 'undefined') {
            return;
        }

        if (model == null) {
            return;
        }

        if (valueStr in internal.strToIndex) {
            var index = internal.strToIndex[valueStr];
            if (currentIndex != index) {
                currentIndex = index;
            }
        }
        else {
            if (currentIndex != -1) {
                currentIndex = -1;
            }
        }
    }
}

