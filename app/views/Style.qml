pragma Singleton
import QtQuick 2.5

QtObject {
	property color windowBackgroundColor: '#000020'

	/* channel */
	property color channelBackgroundColor: '#000040'
	property color channelBorderColor: '#000030'
	property color channelNameColor: '#6080c0'

	/* value */
	property color valueMainColor: '#b0f0b0'
	property color valueSideColor: '#b0f0b0'

	/* slider specific */
	property color sliderColorStart: valueMainColor
	property color sliderColorEnd: sliderColorStart
	property color sliderHandleColor: channelNameColor
	property color sliderHandlePressedColor: sliderHandleColor
	property color sliderHandleBorderColor: '#000'
}