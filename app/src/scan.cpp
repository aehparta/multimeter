
#include "scan.h"

Scan::Scan(QObject *parent) : QObject(parent)
{
	m_scanner_bt = NULL;
	m_scanner_udp = NULL;
	m_autostart = false;
}

void Scan::start()
{
	/* create and start bluetooth scanner */
	// if (!m_scanner_bt) {
	// 	m_scanner_bt = new QBluetoothDeviceDiscoveryAgent(this);
	// 	connect(m_scanner_bt, SIGNAL(deviceDiscovered(QBluetoothDeviceInfo)),
	// 	        this, SLOT(btDiscovered(QBluetoothDeviceInfo)));
	// 	connect(m_scanner_bt, SIGNAL(finished()), this, SLOT(btFinished()));
	// }
	// if (!m_scanner_bt->isActive()) {
	// 	m_scanner_bt->start();
	// }

	/* create and start udp scanner */
	if (!m_scanner_udp) {
		m_scanner_udp = new QUdpSocket(this);
		m_scanner_udp->bind(BCAST_PORT, QUdpSocket::ShareAddress);
		connect(m_scanner_udp, SIGNAL(readyRead()), this, SLOT(udpHasData()));
	}
	m_scanner_udp->writeDatagram("scan", 4, QHostAddress::Broadcast, BCAST_PORT);

	emit started();
	emit changedActive();
}

bool Scan::isActive()
{
	bool bt = false, udp = false;
	if (m_scanner_bt) {
		bt = m_scanner_bt->isActive();
	}
	if (m_scanner_udp) {
		udp = true;
	}
	return bt || udp;
}

QList<QObject *> Scan::channels()
{
	QList<QObject *> list;

	foreach (Device *device, m_devices) {
		foreach (Channel *channel, device->channels()) {
			if (channel->isValid()) {
				list.append(channel);
			}
		}
	}

	return list;
}

void Scan::btDiscovered(const QBluetoothDeviceInfo &dev)
{
	addDevice(dev.address().toString());
}

void Scan::btFinished()
{
	emit finished();
	emit changedActive();
}

void Scan::udpHasData()
{
	QByteArray data;
	while (m_scanner_udp->hasPendingDatagrams()) {
		QHostAddress address;
		data.resize(int(m_scanner_udp->pendingDatagramSize()));
		m_scanner_udp->readDatagram(data.data(), data.size(), &address);
		if (data.startsWith("tcp:")) {
			addDevice(address.toString(), data.mid(4).toInt());
		}
	}
}

void Scan::deviceChannelsChanged()
{
	Device *device = (Device *)sender();
	int n = 1;

	// for (QMap<char, QObject *>::iterator i = channels.begin(); i != channels.end(); i++) {
	// 	QString key = device->id() + "_" + i.key();
	// 	if (m_channels.contains(key)) {
	// 		/* channels already exists, skip */
	// 		continue;
	// 	}
	// 	m_channels.insert(key, i.value());
	// 	n++;
	// 	qDebug() << "CHANGED" << i.key() << i.value()->objectName() << i.value()->property("name");
	// }

	if (n > 0) {
		emit channelsChanged();
	}
}

void Scan::addDevice(const QString &address, int port)
{
	Device *device = new Device(this, address, port);
	/* connect to device channels changed signal */
	connect(device, SIGNAL(channelsChanged()), this, SLOT(deviceChannelsChanged()));
	/* append to list of devices */
	m_devices.append(device);
	if (m_autostart) {
		/* autostart device */
		device->start();
	}
	/* emit discovered */
	emit discovered(device);
}
