
#include "DeviceStream.h"

DeviceStream::DeviceStream(QObject *parent, QString address, quint16 port) :
	QObject(parent), reconnectTimer(this), waitConfigTimer(this)
{
	connect(&reconnectTimer, SIGNAL(timeout()), this, SLOT(reconnectTry()));
	reconnectTimer.setSingleShot(true);
	connect(&waitConfigTimer, SIGNAL(timeout()), this, SLOT(waitConfigCheck()));
	waitConfigTimer.setSingleShot(true);

	m_connected = false;
	m_address = address;
	m_port = port;
	m_socket = NULL;
	m_btSocket = NULL;

	m_stringDataWrite.clear();
	m_stringDataRead.clear();
	m_stringDataReadBuffer.clear();
}

int DeviceStream::start()
{
	if (m_port == 0) {
		m_btSocket = new QBluetoothSocket(QBluetoothServiceInfo::RfcommProtocol);
		connect(m_btSocket, SIGNAL(error(QBluetoothSocket::SocketError)), this, SLOT(connectionError(QBluetoothSocket::SocketError)));
		connect(m_btSocket, SIGNAL(connected()), this, SLOT(connectionReady()));
		connect(m_btSocket, SIGNAL(readyRead()), this, SLOT(dataReadReady()));
		m_btSocket->connectToService(QBluetoothAddress(m_address), QBluetoothUuid(QStringLiteral("00001101-0000-1000-8000-00805F9B34FB")));
	} else {
		m_socket = new QTcpSocket(this);
		connect(m_socket, SIGNAL(connected()), this, SLOT(connectionReady()));
		connect(m_socket, SIGNAL(readyRead()), this, SLOT(dataReadReady()));
		m_socket->connectToHost(m_address, m_port);
	}

	return 0;
}

void DeviceStream::stop()
{
	m_connected = false;

	if (!m_btSocket && !m_socket) {
		return;
	}

	foreach (DeviceChannel *channel, channels) {
		/* set to reconnecting state */
		channel->stop();
	}

	if (m_btSocket) {
		m_btSocket->disconnect();
		m_btSocket->close();
		m_btSocket = NULL;
	}
	if (m_socket) {
		m_socket->disconnect();
		m_socket->close();
		m_socket = NULL;
	}
}

void DeviceStream::send(const QString &data)
{
	QByteArray bdata;
	bdata.append(data);
	bdata.append("\n");
	if (m_btSocket) {
		m_btSocket->write(bdata);
	}
	if (m_socket) {
		m_socket->write(bdata);
	}
}

void DeviceStream::connectionReady()
{
	m_connected = true;
	send("get:config");
	waitConfigCount = 1;
	waitConfigTimer.start(5000);
	reconnectCount = 0;
}

void DeviceStream::connectionError(QBluetoothSocket::SocketError error)
{
	stop();
	reconnectTimer.start(5000);
}

void DeviceStream::dataReadReady()
{
	if (m_btSocket) {
		while (m_btSocket->canReadLine()) {
			QByteArray data = m_btSocket->readLine();
			QString sdata;
			sdata.append(data);
			sdata = sdata.trimmed();
			if (sdata.isEmpty()) {
				break;
			}
			emit received(sdata);
		}
	}
	if (m_socket) {
		while (m_socket->canReadLine()) {
			QByteArray data = m_socket->readLine();
			QString sdata;
			sdata.append(data);
			sdata = sdata.trimmed();
			if (sdata.isEmpty()) {
				break;
			}
			emit received(sdata);
		}
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
		qDebug() << "channel not found for data" << data;
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

	qDebug() << "socket" << m_btSocket << "try to reconnect, times left to try" << (reconnectCountMax - reconnectCount);
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
		qDebug() << "socket" << m_btSocket << "config not received, try to ask it again, times left to try" << (waitConfigCountMax - waitConfigCount);
		send("get:config");
		waitConfigCount++;
		waitConfigTimer.start(5000);
	}
}
