
import QtQuick 2.12
import QtQuick.Layouts 1.12
import QtGraphicalEffects 1.12

Rectangle {
	width: window.width / parseInt(window.width < 500 ? 1 : window.width / 500) - channelsView.spacing
	height: content.height + 10
	color: 'transparent'
	visible: modelData.enabled

	/* background (borders) */
	RectangularGlow {
		anchors.fill: content
		glowRadius: 7
		spread: 0.05
		color: '#303070'
		cornerRadius: 0
	}

	/* content */
	Column {
		id: content
		width: parent.width - 10
		anchors.centerIn: parent
		clip: true
		spacing: 0

		/* header */
		ChannelHeader {
			width: parent.width
		}

		ChannelValue {
			width: parent.width
		}

		Repeater {
			model: modelData.children
			delegate: ChannelValue {
				width: parent.width
			}
		}
	}
}
