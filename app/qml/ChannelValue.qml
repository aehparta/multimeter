
import QtQuick 2.11
import QtQuick.Layouts 1.11
import QtQuick.Controls 1.4
import QtQuick.Controls.Styles 1.4
import QtGraphicalEffects 1.0
import "/js/value.js" as V

RowLayout {
	property bool main: true

	Text {
		Layout.fillWidth: true
		text: V.human(modelData)
		visible: modelData.type != 'slider'
		font: Qt.font({ pixelSize: main ? 80 : 42, weight: 90 })
		horizontalAlignment: Text.AlignRight
	}

	Text {
		id: unitItem
		text: V.unit(modelData)
		visible: V.unit(modelData) && modelData.type != 'slider'
		font: Qt.font({ pixelSize: main ? 72 : 36, weight: 90 })
		horizontalAlignment: Text.AlignRight
	}

	Slider {
		id: sliderItem
		visible: modelData.type == 'slider'
		value: isFinite(modelData.value) ? modelData.value : 0;
		Layout.fillWidth: true
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
