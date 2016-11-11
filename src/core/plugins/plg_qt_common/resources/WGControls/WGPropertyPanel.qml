import WGControls 2.0
import WGControls.Views 2.0

WGPanel 
{
    id: propertyPanel
    objectName: "WGPropertyPanel"
    WGComponent { type: "WGPropertyPanel" }
    color: palette.darkColor

    WGPropertyTreeView
    {
        model: self.model
        anchors.fill: parent
        clamp: true

        onCurrentIndexChanged: {
            var item = view.extendedModel.indexToItem(currentIndex);
            currentPropertyChanged(item.display);
        }
    }
}
