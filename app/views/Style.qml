pragma Singleton
import QtQuick 2.5

QtObject {
	property color windowBackgroundColor: '#264763'

	/* channel */
	property var channel: QtObject {
		property color background: '#172737'
		property var label: QtObject {
			property color color: '#315a7d'
			property font font: Qt.font({ pixelSize: 48, weight: 50 })
		}
		property int spacing: 1
	}

	/* value */
	property var value: QtObject {
		property color color1: '#7da7ca'
		property color color2: '#7da7ca'
		property font font1: Qt.font({ pixelSize: 82, weight: 70 })
		property font font2: Qt.font({ pixelSize: 32, weight: 70 })
	}

	/* slider specific */
	property var slider: QtObject {
		property color color: '#070d12'
		property int height: 24
		property int radius: 12
		property var border: QtObject {
			property color color: '#000000'
			property int width: 2
			property int radius: 16
		}
	}
}