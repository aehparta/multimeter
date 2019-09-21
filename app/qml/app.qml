
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

	ColumnLayout {
		anchors.fill: parent

		Rectangle {
			height: navLabel.height
			Layout.fillWidth: true
			color: '#6060a0'

			RowLayout {
				anchors.fill: parent

				Label {
					id: navLabel
					clip: true
					Layout.fillWidth: true
					Layout.leftMargin: 20
					Layout.rightMargin: 20

					text: 'Devices'
					color: '#202040'
					font: Qt.font({ pixelSize: 36, weight: 80 })
				}

				Label {
					id: navChannelsButton
					Layout.rightMargin: 20

					text: '\uf233'
					color: '#000000'
					font: Qt.font({ pixelSize: 36, weight: 80, family: 'Font Awesome 5 Free' })

					MouseArea {
						anchors.fill: parent
						onClicked: {
							navLabel.text = 'Channels'
						}
					}
				}

				Label {
					id: navDevicesButton
					Layout.rightMargin: 20

					text: '\uf2db'
					color: '#000000'
					font: Qt.font({ pixelSize: 36, weight: 80, family: 'Font Awesome 5 Free' })

					MouseArea {
						anchors.fill: parent
						onClicked: {
							navLabel.text = 'Devices'
						}
					}
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

		ScrollView {
			clip: true
			Layout.fillWidth: true
			Layout.fillHeight: true
			ScrollBar.horizontal.policy: ScrollBar.AlwaysOff
			ScrollBar.vertical.policy: ScrollBar.AlwaysOff
			wheelEnabled: true
			contentWidth: window.width
			contentHeight: {
				switch (navLabel.text) {
				case 'Channels':
					return channelsView.height;
				case 'Devices':
					return devicesView.height
				}
				return 0;
			}

			/* channels view */
			Channels {
				id: channelsView
				width: parent.width
				visible: navLabel.text == 'Channels'
			}

			/* devices view */
			Devices {
				id: devicesView
				width: parent.width
				visible: navLabel.text == 'Devices'
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
