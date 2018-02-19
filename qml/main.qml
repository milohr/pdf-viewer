import QtQuick 1.0
import PdfViewer 1.0

// Theme color: #1fd174

Item {
    width: 800
    height: 600

    Rectangle {
        anchors.top: parent.top
        anchors.bottom: buttons.top
        anchors.right: parent.right
        anchors.left: parent.left

        PdfViewer {
            id: pdf
            anchors.fill: parent
            pageNumber: 0
            source: "test-pdf/Merged.pdf"
            focus: true
            maxZoom: 3

            onZoomChanged: zoomSlider.value = (zoom - 1) / (maxZoom - 1);

            Keys.onPressed: {

                switch(event.key)
                {
                case Qt.Key_W:
                    pdf.pan.y += 10;
                    break;

                case Qt.Key_S:
                    pdf.pan.y -= 10;
                    break;

                case Qt.Key_A:
                    pdf.pan.x += 10;
                    break;

                case Qt.Key_D:
                    pdf.pan.x -= 10;
                    break;

                case Qt.Key_E:
                    pdf.pageOrientation += PdfViewer.HALF_PI;
                    break;

                case Qt.Key_Q:
                    pdf.pageOrientation -= PdfViewer.HALF_PI;
                    break;

                case Qt.Key_Plus:
                    pdf.zoom *= 1.1;
                    break;

                case Qt.Key_Minus:
                    pdf.zoom /= 1.1;
                    break;

                case Qt.Key_Y:
                    pdf.pageNumber--;
                    break;

                case Qt.Key_X:
                    pdf.pageNumber++;
                    break;

                case Qt.Key_R:
                    pdf.zoom = 1;
                    break;

                case Qt.Key_F:
                    pdf.zoom = pdf.coverZoom;
                    break;

                default:
                    break;
                }
            }
        }
    }

    Slider {
        id: zoomSlider
        anchors.top: parent.top
        anchors.bottom: buttons.top
        anchors.right: parent.right
        anchors.margins: 5
        color: "#1fd174"
        value: 0

        onValueChanged: pdf.zoom = 1 + (pdf.maxZoom - 1) * value;
    }

    Rectangle {
         anchors.left: parent.left
         anchors.right: parent.right
         anchors.bottom: buttons.top
         height: 10
         gradient: Gradient {
             GradientStop { position: 0.0; color: "#00000000" }
             GradientStop { position: 1.0; color: "#aaa" }
         }
     }

    Rectangle {
        id: buttons
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.bottom: parent.bottom
        height: buttonRowLeft.implicitHeight  + 2 * buttonRowLeft.anchors.margins
        color: "#fff"

        Row {
            id: buttonRowLeft
            anchors.top: parent.top
            anchors.bottom: parent.bottom
            anchors.left: parent.left
            anchors.right: buttonRowRight.left
            anchors.margins: 10
            spacing: 10

            Button {
                text: "Fit zoom"
                textColor: "white"
                backgroundColor: "#1fd174"
                onClicked: zoomToFitAnimation.start()

                NumberAnimation {
                    id: zoomToFitAnimation
                    target: pdf
                    property: "zoom"
                    to: 1
                    duration: 200
                    easing.type: Easing.InOutQuad;
                }
            }

            Button {
                text: "Cover zoom "
                onClicked: zoomToCoverAnimation.start()

                NumberAnimation {
                    id: zoomToCoverAnimation
                    target: pdf
                    property: "zoom"
                    to: pdf.coverZoom
                    duration: 200
                    easing.type: Easing.InOutQuad;
                }
            }

            Button {
                text: "↶"
                onClicked: pdf.pageOrientation -= PdfViewer.HALF_PI;
            }

            Button {
                text: "↷"
                onClicked: pdf.pageOrientation += PdfViewer.HALF_PI;
            }

            Text {
                text: pdf.documentTitle
                anchors.top: parent.top
                anchors.bottom: parent.bottom
                verticalAlignment: Text.AlignVCenter
                font.bold: true
                visible: pdf.status == PdfViewer.OK
            }

            Text {
                text: "(" + pdf.source + ")"
                anchors.top: parent.top
                anchors.bottom: parent.bottom
                verticalAlignment: Text.AlignVCenter
                opacity: 0.5
                font.bold: true
                visible: pdf.status == PdfViewer.OK
            }

            Text {
                text: "Status: " + pdf.statusMessage
                font.bold: true
                visible: pdf.status != PdfViewer.OK
                anchors.top: parent.top
                anchors.bottom: parent.bottom
                verticalAlignment: Text.AlignVCenter
            }
        }

        Row {
            id: buttonRowRight
            anchors.top: parent.top
            anchors.bottom: parent.bottom
            anchors.right: parent.right
            anchors.margins: 10
            spacing: 10

            Text {
                text: Math.round(pdf.zoom * 100) + "%"
                anchors.top: parent.top
                anchors.bottom: parent.bottom
                verticalAlignment: Text.AlignVCenter
                opacity: 0.5
                font.bold: true
            }

            Button {
                text: "Previous"
                onClicked: pdf.pageNumber--;
            }

            Button {
                text: "Next"
                textColor: "white"
                backgroundColor: "#1fd174"
                onClicked: pdf.pageNumber++;
            }
        }

    }

}

