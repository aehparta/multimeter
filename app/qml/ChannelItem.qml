
import QtQuick 2.11
import QtQuick.Layouts 1.11
import QtQuick.Controls.Material 2.12

ColumnLayout {
	visible: childItems.mainType != 'group' && childItems.mainType != 'switch'
	Layout.leftMargin: 20
	Layout.rightMargin: 20
	Layout.bottomMargin: 5

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
