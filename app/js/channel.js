function Channel() {
	/* default settings */
	this.name = null;
	this.type = null;
	this.mode = null;
	this.method = null;
	this.parent = null;
	this.value = 123;
	/* default number base is 16 (hex) */
	this.base = 16;
	/* default number multiplier of course is one */
	this.multiplier = 1;
	/* default number resolution */
	this.resolution = 1;
	/* options for settings */
	this.options = {
		name: {
			static: true
		}
	};
}

Channel.prototype.set = function(key, value, options) {
	console.log(key, value);
	if (options !== undefined) {
		this.options[key] = options;
	}
	if (this[key] !== value) {
		this[key] = value;
		channelsRepeater.listChanged();
	}
}

Channel.prototype.valid = function() {
	return this.name !== null && this.type !== null && this.mode !== null && this.method !== null;
}

Channel.prototype.recv = function(data) {
	/* parse value and use multiplier */
	var value = parseInt(data, this.base) * this.multiplier;
	/* round up to resolution */
	value = Number(value / this.resolution).toFixed(0) * this.resolution;
	/* cut decimals of the end manually, float value round ups can sometimes add small decimals */
	if ((this.resolution + '').charAt(1) === '.') {
		value = value.toFixed(this.resolution.length - 2);
	}
	this.set('value', value);
	// console.log(this.name, this.value);
}

Channel.prototype.print = function() {
	console.log(this.name, this.options.name.static, this.type, this.mode, this.method);
}