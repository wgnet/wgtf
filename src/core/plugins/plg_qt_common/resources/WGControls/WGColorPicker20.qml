import QtQuick 2.3
import QtQuick.Controls 1.2
import QtQuick.Layouts 1.0

import WGControls 2.0
import WGControls.Color 2.0

/*!
 \A fully featured color picker that can be used in a dialog or as a stand alone control.

 Includes an HSL triangle and color wheel, shade control, customiseable palette, screen picking, Hex entry and more.

 Is intended to support more methods of picking color in the left column

Example:
\code{.js}
WGColorPicker {
    id: colorPicker
    anchors.fill: parent
    initialColor: myStartingColor
    onOkClicked: {
        myNewColor = colorPicker.currentColor
    }
    onCancelClicked: {
        myNewColor = myOldColor
    }
}
\endcode

TODO: Make the customiseable color palette use appwide persistent data.
TODO: Make left column/drop down box expandable with more/optional color picking methods.
TODO: Performance pass
*/


Rectangle {
    id: basePanel
    objectName: "WGColorPicker"
    WGComponent { type: "WGColorPicker20" }

    property vector4d currentColor: Qt.vector4d(0.0,0.5,0,1)

    property real brightnessRange: 1.0

    /*!
        The color before any changes are made or accepted with the color picker.
    */
    property vector4d initialColor: Qt.vector4d(0,0,0,0)

    /*!
        This property holds the temporary color that is displayed in the second preview square if showPreviewColor = true

        It is intended to be used when the user is previewing a potential change to currentColor such as when picking from the screen.
    */
    property vector4d previewedColor: currentColor

    /*!
        This property will change the second preview square's color to previewColor when true

        It is intended to be used when the user is previewing a potential change to currentColor such as when picking from the screen.
    */
    property bool showPreviewColor: false

    /*!
        This property determines if the color picker shows the alpha value at all.

        If false, the alpha value will be locked at 1.0 and the alpha controls will not be displayed.

        The default is true
    */
    property bool showAlphaChannel: true

    /*!
        This property determines if the Ok and Cancel Dialog buttons are displayed.

        The default is true
    */
    property bool showDialogButtons: true

    property bool useHDR: true;

    property bool hue360: false

    property var tonemap: function(col) { return col; }

    property var viewId: null
    property var viewPreference: null

    property var dialogHeight: {
        if (useHDR || showAlphaChannel)
        {
            return defaultSpacing.minimumRowHeight * 17 - defaultSpacing.doubleMargin
        }
        else
        {
            return defaultSpacing.minimumRowHeight * 15 - defaultSpacing.doubleMargin
        }
    }

    /*! \internal */
    color: palette.mainWindowColor

    /*!
        This signal is fired when the Ok dialog button is clicked.
    */
    signal okClicked()

    /*!
        This signal is fired when the Cancel dialog button is clicked.
    */
    signal cancelClicked()

    signal refreshShader()

    onOkClicked: {
        savePrefs()
    }

    onViewPreferenceChanged: {
        if(viewPreference != null)
        {
            loadPrefs()
        }
    }

    function round(num) {
        return num.toFixed(3)
    }

    function savePrefs() {
        if (typeof viewId != "undefined" && viewId != null)
        {
            // split palette array into "x0,y0,z0,w0 x1,y1,z1,w1 ..." string and save to prefs
            var stringPalette = ""
            var pal = colorPalette.savedColors
            for (var c in colorPalette.savedColors)
            {
                stringPalette += (pal[c].x + "," + pal[c].y + "," + pal[c].z + "," + pal[c].w + " ")
            }
            addPreference(viewId, "colorPickerPalette", stringPalette);

            // save visible left panel to preferences
            addPreference(viewId, "colorPickerLeftPanel", leftColumnMenu.currentIndex);
        }
    }

    function loadPrefs() {
        if (typeof viewPreference != "undefined" && viewPreference != null)
        {
            // load saved color palette
            if (typeof viewPreference.colorPickerPalette != "undefined")
            {
                colorPalette.savedColors = []
                var stringColors = viewPreference.colorPickerPalette.split(" ")
                var currentColor = []
                // this seems excessive but it only seems to like interpreting "x,y,z,w' as a vector4d one way
                for (var c in stringColors)
                {
                    currentColor = stringColors[c].split(",")
                    if (currentColor != "")
                    {
                        colorPalette.savedColors.push(Qt.vector4d(currentColor[0],currentColor[1],currentColor[2],currentColor[3]))
                    }
                }
            }
            else
            {
                colorPalette.savedColors = [Qt.vector4d(0,0,0,1),Qt.vector4d(1,1,1,1),Qt.vector4d(0.5,0.5,0.5,1),Qt.vector4d(1,0,0,1),Qt.vector4d(0,1,0,1),Qt.vector4d(0,0,1,1),Qt.vector4d(0,1,1,1),Qt.vector4d(1,0,1,1),Qt.vector4d(1,1,0,1)]
            }
            colorPalette.updatePalette()

            //load last chosen left panel control
            if (typeof viewPreference.colorPickerLeftPanel != "undefined" && leftColumnMenu.count > viewPreference.colorPickerLeftPanel)
            {
                leftColumnMenu.currentIndex = viewPreference.colorPickerLeftPanel
            }
            else
            {
                leftColumnMenu.currentIndex = 0
            }
        }
    }

    RowLayout {
        anchors.top: parent.top
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.topMargin: defaultSpacing.standardMargin
        height: dialogHeight

        spacing: 0

        Item {
            id: leftColumn
            Layout.preferredHeight: dialogHeight
            Layout.preferredWidth: defaultSpacing.minimumRowHeight * 13

            ColumnLayout {
                anchors.fill: parent
                anchors.leftMargin: defaultSpacing.standardMargin
                anchors.rightMargin: defaultSpacing.standardMargin
                spacing: 0

                Item {
                    Layout.fillWidth: true
                    Layout.preferredHeight: width * 0.85

                    //Loads color picking control in left column depending on leftColumnMenu
                    Loader {
                        id: leftPanelLoader
                        anchors.fill: parent

                        sourceComponent: svComponent

                        onStateChanged: {
                            if (Loader.Ready)
                            {
                                refreshShader()
                            }
                        }
                    }

                    Component {
                        //HSV Color Triangle/Wheel
                        id: wheelComponent
                        WGColorWheel {
                            id: colorWheel
                            showShortCuts: false

                            rgbaColor: currentColor

                            useHDR: basePanel.useHDR
                            tonemap: basePanel.tonemap

                            onChangeColor: {
                                if (currentColor != col)
                                {
                                    currentColor = Qt.vector4d(col.x,col.y,col.z,col.w)
                                }
                            }
                        }
                    }

                    Component {
                        //Sat Value Color Square
                        id: svComponent
                        WGHSVSquareSV {
                            id: svSquare

                            rgbaColor: currentColor

                            brightnessRange: basePanel.brightnessRange

                            useHDR: basePanel.useHDR
                            tonemap: basePanel.tonemap

                            onChangeColor: {
                                if (currentColor != col)
                                {
                                    currentColor = Qt.vector4d(col.x,col.y,col.z,col.w)
                                }
                            }
                        }
                    }

                    Component {
                        //HueSat Color Square
                        id: hsComponent
                        WGHSVSquareHS {
                            id: hsSquare

                            rgbaColor: currentColor

                            brightnessRange: basePanel.brightnessRange

                            useHDR: basePanel.useHDR
                            tonemap: basePanel.tonemap

                            onChangeColor: {
                                if (currentColor != col)
                                {
                                    currentColor = Qt.vector4d(col.x,col.y,col.z,col.w)
                                }
                            }
                        }
                    }

                    Component {
                        id: shadeComponent
                        //Hue & Value shade grid
                        WGShadeGrid {
                            id: shadeGrid

                            rgbaColor: currentColor

                            useHDR: basePanel.useHDR
                            tonemap: basePanel.tonemap

                            showPreviewColor: basePanel.showPreviewColor

                            onChangeColor: {
                                if (currentColor != col)
                                {
                                    currentColor = Qt.vector4d(col.x,col.y,col.z,col.w)
                                }
                            }

                            onPreviewColor: {
                                basePanel.showPreviewColor = show
                                if (show && basePanel.previewedColor != col)
                                {
                                    basePanel.previewedColor = col
                                }
                            }
                        }
                    }
                }

                WGHDRSelector {
                    id: hdrSelector
                    Layout.fillWidth: true
                    Layout.preferredHeight: (defaultSpacing.minimumRowHeight + defaultSpacing.rowSpacing) * 2

                    rgbaColor: currentColor

                    useHDR: basePanel.useHDR
                    tonemap: basePanel.tonemap
                    brightnessRange: basePanel.brightnessRange

                    visible: useHDR

                    onChangeBrightness: {
                        basePanel.brightnessRange = br
                    }
                }

                Item {
                    Layout.fillHeight: true
                    Layout.fillWidth: true
                }

                //Picks which control is visible in left column
                WGDropDownBox {
                    id: leftColumnMenu
                    Layout.fillWidth: true
                    Layout.preferredHeight: defaultSpacing.minimumRowHeight

                    model: useHDR ? hdrModel : nonHdrModel

                    enabled: typeof model != "undefined" ? model.count > 1 : true

                    onVisibleChanged: {
                        if (visible)
                        {
                            if(currentIndex > model.count)
                            {
                                currentIndex = model.length - 1
                            }
                        }
                    }

                    ListModel {
                        id: hdrModel
                        ListElement {
                            label: "Sat/Val Square (HDR)"
                            panelIndex: 0
                        }
                    }

                    ListModel {
                        id: nonHdrModel
                        ListElement {
                            label: "Sat/Val Square"
                            panelIndex: 0
                        }
                        ListElement {
                            label: "Hue/Sat Square"
                            panelIndex: 1
                        }
                        ListElement {
                            label: "HSL Color Wheel"
                            panelIndex: 2
                        }
                        ListElement {
                            label: "HSL Shade Grid"
                            panelIndex: 3
                        }
                    }
                    onCurrentIndexChanged: {
                        if (model.get(currentIndex).panelIndex == 0)
                        {
                            leftPanelLoader.sourceComponent = svComponent
                            basePanel.hue360 = false
                            hueSlider.altColorSpace = "HSV"
                        }
                        else if (model.get(currentIndex).panelIndex == 1)
                        {
                            leftPanelLoader.sourceComponent = hsComponent
                            basePanel.hue360 = false
                            hueSlider.altColorSpace = "HSV"
                        }
                        else if (currentIndex == 2)
                        {
                            leftPanelLoader.sourceComponent = wheelComponent
                            basePanel.hue360 = false
                            hueSlider.altColorSpace = "HSL"
                        }
                        else if (model.get(currentIndex).panelIndex == 3)
                        {
                            leftPanelLoader.sourceComponent = shadeComponent
                            basePanel.hue360 = false
                            hueSlider.altColorSpace = "HSL"
                        }
                    }
                }

                Item {
                    Layout.minimumHeight: defaultSpacing.standardMargin
                    Layout.fillWidth: true
                }

                //Customiseable color palette.
                //TODO make this use appwide persistent data
                WGColorPalette {
                    id: colorPalette
                    Layout.fillWidth: true
                    Layout.preferredHeight: {
                        if (!useHDR && showAlphaChannel)
                        {
                            defaultSpacing.minimumRowHeight * 4 + defaultSpacing.standardMargin
                        }
                        else
                        {
                            defaultSpacing.minimumRowHeight * 2 + defaultSpacing.standardMargin
                        }
                    }

                    rgbaColor: currentColor

                    useHDR: basePanel.useHDR
                    tonemap: basePanel.tonemap
                    brightnessRange: basePanel.brightnessRange

                    onChangeBrightness: {
                        basePanel.brightnessRange = br
                    }

                    onChangeColor: {
                        if (currentColor != col)
                        {
                            currentColor = Qt.vector4d(col.x,col.y,col.z,col.w)
                        }
                    }
                }
            }
        }

        Item {
            id: rightColumn
            Layout.preferredHeight: dialogHeight
            Layout.fillWidth: true

            // Preview squares column
            ColumnLayout {
                anchors.fill: parent
                anchors.margins: defaultSpacing.standardMargin
                spacing: 0

                RowLayout {
                    Layout.fillWidth: true
                    Layout.preferredHeight: childrenRect.height

                    // Overlaid squares for comparing initialColor and currentColor
                    WGColorPreview {
                        id: previewSquares
                        Layout.maximumHeight: defaultSpacing.minimumRowHeight * 4
                        Layout.preferredHeight: defaultSpacing.minimumRowHeight * 4
                        Layout.preferredWidth: height
                        Layout.alignment: Qt.AlignTop | Qt.AlignHCenter

                        initialColor: basePanel.initialColor

                        rgbaColor: currentColor

                        useHDR: basePanel.useHDR
                        tonemap: basePanel.tonemap

                        showPreviewColor: basePanel.showPreviewColor

                        previewedColor: basePanel.previewedColor

                        onChangeColor: {
                            if (currentColor != col)
                            {
                                currentColor = Qt.vector4d(col.x,col.y,col.z,col.w)
                            }
                        }

                        onPreviewColor: {
                            basePanel.showPreviewColor = show
                            if (show && basePanel.previewedColor != col)
                            {
                                basePanel.previewedColor = col
                            }
                        }
                    }

                    // Pick from screen, hex values and alpha toggle column.
                    ColumnLayout {
                        Layout.fillWidth: true
                        Layout.maximumHeight: defaultSpacing.minimumRowHeight * 4
                        Layout.preferredHeight: defaultSpacing.minimumRowHeight * 4

                        WGPickScreenButton {
                            id: pickButton
                            Layout.fillWidth: true
                            Layout.preferredHeight: defaultSpacing.minimumRowHeight
                            rgbaColor: currentColor

                            useHDR: basePanel.useHDR

                            tonemap: basePanel.tonemap

                            showPreviewColor: basePanel.showPreviewColor

                            onChangeColor: {
                                if (currentColor != col)
                                {
                                    currentColor = Qt.vector4d(col.x,col.y,col.z,col.w)
                                }
                            }

                            onPreviewColor: {
                                basePanel.showPreviewColor = show
                                if (show && basePanel.previewedColor != col)
                                {
                                    basePanel.previewedColor = col
                                }
                            }
                        }

                        WGLabel {
                            text: "(Press & hold to start, release to pick)"
                            enabled: false
                            Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
                            horizontalAlignment: Text.AlignHCenter
                        }

                        Item {
                            Layout.fillWidth: true
                            Layout.fillHeight: true
                        }

                        RowLayout {
                            Layout.fillWidth: true

                            Item {
                                Layout.fillWidth: true
                                Layout.preferredHeight: 1
                            }

                            WGLabel {
                                text: "Hex Value #(AA)RRGGBB: "
                                enabled: true
                                horizontalAlignment: Text.AlignRight
                            }

                            WGHexColorTextBox {
                                id: hexValue
                                Layout.preferredWidth: rgbSlider.numBoxWidth
                                Layout.preferredHeight: defaultSpacing.minimumRowHeight
                                Layout.alignment: Qt.AlignVCenter | Qt.AlignRight

                                rgbaColor: currentColor

                                useHDR: basePanel.useHDR

                                tonemap: basePanel.tonemap

                                onChangeColor: {
                                    if (currentColor != col)
                                    {
                                        currentColor = Qt.vector4d(col.x,col.y,col.z,col.w)
                                    }
                                }
                            }
                        }
                    }
                }

                Item {
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    WGSeparator {
                        anchors.left: parent.left
                        anchors.right: parent.right
                        anchors.verticalCenter: parent.verticalCenter
                    }
                }

                WGHueSlider {
                    id: hueSlider
                    Layout.fillWidth: true
                    Layout.preferredHeight: (defaultSpacing.minimumRowHeight + defaultSpacing.rowSpacing) * 3
                    rgbaColor: currentColor

                    useHDR: basePanel.useHDR

                    hue360: basePanel.hue360

                    brightnessRange: basePanel.brightnessRange
                    tonemap: basePanel.tonemap

                    onChangeColor: {
                        if (currentColor != col)
                        {
                            currentColor = Qt.vector4d(col.x,col.y,col.z,col.w)
                        }
                    }
                }

                Item {
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    WGSeparator {
                        anchors.left: parent.left
                        anchors.right: parent.right
                        anchors.verticalCenter: parent.verticalCenter
                    }
                }

                WGRgbSlider {
                    id: rgbSlider
                    Layout.fillWidth: true
                    Layout.preferredHeight: (defaultSpacing.minimumRowHeight + defaultSpacing.rowSpacing) * 3
                    rgbaColor: currentColor

                    useHDR: basePanel.useHDR

                    brightnessRange: useHDR ? basePanel.brightnessRange : 1.0
                    tonemap: basePanel.tonemap

                    useHexValue: !useHDR

                    onChangeColor: {
                        if (currentColor != col)
                        {
                           currentColor = Qt.vector4d(col.x,col.y,col.z,col.w)
                        }
                    }
                }

                Item {
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    WGSeparator {
                        anchors.left: parent.left
                        anchors.right: parent.right
                        anchors.verticalCenter: parent.verticalCenter
                    }
                }

                WGAlphaSlider {
                    id: alphaSlider
                    Layout.fillWidth: true
                    Layout.preferredHeight: defaultSpacing.minimumRowHeight
                    visible: showAlphaChannel
                    rgbaColor: currentColor

                    useHDR: basePanel.useHDR

                    brightnessRange: useHDR ? basePanel.brightnessRange : 1.0
                    tonemap: basePanel.tonemap

                    onChangeColor: {
                        if (currentColor != col)
                        {
                           currentColor = Qt.vector4d(col.x,col.y,col.z,col.w)
                        }
                    }
                }

                Item {
                    visible: showDialogButtons
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                }

                RowLayout {
                    visible: showDialogButtons
                    Layout.fillWidth: true
                    Layout.preferredHeight: defaultSpacing.minimumRowHeight

                    WGPushButton {
                        Layout.fillWidth: true
                        Layout.preferredHeight: defaultSpacing.minimumRowHeight
                        Layout.preferredWidth: rightColumn.width / 2
                        text: "Ok"

                        onClicked: {
                            basePanel.okClicked()
                        }
                    }

                    WGPushButton {
                        Layout.fillWidth: true
                        Layout.preferredHeight: defaultSpacing.minimumRowHeight
                        Layout.preferredWidth: rightColumn.width / 2
                        text: "Cancel"

                        onClicked: {
                            basePanel.cancelClicked()
                        }
                    }
                }
            }
        }
    }
}
