import QtQuick 2.5
import QtQuick.Controls 1.4
import QtQuick.Layouts 1.3
import QtWebEngine 1.2

import WGControls 2.0

WGPanel {
    id: viewport
    color: palette.mainWindowColor
    layoutHints: { 'viewport': 0.1 }
    property var sceneDirty: getSceneDirty
	
	Repeater {
        id: objectRepeater
        model: listModel
        Item {
    		property var objData: value
        }
    }	

    property Component webglComponent: WebEngineView {
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
					if(success) {
						initialized = true;
						updateScene();		
                        console.log("WebGL initialised");
					}
					else {
						console.log("WebGL failed to initialise");	
					}
                });

            }
            else if(loadRequest.status == WebEngineView.LoadFailedStatus)
            {
                console.log("WebEngineView failed to load");
            }
        }
    }

    property alias webgl: loader.item
    WGLoader {
        id: loader
        active: qmlView.needsToLoad
        asynchronous: true
        loading: !(status == Loader.Ready)
        anchors.fill: parent
        sourceComponent: webglComponent
    }
	
    onWidthChanged: {
        if(webgl != null && webgl.initialized) {
            webgl.call("setViewportSize", [width, height]);
        }
    }

    onHeightChanged: {
        if(webgl != null && webgl.initialized) {
            webgl.call("setViewportSize", [width, height]);
        }
    }
	
	Connections {
		target: self
		onSelectedIndexChanged: {
			if(webgl != null && webgl.initialized) {
				webgl.call("setSelectedModel", [selectedIndex]);
			}
		}
	}	
	

	onSceneDirtyChanged: {
		if(sceneDirty) {
			updateScene();
		}
	}

	property int countCache: 0	
	function updateScene(){
        if (webgl != null && webgl.initialized) {
			var count = objectRepeater.count;
            if(count != countCache)
            {
                webgl.call("setModelCount", [count]);
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

                webgl.call("setModelProperties", 
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
        }	
	}

    MouseArea {
        id: webglMouseArea
        anchors.fill: parent
        onClicked: {
			if (webgl != null && webgl.initialized) {
                webgl.call("getPickedModel", [mouse.x, mouse.y], function(model)
                {
                    selectObject(model);
                });
			}

			viewport.focus = true;
        }

        DropArea {
            anchors.fill: parent
            keys: ["text/uri-list"]
            onDropped: {
                if (webgl != null && webgl.initialized)
                {
                    if ( drop.hasUrls && drop.urls.length === 1 )
                    {
                        var path = drop.urls[0].toString();
                        if (path.toLowerCase().match(/\.(jpg|jpeg|png)$/))
                        {
                            webgl.call("getPickedModel", [drop.x, drop.y], function(model)
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
