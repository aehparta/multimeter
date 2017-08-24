function onReceiveData(data)
{
	if (data.indexOf('config:') === 0)
	{
		if (data.indexOf(':channels:') !== -1)
		{
			var i = data.indexOf(':channels:') + 10;
			deviceChannels = data.substr(i);
		}
		else if (data.indexOf(':name:') !== -1)
		{
			var i = data.indexOf(':name:') + 6;
			itemLabel.text = data.substr(i);
			item.height = itemIcon.height + itemValue.height;
		}
		else if (data.indexOf(':type:') !== -1)
		{
			var i = data.indexOf(':type:') + 6;
			deviceType = data.substr(i).trim();
		}
		else if (data.indexOf(':mode:') !== -1)
		{
			var i = data.indexOf(':mode:') + 6;
			deviceMode = data.substr(i).trim();
		}
		else if (data.indexOf(':zero:') !== -1)
		{
			var i = data.indexOf(':zero:') + 6;
			deviceZero = parseFloat(data.substr(i));
		}
		else if (data.indexOf(':divider:') !== -1)
		{
			var i = data.indexOf(':divider:') + 9;
			deviceDivider = parseFloat(data.substr(i));
		}
		else if (data.indexOf(':resolution:') !== -1)
		{
			var i = data.indexOf(':resolution:') + 12;
			deviceResolution = parseFloat(data.substr(i));
			deviceDecimals = 0;
			for (var j = deviceResolution; j < 1; j *= 10)
			{
				deviceDecimals++;
			}
		}
		console.log(data);
	}
	else if (data.charAt(0) >= 'A' && data.charAt(0) < ('A' + deviceChannels))
	{
		var units = {
			voltage: 'V',
			current: 'A',
			resistance: 'Ω',
			frequency: 'Hz',
		};
		var channel = data.charAt(0) - 'A';
		var n = parseInt(data.substr(1), deviceSampleRadix);
		if (isNaN(n))
		{
			console.log('invalid sample: ' + data);
			return;
		}
		deviceAverageSample += n * n;
		deviceAverageSampleCount++;
		if (deviceAverageSampleCount >= deviceAverageSampleMax)
		{
			var value = Number((Math.sqrt(deviceAverageSample / deviceAverageSampleCount) - deviceZero) / deviceDivider).toFixed(deviceDecimals);
			itemValue.text = value + ' ' + units[deviceMode];
			deviceAverageSample = 0;
			deviceAverageSampleCount = 0;
		}
	}
}

function valueChanged()
{
	if (modelData.chValue === undefined)
	{
		/* connection error */
		return '---';
	}
	
	var decimals = 0;
	for (var j = modelData.chResolution; j < 1; j *= 10)
	{
		decimals++;
	}

	if (modelData.chType == 'datetime' && modelData.chMode == 'source')
	{
		itemValue.visible = true;
		return Qt.formatTime(modelData.chValue, 'hh:mm:ss');
	}
	else if (modelData.chType == 'voltage')
	{
		itemValue.visible = true;
		return Number(modelData.chValue).toFixed(decimals) + ' V';
	}
	else if (modelData.chType == 'current')
	{
		itemValue.visible = true;
		return Number(modelData.chValue).toFixed(decimals) + ' A';
	}
	else if (modelData.chType == 'resistance')
	{
		itemValue.visible = true;
		return modelData.chValue + ' Ω';
	}
	else if (modelData.chType == 'frequency')
	{
		itemValue.visible = true;
		return modelData.chValue + ' Hz';
	}
	else if (modelData.chType == 'wattage')
	{
		itemValue.visible = true;
		return modelData.chValue + ' W';
	}

	return '';
}

function valueLeftTopChanged()
{
	if (modelData.chValue === undefined)
	{
		/* connection error */
		return '---';
	}

	return '';
}

function valueRightTopChanged()
{
	if (modelData.chValue === undefined)
	{
		/* connection error */
		return '---';
	}

	if (modelData.chType == 'datetime')
	{
		itemValueRightTop.visible = true;
		return Qt.formatDate(modelData.chValue, 'd.M.yyyy');
	}
	return '';
}

function valueSetRequest()
{
	console.log('value set request on ' + modelData.chName);
	console.log(modelData.chMethod + ' ' + modelData.chMode + ' ' + modelData.chType);
	console.log(modelData.chFirstChild);

	var channel = modelData.chThis;

	if (modelData.chMethod != 'push' || modelData.chMode != 'sink')
	{
		if (!modelData.chFirstChild)
		{
			return;
		}
		if (modelData.chFirstChild.chMethod != 'push' || modelData.chFirstChild.chMode != 'sink')
		{
			return;
		}

		channel = modelData.chFirstChild;
	}

	if (channel.chType == 'datetime')
	{
		channel.chValue = new Date();
	}
}


function valueSliderChanged()
{
	if (modelData.chValue === undefined)
	{
		/* connection error etc */
		return 0;
	}
	
	return Number(modelData.chValue);
}
