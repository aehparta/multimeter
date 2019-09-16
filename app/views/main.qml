import QtQuick 2.5
import QtQuick.Controls 1.4
import QtQuick.Dialogs 1.2
import QtBluetooth 5.2
import QtQuick.Controls.Styles 1.4
import QtQuick.Layouts 1.0
import "/js/device.js" as DeviceJs
import "."


ApplicationWindow {
	id: window
	visible: true
	width: 640
	height: 800
	title: 'Multimeter'
	color: Style.windowBackgroundColor

	Component.onCompleted: {
		devicesObject.scan();
	}

	Connections {
		target: devicesObject
		onScanStarted: {
			console.log('scan started');
		}
		onScanFinished: {
			console.log('scan finished');
		}
	}

	Column {
		anchors.fill: parent

		ListView {
			id: channels
			anchors.fill: parent
			clip: true
			focus: true
			model: channelsModel
			spacing: Style.channel.spacing

			Rectangle {
				
			}

			delegate: DeviceView {
			}
		}
	}
}

