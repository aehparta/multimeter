
import QtQuick 2.11
import QtQuick.Layouts 1.11
import QtQuick.Controls 1.4
import QtQuick.Controls.Styles 1.4
import QtGraphicalEffects 1.11
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
        visible: modelData.type != 'slider'
        Text {
            id: valueItem
            anchors.bottom: parent.bottom
            anchors.right: unitItem.visible ? unitItem.left : parent.right
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
        visible: modelData.type == 'slider'
        anchors.bottomMargin: main ? 10 : 3

        Slider {
            id: sliderItem
            value: isFinite(modelData.value) ? modelData.value : 0;
            anchors.left: parent.left
            anchors.right: parent.right
	        anchors.leftMargin: main ? 10 : 5
	        anchors.rightMargin: main ? 10 : 5
	        height: main ? 64 : 36
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
