import QtQml 2.2
import QtQuick 2.5
import QtQuick.Controls 1.4
import QtQuick.Dialogs 1.2
import QtQuick.Layouts 1.0
// import Device 1.0
import "/js/device.js" as DeviceJs
import "."

Column {
	Row {
		id: itemValueTop
		anchors.right: parent.right
		anchors.left: parent.left

		Text {
			id: itemValueLeftTop
			visible: false
			anchors.left: parent.left
			anchors.leftMargin: 10
			text: DeviceJs.valueLeftTopChanged()
            font: Style.value.font2
			color: Style.value.color2
		}

		Text {
			id: itemValueRightTop
			visible: false
			anchors.right: parent.right
			anchors.rightMargin: 10
			text: DeviceJs.valueRightTopChanged()
			font: Style.value.font2
			color: Style.value.color2
		}
	}

	Text {
		id: itemValue
		visible: false
		anchors.right: parent.right
		anchors.rightMargin: 10
		text: DeviceJs.valueChanged()
		font: Style.value.font1
		color: Style.value.color1
		MouseArea {
			anchors.fill: parent
			onClicked: DeviceJs.valueSetRequest()
		}
	}
}
