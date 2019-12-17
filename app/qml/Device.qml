
import QtQuick 2.11
import QtQuick.Controls 2.4
import QtQuick.Layouts 1.11
import QtQuick.Controls.Material 2.12

Rectangle {
	implicitWidth: window.width
	implicitHeight: deviceView.implicitHeight

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
				}
			}

			MouseArea {
				width: childrenRect.width
				height: childrenRect.height
				Layout.fillWidth: true

				onClicked: modelData.selected = !modelData.selected;

				Row {
					Label {
						text: '\uf071'
						font: Qt.font({ pixelSize: 64, weight: 80, family: 'Font Awesome 5 Free' })
						visible: !modelData.connected
					}
					Column {
						id: label
						clip: true
						Label {
							text: modelData.name
							font: Qt.font({ pixelSize: 36, weight: 50 })
						}
						Label {
							text: modelData.address() + (modelData.port() < 0 ? '' : ':' + modelData.port())
							font: Qt.font({ pixelSize: 24, weight: 50 })
						}
					}
				}
			}

			/* actions */
			Row {
				height: parent.height
				ToolButton {
					width: height
					text: childrenVisible ? '\uf078' : '\uf054'
					font: Qt.font({ pixelSize: 64, weight: 80, family: 'Font Awesome 5 Free' })
					onClicked: childrenVisible = !childrenVisible
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

	/* selected overlay */
	Rectangle {
		anchors.fill: parent
		color: modelData.selected ? Material.primary : 'transparent'
		opacity: 0.3
	}
}