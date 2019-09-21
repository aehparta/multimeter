
import QtQuick 2.11

Rectangle {
	id: header
	height: icon.height + icon.anchors.topMargin * 2 + footer.height
	color: '#6060a0'

	Image {
		id: icon
		anchors.top: parent.top
		anchors.left: parent.left
		anchors.topMargin: 10
		anchors.leftMargin: 10
		source: 'qrc:/icons/' + (modelData.type == 'switch' ? (modelData.value == '0' ? 'switch' : 'switch-on') : modelData.type) + '.png';
		height: label.height;
		fillMode: Image.PreserveAspectFit

		MouseArea {
			anchors.fill: parent
			onClicked: {
				if (modelData.type == 'switch') {
					modelData.value = modelData.value == '0' ? '1' : '0';
				}
			}
		}
	}

	Text {
		id: label
		anchors.top: parent.top
		anchors.left: icon.right
		anchors.topMargin: 10
		anchors.leftMargin: 10
		text: modelData.name
		color: '#101040'
		font: Qt.font({ pixelSize: 48, weight: 70 })
	}

	/* footer (bottom border) */
	Rectangle {
		id: footer
		anchors.bottom: parent.bottom
		anchors.left: parent.left
		anchors.right: parent.right
		height: 2
		color: '#303070'
		// height: 7
		// gradient: Gradient {
		// 	GradientStop { position: 0.0; color: header.color }
		// 	GradientStop { position: 1.0; color: '#303070' }
		// }
	}
}
