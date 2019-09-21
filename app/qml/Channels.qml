
import QtQuick 2.11
import QtQuick.Controls 2.4

Flow {
	id: channelsView
	spacing: 5	
	Repeater {
		id: channelsRepeater
		model: scan.channels
		delegate: Channel {}
	}
}
