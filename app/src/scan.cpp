
#include "scan.h"

Scan::Scan(QObject *parent) : QObject(parent)
{
	m_scanner_bt = NULL;
	m_scanner_udp = NULL;
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

void Scan::start()
{
	/* create and start bluetooth scanner */
	if (!m_scanner_bt) {
		m_scanner_bt = new QBluetoothDeviceDiscoveryAgent(this);
		connect(m_scanner_bt, SIGNAL(deviceDiscovered(QBluetoothDeviceInfo)),
		        this, SLOT(btDiscovered(QBluetoothDeviceInfo)));
		connect(m_scanner_bt, SIGNAL(finished()), this, SLOT(btFinished()));
	}
	if (!m_scanner_bt->isActive()) {
		m_scanner_bt->start();
	}

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

void Scan::btDiscovered(const QBluetoothDeviceInfo &dev)
{
	Device *device = new Device(this, dev.address().toString());
	emit discovered(device);
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
			Device *device = new Device(this, address.toString(), data.mid(4).toInt());
			emit discovered(device);
		}
	}
}
