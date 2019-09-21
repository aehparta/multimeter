
import QtQuick 2.11
import QtQuick.Controls 2.11
import QtQuick.Layouts 1.11

Rectangle {
	implicitWidth: content.implicitWidth
	implicitHeight: content.implicitHeight
	color: modelData.enabled ? '#9090e0' : '#6060a0'

	ColumnLayout {
		id: content
		
		RowLayout {
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
				text: channels.visible ? '\uf078' : '\uf054'
				font: Qt.font({ pixelSize: 64, weight: 80, family: 'Font Awesome 5 Free' })
				MouseArea {
					anchors.fill: parent
					onClicked: channels.visible = !channels.visible
				}
			}
		}

		ColumnLayout {
			id: channels
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
}
