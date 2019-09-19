
import QtQuick 2.12
import QtQuick.Layouts 1.12
import QtGraphicalEffects 1.12

Rectangle {
	width: window.width / parseInt(window.width / 500) - channelsView.spacing
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

		Rectangle {
			height: value.height
			width: parent.width
			color: '#9090e0'

			Text {
				id: value
				anchors.bottom: parent.bottom
				anchors.right: unit.left
				text: modelData.value
				color: '#000000'
				font: Qt.font({ pixelSize: 96, weight: 90 })
			}

			Text {
				id: unit
				anchors.bottom: parent.bottom
				anchors.right: parent.right
				text: {
					var units = {
						current: 'A',
						frequency: 'Hz',
						humidity: '%',
						resistance: 'Ω',
						voltage: 'V',
						wattage: 'W',
						/* temperature scales */
						temperature: '°C',
						celsius: '°C',
						kelvin: 'K',
						fahrenheit: '°F',
					};
					return units[modelData.type] !== undefined ? ' ' + units[modelData.type] : '';
				}
				color: '#101030'
				font: Qt.font({ pixelSize: 96, weight: 90 })
			}
		}
	}
}
