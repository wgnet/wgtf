/*!
    \brief An item that acts as a helper for users manually enabling/disabling copy/paste functionality
    of a control

Example:
\code{.js}
// disable copy&paste inside WGTextBox
WGTextBox {
    id: searchBox
    Component.onCompleted: {
        WGCopyableHelper.disableChildrenCopyable(searchBox);
    }
}
\endcode
*/


/*! This function recursively finds copyable children and disables them.
*/
function disableChildrenCopyable(parentObject) {
    if ((parentObject === null))
    {
        return;
    }
    for (var i=0; i<parentObject.children.length; i++)
    {
        if (typeof parentObject.children[i].parentCopyable !== "undefined")
        {
            parentObject.children[i].enabled = false;
            parentObject.children[i].visible = false;
        }
        else
        {
            disableChildrenCopyable( parentObject.children[i] );
        }
    }
}

/*! This function recursively finds copyable children and enables them.
*/
function enableChildrenCopyable(parentObject) {
    if ((parentObject === null)) {
        return;
    }
    for (var i = 0; i < parentObject.children.length; i++) {
        if (typeof parentObject.children[i].parentCopyable !== "undefined") {
            if (parentObject.children[i].enabled) {
                parentObject.children[i].enabled = true;
                parentObject.children[i].visible = true;
            }
        }
        else {
            disableChildrenCopyable(parentObject.children[i]);
        }
    }
}


