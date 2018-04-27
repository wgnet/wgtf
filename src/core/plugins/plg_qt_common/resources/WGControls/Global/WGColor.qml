pragma Singleton
import QtQuick 2.5

import WGControls 2.0

Item {
    WGComponent { type: "WGColor" }

    function setExposure(v, intensity)
    {
        var exposure = Math.pow(2.0, intensity);
        return Qt.vector4d(
            v.x * exposure,
            v.y * exposure,
            v.z * exposure,
            v.w * exposure);
    }

    function hueToIntensity(v1, v2, h)
    {
        h = h - Math.floor(h);
        if (h < 1.0 / 6.0)
        {
            return v1 + (v2 - v1) * 6.0 * h;
        }
        else if (h < 1.0 / 2.0)
        {
            return v2;
        }
        else if (h < 2.0 / 3.0)
        {
            return v1 + (v2 - v1) * 6.0 * (2.0 / 3.0 - h);
        }
        return v1;
    }

    function hslToRgb(h,s,l) {
        var v1;
        var v2;
        if (l < 0.5)
        {
            v2 = l * (1.0 + s);
        }
        else
        {
            v2 = (l + s) - (s * l);
        }

        v1 = 2.0 * l - v2;

        var d = 1.0 / 3.0;
        var r = hueToIntensity(v1, v2, h + d);
        var g = hueToIntensity(v1, v2, h);
        var b = hueToIntensity(v1, v2, h - d);
        return Qt.vector3d(r, g, b);
    }

    function rgbToHsl(r, g, b, h, s, l){
        var max = Math.max(r, g, b)
        var min = Math.min(r, g, b);

        h = typeof h !== 'undefined' ? h : 0.0
        s = typeof s !== 'undefined' ? s : 0.0

        // h and s can be left at earlier values if it is greyscale but l has to change.
        l = (max + min) / 2;

        if(max != min)
        {
            var d = max - min;
            s = l > 0.5 ? d / (2 - max - min) : d / (max + min);
            switch(max){
                case r: h = (g - b) / d + (g < b ? 6 : 0); break;
                case g: h = (b - r) / d + 2; break;
                case b: h = (r - g) / d + 4; break;
            }
            h /= 6;
        }

        return Qt.vector3d(h, s, l);
    }

    function rgbToHsv(r, g, b, h, s, v) {

        h = typeof h !== 'undefined' ? h : 0
        s = typeof s !== 'undefined' ? s : 0
        v = typeof v !== 'undefined' ? v : 0

        var max = Math.max(r, g, b);
        var min = Math.min(r, g, b);
        v = max;

        var d = max - min;
        if (max > 0)
        {
            s = d / max;
        }

        if (max != min)
        {
            switch (max)
            {
                case r: h = (g - b) / d + (g < b ? 6 : 0); break;
                case g: h = (b - r) / d + 2; break;
                case b: h = (r - g) / d + 4; break;
            }

        h /= 6;
        }
        return Qt.vector3d(h, s, v);
    }

    function hsvToRgb(h, s, v) {
        var r, g, b;

        var i = Math.floor(h * 6);
        var f = h * 6 - i;
        var p = v * (1 - s);
        var q = v * (1 - f * s);
        var t = v * (1 - (1 - f) * s);

        switch (i % 6) {
            case 0: r = v; g = t; b = p; break;
            case 1: r = q; g = v; b = p; break;
            case 2: r = p; g = v; b = t; break;
            case 3: r = p; g = q; b = v; break;
            case 4: r = t; g = p; b = v; break;
            case 5: r = v; g = p; b = q; break;
        }
        return Qt.vector3d(r, g, b);
    }
}
