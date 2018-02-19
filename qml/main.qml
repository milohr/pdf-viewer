import QtQuick 1.0
import PdfViewer 1.0

Item {
    width: 800
    height: 600

    Rectangle {
        anchors.top: parent.top
        anchors.bottom: parent.bottom
        anchors.left: parent.left
        width: parent.width / 2
        color: "#1fd174"

        Text {
            text: pdf.pageNumber
            id: text
            wrapMode: Text.Wrap
            color: "white"
            font.bold: true
            font.pointSize: 27
            anchors.centerIn: parent
            width: parent.width
            horizontalAlignment: Text.AlignHCenter
        }

        MouseArea {
            id: mouseArea
            anchors.fill: parent
        }
    }

    Rectangle {
        anchors.top: parent.top
        anchors.bottom: parent.bottom
        anchors.right: parent.right
        width: parent.width / 2

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

}

