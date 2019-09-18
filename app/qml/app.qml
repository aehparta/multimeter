import QtQuick 2.5
import QtQuick.Controls 2.5
import "/js/app.js" as App
// import "/js/test.js" as Test
import "."

ApplicationWindow {
	id: window
	visible: true
	width: 640
	height: 800
	color: '#000020'
	ScrollView {
		anchors.left: parent.left
		anchors.top: parent.top
		anchors.right: parent.right
		anchors.bottom: parent.bottom
		ScrollBar.horizontal.policy: ScrollBar.AlwaysOff
		ScrollBar.vertical.policy: ScrollBar.AlwaysOff

		Flow {
			id: channelsView
			spacing: 5
			width: window.width

			Repeater {
				id: channelsRepeater
				property var list: []
				model: list
				delegate: Channel {

				}
			}

			// Timer {
			// 	interval: 1000
			// }
		}
	}
}
