import QtQuick 2.5
import QtQuick.Controls 1.4
import QtQuick.Controls.Styles 1.4
import QtQuick.Controls.Styles 1.4
import QtGraphicalEffects 1.0
import QtQuick.Layouts 1.0
import "/js/device.js" as DeviceJs
import "."

Column {
	property color colorStart: modelData.colors.length > 0 ? modelData.colors[0] : Style.slider.color
	property color colorEnd: modelData.colors.length > 1 ? modelData.colors[1] : colorStart

	Slider {
		anchors.left: parent.left
		anchors.right: parent.right
		anchors.leftMargin: 5
		anchors.rightMargin: 5
		// value: DeviceJs.valueSliderChanged()
		maximumValue: 255
		minimumValue: 0
		stepSize: 1

		onValueChanged: {
			modelData.value = value;
		}

		style: SliderStyle {
			groove: Rectangle {
				implicitHeight: Style.slider.height
				layer.enabled: true
				layer.effect: LinearGradient {
					end: Qt.point(width, 0)
					gradient: Gradient {
						GradientStop { position: 0.0; color: colorStart }
						GradientStop { position: 1.0; color: colorEnd }
					}
				}
				radius: Style.slider.radius
			}
			handle: Rectangle {
				anchors.centerIn: parent
				color: colorEnd
				border.color: Style.slider.border.color
				border.width: Style.slider.border.width
				radius: Style.slider.border.radius
				implicitWidth: 40
				implicitHeight: 48
			}
		}
	}
}
