import QtQuick.Controls.Material 2.12
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
	// flags: Qt.FramelessWindowHint
	
	header: ToolBar {
		RowLayout {
			anchors.fill: parent

			Label {
				id: navLabel
				clip: true
				horizontalAlignment: Qt.AlignHCenter
				verticalAlignment: Qt.AlignVCenter
				Layout.fillWidth: true
				Layout.leftMargin: 20
				Layout.rightMargin: 20

				text: 'Channels'
				font: Qt.font({ pixelSize: 36, weight: 80 })
			}

			ToolButton {
				text: '\uf233'
				font: Qt.font({ pixelSize: 36, weight: 80, family: 'Font Awesome 5 Free' })
				onClicked: navLabel.text = 'Channels'
			}

			ToolButton {
				text: '\uf2db'
				font: Qt.font({ pixelSize: 36, weight: 80, family: 'Font Awesome 5 Free' })
				onClicked: navLabel.text = 'Devices'
			}

			ToolButton {
				id: navScanButton
				text: '\uf2f1'
				font: Qt.font({ pixelSize: 36, weight: 80, family: 'Font Awesome 5 Free' })
				onClicked: {
					if (!scan.active) {
						scan.autostart = true;
						scan.start();
					}
				}
				RotationAnimator {
					target: navScanButton.contentItem
					from: 0;
					to: 360;
					duration: 1000
					running: scan.active
					loops: Animation.Infinite
				}
			}

			ToolButton {
				text: '\uf065'
				font: Qt.font({ pixelSize: 36, weight: 80, family: 'Font Awesome 5 Free' })
				onClicked: window.visibility = window.visibility == Window.FullScreen ? Window.AutomaticVisibility : Window.FullScreen
			}
		}
	}

	ColumnLayout {
		anchors.fill: parent

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
		property alias x: window.x
		property alias y: window.y
		property alias width: window.width
		property alias height: window.height
	}

	Component.onCompleted: {
		window.visibility = fullscreen ? Window.FullScreen : Window.AutomaticVisibility
	}
}
