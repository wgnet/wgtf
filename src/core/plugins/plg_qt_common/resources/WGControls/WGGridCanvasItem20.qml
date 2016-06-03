import QtQuick 2.3

Item
{
    property var viewTransform

    property var _xScale: viewTransform.xScale
    property var _yScale: viewTransform.yScale
    property var _xOrigin: viewTransform.origin.x
    property var _yOrigin: viewTransform.origin.y

    transform:
    [
       Scale
       {
           origin.x : 0
           origin.y : 0
           xScale : _xScale
           yScale : -_yScale
       },
       Translate
       {
           x : _xOrigin
           y : _yOrigin
       }
    ]
}
