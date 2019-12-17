
import QtQuick 2.11
import QtQuick.Controls 2.4
import QtQuick.Layouts 1.11

ScrollView {
	clip: true
	ScrollBar.horizontal.policy: ScrollBar.AlwaysOff
	ScrollBar.vertical.policy: ScrollBar.AlwaysOff
	wheelEnabled: true
	contentHeight: content.height

	ColumnLayout {
		id: content
		width: window.width
		spacing: 5

		Repeater {
			model: scan.devices
			delegate: Device {}
		}
	}
}
