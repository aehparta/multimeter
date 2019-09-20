
import QtQuick 2.12
import QtQuick.Layouts 1.12
import QtQuick.Controls 1.4
import QtQuick.Controls.Styles 1.4
import QtGraphicalEffects 1.12
import "/js/value.js" as V

Item {
	property bool main: true

    height: {
    	if (modelData.type == "slider") {
			return sliderItem.height + sliderContainer.anchors.topMargin + sliderContainer.anchors.bottomMargin;
		}
		return valueItem.height;
    }

    Item {
        anchors.fill: parent
        visible: {
            switch (modelData.type) {
            case 'slider':
                return false;
            }
            return true;
        }
        Text {
            id: valueItem
            anchors.bottom: parent.bottom
            anchors.right: unitItem.left
            anchors.rightMargin: main ? 20 : 10
            text: V.human(modelData)
            color: '#000000'
            font: Qt.font({ pixelSize: main ? 96 : 42, weight: 90 })
        }

        Text {
            id: unitItem
            anchors.bottom: parent.bottom
            anchors.right: parent.right
            anchors.rightMargin: main ? 20 : 10
            text: V.unit(modelData)
            visible: V.unit(modelData)
            color: '#202050'
            font: Qt.font({ pixelSize: main ? 82 : 36, weight: 90 })
        }
    }

    Item {
    	id: sliderContainer
        anchors.fill: parent
        anchors.topMargin: main ? 10 : 5
        anchors.bottomMargin: main ? 10 : 5
        anchors.leftMargin: main ? 10 : 5
        anchors.rightMargin: main ? 10 : 5
        visible: modelData.type == 'slider'

        Slider {
            id: sliderItem
            value: modelData.value
            anchors.left: parent.left
            anchors.right: parent.right
            maximumValue: 255
            minimumValue: 0
            stepSize: 1

            property color colorStart: modelData.color[0] ? modelData.color[0] : '#ffffff'
            property color colorEnd: modelData.color[1] ? modelData.color[1] : sliderItem.colorStart

            onValueChanged: {
                if (modelData.mode != 'sink' || modelData.method != 'push' || modelData.value == value) {
                    return;
                }
                modelData.value = value;
            }

            style: SliderStyle {
                groove: Rectangle {
                    implicitHeight: main ? 16 : 12
                    layer.enabled: true
                    layer.effect: LinearGradient {
                        end: Qt.point(width, 0)
                        gradient: Gradient {
                            GradientStop { position: 0.0; color: sliderItem.colorStart }
                            GradientStop { position: 1.0; color: sliderItem.colorEnd }
                        }
                    }
                    radius: 8
                }
                handle: Rectangle {
                    anchors.centerIn: parent
                    color: control.pressed ? sliderItem.colorEnd : sliderItem.colorEnd
                    border.color: '#000000'
                    border.width: 2
                    implicitWidth: main ? 40 : 28
                    implicitHeight: main ? 48 : 28
                    radius: main ? 16 : 8
                }
            }
        }
    }
}
