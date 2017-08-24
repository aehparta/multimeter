
#include "DeviceStream.h"

#ifdef USE_BLUEZ
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/socket.h>
#include <bluetooth/bluetooth.h>
#include <bluetooth/hci.h>
#include <bluetooth/hci_lib.h>
#include <bluetooth/rfcomm.h>
#endif

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
	if (address != m_address)
	{
		m_address = address;
		emit addressChanged();
	}
}

QString DeviceStream::stringData()
{
	if (m_stringDataRead.isEmpty())
	{
		return NULL;
	}
	return m_stringDataRead.takeFirst();
}

void DeviceStream::setStringData(const QString &data)
{
#ifdef USE_BLUEZ
	if (m_socket < 1)
	{
		return;
	}
	m_stringDataWrite.append(data);
	m_notifierWrite->setEnabled(true);
#else
	if (!m_socket)
	{
		return;
	}
	QByteArray bdata;
	bdata.append(data);
	m_socket->write(bdata);
	m_socket->write("\n");
#endif
}

int DeviceStream::start()
{
#ifdef USE_BLUEZ
	struct sockaddr_rc addr;
	int s, status;
	QByteArray device_address;

	/* allocate a socket */
	s = socket(AF_BLUETOOTH, SOCK_STREAM | SOCK_NONBLOCK, BTPROTO_RFCOMM);

	/* set the connection parameters */
	memset(&addr, 0, sizeof(addr));
	addr.rc_family = AF_BLUETOOTH;
	addr.rc_channel = (uint8_t)1;
	device_address.append(m_address);
	str2ba(device_address.data(), &addr.rc_bdaddr);

	/* connect to device */
	status = ::connect(s, (struct sockaddr *)&addr, sizeof(addr));

	/* check status */
	if (status != 0 && status != EINPROGRESS && status != -1)
	{
		qDebug() << "socket error: " << strerror(status);
		return -1;
	}

	m_socket = s;
	m_notifierWrite = new QSocketNotifier(m_socket, QSocketNotifier::Write, this);
	m_notifierRead = new QSocketNotifier(m_socket, QSocketNotifier::Read, this);
	m_notifierException = new QSocketNotifier(m_socket, QSocketNotifier::Exception, this);
	connect(m_notifierWrite, SIGNAL(activated(int)), this, SLOT(dataWriteReady()));
	connect(m_notifierRead, SIGNAL(activated(int)), this, SLOT(dataReadReady()));
	connect(m_notifierException, SIGNAL(activated(int)), this, SLOT(connectionError()));
	m_notifierWrite->setEnabled(true);
	m_notifierRead->setEnabled(true);
	m_notifierException->setEnabled(true);
	qDebug() << "socket created" << m_socket;
#else
	m_socket = new QBluetoothSocket(QBluetoothServiceInfo::RfcommProtocol);
	connect(m_socket, SIGNAL(error(QBluetoothSocket::SocketError)), this, SLOT(connectionError(QBluetoothSocket::SocketError)));
	connect(m_socket, SIGNAL(connected()), this, SLOT(connectionReady()));
	connect(m_socket, SIGNAL(readyRead()), this, SLOT(dataReadReady()));
	m_socket->connectToService(QBluetoothAddress(m_address), QBluetoothUuid(QStringLiteral("00001101-0000-1000-8000-00805F9B34FB")));
#endif

	return 0;
}

void DeviceStream::stop()
{
#ifdef USE_BLUEZ
	if (m_socket < 1)
	{
		return;
	}
	qDebug() << "socket disconnect" << m_socket;

	foreach (DeviceChannel *channel, channels)
	{
		/* set to reconnecting state */
		channel->stop();
	}

	m_notifierWrite->disconnect();
	m_notifierRead->disconnect();
	m_notifierException->disconnect();
	delete m_notifierWrite;
	delete m_notifierRead;
	delete m_notifierException;
	::close(m_socket);
	m_notifierWrite = NULL;
	m_notifierRead = NULL;
	m_notifierException = NULL;
	m_socket = -1;
	m_connected = false;
#else
	if (!m_socket)
	{
		return;
	}
	qDebug() << "socket disconnect" << m_socket;

	foreach (DeviceChannel *channel, channels)
	{
		/* set to reconnecting state */
		channel->stop();
	}

	m_socket->disconnect();
	m_socket->close();
	delete m_socket;
	m_socket = NULL;
	m_connected = false;
#endif
}

#ifndef USE_BLUEZ
void DeviceStream::connectionReady()
{
	m_connected = true;
	setStringData("get:config");
	waitConfigCount = 1;
	waitConfigTimer.start(5000);
}
#endif

#ifndef USE_BLUEZ
void DeviceStream::connectionError(QBluetoothSocket::SocketError error)
#else
void DeviceStream::connectionError()
#endif
{
	qDebug() << "connection error";
	stop();
	reconnectCount = 1;
	reconnectTimer.start(5000);
}

void DeviceStream::dataWriteReady()
{
#ifdef USE_BLUEZ
	if (!m_connected)
	{
		qDebug() << "connection up";
		m_connected = true;
		setStringData("get:config");
		waitConfigCount = 1;
		waitConfigTimer.start(5000);
	}

	if (!m_stringDataWrite.isEmpty())
	{
		QByteArray data;
		data.append(m_stringDataWrite.takeFirst());
		if (::write(m_socket, data.data(), data.size()) < 0)
		{
			qDebug() << "write failed";
			/* @todo handle error */
		}
		if (::write(m_socket, "\n", 1) != 1)
		{
			/* same here */
		}
	}

	if (m_stringDataWrite.isEmpty())
	{
		m_notifierWrite->setEnabled(false);
	}
#endif
}

void DeviceStream::dataReadReady()
{
#ifdef USE_BLUEZ
	char c;
	while (1)
	{
		int r = ::read(m_socket, &c, 1);
		if (r == -1)
		{
			/* no data to read */
			return;
		}
		else if (r != 1)
		{
			/* error, disconnected etc */
			qDebug() << "error" << strerror(r);
			return;
		}
		if (c == '\n')
		{
			if (m_stringDataReadBuffer.isEmpty())
			{
				continue;
			}
			// qDebug() << m_stringDataReadBuffer;
			m_stringDataRead.append(m_stringDataReadBuffer);
			m_stringDataReadBuffer.clear();
			emit stringDataChanged();
			return;
		}
		m_stringDataReadBuffer.append(c);
	}
#else
	while (m_socket->canReadLine())
	{
		QByteArray data = m_socket->readLine();
		QString sdata;
		sdata.append(data);
		sdata = sdata.trimmed();
		if (sdata.isEmpty())
		{
			break;
		}
		m_stringDataRead.append(sdata);
		emit stringDataChanged();
	}
#endif
}

DeviceChannel *DeviceStream::getChannel(int id, bool create)
{
	foreach (DeviceChannel *channel, channels)
	{
		if (channel->chGetIndex() == id)
		{
			return channel;
		}
	}

	if (!create)
	{
		return NULL;
	}

	DeviceChannel *channel = new DeviceChannel(id, this);
	channels.append(channel);

	return channel;
}

QList<QObject *> DeviceStream::getChannels()
{
	QList<QObject *> chs;

	foreach (DeviceChannel *channel, channels)
	{
		if (channel->chHasValidParentChannel())
		{
			continue;
		}
		if (channel->isConnected())
		{
			chs.append(channel);
		}
	}

	return chs;
}

QList<DeviceChannel *> DeviceStream::getChannelsByParent(int parent)
{
	QList<DeviceChannel *> chs;

	foreach (DeviceChannel *channel, channels)
	{
		if (!channel->isConnected())
		{
			continue;
		}
		if (channel->chGetParentChannel() == parent)
		{
			chs.append(channel);
		}
	}

	return chs;
}

bool DeviceStream::receiveData(QString data)
{
	if (data.length() < 2)
	{
		return false;
	}

	int channel_number = (int)data.at(0).toLatin1() - (int)'A';
	DeviceChannel *channel = NULL;
	foreach (DeviceChannel *c, channels)
	{
		if (!c->isConnected())
		{
			continue;
		}
		if (c->chGetIndex() == channel_number)
		{
			channel = c;
			break;
		}
	}
	if (!channel)
	{
		return false;
	}

	data = data.mid(1);
	return channel->receiveData(data);
}

void DeviceStream::reconnectTry()
{
	if (reconnectCount >= reconnectCountMax)
	{
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
	if (channels.count() > 0 || waitConfigCount >= waitConfigCountMax)
	{
		if (waitConfigCount >= waitConfigCountMax)
		{
			qDebug() << "max config requests send (" << waitConfigCountMax << ") and connection not up";
		}
		waitConfigTimer.stop();
	}
	else
	{
		qDebug() << "socket" << m_socket << "config not received, try to ask it again, times left to try" << (waitConfigCountMax - waitConfigCount);
		setStringData("get:config");
		waitConfigCount++;
		waitConfigTimer.start(5000);
	}
}
