import QtQuick 2.5
import QtQuick.Controls 1.4
import QtQuick.Layouts 1.3
import QtWebEngine 1.2
import WGControls 1.0

Rectangle {
    id: viewport
    color: palette.mainWindowColor
    property var title: "Viewport"
    property var layoutHints: { 'viewport': 0.1 }

	WGListModel
        {
            id: objectModel
            source: listSource
            ValueExtension {}
        }
	Repeater {
        id: objectRepeater
        model: objectModel
		
        delegate: Item {
		property var objData: value
        }
		
    }

    WebEngineView {
        id: webglEngine
        anchors.fill: parent
        url: "js/main.html"
        property var initialized: false

        /**
        * Calls a function which is registered with the document
        * Avaliable functions will have a signature: document.name = function()
        * @note is asynchronous
        * @param name The name of the function to call
        * @param args A list of arguments to pass
        * @param onComplete Optional callback to retrieve the results
        */
        function call(name, args, onComplete)
        {
            args = args || [];
            var arguments = "";
            if(args.length > 0)
            {
                // Add "" to any string arguments
                function getArg(arg)
                {
                    return typeof arg === "string" ? "\"" + arg + "\"" : arg;
                }

                arguments = getArg(args[0]);
                for (var i = 1; i < args.length; i++) 
                {
                    arguments += "," + getArg(args[i]);
                }
            }

            runJavaScript("document." + name + "(" + arguments + ")", onComplete);
        }

        onJavaScriptConsoleMessage: {
            console.log( sourceID + " (" + lineNumber + "):", message);
        }

        /**
        * Signal for when the web engine loading is changed
        */
        onLoadingChanged:{
            if(loadRequest.status == WebEngineView.LoadSucceededStatus)
            {
                call("engineStart", [width, height], function(success) 
                { 
                    console.log(success ?
                        "WebGL initialised" :
                        "WebGL failed to initialise");

                    initialized = true;
                    webglTick.running = true;
                });      

            }
            else if(loadRequest.status == WebEngineView.LoadFailedStatus)
            {
                console.log("WebEngineView failed to load");
            }
        }
    }

    onWidthChanged: {
        if(webglEngine.initialized)
        {
            webglEngine.call("setViewportSize", [width, height]);
        }
    }

    onHeightChanged: {
        if(webglEngine.initialized)
        {
            webglEngine.call("setViewportSize", [width, height]);
        }
    }

    Timer {
        id: webglTick
        interval: 100
        running: false
        repeat: true

        property var countCache: 0
        property var modelCache: 0

        onTriggered: {
			var count = objectRepeater.count;
            if(count != countCache)
            {
                webglEngine.call("setModelCount", [count]);
                countCache = count;
            }
			for(var i = 0; i < count; i++)
			{
				var obj = objectRepeater.itemAt(i).objData;
				var position = obj.position;
                var rotation = obj.rotation;
                var scale = obj.scale;
                var isVisible = obj.visible;
                var textureName = obj.map1;

                webglEngine.call("setModelProperties", 
                [
                    i,
                    isVisible,
                    textureName,
                    position.x, 
                    position.y, 
                    position.z,
                    rotation.x, 
                    rotation.y, 
                    rotation.z,
                    scale.x, 
                    scale.y, 
                    scale.z
                ]);
			}

            var selected = rootObjectIndex;
            if(selected != modelCache)
            {
                webglEngine.call("setSelectedModel", [selected]);
                modelCache = selected;
            }
        }
    }

    MouseArea {
        id: webglMouseArea
        anchors.fill: parent
        onClicked: {
			if ( webglEngine.initialized )
			{
                webglEngine.call("getPickedModel", [mouse.x, mouse.y], function(model)
                {
                    updateRootObject(model);
                });
            }
        }

        DropArea {
            anchors.fill: parent
            keys: ["text/uri-list"]
            onDropped: {
                if ( webglEngine.initialized )
                {
                    if ( drop.hasUrls && drop.urls.length === 1 )
                    {
                        var path = drop.urls[0].toString();
                        if (path.toLowerCase().match(/\.(jpg|jpeg|png)$/))
                        {
                            webglEngine.call("getPickedModel", [drop.x, drop.y], function(model)
                            {
                                setTexture(model, getObjectTexture(model), path);
                            });
                        }
                    }
                }
            }
        }
    }
}
