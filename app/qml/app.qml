
import QtQuick 2.12
import QtQuick.Controls 1.4
import QtQuick.Controls 2.12
import QtQuick.Layouts 1.12
import "."

ApplicationWindow {
	id: window
	visible: true
	width: 1280
	height: 800
	color: '#171743'

	Devices {
		id: devicesView
	}

	ColumnLayout {
		anchors.fill: parent

		Rectangle {
			height: navLabel.height
			Layout.fillWidth: true
			color: '#6060a0'

			RowLayout {
				anchors.fill: parent

				Label {
					Layout.leftMargin: 20

					text: '\uf054'
					color: '#000000'
					font: Qt.font({ pixelSize: 36, weight: 80, family: 'Font Awesome 5 Free' })

					MouseArea {
						anchors.fill: parent
						onClicked: devicesView.open()
					}
				}

				Label {
					id: navLabel
					clip: true
					Layout.fillWidth: true
					Layout.leftMargin: 20
					Layout.rightMargin: 20

					text: 'All devices'
					color: '#202040'
					font: Qt.font({ pixelSize: 36, weight: 80 })
				}

				Text {
					id: navMenuButton
					Layout.rightMargin: navLabel.Layout.leftMargin

					text: '☰'
					font: navLabel.font

					MouseArea {
						anchors.fill: parent
						onClicked: contentView.push(datetimePickerView)
					}
				}
			}
		}

		/* divider */
		Item {
			Layout.fillWidth: true
			height: 0
		}

		/* content */
		StackView {
			id: contentView
			initialItem: channelsView
			Layout.fillWidth: true
			Layout.fillHeight: true
		}

		/* channels view */
		Component {
			id: channelsView
			Channels {
			}
		}

		/* date and time picker */
		Component {
			id: datetimePickerView
			ColumnLayout {
				Layout.fillWidth: true
				Layout.fillHeight: true
				Calendar {
					frameVisible: false
					Layout.fillWidth: true
					Layout.fillHeight: true
					Layout.alignment: Qt.AlignHCenter | Qt.AlignTop
				}
				RowLayout {
					Layout.fillWidth: true
					Tumbler {
						model: 24
					}
					Tumbler {
						model: 60
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
