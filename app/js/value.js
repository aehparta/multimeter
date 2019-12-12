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

var unit_prefixes = {
	1e-12: 'n',
	1e-9: 'p',
	1e-6: 'μ',
	1e-3: 'm',
	1: '',
	1e3: 'k',
	1e6: 'M',
	1e9: 'G',
	1e12: 'T',
	1e15: 'P',
	1e18: 'E'
};

/* types that use their value plain and are not example number */
var plain_types = [
	'group',
	'date',
	'time',
	'datetime',
	'plain'
];

function divider(data) {
	/* if constant divider is specified by device configuration */
	if (data.divider != 0 && unit_prefixes[data.divider] !== undefined) {
		return data.divider;
	}
	/* if device type is not something we want to use the divider on */
	if (units[data.type] === undefined) {
		return 1;
	}
	if (['humidity', 'temperature', 'celsius', 'fahrenheit'].includes(data.type)) {
		return 1;
	}
	/* calculate divider */
	var v = Math.abs(data.value);
	for (var m in unit_prefixes) {
		if (m <= v && (v < (m * 1000) || m >= 1e18)) {
			return m;
		}
	};
	return 1;
}

function unit(data) {
	/* if device type does not have a unit */
	if (units[data.type] === undefined) {
		return false;
	}
	/* resolve unit and prefix */
	var m = divider(data);
	return unit_prefixes[m] + units[data.type];
}

function human(data) {
	/* skip plain types */
	if (plain_types.includes(data.type) || data.plain) {
		return data.value;
	}
	/* check that value is a valid number */
	if (!isFinite(data.value) || data.value.length < 1) {
		return NaN;
	}
	/* get unit prefix divider */
	var m = divider(data);
	/* calculate value using unit prefix divider */
	var value = data.value / m;

	/* if resolution is not set, convert to max of 6 characters */
	if (data.resolution == 0) {
		/* 4 decimals max with '0.' in front */
		var i = 4;
		do {
			try {
				value = Number(value).toFixed(i);
			} catch (e) {
				/* large numbers throw a RangeError exception with toFixed */
			}
			i--;
		} while (value.length > 6 && i >= 0);
		/* trim trailing zeroes off if value has decimals */
		if (value.includes('.')) {
			value = value.replace(/[0]+$/gm, '');
			value = '0' <= value[0] && value[0] <= '9' ? value : '0' + value;
		}
	} else {
		/* resolution is set, render by resolution
		 * first find decimals in resolution combined with divider
		 */
		var r = Number(data.resolution) / m;
		var decimals = Math.floor(r) === r ? 0 : (r.toString().split('.')[1].length || 0);
		/* round up to resolution */
		value = parseInt(value / r) * r;
		/* cut decimals just to be sure since float value math operations sometimes are not as precise as we want */
		value = value.toFixed(decimals);
	}

	return value;
}

/* round up to resolution */
// number = double(int(number / m_resolution)) * m_resolution;
/* cut decimals of the end manually, float value round ups can sometimes add small decimals */
// int decimals = 0;
// QString r = QString("%1").arg((m_resolution - double((int(m_resolution)))));
// if (r.mid(1, 1) == ".") {
// 	decimals = r.size() - 2;
// }