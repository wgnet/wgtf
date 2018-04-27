pragma Singleton
import QtQuick 2.5
import WGControls 2.0

Item {
	WGComponent { type: "WGTreeViewBaseGlobals" }

	property var treeViewBaseChildComponent__;

	function getWGTreeViewBaseChildComponent()	{
		if(!treeViewBaseChildComponent__)
		{
			treeViewBaseChildComponent__ = Qt.createComponent( "WGTreeViewBaseChild20.qml");
		}
		return treeViewBaseChildComponent__;
	}
}