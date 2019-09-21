
import QtQuick 2.11
import QtQuick.Controls 2.4
import QtQuick.Layouts 1.11

ColumnLayout {
	spacing: 5
	Repeater {
		model: scan.devices
		delegate: Device {}
	}
}
