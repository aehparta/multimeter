
import QtQuick 2.11
import QtQuick.Layouts 1.11
import QtQuick.Controls.Material 2.12

ColumnLayout {
	visible: childItems.mainType != 'group' && childItems.mainType != 'switch'
	Layout.rightMargin: 20

	// Rectangle {
	// 	anchors.top: parent.top
	// 	width: parent.width
	// 	height: 1
	// 	color: Material.primary
	// }

	/* subvalues */
	GridLayout {
		columns: 2
		Repeater {
			model: modelData.children
			delegate: ChannelValue {
				main: false
			}
		}
	}

	/* main value */
	ChannelValue {
		Layout.fillWidth: true
	}
}
