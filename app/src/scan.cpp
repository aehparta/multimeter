
#include "scan.h"

Scan::Scan(QObject *parent)
	: QObject(parent), m_scanner_udp_timer(this)
{
	m_scanner_bt = NULL;
	m_scanner_bt_enabled = true;
	m_scanner_udp = NULL;
	m_scanner_udp_enabled = true;
	m_autostart = true;
	m_selected = 0;

	m_scanner_udp_timer.setInterval(5000);
	m_scanner_udp_timer.setSingleShot(true);
	connect(&m_scanner_udp_timer, SIGNAL(timeout()), this, SLOT(udpFinished()));

	/* preload saved devices */
	QSettings settings;
	settings.beginGroup("devices");
	QStringList ids = settings.childGroups();
	settings.endGroup();
	foreach (QString id, ids) {
		addDevice(new Device(this, id));
	}
}

Scan::~Scan()
{
	foreach (QObject *o, m_devices) {
		Device *device = (Device *)o;
		device->save();
	}
}

void Scan::btEnable(bool value)
{
	if (m_scanner_bt_enabled != value) {
		m_scanner_bt_enabled = value;
		emit btEnabledChanged();
	}
}

void Scan::udpEnable(bool value)
{
	if (m_scanner_udp_enabled != value) {
		m_scanner_udp_enabled = value;
		emit udpEnabledChanged();
	}
}

void Scan::start()
{
	/* create and start bluetooth scanner */
	if (m_scanner_bt_enabled) {
		if (!m_scanner_bt) {
			m_scanner_bt = new QBluetoothDeviceDiscoveryAgent(this);
			connect(m_scanner_bt, SIGNAL(deviceDiscovered(QBluetoothDeviceInfo)),
			        this, SLOT(btDiscovered(QBluetoothDeviceInfo)));
			connect(m_scanner_bt, SIGNAL(finished()), this, SLOT(btFinished()));
		}
		if (!m_scanner_bt->isActive()) {
			m_scanner_bt->start();
		}
	}

	/* create and start udp scanner */
	if (m_scanner_udp_enabled) {
		if (!m_scanner_udp) {
			m_scanner_udp = new QUdpSocket(this);
			m_scanner_udp->bind(BCAST_PORT, QUdpSocket::ShareAddress);
			connect(m_scanner_udp, SIGNAL(readyRead()), this, SLOT(udpHasData()));
		}
		m_scanner_udp->writeDatagram("scan", 4, QHostAddress::Broadcast, BCAST_PORT);
		m_scanner_udp_timer.start();
	}

	emit started();
	emit activeChanged();
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

	foreach (QObject *o, m_devices) {
		Device *device = (Device *)o;
		foreach (QObject *oo, device->channels()) {
			Channel *channel = (Channel *)oo;
			if (channel->isValid() && !channel->parentChannel()) {
				list.append(channel);
			}
		}
	}

	return list;
}

QList<QObject *> Scan::devices()
{
	return m_devices;
}

void Scan::selectedRemove()
{
	foreach (QObject *o, m_devices) {
		Device *device = (Device *)o;
		if (device->getSelected()) {
			m_devices.removeOne(o);
			device->remove();
			delete device;
		}
	}
	emit devicesChanged();
}

void Scan::btDiscovered(const QBluetoothDeviceInfo &dev)
{
	//if (!dev.serviceUuids().contains(QBluetoothUuid::SerialPort)) {
	//	return;
	//}
	qDebug() << "discovered" << dev.serviceClasses() << dev.serviceUuids();
	addDevice(dev.address().toString(), -1, dev.name());
}

void Scan::btFinished()
{
	emit finished();
	emit activeChanged();
}

void Scan::udpFinished()
{
	m_scanner_udp->close();
	delete m_scanner_udp;
	m_scanner_udp = NULL;
	emit finished();
	emit activeChanged();
}

void Scan::udpHasData()
{
	QByteArray data;
	while (m_scanner_udp->hasPendingDatagrams()) {
		QHostAddress address;
		data.resize(int(m_scanner_udp->pendingDatagramSize()));
		m_scanner_udp->readDatagram(data.data(), data.size(), &address);
		if (data.startsWith("tcp:")) {
			bool ok;
			quint32 ipv4 = address.toIPv4Address(&ok);
			if (ok) {
				address.setAddress(ipv4);
			}
			addDevice(address.toString(), data.mid(4).toInt(), "");
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

void Scan::deviceSelectedChanged()
{
	Device *device = (Device *)sender();
	m_selected += device->getSelected() ? 1 : -1;
	emit selectedChanged();
}

bool Scan::addDevice(const QString &address, int port, QString name)
{
	return addDevice(new Device(this, port < 1 ? false : true, address, port, name));
}

bool Scan::addDevice(Device *device)
{
	/* check that this device does not already exist */
	foreach (QObject *o, m_devices) {
		if (((Device *)o)->id() == device->id()) {
			delete device;
			return false;
		}
	}
	/* connect to device channels changed signal */
	connect(device, SIGNAL(channelsChanged()), this, SLOT(deviceChannelsChanged()));
	/* select signal */
	connect(device, SIGNAL(selectedChanged()), this, SLOT(deviceSelectedChanged()));
	/* append to list of devices */
	m_devices.append(device);
	if (m_autostart) {
		/* autostart device */
		device->start();
	}
	/* emit discovery*/
	emit discovered(device);
	emit devicesChanged();

	return true;
}
