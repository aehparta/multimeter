
import QtQuick 2.11
import QtQuick.Controls 2.4
import QtQuick.Layouts 1.11

ColumnLayout {
	id: deviceView
	Layout.fillWidth: true

	RowLayout {
		Layout.fillWidth: true
		CheckBox {
			checked: modelData.enabled
			onClicked: modelData.enabled = checked
		}
		ColumnLayout {
			Layout.fillWidth: true
			Label {
				text: modelData.name
				color: '#000000'
				font: Qt.font({ pixelSize: 36, weight: 50 })
			}
			Label {
				text: modelData.address() + (modelData.port() < 0 ? '' : ':' + modelData.port())
				color: '#303060'
				font: Qt.font({ pixelSize: 24, weight: 50 })
			}
		}
		Label {
			width: 100
			text: deviceChannelsView.visible ? '\uf078' : '\uf054'
			font: Qt.font({ pixelSize: 64, weight: 80, family: 'Font Awesome 5 Free' })
			Layout.alignment: Qt.AlignRight
			MouseArea {
				anchors.fill: parent
				onClicked: deviceChannelsView.visible = !deviceChannelsView.visible
			}
		}
	}

	ColumnLayout {
		id: deviceChannelsView
		Layout.fillWidth: true
		visible: false
		Repeater {
			model: modelData.channels
			delegate: RowLayout {
				Layout.leftMargin: 40
				CheckBox {
					checked: modelData.enabled
					onClicked: modelData.enabled = checked
				}
				ColumnLayout {
					Label {
						text: modelData.name
						color: '#000000'
						font: Qt.font({ pixelSize: 36, weight: 50 })
					}
				}
			}
		}
	}
}
