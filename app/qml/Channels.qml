
import QtQuick 2.11
import QtQuick.Controls 2.4
import QtQuick.Controls.Material 2.12

ScrollView {
	clip: true
	ScrollBar.horizontal.policy: ScrollBar.AlwaysOff
	ScrollBar.vertical.policy: ScrollBar.AlwaysOff
	wheelEnabled: true
	contentHeight: content.height

	Flow {
		id: content
		width: window.width
		Repeater {
			id: channelsRepeater
			model: scan.channels
			delegate: Channel {}
		}
	}
}
