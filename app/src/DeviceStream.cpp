
#include "DeviceStream.h"

DeviceStream::DeviceStream(QObject *parent) :
	QObject(parent), reconnectTimer(this), waitConfigTimer(this)
{
	connect(&reconnectTimer, SIGNAL(timeout()), this, SLOT(reconnectTry()));
	reconnectTimer.setSingleShot(true);
	connect(&waitConfigTimer, SIGNAL(timeout()), this, SLOT(waitConfigCheck()));
	waitConfigTimer.setSingleShot(true);

	m_connected = false;
	m_socket = 0;
	m_address = "invalid";

	m_stringDataWrite.clear();
	m_stringDataRead.clear();
	m_stringDataReadBuffer.clear();
}

QString DeviceStream::address() const
{
	return m_address;
}

void DeviceStream::setAddress(const QString &address)
{
	if (address != m_address) {
		m_address = address;
		emit addressChanged();
	}
}

QString DeviceStream::stringData()
{
	if (m_stringDataRead.isEmpty()) {
		return NULL;
	}
	return m_stringDataRead.takeFirst();
}

void DeviceStream::setStringData(const QString &data)
{
	if (!m_socket) {
		return;
	}
	QByteArray bdata;
	bdata.append(data);
	m_socket->write(bdata);
	m_socket->write("\n");
}

int DeviceStream::start()
{
	m_socket = new QBluetoothSocket(QBluetoothServiceInfo::RfcommProtocol);
	connect(m_socket, SIGNAL(error(QBluetoothSocket::SocketError)), this, SLOT(connectionError(QBluetoothSocket::SocketError)));
	connect(m_socket, SIGNAL(connected()), this, SLOT(connectionReady()));
	connect(m_socket, SIGNAL(readyRead()), this, SLOT(dataReadReady()));
	m_socket->connectToService(QBluetoothAddress(m_address), QBluetoothUuid(QStringLiteral("00001101-0000-1000-8000-00805F9B34FB")));

	return 0;
}

void DeviceStream::stop()
{
	if (!m_socket) {
		return;
	}
	qDebug() << "socket disconnect" << m_socket;

	foreach (DeviceChannel *channel, channels) {
		/* set to reconnecting state */
		channel->stop();
	}

	m_socket->disconnect();
	m_socket->close();
	m_socket = NULL;
	m_connected = false;
}

void DeviceStream::connectionReady()
{
	m_connected = true;
	setStringData("get:config");
	waitConfigCount = 1;
	waitConfigTimer.start(5000);
	reconnectCount = 0;
}

void DeviceStream::connectionError(QBluetoothSocket::SocketError error)
{
	qDebug() << "connection error" << error;
	stop();
	reconnectTimer.start(5000);
}

void DeviceStream::dataReadReady()
{
	while (m_socket->canReadLine()) {
		QByteArray data = m_socket->readLine();
		QString sdata;
		sdata.append(data);
		sdata = sdata.trimmed();
		if (sdata.isEmpty()) {
			break;
		}
		m_stringDataRead.append(sdata);
		emit stringDataChanged();
	}
}

DeviceChannel *DeviceStream::getChannel(int id, bool create)
{
	foreach (DeviceChannel *channel, channels) {
		if (channel->chGetIndex() == id) {
			return channel;
		}
	}

	if (!create) {
		return NULL;
	}

	DeviceChannel *channel = new DeviceChannel(id, this);
	channels.append(channel);

	return channel;
}

QList<QObject *> DeviceStream::getChannels()
{
	QList<QObject *> chs;

	foreach (DeviceChannel *channel, channels) {
		if (channel->chHasValidParentChannel()) {
			continue;
		}
		if (channel->isConnected()) {
			chs.append(channel);
		}
	}

	return chs;
}

QList<DeviceChannel *> DeviceStream::getChannelsByParent(int parent)
{
	QList<DeviceChannel *> chs;

	foreach (DeviceChannel *channel, channels) {
		if (!channel->isConnected()) {
			continue;
		}
		if (channel->chGetParentChannel() == parent) {
			chs.append(channel);
		}
	}

	return chs;
}

bool DeviceStream::receiveData(QString data)
{
	if (data.length() < 2) {
		return false;
	}

	int channel_number = (int)data.at(0).toLatin1() - (int)'A';
	DeviceChannel *channel = NULL;
	foreach (DeviceChannel *c, channels) {
		if (!c->isConnected()) {
			continue;
		}
		if (c->chGetIndex() == channel_number) {
			channel = c;
			break;
		}
	}
	if (!channel) {
		return false;
	}

	data = data.mid(1);
	return channel->receiveData(data);
}

void DeviceStream::reconnectTry()
{
	if (reconnectCount >= reconnectCountMax) {
		qDebug() << "max reconnects reached (" << reconnectCountMax << ")";
		return;
	}

	qDebug() << "socket" << m_socket << "try to reconnect, times left to try" << (reconnectCountMax - reconnectCount);
	stop();
	start();
	reconnectCount++;
}

void DeviceStream::waitConfigCheck()
{
	if (channels.count() > 0 || waitConfigCount >= waitConfigCountMax) {
		if (waitConfigCount >= waitConfigCountMax) {
			qDebug() << "max config requests send (" << waitConfigCountMax << ") and connection not up";
		}
		waitConfigTimer.stop();
	} else {
		qDebug() << "socket" << m_socket << "config not received, try to ask it again, times left to try" << (waitConfigCountMax - waitConfigCount);
		setStringData("get:config");
		waitConfigCount++;
		waitConfigTimer.start(5000);
	}
}
