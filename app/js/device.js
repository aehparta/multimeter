.import "/js/channel.js" as Channel

function Device(dev) {
	this.dev = dev;
	this.channels = {};
	this.dev.onReceive.connect(this, this.recv);
	this.dev.onConnected.connect(this, function() {
		this.send('get config');
	});
	this.components = {
		channel: Qt.createComponent("/qml/Channel.qml")
	};
}

Device.prototype.start = function() {
	this.dev.start();
}

Device.prototype.send = function(data) {
	this.dev.send(data);
}

Device.prototype.recv = function(data) {
	/* get current channel and validate */
	var cc = data.charCodeAt(0);
	if (cc < 65 || cc > 90) {
		/* invalid channel, must be upper-case ascii letter */
		return;
	}
	var channel = data.charAt(0);

	/* check if this is a data line */
	if (data.charAt(1) === '=') {
		/* skip if this is not yet a valid channel */
		if (this.channels[channel] === undefined) {
			return;
		}
		if (!this.channels[channel].valid()) {
			return;
		}
		/* valid channel data received */
		this.channels[channel].recv(data.slice(2));
		return;
	}

	/* check if this is a configuration line */
	if (data.charAt(1) === ':') {
		var cfg = data.split(':', 3);
		if (cfg.length != 3) {
			return;
		}
		/* this was a config line */
		var value = cfg[2];
		/* parse possible options from key */
		var parts = cfg[1].split(',');
		var key = parts[0];
		var options = {};
		for (var i = 1; i < parts.length; i++) {
			var opt = parts[i].split('=', 2);
			options[opt[0]] = opt.length == 2 ? opt[1] : true;
		}
		/* create channel if it does not exist */
		if (this.channels[channel] === undefined) {
			this.channels[channel] = new Channel.Channel();
		}
		/* set value to channel config */
		this.channels[channel].set(key, value, options);
		/* if channel is valid (all settings received) make it visible */
		if (this.channels[channel].valid() && !this.channels[channel].id) {
			this.channels[channel].id = this.dev.id() + '_' + channel;
			channelsRepeater.list.push(this.channels[channel]);
		}
		channelsRepeater.listChanged();
		return;
	}

}