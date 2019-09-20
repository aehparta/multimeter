var units = {
	current: 'A',
	frequency: 'Hz',
	humidity: '%',
	resistance: 'Ω',
	voltage: 'V',
	wattage: 'W',
	/* temperature scales */
	temperature: '°C',
	celsius: '°C',
	kelvin: 'K',
	fahrenheit: '°F',
};

var thousands = {
	1: '',
	1e3: 'k',
	1e6: 'M',
	1e9: 'G',
	1e12: 'T'
};

function unit(type, value) {
	var unit = units[type] !== undefined ? units[type] : false;
	if (['resistance', 'frequency'].includes(type)) {
		var div = 1e3;
		for (; thousands[div]; div *= 1e3) {
			if (div > value) {
				break;
			}
		}
  		unit = thousands[div / 1e3] + unit;
	}
	return unit;
}

function human(type, value)
{
	return value;
}