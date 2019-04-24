
function valueChanged() {
	if (modelData.chValue === undefined) {
		/* connection error */
		return '---';
	}

	var decimals = 0;
	for (var j = modelData.chResolution; j < 1; j *= 10) {
		decimals++;
	}

	if (modelData.chType === 'datetime' && modelData.chMode === 'source') {
		itemValue.visible = true;
		return Qt.formatTime(modelData.chValue, 'hh:mm:ss');
	} else if (modelData.chType === 'voltage') {
		itemValue.visible = true;
		return Number(modelData.chValue).toFixed(decimals) + ' V';
	} else if (modelData.chType === 'current') {
		itemValue.visible = true;
		return Number(modelData.chValue).toFixed(decimals) + ' A';
	} else if (modelData.chType === 'resistance') {
		itemValue.visible = true;
		return Number(modelData.chValue).toFixed(decimals) + ' Ω';
	} else if (modelData.chType === 'frequency') {
		itemValue.visible = true;
		return Number(modelData.chValue).toFixed(decimals) + ' Hz';
	} else if (modelData.chType === 'wattage') {
		itemValue.visible = true;
		return Number(modelData.chValue).toFixed(decimals) + ' W';
	} else if (modelData.chType === 'temperature') {
		itemValue.visible = true;
		return Number(modelData.chValue).toFixed(decimals) + ' °C';
	} else if (modelData.chType === 'humidity') {
		itemValue.visible = true;
		return Number(modelData.chValue).toFixed(decimals) + ' %';
	}

	return '';
}

function valueLeftTopChanged() {
	if (modelData.chValue === undefined) {
		/* connection error */
		return '---';
	}

	return '';
}

function valueRightTopChanged() {
	if (modelData.chValue === undefined) {
		/* connection error */
		return '---';
	}

	if (modelData.chType === 'datetime') {
		itemValueRightTop.visible = true;
		return Qt.formatDate(modelData.chValue, 'd.M.yyyy');
	}
	return '';
}

function valueSetRequest() {
	console.log('value set request on ' + modelData.chName);
	console.log(modelData.chMethod + ' ' + modelData.chMode + ' ' + modelData.chType);
	console.log(modelData.chFirstChild);

	var channel = modelData.chThis;

	if (modelData.chMethod !== 'push' || modelData.chMode !== 'sink') {
		if (!modelData.chFirstChild) {
			return;
		}
		if (modelData.chFirstChild.chMethod !== 'push' || modelData.chFirstChild.chMode !== 'sink') {
			return;
		}

		channel = modelData.chFirstChild;
	}

	if (channel.chType === 'datetime') {
		channel.chValue = new Date();
	}
}

function valueSliderChanged() {
	if (modelData.chValue === undefined) {
		/* connection error etc */
		return 0;
	}

	return Number(modelData.chValue);
}