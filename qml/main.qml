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
        anchors.top: parent.top
        anchors.bottom: buttons.top
        anchors.right: parent.right
        anchors.margins: 5
        color: "#1fd174"
    }

    Rectangle {
         anchors.left: parent.left
         anchors.right: parent.right
         anchors.bottom: buttons.top
         height: 10
         gradient: Gradient {
             GradientStop { position: 0.0; color: "#00bbbbbb" }
             GradientStop { position: 1.0; color: "#bbb" }
         }
     }

    Rectangle {
        id: buttons
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.bottom: parent.bottom
        height: buttonRow.implicitHeight  + 2 * buttonRow.anchors.margins
        color: "#fff"

        Row {
            anchors.fill: parent
            anchors.margins: 10
            id: buttonRow
            spacing: 10

            Button {
                text: "Fit zoom"
                textColor: "white"
                backgroundColor: "#1fd174"
                onClicked: pdf.zoom = 1
            }

            Button {
                text: "Cover zoom"
                onClicked: pdf.zoom = pdf.coverZoom
            }

            Button {
                text: "↶"
                onClicked: pdf.pageOrientation -= PdfViewer.HALF_PI;
            }

            Button {
                text: "↷"
                onClicked: pdf.pageOrientation += PdfViewer.HALF_PI;
            }
        }

        Row {
            anchors.top: parent.top
            anchors.bottom: parent.bottom
            anchors.right: parent.right
            anchors.margins: 10
            spacing: 10

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

