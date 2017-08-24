import QtQml 2.2
import QtQuick 2.5
import QtQuick.Controls 1.4
import QtQuick.Dialogs 1.2
import QtQuick.Layouts 1.0
// import Device 1.0
import "device.js" as DeviceJs
import "."

ColumnLayout {
	RowLayout {
		anchors.right: parent.right
		anchors.left: parent.left

		Text {
			id: itemValueLeftTop
			visible: false
			anchors.left: parent.left
			anchors.leftMargin: 10
			text: DeviceJs.valueLeftTopChanged()
			font.pointSize: 28
			color: Style.valueSideColor
		}

		Text {
			id: itemValueRightTop
			visible: false
			anchors.right: parent.right
			anchors.rightMargin: 10
			text: DeviceJs.valueRightTopChanged()
			font.pointSize: 28
			color: Style.valueSideColor
		}
	}

	Text {
		id: itemValue
		visible: false
		anchors.right: parent.right
		anchors.rightMargin: 10
		text: DeviceJs.valueChanged()
		font.pointSize: 76
		color: Style.valueMainColor
		MouseArea {
			anchors.fill: parent
			onClicked: DeviceJs.valueSetRequest()
		}
	}
}
