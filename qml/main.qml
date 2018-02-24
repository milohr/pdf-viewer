import QtQuick 1.0
import PdfViewer 1.0

Item {
    width: 800
    height: 600
    property color themeColor: "#1fd174"

    Rectangle {
        anchors.fill: pdf
        color: "#eee"
    }

    // PDF Viewer:
    PdfViewer {
        id: pdf
        anchors.top: parent.top
        anchors.bottom: buttons.top
        anchors.right: parent.right
        anchors.left: parent.left
        pageNumber: 0
        source: "../../../pdf-viewer/test-pdf/Merged.pdf"
        focus: true
        maxZoom: 4

        onZoomChanged: zoomSlider.value = (zoom - pdf.fitZoom) / (maxZoom - 1)

        Keys.onPressed: {

                switch(event.key)
                {
                case Qt.Key_W:
                    pdf.pan.y += 10
                    break

                case Qt.Key_S:
                    pdf.pan.y -= 10
                    break

                case Qt.Key_A:
                    pdf.pan.x += 10
                    break

                case Qt.Key_D:
                    pdf.pan.x -= 10
                    break

                case Qt.Key_E:
                    pdf.rotatePageClockwise()
                    break

                case Qt.Key_Q:
                    pdf.rotatePageCounterClockwise()
                    break

                case Qt.Key_Plus:
                    pdf.zoomIn(1.1)
                    break

                case Qt.Key_Minus:
                    pdf.zoomOut(1.1)
                    break

                case Qt.Key_Y:
                    pdf.pageNumber--
                    break

                case Qt.Key_X:
                    pdf.pageNumber++
                    break

                case Qt.Key_R:
                    pdf.zoom = pdf.fitZoom
                    pdf.pan = pdf.fitPan
                    break

                case Qt.Key_F:
                    pdf.zoom = pdf.coverZoom
                    pdf.pan = pdf.coverPan
                    break

                default:
                    break
                }
            }
    }

    // Zoom slider:
    Slider {
        id: zoomSlider
        anchors.top: parent.top
        anchors.bottom: buttons.top
        anchors.right: parent.right
        anchors.margins: 5
        color: themeColor
        value: 0

        onValueChanged: pdf.zoom = 1 + (pdf.maxZoom - pdf.fitZoom) * value
    }

    // Shadow of the button row:
    Rectangle {
         anchors.left: parent.left
         anchors.right: parent.right
         anchors.bottom: buttons.top
         height: 10
         gradient: Gradient {
             GradientStop {
                 position: 0.0
                 color: "#00000000"
             }
             GradientStop {
                 position: 1.0
                 color: "#55000000"
             }
         }
    }

    // Button row container:
    Rectangle {
        id: buttons
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.bottom: parent.bottom
        height: buttonRowLeft.implicitHeight  + 2 * buttonRowLeft.anchors.margins
        color: "#fff"

        // Left button row:
        Row {
            id: buttonRowLeft
            anchors.top: parent.top
            anchors.bottom: parent.bottom
            anchors.left: parent.left
            anchors.right: buttonRowRight.left
            anchors.margins: 10
            spacing: 10

            // Fit zoom button:
            Button {
                text: "Fit zoom"
                textColor: "white"
                backgroundColor: themeColor
                onClicked: zoomToFitAnimation.start()

                ParallelAnimation {
                    id: zoomToFitAnimation

                    NumberAnimation {
                        target: pdf
                        property: "zoom"
                        to: 1
                        duration: 200
                        easing.type: Easing.InOutQuad
                    }

                    PropertyAnimation {
                        target: pdf
                        property: "pan"
                        to: pdf.fitPan
                        duration: 200
                        easing.type: Easing.InOutQuad
                    }
                }

            }

            // Cover zoom button:
            Button {
                text: "Cover zoom "
                onClicked: zoomToCoverAnimation.start()

                ParallelAnimation {
                    id: zoomToCoverAnimation

                    NumberAnimation {
                        target: pdf
                        property: "zoom"
                        to: pdf.coverZoom
                        duration: 200
                        easing.type: Easing.InOutQuad
                    }

                    PropertyAnimation {
                        target: pdf
                        property: "pan"
                        to: pdf.coverPan
                        duration: 200
                        easing.type: Easing.InOutQuad
                    }
                }
            }

            // Rotate counter-clockwise:
            Button {
                text: "↶"
                onClicked: pdf.rotatePageCounterClockwise()
            }

            // Rotate clockwise:
            Button {
                text: "↷"
                onClicked: pdf.rotatePageClockwise()
            }

            // Document title:
            Text {
                text: pdf.documentTitle
                anchors.top: parent.top
                anchors.bottom: parent.bottom
                verticalAlignment: Text.AlignVCenter
                font.bold: true
                visible: pdf.status == PdfViewer.OK
            }

            // Document status message, in case that the status is not okay:
            Text {
                text: pdf.statusMessage
                font.bold: true
                visible: pdf.status != PdfViewer.OK
                anchors.top: parent.top
                anchors.bottom: parent.bottom
                verticalAlignment: Text.AlignVCenter
            }

            // Document file path:
            Text {
                text: "(" + pdf.source + ")"
                anchors.top: parent.top
                anchors.bottom: parent.bottom
                verticalAlignment: Text.AlignVCenter
                opacity: 0.5
                font.bold: true
            }
        }

        // Right button row:
        Row {
            id: buttonRowRight
            anchors.top: parent.top
            anchors.bottom: parent.bottom
            anchors.right: parent.right
            anchors.margins: 10
            spacing: 10

            // Zoom indicator:
            Text {
                text: Math.round(pdf.zoom * 100) + "%"
                anchors.top: parent.top
                anchors.bottom: parent.bottom
                verticalAlignment: Text.AlignVCenter
                opacity: 0.5
                font.bold: true
            }

            // Previous page:
            Button {
                text: "Previous"
                onClicked: pdf.pageNumber--
            }

            // Next page:
            Button {
                text: "Next"
                textColor: "white"
                backgroundColor: themeColor
                onClicked: pdf.pageNumber++
            }
        }

    }

}

