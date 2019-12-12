
import QtQuick 2.11
import QtQuick.Controls 2.4
import QtQuick.Controls.Material 2.12

Flow {
	id: channelsView
	Repeater {
		id: channelsRepeater
		model: scan.channels
		delegate: Channel {}
	}
}
