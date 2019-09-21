
import QtQuick 2.11
import QtQuick.Controls 2.4

ScrollView {
	clip: true
	ScrollBar.horizontal.policy: ScrollBar.AlwaysOff
	ScrollBar.vertical.policy: ScrollBar.AlwaysOff
	wheelEnabled: true
	contentWidth: window.width

	Flow {
		id: channelsView
		spacing: 5
		width: window.width
		// height: window.height
		// flow: Flow.TopToBottom
		
		Repeater {
			id: channelsRepeater
			model: scan.channels
			delegate: Channel {
			}
		}
	}
}
