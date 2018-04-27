import QtQuick 2.3
import QtQuick.Controls 1.2
import QtQuick.Layouts 1.0


import WGControls 2.0
import WGControls.Private 2.0
import WGControls.Layouts 2.0
import WGControls.Views 2.0
import WGControls.Global 2.0

WGPanel {
    id: themePanel
    WGComponent { type: "CustomUIThemesDialog" }

    title: qsTr("QML Preferences Panel")
    layoutHints: { 'floatingPanel': 1.0 }

    property color newCustomWindowColor: "#000000"
    property color newCustomHighlightColor: "#FFFFFF"

    property bool customTheme: palette.theme == 6

    property int tempTheme: palette.theme

    property int initialTheme: palette.theme
    property color initialWindowColor: palette.mainWindowColor
    property color initialHighlightColor: palette.highlightColor
    property color initialReadonlyColor: palette.readonlyColor
    property color initialCustomWindowColor: palette.customWindowColor
    property color initialCustomHighlightColor: palette.customHighlightColor
    property color initialCustomReadonlyColor: palette.customReadonlyColor
    property bool initialDarkText: palette.darkText
    property real initialLightContrast: palette.lightContrast
    property real initialDarkContrast: palette.darkContrast
    property string initialCode: ""

    property bool newTheme: tempTheme != palette.theme || customTheme ? true : false
    property bool themeChanged: false

    property bool customChanged: false

    property bool stayOnTop: true

    property bool validCode: testThemeCode(themeCode)

    property string themeCode: validCode ? "" : "ERROR"

    signal newCode

    function generateCode() {
        var code = []
        code.push(customTheme ? palette.customWindowColor : palette.mainWindowColor)
        code.push(customTheme ? palette.customHighlightColor : palette.highlightColor)
        code.push(customTheme ? palette.customReadonlyColor : palette.readonlyColor)
        code.push(customTheme ? (palette.customDarkText ? 1 : 0) : (palette.darkText ? 1 : 0))
        code.push(customTheme ? palette.customDarkContrast : palette.darkContrast)
        code.push(customTheme ? palette.customLightContrast : palette.lightContrast)
        themePanel.themeCode = code.toString()
    }

    function testThemeCode(codeString) {
        var newCode = codeString.split(",")
        var testCode = newCode.length == 6 ? true : false
        if (testCode)
        {
            var colRegEx = new RegExp(/^#([A-Fa-f0-9]{8}|[A-Fa-f0-9]{6}|[A-Fa-f0-9]{3})$/)
            for (var c = 0; c < 3; c++)
            {
                if (!colRegEx.test(newCode[c].toString()))
                {
                    testCode = false
                }
            }
            if (isNaN(newCode[3]) || newCode[3] < 0 || newCode[3] > 1)
            {
                testCode = false
            }

            if (isNaN(newCode[4]) || isNaN(newCode[5]))
            {
                testCode = false
            }
        }
        return testCode
    }

    onNewCode: {
        if (testThemeCode(themeCode))
        {
            var newCode = themeCode.split(",")
            palette.customWindowColor = newCode[0]
            palette.customHighlightColor = newCode[1]
            palette.customReadonlyColor = newCode[2]
            if (newCode[3] == 0)
            {
                palette.customDarkText == false
            }
            else if (newCode[3] == 1)
            {
                palette.customDarkText == true
            }
            palette.customDarkContrast = newCode[4]
            palette.customLightContrast = newCode[5]
        }
    }

    color: palette.mainWindowColor

    focus: true

    width: 530
    height: 345

    Component.onCompleted: {
        themePanel.initialTheme = palette.theme
        themePanel.tempTheme = palette.theme
        themePanel.initialWindowColor = palette.mainWindowColor
        themePanel.initialHighlightColor = palette.highlightColor
        themePanel.initialReadonlyColor = palette.readonlyColor
        themePanel.initialCustomWindowColor = palette.customWindowColor
        themePanel.initialCustomHighlightColor = palette.customHighlightColor
        themePanel.initialCustomReadonlyColor = palette.customReadonlyColor
        themePanel.initialDarkText = palette.darkText
        themePanel.initialLightContrast = palette.lightContrast
        themePanel.initialDarkContrast = palette.darkContrast
        themePanel.newTheme = false
        themePanel.themeChanged = false
        themePanel.customChanged = false

        generateCode()
        themePanel.initialCode = themeCode
    }

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: defaultSpacing.standardMargin
        WGFormLayout {

            WGExpandingRowLayout {
                Layout.fillWidth: true
                Layout.preferredHeight: defaultSpacing.minimumRowHeight

                label: "Current Theme "

                WGDropDownBox {
                    id: themeDropDown

                    textRole: "label"
                    imageRole: "windowColor"

                    model: ListModel {
                        dynamicRoles: true
                    }

                    Layout.preferredWidth: 230 + applyThemeButtons.spacing
                    Layout.preferredHeight: defaultSpacing.minimumRowHeight

                    currentIndex: palette.theme

                    onCurrentIndexChanged: {
                        if (currentIndex != palette.theme)
                        {
                            palette.theme = currentIndex
                            tempTheme = currentIndex
                            themePanel.themeChanged = true
                            themePanel.newTheme = true
                            generateCode()
                        }
                    }

                    imageDelegate: Rectangle {
                        id: imageDelegate
                        height: themeDropDown.imageMaxHeight
                        width: themeDropDown.imageMaxHeight
                        radius: defaultSpacing.standardRadius
                        color: themeDropDown.model.count > 0 ? themeDropDown.model.get(themeDropDown.currentIndex).windowColor : "white"

                        Rectangle {
                            anchors.fill: parent
                            anchors.margins: 4
                            radius: defaultSpacing.halfRadius
                            color: themeDropDown.model.count > 0 ? themeDropDown.model.get(themeDropDown.currentIndex).highlightColor : "black"
                        }
                    }

                    delegate: WGDropDownDelegate {
                        id: listDelegate
                        property string image: "true"
                        parentControl: themeDropDown
                        width: Math.max(themeDropDown.labelMaxWidth + (themeDropDown.imageRole ? themeDropDown.height : 0) + (themeDropDown.showRowIndicator ? themeDropDown.height : 0) + (defaultSpacing.doubleMargin * 2), themeDropDown.width)
                        text: themeDropDown.textRole ? (Array.isArray(themeDropDown.model) ? modelData[themeDropDown.textRole] : model[themeDropDown.textRole]) : modelData
                        checkable: true
                        autoExclusive: true
                        checked: themeDropDown.currentIndex === index

                        imageDelegate: Rectangle {
                            id: imageDropDownDelegate
                            height: parentControl.imageMaxHeight
                            width: parentControl.imageMaxHeight
                            radius: defaultSpacing.standardRadius
                            color: parentControl.model.get(index).windowColor

                            Connections {
                                target: themePanel
                                onCustomChangedChanged: {
                                    imageDropDownDelegate.color = parentControl.model.get(index).windowColor
                                    imageDropDownInnerDelegate.color = parentControl.model.get(index).highlightColor
                                }
                                onNewThemeChanged: {
                                    imageDropDownDelegate.color = parentControl.model.get(index).windowColor
                                    imageDropDownInnerDelegate.color = parentControl.model.get(index).highlightColor
                                }
                            }

                            Rectangle {
                                id: imageDropDownInnerDelegate
                                anchors.fill: parent
                                anchors.margins: 4
                                radius: defaultSpacing.halfRadius
                                color: parentControl.model.get(index).highlightColor
                            }
                        }

                        onTextChanged: {
                            updateLabelWidths()
                        }

                        function updateLabelWidths() {
                            if (listDelegate.text != "")
                            {
                                var oldString = themeDropDown.maxTextString.text
                                var oldWidth = themeDropDown.maxTextString.width
                                themeDropDown.maxTextString.text = listDelegate.text
                                themeDropDown.maxTextString.text = themeDropDown.maxTextString.width > oldWidth ? listDelegate.text : oldString
                            }
                        }
                    }

                    Component.onCompleted:
                    {
                        model.append({"label": "Dark", "windowColor": "#383a3d", "highlightColor": "#3399ff"})
                        model.append({"label": "Light", "windowColor": "#d8d8d8", "highlightColor": "#438bbf"})
                        model.append({"label": "Battle Red", "windowColor": "#333333", "highlightColor": "#b0302c"})
                        model.append({"label": "Army Brown", "windowColor": "#2e2c27", "highlightColor": "#e5a139"})
                        model.append({"label": "Air Force Green", "windowColor": "#27393d", "highlightColor": "#39b2cf"})
                        model.append({"label": "Navy Blue", "windowColor": "#1d3340", "highlightColor": "#15a3d2"})
                        model.append({"label": "Custom", "windowColor": palette.customWindowColor, "highlightColor": palette.customHighlightColor})
                    }
                }
                WGPushButton {
                    text: "Set as Custom"
                    enabled: themeDropDown.currentText != "Custom"
                    Layout.preferredWidth: 100 + parent.spacing
                    onClicked: {
                        palette.customWindowColor = palette.mainWindowColor
                        palette.customHighlightColor = palette.highlightColor
                        palette.customReadonlyColor = palette.readonlyColor
                        palette.customDarkText = palette.darkText
                        palette.customDarkContrast = palette.darkContrast
                        palette.customLightContrast = palette.lightContrast
                        themeDropDown.currentIndex = themeDropDown.count - 1
                        themePanel.newTheme = true
                        themePanel.themeChanged = true
                        palette.theme = tempTheme
                    }
                }

                Item {
                    Layout.fillWidth: true
                    Layout.preferredHeight: defaultSpacing.minimumRowHeight
                }
            }

            WGExpandingRowLayout {
                Layout.fillWidth: true
                Layout.preferredHeight: defaultSpacing.minimumRowHeight
                label: "Theme Code"
                WGTextBox {
                    id: themeCodeBox
                    Layout.preferredWidth: themeDropDown.width
                    text: validCode ? themeCode : "!!ERROR!!"
                    readOnly: true
                }
                WGPushButton {
                    text: "Copy"
                    Layout.preferredWidth: 50
                    onClicked: {
                        themeCodeBox.selectAll()
                        themeCodeBox.copy()
                        themeCodeBox.deselect()
                    }
                }
                WGPushButton {
                    text: "Paste"
                    Layout.preferredWidth: 50
                    onClicked: {
                        var oldCode = themeCode
                        themeCodeBox.readOnly = false
                        themeCodeBox.selectAll()
                        themeCodeBox.paste()
                        themeCodeBox.deselect()
                        themeCodeBox.readOnly = true

                        themeCode = themeCodeBox.text
                        themePanel.newCode()

                        if (validCode)
                        {
                            themeDropDown.currentIndex = themeDropDown.count - 1
                            themePanel.newTheme = true
                            themePanel.themeChanged = true
                            palette.theme = tempTheme
                        }
                        else
                        {
                            console.error("CustomUIThemesDialog.QML: Bad Theme Code Entered.")
                        }

                        themeCodeBox.text = Qt.binding(function (){return validCode ? themeCode : "!!ERROR!!"})
                    }
                }
            }

            Item {
                Layout.fillWidth: true
                Layout.preferredHeight: defaultSpacing.minimumRowHeight
            }

            WGColorButton {
                id: mwColor
                label: "Custom Window Color "
                color: customTheme ? Qt.vector4d(palette.customWindowColor.r,palette.customWindowColor.g,palette.customWindowColor.b,palette.customWindowColor.a) :
                                     Qt.vector4d(palette.mainWindowColor.r,palette.mainWindowColor.g,palette.mainWindowColor.b,palette.mainWindowColor.a)
                defaultColorDialog: false
                enabled: customTheme
                onClicked: {
                    openDialog()
                }

                onColorChosen: {
                    var tempColor = Qt.rgba(selectedColor.x,selectedColor.y,selectedColor.z,selectedColor.w)
                    palette.customWindowColor = tempColor
                    if(customTheme)themeDropDown.model.set(6, {"windowColor": tempColor})
                    themePanel.newTheme = true
                    themePanel.themeChanged = true
                    palette.theme = tempTheme
                }
            }

            WGColorButton {
                id: hlColor
                label: "Custom Highlight Color "
                color: customTheme ? Qt.vector4d(palette.customHighlightColor.r,palette.customHighlightColor.g,palette.customHighlightColor.b,palette.customHighlightColor.a) :
                                     Qt.vector4d(palette.highlightColor.r,palette.highlightColor.g,palette.highlightColor.b,palette.highlightColor.a)
                defaultColorDialog: false
                enabled: customTheme
                onClicked: {
                    openDialog()
                }

                onColorChosen: {
                    var tempColor = Qt.rgba(selectedColor.x,selectedColor.y,selectedColor.z,selectedColor.w)
                    palette.customHighlightColor = tempColor
                    if(customTheme)themeDropDown.model.set(6, {"highlightColor": tempColor})
                    themePanel.newTheme = true
                    themePanel.themeChanged = true
                    palette.theme = tempTheme
                }
            }

            WGCheckBox {
                label: "Dark Text "
                checked: customTheme ? palette.customDarkText : palette.darkText

                enabled: customTheme

                onClicked: {
                    palette.customDarkText = checked
                    themePanel.newTheme = true
                    themePanel.themeChanged = true
                    palette.theme = tempTheme
                }
            }

            WGColorButton {
                id: roColor
                label: "Read Only Text Color "
                color: customTheme ? Qt.vector4d(palette.customReadonlyColor.r,palette.customReadonlyColor.g,palette.customReadonlyColor.b,palette.customReadonlyColor.a) :
                                     Qt.vector4d(palette.readonlyColor.r,palette.readonlyColor.g,palette.readonlyColor.b,palette.readonlyColor.a)
                defaultColorDialog: false
                enabled: customTheme
                onClicked: {
                    openDialog()
                }

                onColorChosen: {
                    var tempColor = Qt.rgba(selectedColor.x,selectedColor.y,selectedColor.z,selectedColor.w)
                    palette.customReadonlyColor = tempColor
                    themePanel.newTheme = true
                    themePanel.themeChanged = true
                    palette.theme = tempTheme
                }
            }

            WGExpandingRowLayout {
                Layout.preferredHeight: defaultSpacing.minimumRowHeight
                Layout.fillWidth: true

                Item {
                    Layout.fillWidth: true
                    Layout.preferredHeight: defaultSpacing.minimumRowHeight

                    WGTextBox {
                        anchors.fill: parent
                        text: "Sample Text"
                    }
                }
                Item {
                    Layout.fillWidth: true
                    Layout.preferredHeight: defaultSpacing.minimumRowHeight

                    WGTextBox {
                        anchors.fill: parent
                        text: "Readonly Text"
                        readOnly: true
                    }
                }
                Item {
                    Layout.fillWidth: true
                    Layout.preferredHeight: defaultSpacing.minimumRowHeight

                    WGTextBox {
                        anchors.fill: parent
                        text: "Disabled Text"
                        enabled: false
                    }
                }
            }

            WGExpandingRowLayout {
                Layout.preferredHeight: defaultSpacing.minimumRowHeight
                Layout.fillWidth: true
                enabled: customTheme

                label: "Dark Contrast "

                Rectangle {
                    Layout.fillWidth: true
                    Layout.preferredHeight: defaultSpacing.minimumRowHeight
                    color: Qt.rgba(0,0,0,darkSlider.value / 255)
                }
                Rectangle {
                    Layout.fillWidth: true
                    Layout.preferredHeight: defaultSpacing.minimumRowHeight
                    color: Qt.rgba(0,0,0,(darkSlider.value * 2) / 255)
                }
                Rectangle {
                    Layout.fillWidth: true
                    Layout.preferredHeight: defaultSpacing.minimumRowHeight
                    color: Qt.rgba(0,0,0,(darkSlider.value * 3) / 255)
                }
            }

            WGSliderControl {
                id: darkSlider
                Layout.fillWidth: true
                minimumValue: 0
                maximumValue: 100
                stepSize: 1
                value: customTheme ? palette.customDarkContrast : palette.darkContrast
                enabled: customTheme
                onChangeValue: {
                    palette.customDarkContrast = val
                    themePanel.newTheme = true
                    themePanel.themeChanged = true
                    palette.theme = tempTheme
                }
            }

            WGExpandingRowLayout {
                Layout.preferredHeight: defaultSpacing.minimumRowHeight
                Layout.fillWidth: true
                enabled: customTheme

                label: "Light Contrast"

                Rectangle {
                    Layout.fillWidth: true
                    Layout.preferredHeight: defaultSpacing.minimumRowHeight
                    color: Qt.rgba(1,1,1,lightSlider.value / 255)
                }
                Rectangle {
                    Layout.fillWidth: true
                    Layout.preferredHeight: defaultSpacing.minimumRowHeight
                    color: Qt.rgba(1,1,1,(lightSlider.value * 2) / 255)
                }
                Rectangle {
                    Layout.fillWidth: true
                    Layout.preferredHeight: defaultSpacing.minimumRowHeight
                    color: Qt.rgba(1,1,1,(lightSlider.value * 3) / 255)
                }
            }

            WGSliderControl {
                id: lightSlider
                Layout.fillWidth: true
                minimumValue: 0
                maximumValue: 100
                stepSize: 1
                value: customTheme ? palette.customLightContrast : palette.lightContrast
                enabled: customTheme

                onChangeValue: {
                    palette.customLightContrast = val
                    themePanel.newTheme = true
                    themePanel.themeChanged = true
                    palette.theme = tempTheme
                }
            }
        }

        WGSeparator {}

        WGExpandingRowLayout {
            id: applyThemeButtons
            Layout.preferredHeight: defaultSpacing.minimumRowHeight

            Item {
                Layout.fillWidth: true
                Layout.preferredHeight: defaultSpacing.minimumRowHeight
            }

            WGPushButton {
                Layout.preferredWidth: 100
                text: "Undo"

                enabled: themePanel.themeChanged

                onClicked: {
                    palette.customWindowColor = themePanel.initialCustomWindowColor
                    palette.customHighlightColor = themePanel.initialCustomHighlightColor
                    palette.customReadonlyColor = themePanel.initialCustomReadonlyColor
                    themeDropDown.model.set(6, {"windowColor": themePanel.initialCustomWindowColor})
                    themeDropDown.model.set(6, {"highlightColor": themePanel.initialCustomHighlightColor})
                    palette.customDarkText = themePanel.initialDarkText
                    palette.customLightContrast = themePanel.initialLightContrast
                    palette.customDarkContrast = themePanel.initialDarkContrast
                    themeDropDown.currentIndex = themePanel.initialTheme
                    palette.theme = themePanel.initialTheme
                    tempTheme = themePanel.initialTheme
                    themePanel.themeCode = themePanel.initialCode
                    themePanel.newTheme = false
                    themePanel.themeChanged = false
                }
            }

            WGPushButton {
                Layout.preferredWidth: 100
                text: themePanel.themeChanged ? "Apply Changes" : "Close"

                onClicked: {
                    dialog.close(!themePanel.themeChanged)
                }
            }

            Item {
                Layout.fillWidth: true
                Layout.preferredHeight: defaultSpacing.minimumRowHeight
            }
        }
    }
}
