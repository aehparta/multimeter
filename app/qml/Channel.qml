
import QtQuick 2.11
import QtQuick.Layouts 1.11
import QtGraphicalEffects 1.0
import QtQuick.Controls.Material 2.12
import QtQuick.Controls 2.12

Item {
	visible: modelData.enabled
	width: parent.width / parseInt(parent.width < 768 ? 1 : parent.width / 768)
	height: content.height + 20

	/* background (borders) */
	RectangularGlow {
		anchors.fill: content
		glowRadius: 4
		spread: 0.05
		color: '#a0a0a0'
		cornerRadius: 0
	}

	/* content */
	Page {
		id: content
		clip: true
		anchors.left: parent.left
		anchors.right: parent.right
		anchors.top: parent.top
		anchors.margins: 5

		/* header */
		header: ChannelHeader {}

		/* content */
		ColumnLayout {
			id: contentItem
			anchors.fill: parent

			ChannelItem {
				Layout.fillWidth: true
			}

			Repeater {
				id: childItems
				property string mainType: modelData.type
				model: modelData.children
				delegate: ChannelItem {
					visible: childItems.mainType == 'group' || childItems.mainType == 'switch'
				}
			}
		}
	}
}