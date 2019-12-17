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

var unit_prefixes = [{
	'd': 1e18,
	'u': 'E'
}, {
	'd': 1e15,
	'u': 'P'
}, {
	'd': 1e12,
	'u': 'T'
}, {
	'd': 1e9,
	'u': 'G'
}, {
	'd': 1e6,
	'u': 'M'
}, {
	'd': 1e3,
	'u': 'k'
}, {
	'd': 1,
	'u': ''
}, {
	'd': 1e-3,
	'u': 'm'
}, {
	'd': 1e-6,
	'u': 'μ'
}, {
	'd': 1e-9,
	'u': 'p'
}, {
	'd': 1e-12,
	'u': 'n'
}];

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
		return {
			'd': data.divider,
			'u': ''
		}
	}
	/* if device type is not something we want to use the divider on */
	if (units[data.type] === undefined) {
		return {
			'd': 1,
			'u': ''
		};
	}
	if (['humidity', 'temperature', 'celsius', 'fahrenheit'].includes(data.type)) {
		return {
			'd': 1,
			'u': ''
		};
	}
	/* calculate divider */
	var v = Math.abs(data.value);
	for (var i in unit_prefixes) {
		var p = unit_prefixes[i];
		/* check that it is not smaller than resolution */
		if (data.resolution >= p.d) {
			return p;
		}
		/* check if this is correct divider */
		if (p.d <= v && (v < (p.d * 1000) || p.d >= 1e18)) {
			return p;
		}
	};
	return {
		'd': 1,
		'u': ''
	};
}

function unit(data) {
	/* if device type does not have a unit */
	if (units[data.type] === undefined) {
		return false;
	}
	/* resolve unit and prefix */
	var p = divider(data);
	return p.u + units[data.type];
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
	var p = divider(data);
	/* calculate value using unit prefix divider */
	var value = data.value / p.d;
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
			value = value.replace(/\.[0]+$/gm, '');
			value = '0' <= value[0] && value[0] <= '9' ? value : '0' + value;
		}
	} else {
		/* resolution is set, render by resolution
		 * first find decimals in resolution combined with divider
		 */
		var r = Number(data.resolution) / p.d;
		var decimals = Math.floor(r) === r ? 0 : (r.toString().split('.')[1].length || 0);
		/* round up to resolution */
		value = parseInt(value / r) * r;
		/* cut decimals just to be sure since float value math operations sometimes are not as precise as we want */
		value = value.toFixed(decimals);
	}

	if (data.type == 'wattage') {
		console.log(data.value, value, data.resolution, p.d);
	}


	return value;
}
