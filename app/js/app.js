.import "/js/device.js" as Device

scan.onDiscovered.connect(function(dev) {
	var device = new Device.Device(dev);
	device.start();
	console.log('found possible device: ' + dev.address() + (dev.port() > 0 ? ':' + dev.port() : ''));
});

scan.start();