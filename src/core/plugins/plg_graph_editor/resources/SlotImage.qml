import QtQuick 2.1

Rectangle
{
    // I'd keep this as a purely visual object that determines the color of the slot circle.
    // Have moved functionality to Slot.qml... which now seems a bit circular

    id : slotImage

    radius: width / 2

    color: slotColor

    width : 12
    height : 12
    z : parent.z + 10

    Rectangle {
        anchors.fill: parent
        anchors.margins: 2
        radius: width / 2

        //TODO: Turn this off (or maybe very transparent white) if slot is connected to remove 'hole'
        color: "#66000000"
    }

    states :
    [
        State
        {
            name : "interactive"
            when : connectionStartSlot != null
            PropertyChanges
            {
                target: slotImage
                //TODO Is this the right test for if a slot is a valid connection or not?
                color : connectionStartSlot.isInput != isInput ? slotColor : "#88888888"
            }
        }
    ]
}
