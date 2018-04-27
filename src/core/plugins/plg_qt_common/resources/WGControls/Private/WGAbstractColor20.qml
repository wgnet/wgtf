import QtQuick 2.3
import QtQuick.Controls 1.2
import QtQuick.Layouts 1.0
import QtGraphicalEffects 1.0

import WGControls 2.0
import WGControls.Global 2.0

/*!
 \A base class for an abstract color object that binds to a vec4 color, handles tonemapping and emits signals to change that color.

Example:
\code{.js}
WGAbstractColor {
    id: myNewColorControl
    rgbaColor: myVec4Color

    //code that selects a new color and calls changeColor(col)
    ...

    onChangeColor: {
        if (myVec4Color != col)
        {
            myVec4Color = Qt.vector4d(col.x,col.y,col.z,col.w)
        }
    }
}
\endcode
*/


Item {
    id: colorObject
    objectName: "WGAbstractColor"
    WGComponent { type: "WGAbstractColor20" }

    /*!
        The root RGBA color that drives the abstract color object
    */
    property var rgbaColor: Qt.vector4d(0,0,0,0)

    /*!
        A vec3 that represents the alternate color space for the control
    */
    property var altColor: Qt.vector3d(0,0,0)

    /*!
        A string representing the alternate color space altColor will be calculated in.

        Valid values are ("HSV", "HSL", "RGB")

        The default value, or if a valid value is not entered is "HSV"
    */
    property string altColorSpace: "HSV"

    /*!
        A temporary color preview vector4 (r,g,b,a) returned by the control.
    */
    property vector4d previewedColor: Qt.vector4d(0,0,0,0)

    /*!
        True if the previewColor should be used.
    */
    property bool showPreviewColor: false

    /*!
        If the color control should use an HDR color space. Has no actual effect on the base class.
    */
    property bool useHDR: false

    /*!
        The tonemapping algorithm to convert an HDR color to a visible LDR color.

        The default returns its input without changing anything.
    */
    property var tonemap: function(col) { return col; }

    /*!
        The number of decimal places colors can be rounded to.

        If controls do not round colors to the same values, unecessary changed signals will be fired
        creating binding loops.

        The default is 5
    */
    property int decimalPlaces: 5

    /* Accepts a vec3 color in the alternateColorSpace and returns a vec4 in rgba color space */
    function altToRgba (col) {
        var tempCol
        if (altColorSpace == "HSL")
        {
            tempCol = WGColor.hslToRgb(col.x,col.y,col.z)
            tempCol = Qt.vector4d(tempCol.x, tempCol.y, tempCol.z, rgbaColor.w)
        }
        else if (altColorSpace == "RGB")
        {
            tempCol = Qt.vector4d(col.x,col.y,col.z,rgbaColor.w)
        }
        else
        {
            tempCol = WGColor.hsvToRgb(col.x,col.y,col.z)
            tempCol = Qt.vector4d(tempCol.x, tempCol.y, tempCol.z, rgbaColor.w)
        }
        return tempCol
    }

    /* Accepts a vec3 or vec4 color in the rgba space and returns a vec3 in the alternate color space */
    function rgbaToAlt (col) {
        var tempCol
        if (altColorSpace == "HSL")
        {
            tempCol = WGColor.rgbToHsl(col.x,col.y,col.z,altColor.x,altColor.y,altColor.z)
        }
        else if (altColorSpace == "RGB")
        {
            tempCol = Qt.vector3d(col.x,col.y,col.z)
        }
        else
        {
            tempCol = WGColor.rgbToHsv(col.x,col.y,col.z,altColor.x,altColor.y,altColor.z)
        }
        return tempCol
    }

    onAltColorSpaceChanged: {
        changeAltColor(rgbaToAlt(Qt.vector4d(rgbaColor.x, rgbaColor.y, rgbaColor.z, rgbaColor.w)))
    }

    /* updates the alternate colors */
    onRgbaColorChanged: {
        var tempCol = rgbaToAlt(rgbaColor)
        if (altColor != tempCol) {
            altColor = tempCol
        }
    }

    /* rounds the colors to a fixed set of decimalPlaces*/
    function round(num) {
        return Number(Math.round(num+'e'+decimalPlaces)+'e-'+decimalPlaces);
    }

    /* updates the altColor with col and then sends changeColor with converted RGBA values. */
    function changeAltColor(col) {
        altColor = col
        changeColor(altToRgba(col))
    }

    /*
    Signal to update the  root color with in RGBA space

    Expects a vector4 (r,g,b,a)
    */
    signal changeColor (var col)

    /*
    Signal to show the preview color vector 4d (col) true or false (show)

    */
    signal previewColor (var show, var col)
}
