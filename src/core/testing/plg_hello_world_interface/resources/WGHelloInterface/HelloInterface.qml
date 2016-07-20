import QtQuick 2.3
import QtQuick.Controls 1.2
import QtQuick.Layouts 1.0
import WGControls 1.0
 
/*!
    \brief Hello World example.
    Please write Doxygen comments like this.
*/
WGPanel {
 
    // All elements should have an id
    id: helloPanelInterface
  
    /*! Title of panel.
        Document new properties which are added with the "property" keyword.
    */
    property var title: qsTr( "Hello World Interface" )
 
    color: palette.MainWindowColor
 
    Canvas {
        id: canvas2d
        anchors.fill: parent
        property var gradientColor: "orange"
        property var image: "icons/icon_100x100.png"
        property var imageOffsetX: 0.0
        property var imageOffsetY: 0.0
 
        Component.onCompleted: {
            loadImage( image );
        }
 
        /*! When an image is loaded refresh the canvas
        */
        onImageLoaded: {
            requestPaint();
        }

        /*! When these properties are changed refresh the canvas
        */
        onImageOffsetXChanged: {
            requestPaint();
        }
        onImageOffsetYChanged: {
            requestPaint();
        }
        onGradientColorChanged: {
            requestPaint();
        }
 
        /*! Paints the canvas only if window is resized or requestPaint is called
        */
        onPaint: {
            var ctx = getContext("2d");
 
            var gradient = ctx.createLinearGradient( 0, 0, 0, height );
            gradient.addColorStop( 0, "white" );
            gradient.addColorStop( 1, gradientColor );
            ctx.fillStyle = gradient;
            ctx.fillRect( 0, 0, width, height );
  
            var imagePixelOffsetX = Math.floor(imageOffsetX * width);
            var imagePixelOffsetY = Math.floor(imageOffsetY * height);

            ctx.fillStyle = "black"
            ctx.font = "30px Arial";
            ctx.fillText( "Hello World Interface", imagePixelOffsetX + 120, imagePixelOffsetY + 60 );

            ctx.shadowOffsetX = 10;
            ctx.shadowOffsetY = 10;
            ctx.shadowColor = "black"
            ctx.shadowBlur = 10
            ctx.drawImage( image, imagePixelOffsetX, imagePixelOffsetY );
            ctx.shadowColor = "transparent"
        }

        ColumnLayout
        {
            anchors.fill: parent

            RowLayout
            {
                Layout.fillWidth: true

                WGSliderControl {
                    id: imagePositionSliderX
                    Layout.fillWidth: true
                    minimumValue: 0.0
                    maximumValue: 1.0
                    stepSize: 0.01
                    label: "X:"
                    value: 0.1

                    onValueChanged: {
                        canvas2d.imageOffsetX = value;
                    }

                    Component.onCompleted: {
                        canvas2d.imageOffsetX = value;
                    }
                }

                WGSliderControl {
                    id: imagePositionSliderY
                    Layout.fillWidth: true
                    minimumValue: 0.0
                    maximumValue: 1.0
                    stepSize: 0.01
                    label: "Y:"
                    value: 0.25

                    onValueChanged: {
                        canvas2d.imageOffsetY = value;
                    }

                    Component.onCompleted: {
                        canvas2d.imageOffsetY = value;
                    }
                }
            }

            WGColorSlider {
                id: gradientColorSlider
                Layout.fillWidth: true
                minimumValue: 0.0
                maximumValue: 1.0
                stepSize: 0.01
                colorData: [Qt.hsva(0.0, 1, 1, 1), Qt.hsva(0.25, 1, 1, 1), Qt.hsva(0.5, 1, 1, 1), Qt.hsva(0.75, 1, 1, 1), Qt.hsva(1.0, 1, 1, 1)]
                positionData: [0, 0.25, 0.5, 0.75, 1.0]
                linkColorsToHandles: false
                value: 0.1
                showAlphaChannel: false

                onValueChanged: {
                    canvas2d.gradientColor = Qt.hsva(value, 1, 1, 1);
                }

                Component.onCompleted: {
                    canvas2d.gradientColor = Qt.hsva(value, 1, 1, 1);
                }
            }

            /*! Spacer item to fill in the blank space
            */
            Item {
                Layout.fillHeight: true
                Layout.fillWidth: true
            }
        }
    }
}