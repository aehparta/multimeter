import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Layouts 1.11
import QtQuick.Window 2.11

Drawer {
	id: leftPanel
	Column {
		ToolButton {
			text: '\uf233 Channels'
			font: Qt.font({ pixelSize: 36, weight: 80, family: 'Font Awesome 5 Free' })
			onClicked: {
				window.page = 'channels';
				leftPanel.close();
			}
		}

		ToolButton {
			text: '\uf2db Devices'
			font: Qt.font({ pixelSize: 36, weight: 80, family: 'Font Awesome 5 Free' })
			onClicked: {
				window.page = 'devices';
				leftPanel.close();
			}
		}

		ToolButton {
			text: '\uf2f1 Scan'
			font: Qt.font({ pixelSize: 36, weight: 80, family: 'Font Awesome 5 Free' })
			onClicked: {
				if (!scan.active) {
					scan.autostart = true;
					scan.start();
				}
				leftPanel.close();
			}
		}

		ToolButton {
			text: '\uf065 Fullscreen'
			font: Qt.font({ pixelSize: 36, weight: 80, family: 'Font Awesome 5 Free' })
			onClicked: {
				window.visibility = window.visibility == Window.FullScreen ? Window.AutomaticVisibility : Window.FullScreen;
				leftPanel.close();
			}
		}
	}
}
