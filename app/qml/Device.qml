
import QtQuick 2.11
import QtQuick.Controls 2.4
import QtQuick.Layouts 1.11

Rectangle {
	implicitWidth: window.width
	implicitHeight: deviceView.implicitHeight
	color: '#6060a0'

	property bool childrenVisible: false

	ColumnLayout {
		id: deviceView
		width: parent.width

		RowLayout {
			id: deviceHeader

			Switch {
				checked: modelData.enabled
				onClicked: modelData.enabled = checked
				background: Rectangle {
					implicitWidth: deviceHeader.height
					implicitHeight: deviceHeader.height
					color: '#505090'
				}
			}

			ColumnLayout {
				clip: true
				Label {
					Layout.fillWidth: true
					text: modelData.name
					color: '#000000'
					font: Qt.font({ pixelSize: 36, weight: 50 })
				}
				Label {
					Layout.fillWidth: true
					text: modelData.address() + (modelData.port() < 0 ? '' : ':' + modelData.port())
					color: '#303060'
					font: Qt.font({ pixelSize: 24, weight: 50 })
				}
			}

			/* extra options */
			Rectangle {
				implicitWidth: deviceHeader.height
				implicitHeight: deviceHeader.height
				color: '#505090'
				Label {
					text: childrenVisible ? '\uf078' : '\uf054'
					font: Qt.font({ pixelSize: 64, weight: 80, family: 'Font Awesome 5 Free' })
					Layout.alignment: Qt.AlignRight
					MouseArea {
						anchors.fill: parent
						onClicked: childrenVisible = !childrenVisible
					}
				}
			}
		}

		Repeater {
			model: modelData.channels
			delegate: RowLayout {
				width: parent.width
				visible: childrenVisible
				Layout.leftMargin: 40
				CheckBox {
					checked: modelData.enabled
					onClicked: modelData.enabled = checked
				}
				ColumnLayout {
					Label {
						text: modelData.name
						color: '#000000'
						font: Qt.font({ pixelSize: 36, weight: 50 })
					}
				}
			}
		}
	}
}