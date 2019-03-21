import QtQuick 2.5
import QtQuick.Controls 1.4
import QtQuick.Dialogs 1.2
import QtBluetooth 5.2
import QtQuick.Controls.Styles 1.4
import QtQuick.Layouts 1.0
import "device.js" as DeviceJs
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

	ColumnLayout {
		anchors.fill: parent

		ListView {
			id: channels
			anchors.top: parent.top
			anchors.bottom: parent.bottom
			anchors.left: parent.left
			anchors.right: parent.right
			clip: true
			focus: true
			model: channelsModel
			//spacing: 2

			Rectangle {
				id: refresh
				anchors.left: parent.left
				anchors.right: parent.right
				color: '#000040'
				height: 100;
				y: devicesObject.scanInProgress ? 0 : -channels.contentY - height

				Text {
					id: refreshText
					anchors.fill: parent
					text: 'Scanning...'
					font.pointSize: 32
					color: '#b0f0b0'
					horizontalAlignment: Text.AlignHCenter
					verticalAlignment: Text.AlignVCenter
				}

				SequentialAnimation on color {
					loops: Animation.Infinite
					ColorAnimation { from: '#000020'; to: '#0000a0'; duration: 2000 }
					ColorAnimation { from: '#0000a0'; to: '#000020'; duration: 2000 }
				}

				states: [
					State {
						name: "pull to scan"
						when: (channels.contentY > -(refresh.height + 100) && !devicesObject.scanInProgress)
						PropertyChanges {
							target: refreshText
							text: 'Pull to start scan...'
						}
					},
					State {
						name: "start scan"
						when: (channels.contentY <= -(refresh.height + 100) && !devicesObject.scanInProgress)
						StateChangeScript {
							name: "start scan script"
							script: {
								devicesObject.scan();
							}
						}
					}
				]
			}

			delegate: DeviceView {
			}
		}
	}
}

