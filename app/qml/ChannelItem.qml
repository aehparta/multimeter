
import QtQuick 2.11
import QtQuick.Layouts 1.11

Rectangle {
	height: mainValue.height + subValues.height
	color: '#9090e0'
	border.width: 1
	border.color: '#606090'
	visible: {
		switch (modelData.type) {
		case 'switch':
			return false;
		case 'group':
			return false;
		}
		return true;
	}

	ColumnLayout {
		anchors.fill: parent

		GridLayout {
			id: subValues
			columns: 2
			Layout.fillWidth: true
			Repeater {
				model: modelData.children
				delegate: ChannelValue {
					Layout.fillWidth: true
					main: false
				}
			}
		}

		ChannelValue {
			id: mainValue
			Layout.fillWidth: true
		}
	}
}
