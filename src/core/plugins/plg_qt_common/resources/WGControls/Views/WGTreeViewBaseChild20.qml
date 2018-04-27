import QtQuick 2.5
import QtQuick.Controls 1.4
import QtQml.Models 2.2

import WGControls 2.0
import WGControls.Views 2.0

WGTreeViewBase20{
	id: treeViewBaseChild
	WGComponent { type: "WGTreeViewBaseChild20" }

	property var parentTree;
	property var childParent;
	property var childModel;

	function doNothing(mouse, itemIndex, rowIndex) {}

	width: (typeof parentTree !== "undefined" ) ? parentTree.width - childParent.x : 0;
	height: ( typeof parentTree !== "undefined" ) ? parentTree.height - childParent.y + childParent.height : 0;
	interactive: false;
	view: parentTree ? parentTree.view : null;
	model: childModel;
	depth: parentTree ? parentTree.depth + 1 : 0;
	spacing: parentTree ? parentTree.spacing : 0;
	expandToDepth: parentTree ? parentTree.expandToDepth : 0;
	Keys.forwardTo: [parentTree];
	__onItemPressed: parentTree ? parentTree.itemPressed : doNothing;
	__onItemClicked: parentTree ? parentTree.itemClicked : doNothing;
	__onItemDoubleClicked: parentTree ? parentTree.itemDoubleClicked : doNothing;
}
