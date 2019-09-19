
import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Layouts 1.12

Drawer {
	height: window.height

	Rectangle {
		implicitWidth: container.implicitWidth
		height: parent.height
		color: '#6060a0'

		ColumnLayout {
			id: container

			Label {
				id: title
				text: 'Devices'
				color: '#000000'
				font: Qt.font({ pixelSize: 36, weight: 50 })
			}

			Repeater {
				model: scan.devices
				delegate: Device {}
			}
			
			Item {
				Layout.fillWidth: true
				Layout.fillHeight: true
			}
		}
	}
}