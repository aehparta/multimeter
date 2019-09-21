
import QtQuick 2.11
import QtQuick.Controls 1.4
import QtQuick.Controls 2.4
import QtQuick.Layouts 1.11
import QtQuick.Window 2.11
import Qt.labs.settings 1.0
import "."

ApplicationWindow {
	id: window
	visible: true
	width: 640
	height: 480
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

				Label {
					id: navScanButton
					Layout.rightMargin: 20

					text: '\uf2f1'
					color: scan.active ? '#202050' : '#000000'
					font: Qt.font({ pixelSize: 36, weight: 80, family: 'Font Awesome 5 Free' })

					MouseArea {
						anchors.fill: parent
						onClicked: {
							if (!scan.active) {
								scan.autostart = true;
								scan.start();
							}
						}
					}

					RotationAnimator {
						target: navScanButton
						from: 0;
						to: 360;
						duration: 1000
						running: scan.active
						loops: Animation.Infinite
					}
				}

				Label {
					id: navFullscreenButton
					Layout.rightMargin: 20

					text: '\uf065'
					color: '#000000'
					font: Qt.font({ pixelSize: 36, weight: 80, family: 'Font Awesome 5 Free' })

					MouseArea {
						anchors.fill: parent
						onClicked: {
							window.visibility = window.visibility == Window.FullScreen ? Window.AutomaticVisibility : Window.FullScreen;
						}
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

	/* fullscreen off when escape is pressed */
	Item {
		focus: true
		Keys.onEscapePressed: {
			window.visibility = Window.AutomaticVisibility;
		}
	}

	Settings {
		id: settings
	}

	Component.onCompleted: {
		if (settings.value("window/width") > 0) {
			window.width = settings.value("window/width");
			window.height = settings.value("window/height");
			window.x = settings.value("window/x");
			window.y = settings.value("window/y");
		}
		window.visibility = fullscreen ? Window.FullScreen : Window.AutomaticVisibility

		/* start scanner */
		// scan.autostart = true;
		// scan.start();
	}

	Component.onDestruction: {
		if (window.visibility != Window.FullScreen) {
			settings.setValue('window/width', window.width);
			settings.setValue('window/height', window.height);
			settings.setValue('window/x', window.x);
			settings.setValue('window/y', window.y);
		}
	}
}
