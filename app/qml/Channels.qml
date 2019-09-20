
import QtQuick 2.12
import QtQuick.Controls 2.12

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
