import QtQuick 1.0

Item {

    property color backgroundColor: "#ddd"
    property color textColor: "black"
    property string text: "Button"

    signal clicked

    id: root
    height: text.implicitHeight + 2 * text.anchors.margins
    width: text.implicitWidth + 2 * text.anchors.margins

    MouseArea {
        id: mouseArea
        anchors.fill: parent
        onClicked: {
            rippleAnimation.stop()
            rippleAnimation.start()
            root.clicked()
        }
    }

    Rectangle {
        id: button
        color: root.backgroundColor
        anchors.fill: parent
        radius: (height / 2) * 0.4
        clip: true

        Circle {
            id: ripple
            color: Qt.darker(root.backgroundColor, 3)
            anchors.centerIn: parent
            width: 0

            ParallelAnimation {
                id: rippleAnimation
                running: false

                NumberAnimation {
                    target: ripple
                    duration: 150
                    properties: "width"
                    from: 0
                    to: button.width
                    easing.type: Easing.OutCirc
                    easing.amplitude: 2.0
                }

                NumberAnimation {
                    target: ripple
                    duration: 250
                    properties: "opacity"
                    from: 0.6
                    to: 0
                    easing.type: Easing.InCirc
                }
            }
        }
    }

    Text {
        id: text
        anchors.fill: parent
        anchors.margins: 10
        text: root.text
        color: root.textColor
        font.bold: true
        font.capitalization: Font.AllUppercase
    }

}
