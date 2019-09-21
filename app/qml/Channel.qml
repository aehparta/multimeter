
import QtQuick 2.11
import QtQuick.Layouts 1.11
import QtGraphicalEffects 1.0

Rectangle {
    width: window.width / parseInt(window.width < 500 ? 1 : window.width / 500) - channelsView.spacing
    height: contentItem.height + 10
	color: 'transparent'
	visible: modelData.enabled

	/* background (borders) */
	RectangularGlow {
		anchors.fill: contentItem
		glowRadius: 7
		spread: 0.05
		color: '#303070'
		cornerRadius: 0
	}

	/* content */
	Column {
		id: contentItem
		width: parent.width - 10
		anchors.centerIn: parent
		clip: true
		spacing: 0

		/* header */
		ChannelHeader {
			width: parent.width
		}

		ChannelItem {
			width: parent.width
		}

		Repeater {
			id: childItems
			property string mainType: modelData.type
			model: modelData.children
			delegate: ChannelItem {
				visible: childItems.mainType == 'group' || childItems.mainType == 'switch'
				width: parent.width
			}
		}
	}
}
