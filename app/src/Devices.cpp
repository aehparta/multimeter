

#include "Devices.h"


Devices::Devices(QObject *parent) :
	QObject(parent), timer(this)
{
	scannerBt = NULL;
	scannerUdp = NULL;
}

void Devices::scan()
{
	/* create and start bluetooth scanner */
	if (1) {
		if (!scannerBt) {
			scannerBt = new QBluetoothDeviceDiscoveryAgent(this);
			connect(scannerBt, SIGNAL(deviceDiscovered(QBluetoothDeviceInfo)),
			        this, SLOT(scannerBtFoundDevice(QBluetoothDeviceInfo)));
			connect(scannerBt, SIGNAL(finished()), this, SLOT(scannerBtFinished()));
		}
		if (!scannerBt->isActive()) {
			scannerBt->start();
		}
	} else {
		qDebug() << "no bluetooth adapters found, skip bluetooth";
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
	DeviceStream *stream = new DeviceStream(this, device.address().toString());
	connect(stream, SIGNAL(received(QString)), this, SLOT(deviceStreamReceiveData(QString)));
	stream->start();
	streams.append(stream);

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
			quint16 port = data.mid(4).toInt();

			DeviceStream *stream = new DeviceStream(this, address.toString(), port);
			connect(stream, SIGNAL(received(QString)), this, SLOT(deviceStreamReceiveData(QString)));
			stream->start();
			streams.append(stream);

			qDebug() << "found tcp device at " << address.toString() << port;
		}
	}
}

void Devices::deviceStreamReceiveData(QString data)
{
	DeviceStream *stream = (DeviceStream *)sender();
	if (data.startsWith("config:")) {
		qDebug() << data;
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
