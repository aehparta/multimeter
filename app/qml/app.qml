import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Layouts 1.12
import "."

ApplicationWindow {
	id: window
	visible: true
	width: 1280
	height: 800
	color: '#171743'

	ColumnLayout {
		anchors.fill: parent

		Rectangle {
			height: navLabel.height
			Layout.fillWidth: true
			color: '#6060a0'

			RowLayout {
				anchors.fill: parent

				Text {
					id: navLabel
					clip: true
					Layout.fillWidth: true
					Layout.leftMargin: 20
					Layout.rightMargin: 20

					text: '➤ All devices'
					font: Qt.font({ pixelSize: 36, weight: 50 })

					MouseArea {
						anchors.fill: parent
						// onClicked: pages.currentIndex = 1
					}
				}

				Text {
					id: navMenuButton
					Layout.rightMargin: navLabel.Layout.leftMargin

					text: '☰'
					font: navLabel.font
				}
			}
		}

		/* divider */
		Item {
			Layout.fillWidth: true
			height: 0
		}

		/* content */
		ScrollView {
			clip: true
			Layout.fillWidth: true
			Layout.fillHeight: true
			ScrollBar.horizontal.policy: ScrollBar.AlwaysOff
			ScrollBar.vertical.policy: ScrollBar.AlwaysOff

			Flow {
				id: channelsView
				spacing: 5
				width: window.width

				Repeater {
					id: channelsRepeater
					model: scan.channels
					delegate: Channel {

					}
				}
			}
		}
	}

	Component.onCompleted: {
		scan.autostart = true;
		scan.start()
	}
}
