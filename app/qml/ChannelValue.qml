
import QtQuick 2.12
import QtQuick.Layouts 1.12
import QtQuick.Controls 1.4
import QtQuick.Controls.Styles 1.4
import QtGraphicalEffects 1.12

Rectangle {
	height: value.height
	color: '#9090e0'
	visible: {
		switch (modelData.type) {
		case 'switch':
			return false;
		case 'group':
			return false;
		}
		return true;
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
			id: value
			anchors.bottom: parent.bottom
			anchors.right: unit.left
			text: modelData.value
			color: '#000000'
			font: Qt.font({ pixelSize: 96, weight: 90 })
		}

		Text {
			id: unit
			anchors.bottom: parent.bottom
			anchors.right: parent.right
			text: {
				var units = {
					current: 'A',
					frequency: 'Hz',
					humidity: '%',
					resistance: 'Ω',
					voltage: 'V',
					wattage: 'W',
					/* temperature scales */
					temperature: '°C',
					celsius: '°C',
					kelvin: 'K',
					fahrenheit: '°F',
				};
				return units[modelData.type] !== undefined ? ' ' + units[modelData.type] : '';
			}
			color: '#101030'
			font: Qt.font({ pixelSize: 96, weight: 90 })
		}
	}

	Item {
		anchors.fill: parent
		visible: modelData.type == 'slider'

		Slider {
			id: slider
			value: modelData.value
			anchors.left: parent.left
			anchors.right: parent.right
			anchors.leftMargin: 5
			anchors.rightMargin: 5
			maximumValue: 255
			minimumValue: 0
			stepSize: 1

			property color colorStart: '#000000'
			property color colorEnd: '#000000'

			onValueChanged: {
				modelData.value = slider.value;
			}

			style: SliderStyle {
				groove: Rectangle {
					implicitHeight: 16
					layer.enabled: true
					layer.effect: LinearGradient {
						end: Qt.point(width, 0)
						gradient: Gradient {
							GradientStop { position: 0.0; color: colorStart }
							GradientStop { position: 1.0; color: colorEnd }
						}
					}
					radius: 8
				}
				handle: Rectangle {
					anchors.centerIn: parent
					color: control.pressed ? '#000000' : '#000000'
					border.color: '#000000'
					border.width: 2
					implicitWidth: 40
					implicitHeight: 48
					radius: 16
				}
			}
		}
	}

}
