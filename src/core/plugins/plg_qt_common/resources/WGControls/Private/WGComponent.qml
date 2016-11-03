import QtQuick 2.5

/*!
 \ingroup wgcontrols
 \brief holds information about a reusable component
*/
Item {
    id: component
    property string type: ""    
    
    Binding {
        id: binder
    }
    
    Component.onCompleted: {
        if(typeof qmlComponents !== 'undefined') { 
            binder.property = "type";
            binder.target = qmlComponents;
            binder.value = component.type;
        }
    }
}