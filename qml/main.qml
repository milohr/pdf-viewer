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
            id: text
            text: pdf.statusMessage
            color: "white"
            font.bold: true
            font.pointSize: 27
            anchors.centerIn: parent
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
            pageNumber: 4
            source: "test-pdf/Merged.pdf"
        }
    }

}

