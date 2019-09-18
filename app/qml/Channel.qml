
import QtQuick 2.5
import QtQuick.Layouts 1.12

ColumnLayout {
	width: window.width / parseInt(window.width / 400) - channelsView.spacing
	clip: true
	Layout.margins: 5

	/* header */
	ChannelHeader {
		id: header
		Layout.fillWidth: true
	}

	Rectangle {
		id: content
		height: value.height
		color: '#ff0000'
		Layout.fillWidth: true

		Text {
			id: value
			anchors.top: parent.top
			anchors.right: parent.right
			text: modelData.value
			font: Qt.font({ pixelSize: 82, weight: 70 })
		}
	}

}
