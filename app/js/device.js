
function valueChanged() {
	if (modelData.value === undefined) {
		/* connection error */
		return '---';
	}

	var decimals = 0;
	for (var j = modelData.resolution; j < 1; j *= 10) {
		decimals++;
	}

	if (modelData.type === 'datetime' && modelData.mode === 'source') {
		itemValue.visible = true;
		return Qt.formatTime(modelData.value, 'hh:mm:ss');
	} else if (modelData.type === 'voltage') {
		itemValue.visible = true;
		return Number(modelData.value).toFixed(decimals) + ' V';
	} else if (modelData.type === 'current') {
		itemValue.visible = true;
		return Number(modelData.value).toFixed(decimals) + ' A';
	} else if (modelData.type === 'resistance') {
		itemValue.visible = true;
		return Number(modelData.value).toFixed(decimals) + ' Ω';
	} else if (modelData.type === 'frequency') {
		itemValue.visible = true;
		return Number(modelData.value).toFixed(decimals) + ' Hz';
	} else if (modelData.type === 'wattage') {
		itemValue.visible = true;
		return Number(modelData.value).toFixed(decimals) + ' W';
	} else if (modelData.type === 'temperature') {
		itemValue.visible = true;
		return Number(modelData.value).toFixed(decimals) + ' °C';
	} else if (modelData.type === 'humidity') {
		itemValue.visible = true;
		return Number(modelData.value).toFixed(decimals) + ' %';
	}

	return '';
}

function valueLeftTopChanged() {
	if (modelData.value === undefined) {
		/* connection error */
		return '---';
	}

	return '';
}

function valueRightTopChanged() {
	if (modelData.value === undefined) {
		/* connection error */
		return '---';
	}

	if (modelData.type === 'datetime') {
		itemValueRightTop.visible = true;
		return Qt.formatDate(modelData.value, 'd.M.yyyy');
	}
	return '';
}

function valueSetRequest() {
	console.log('value set request on ' + modelData.name);
	console.log(modelData.method + ' ' + modelData.mode + ' ' + modelData.type);
	console.log(modelData.firstChild);

	var channel = modelData.this;

	if (modelData.method !== 'push' || modelData.mode !== 'sink') {
		if (!modelData.firstChild) {
			return;
		}
		if (modelData.firstChild.method !== 'push' || modelData.firstChild.mode !== 'sink') {
			return;
		}

		channel = modelData.firstChild;
	}

	if (channel.type === 'datetime') {
		channel.value = new Date();
	}
}

function valueSliderChanged() {
	if (modelData.value === undefined) {
		/* connection error etc */
		return 0;
	}

	return Number(modelData.value);
}