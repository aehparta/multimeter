
import QtQuick 2.5

Rectangle {
	color: '#303090'
	height: icon.height + icon.anchors.topMargin * 2

	Image {
		id: icon
		anchors.top: parent.top
		anchors.left: parent.left
		anchors.topMargin: 10
		anchors.leftMargin: 10
		source: 'qrc:/icons/' + (modelData.type == 'switch' ? (modelData.value ? 'switch-on' : 'switch') : modelData.type) + '.png';
		height: label.height;
		fillMode: Image.PreserveAspectFit

		MouseArea {
			anchors.fill: parent
			onClicked: {
				if (modelData.type == 'switch') {
					modelData.xxx = true;
					// modelData.set('value', !modelData.value);
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
}
