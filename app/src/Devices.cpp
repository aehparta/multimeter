

#include "Devices.h"


Devices::Devices(QObject *parent) :
	QObject(parent), timer(this)
{
	QSettings settings;
	int size = settings.beginReadArray("devices");
	for (int i = 0; i < size; i++) {
		settings.setArrayIndex(i);
		addDevice(settings.value("address").toString(), settings.value("port").toInt());
	}
	settings.endArray();

	scannerBt = NULL;
	scannerUdp = NULL;
}

Devices::~Devices()
{
	QSettings settings;
	settings.beginWriteArray("devices");
	for (int i = 0; i < streams.size(); i++) {
		settings.setArrayIndex(i);
		settings.setValue("address", streams[i]->getAddress());
		settings.setValue("port", streams[i]->getPort());
	}
	settings.endArray();
}

void Devices::scan()
{
	/* create and start bluetooth scanner */
	if (!scannerBt) {
		scannerBt = new QBluetoothDeviceDiscoveryAgent(this);
		connect(scannerBt, SIGNAL(deviceDiscovered(QBluetoothDeviceInfo)),
		        this, SLOT(scannerBtFoundDevice(QBluetoothDeviceInfo)));
		connect(scannerBt, SIGNAL(finished()), this, SLOT(scannerBtFinished()));
	}
	if (!scannerBt->isActive()) {
		scannerBt->start();
	}

	/* create and start udp scanner */
	if (!scannerUdp) {
		scannerUdp = new QUdpSocket(this);
		scannerUdp->bind(BCAST_PORT, QUdpSocket::ShareAddress);
		connect(scannerUdp, SIGNAL(readyRead()), this, SLOT(scannerUdpHasData()));
	}
	scannerUdp->writeDatagram("scan", 4, QHostAddress::Broadcast, BCAST_PORT);

	emit scanStarted();
	emit scanStateChanged();
}

bool Devices::isScanning() const
{
	bool bt = false, udp = false;
	if (scannerBt) {
		bt = scannerBt->isActive();
	}
	if (scannerUdp) {
		udp = true;
	}
	return bt || udp;
}

void Devices::scannerBtFoundDevice(const QBluetoothDeviceInfo &device)
{
	addDevice(device.address().toString());
	qDebug() << "found bluetooth device at address" << device.address();
}

void Devices::scannerBtFinished()
{
	emit scanFinished();
	emit scanStateChanged();
}

void Devices::scannerUdpHasData()
{
	QByteArray data;
	while (scannerUdp->hasPendingDatagrams()) {
		QHostAddress address;
		data.resize(int(scannerUdp->pendingDatagramSize()));
		scannerUdp->readDatagram(data.data(), data.size(), &address);
		if (data.startsWith("tcp:")) {
			addDevice(address.toString(), data.mid(4).toInt());
			qDebug() << "found tcp device at " << address.toString() << data.mid(4).toInt();
		}
	}
}

void Devices::addDevice(QString address, int port)
{
	/* skip existing devices */
	foreach (DeviceStream *stream, streams) {
		if (stream->getAddress() == address && stream->getPort() == port) {
			return;
		}
	}
	/* new device, add to streams */
	qDebug() << "new device" << address << port;
	DeviceStream *stream = new DeviceStream(this, address, port);
	connect(stream, SIGNAL(received(QString)), this, SLOT(deviceStreamReceiveData(QString)));
	stream->start();
	streams.append(stream);
}

void Devices::deviceStreamReceiveData(QString data)
{
	DeviceStream *stream = (DeviceStream *)sender();
	if (data.startsWith("config:")) {
		qDebug() << "received config data:" << data;
		parseConfigLine(stream, data);
	} else {
		stream->receiveData(data);
	}
}

void Devices::parseConfigLine(DeviceStream *stream, const QString &data)
{
	QStringList parts = data.split(":");

	if (parts.count() < 5) {
		return;
	}

	if (parts[1] == "channel") {
		if (parseConfigLineChannel(stream, parts)) {
			checkNewChannels();
		}
	}
}

bool Devices::parseConfigLineChannel(DeviceStream *stream, const QStringList &data)
{
	int channel_number = (int)data[2].at(0).toLatin1() - (int)'A';

	if (channel_number < 0 || channel_number > 31) {
		/* invalid channel number */
		return false;
	}

	DeviceChannel *channel = stream->getChannel(channel_number, true);
	return channel->receiveConfigValue(data);
}

void Devices::checkNewChannels()
{
	QList<QObject *> gs;

	foreach (DeviceStream *stream, streams) {
		QList<QObject *> scs = stream->getChannels();
		foreach (QObject *object, scs) {
			DeviceGroup *group = new DeviceGroup(object);
			gs.append(group);
		}
	}

	if (gs != groups) {
		groups = gs;
		rootContext->setContextProperty("channelsModel", QVariant::fromValue(groups));
	}
}

void Devices::setRootContext(QQmlContext *ctxt)
{
	rootContext = ctxt;
}
