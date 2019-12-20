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
	
	property string page: 'channels'

	header: ToolBar {
		RowLayout {
			anchors.fill: parent

			ToolButton {
				text: '\uf142'
				font: Qt.font({ pixelSize: 36, weight: 80, family: 'Font Awesome 5 Free' })
				onClicked: leftPanel.open()
			}

			Label {
				id: navLabel
				clip: true
				horizontalAlignment: Qt.AlignHCenter
				verticalAlignment: Qt.AlignVCenter
				Layout.fillWidth: true
				Layout.leftMargin: 20
				Layout.rightMargin: 20

				text: window.page
				font: Qt.font({ pixelSize: 36, weight: 80 })
			}

			ToolButton {
				text: '\uf2ed'
				font: Qt.font({ pixelSize: 36, weight: 80, family: 'Font Awesome 5 Free' })
				visible: scan.selected > 0 && window.page == 'devices'
				onClicked: scan.selectedRemove()
			}

			ToolButton {
				id: scanActive
				text: '\uf2f1'
				font: Qt.font({ pixelSize: 36, weight: 80, family: 'Font Awesome 5 Free' })
				visible: scan.active
				RotationAnimator {
					target: scanActive.contentItem
					from: 0;
					to: 360;
					duration: 1000
					running: scan.active
					loops: Animation.Infinite
				}
			}
		}
	}

	Channels {
		id: channelsView
		anchors.fill: parent
		visible: window.page == 'channels'
	}

	Devices {
		id: devicesView
		anchors.fill: parent
		visible: window.page == 'devices'
	}

	/* left slide panel */
	LeftPanel {
		id: leftPanel
		// width: window.width * 0.7
		height: window.height
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
