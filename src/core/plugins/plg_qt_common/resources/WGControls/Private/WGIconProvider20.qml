/****************************************************************************
**
** Copyright (C) 2013 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the Qt Quick Controls module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:BSD$
** You may use this file under the terms of the BSD license as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of Digia Plc and its Subsidiary(-ies) nor the names
**     of its contributors may be used to endorse or promote products derived
**     from this software without specific prior written permission.
**
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
**
** $QT_END_LICENSE$
**
****************************************************************************/

import QtQuick 2.5
import QtQuick.Controls 1.4
import QtQuick.Controls.Private 1.0
import QtQuick.Controls.Styles 1.4
import Qt.labs.folderlistmodel 2.1

import WGControls 2.0

/*!
 \ingroup wgcontrols
 \brief A QML object that provides icons. This should probably be moved into C++ at some stage.

*/

Item {
    id: root
    objectName: "WGIconProvider"
    WGComponent { type: "WGIconProvider20" }

    // The available sizes of icons
    property var iconImageSizes: [16, 48, 128]

    // Extensions that currently support thumbnails
    property var thumbs: ["png", "jpg", "jpeg", "bmp", "gif", "ico", "dds", "tga"]

    // Extensions in these arrays will use the icon of the first item in that array
    property var combinedIcons: [
        ["txt", "rtf", "pdf", "doc"],
        ["script", "js", "bat", "py", "lua"],
        ["audio","wav", "mp3", "ogg", "aiff", "aif", "mod"],
        ["texture", "tif", "tiff", "psd", "image", "tga", "png", "jpg", "jpeg", "bmp", "gif", "ico", "dds"],
        ["model", "fbx", "dae", "obj"]
    ]

    // Map of icon names [0] and image components in different sizes [1+] for icons
    property var imageIndex

    property bool showThumbnails: false

    visible: false

    FolderListModel {
        id: iconModel
        folder: "icons/file_icons/"
        nameFilters: ["*.png"]
    }

    // a repeater of Items in the folder that stores the properties and component for the icons.
    Repeater {
        id: imageRepeater
        model: iconModel.count
        Item {
            property string filename: iconModel.get(index, "fileName")
            property string name: filename.slice(0, filename.indexOf("_"))
            property int size: parseInt(filename.slice(filename.indexOf("_") + 1, filename.indexOf(".")))
            property url imgUrl: iconModel.get(index, "fileURL")
        }

        onCountChanged: {
            if (count == iconModel.count && count != 0)
            {
                getImageIndexes()
            }
        }
    }

    // returns the image Component for an image at index that matches the size
    function getImageFromIndex(index, size)
    {
        return imageIndex[iconImageSizes.indexOf(size) + 1][index]
    }

    // Creates the map of icon names and image components
    function getImageIndexes() {

        //creates the array of arrays (map)
        var imgArray = []

        for (var s=0; s < iconImageSizes.length + 1; s++)
        {
            //creates the inner array
            var innerArray = []
            for (var i=0; i < imageRepeater.count; i++)
            {
                var repItem = imageRepeater.itemAt(i)
                if (s == 0 && innerArray.indexOf(repItem.name) == -1)
                {
                    // if this is the first array, fill it with the trimmed name of the icon without any repetition
                    innerArray.push(repItem.name)
                }
                else
                {
                    // otherwise fill it with the image component from the repeater.
                    if (repItem.size == iconImageSizes[s - 1])
                    {
                        innerArray.push(repItem.imgUrl)
                    }
                }
            }
            imgArray.push(innerArray)
        }
        imageIndex = imgArray
    }

    // replaces slashes and makes the path lower case
    function normalisePath(path) {
        if (typeof(path) != "string") {
            return "";
        }
        function filterEmpty(str) { return str!= ""; }
        return path.replace(/[/]/g, "\\").split("\\").filter(filterEmpty).join("\\").toLowerCase();
    }

    // returns the file extension as a string
    function getExtension(path) {
        if (typeof(path) != "string") {
            return "";
        }
        return path.substr(path.lastIndexOf('.') + 1)
    }

    // gets the appropriate icon size for the given size.
    function getIconSize(size) {
        var pixelSize = iconImageSizes[iconImageSizes.length - 1]

        // use the exact size if it exists
        if (iconImageSizes.indexOf(size) != -1)
        {
            pixelSize = size
        }
        // otherwise use the next size up
        else
        {
            for (var i = 0; i < iconImageSizes.length; i++) {
                if (size < iconImageSizes[i])
                {
                    pixelSize = iconImageSizes[i]
                }
            }
        }
        return pixelSize
    }

    // if the extension is in the array of extensions that use thumbnails return true
    function useThumbnail(extension) {
        if (thumbs.indexOf(extension) != -1)
        {
            return true
        }
        else
        {
            return false
        }
    }

    // returns an Image URL based on the path of an asset and size. Trims the extension as needed.
    function getImage(path, size, folder) {

        var normPath = normalisePath(path)

        var ext = folder ? "folder" : getExtension(normPath)

        if (showThumbnails && iconSize > 16)
        {
            if (useThumbnail(ext))
            {
                return "file:///" + normPath
            }
        }
        if (imageIndex[0].indexOf(ext) != -1)
        {
            return getImageFromIndex(imageIndex[0].indexOf(ext), size)
        }
        else
        {
            var modExt = getModifiedExtension(ext)
            return getImageFromIndex(imageIndex[0].indexOf(modExt), size)
        }
    }

    // if an extension uses an image that does not match its name, return the extension that matches an image. If there isn't one, return a default file icon.
    function getModifiedExtension(extension)
    {
        for (var i=0; i<combinedIcons.length; i++)
        {
            if(combinedIcons[i].indexOf(extension) != -1)
            {
                return combinedIcons[i][0]
            }
        }
        if (extension.slice(0,2) == "ds")
        {
            return "dsfile"
        }
        else
        {
            return "file"
        }
    }
}
