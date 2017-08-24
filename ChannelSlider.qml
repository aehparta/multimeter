import QtQuick 2.5
import QtQuick.Controls 1.4
import QtQuick.Controls.Styles 1.4
import QtQuick.Controls.Styles 1.4
import QtGraphicalEffects 1.0
import QtQuick.Layouts 1.0
import "device.js" as DeviceJs
import "."

ColumnLayout {
	property color colorStart: modelData.chColors.length > 0 ? modelData.chColors[0] : Style.sliderColorStart
	property color colorEnd: modelData.chColors.length > 1 ? modelData.chColors[1] : colorStart

	Slider {
		anchors.left: parent.left
		anchors.right: parent.right
		anchors.leftMargin: 5
		anchors.rightMargin: 5
		value: DeviceJs.valueSliderChanged()
		maximumValue: 100
		minimumValue: 0
		stepSize: 1

		onValueChanged: {
			modelData.chValue = value;
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
				color: control.pressed ? Style.sliderHandlePressedColor : Style.sliderHandleColor
				border.color: Style.sliderHandleBorderColor
				border.width: 2
				implicitWidth: 40
				implicitHeight: 48
				radius: 16
			}
		}
	}
}
