
import QtQuick 2.12
import QtQuick.Controls 2.12

ScrollView {
	clip: true
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
