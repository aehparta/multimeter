
import QtQuick 2.11
import QtQuick.Controls 2.4
import QtQuick.Layouts 1.11

ToolBar {
	Row {
		padding: 5
		spacing: 5

		Image {
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
			text: modelData.name
			font: Qt.font({ pixelSize: 48, weight: 70 })
		}
	}
}