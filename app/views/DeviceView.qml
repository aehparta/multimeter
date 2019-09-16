import QtQml 2.2
import QtQuick 2.5
import QtQuick.Controls 1.4
import QtQuick.Dialogs 1.2
import QtQuick.Layouts 1.0

import "/js/device.js" as DeviceJs
import "."

Rectangle {
	id: item
	width: parent.width
	color: Style.channel.background
	clip: true
	height: itemIcon.height + valuesViewContainer.height + 20;

	Rectangle {
		id: header
		anchors.top: parent.top
		height: itemIcon.height + 20
		anchors.right: parent.right
		anchors.left: parent.left
		color: Style.channel.background

		Image {
			id: itemIcon
			anchors.top: parent.top
			anchors.left: parent.left
			anchors.topMargin: 10
			anchors.leftMargin: 10
			source: 'qrc:/icons/' + (groupType == 'switch' ? (groupValue ? 'switch-on' : 'switch') : groupType) + '.png';
			height: label.height;
			fillMode: Image.PreserveAspectFit

			MouseArea {
				anchors.fill: parent
				onClicked: {
					if (groupType == 'switch')
					{
						groupValue = !groupValue;
					}
				}
			}
		}

		Text {
			id: label
			anchors.top: parent.top
			anchors.left: itemIcon.right
			anchors.topMargin: 10
			anchors.leftMargin: 10
			text: groupName
			font: Style.channel.label.font
			color: Style.channel.label.color

			MouseArea {
				anchors.fill: parent
				onClicked: {
					if (!groupNameIsStatic)
					{
						dialogRenameValue.text = label.text
						dialogRename.open();
					}
				}
			}
		}
	}

	Column {
		id: valuesViewContainer
		anchors.right: parent.right
		anchors.left: parent.left
		anchors.top: header.bottom
		spacing: 2
		Repeater {
			id: valuesView
			model: groupChannels
			delegate: Item {
				width: valuesViewContainer.width
				height: {
					var h = 0;
					h += channelValue.visible ? channelValue.height : 0;
					h += channelSlider.visible ? channelSlider.height : 0;
					return h;
				}
				ChannelValue {
					id: channelValue
					anchors.right: parent.right
					anchors.left: parent.left
					visible: (modelData.type != 'slider')
				}
				ChannelSlider {
					id: channelSlider
					anchors.right: parent.right
					anchors.left: parent.left
					visible: (modelData.type == 'slider')
				}
			}
		}
	}

	Component {
		id: componentChannelValue
		ChannelValue {
			width: valuesViewContainer.width
		}
	}

	Component {
		id: componentChannelSlider
		ChannelSlider {
			width: valuesViewContainer.width
		}
	}

	Component {
		id: delegateValue
		Loader {
			sourceComponent: modelData.type != 'slider' ? componentChannelValue : componentChannelSlider
		}
	}

	Dialog {
		id: dialogRename
		visible: false
		title: 'Rename'
		standardButtons: StandardButton.Save | StandardButton.Cancel

		onAccepted: {
			label.text = dialogRenameValue.text;
		}

		TextInput {
			id: dialogRenameValue
			text: ''
			font.pointSize: 30
			focus: true
			height: 30
			color: '#6080c0'
			anchors.left: parent.left
			anchors.top: parent.top
			anchors.right: parent.right
		}
	}
}
