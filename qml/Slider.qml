import QtQuick 1.1

Item {
    /**
      * The radius of the dragger while not currently being dragged.
      */
    property real radiusDefault: 40

    /**
      * The radius the dragger' outer ring will grow to when being dragged.
      */
    property real radiusOuter: 100

    /**
      * The radius the dragger' inner circle will shrunk to when being dragged.
      */
    property real radiusInner: 20

    /**
      * The draggers color.
      */
    property color color: "#444"

    /**
      * The dragging line color.
      */
    property color lineColor: "#ddd"

    id: root
    width: radiusOuter

    /**
      * A box creating additional padding, so that the dragger does not
      * exceed the rectangle bounds when it grows.
      */
    Item {
        id: box
        anchors.fill: parent
        anchors.margins: (root.radiusOuter - root.radiusDefault) / 2

        /**
          * The vertical line the dragger is dragged along.
          */
        Rectangle {
            id: line
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.top: parent.top
            anchors.bottom: parent.bottom
            anchors.topMargin: root.radiusInner / 2
            anchors.bottomMargin: root.radiusInner / 2
            width: 5
            radius: width / 2
            color: root.lineColor
        }

        /**
          * The item that is actually being dragged. Any visual items that move are just attached to this one.
          * This is mainly due to the fact that the visual items are resized by animations.
          */
        Item {
            id: draggerControl
            anchors.horizontalCenter: parent.horizontalCenter
            width: root.radiusDefault
            height: width

            MouseArea {
                id: draggerMouseArea
                anchors.fill: parent
                drag.target: parent
                drag.axis: Drag.YAxis
                drag.minimumY: 0
                drag.maximumY: box.height - root.radiusDefault
            }
        }

        /**
          * The inner drag circle, shrunking when being dragged.
          */
        Circle {
            id: draggerInner
            anchors.centerIn: draggerControl
            width: root.radiusDefault
            color: root.color

            states: State {
                when: draggerMouseArea.pressed
                PropertyChanges {
                    target: draggerInner
                    width: root.radiusInner
                }
            }

            transitions: Transition {
                NumberAnimation {
                    duration: 200
                    properties: "width"
                    easing.type: Easing.OutCirc
                }
            }
        }

        /**
          * The outer drag circle, grwoing when being dragged.
          */
        Circle {
            id: draggerOuter
            anchors.centerIn: draggerControl
            width: root.radiusDefault
            color: root.color

            states: State {
                when: draggerMouseArea.pressed
                PropertyChanges {
                    target: draggerOuter
                    width: root.radiusOuter
                    opacity: 0.4
                }
            }

            transitions: Transition {
                NumberAnimation {
                    duration: 200
                    properties: "width,opacity"
                    easing.type: Easing.OutCirc
                }
            }
        }
    }
}
