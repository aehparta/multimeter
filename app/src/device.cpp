
#include "device.h"

Device::Device(QObject *parent, bool enabled, QString address, int port) : QObject(parent)
{
	m_address = address;
	m_port = port;

	m_name = id();
	m_enabled = enabled;

	m_socket_tcp = NULL;
	m_socket_bt = NULL;

	qDebug() << "new device" << id() << m_channels.size();
}

Device::Device(QObject *parent, QString settings_group) : QObject(parent)
{
	QSettings settings;
	settings.beginGroup("devices");
	settings.beginGroup(settings_group);

	/* load device information */
	m_name = settings.value("name").toString();
	m_address = settings.value("address").toString();
	m_port = settings.value("port").toInt();
	m_enabled = settings.value("enabled").toBool();

	settings.endGroup();
	settings.endGroup();

	m_socket_tcp = NULL;
	m_socket_bt = NULL;

	qDebug() << "new device from config" << id() << m_channels.size();
}

Device::~Device()
{
	if (m_address.isEmpty()) {
		qDebug() << "invalid device?" << m_address << m_port;
		return;
	}

	QSettings settings;
	settings.beginGroup("devices");
	settings.beginGroup(id());

	/* save device information */
	settings.setValue("name", m_name);
	settings.setValue("address", m_address);
	settings.setValue("port", m_port);
	settings.setValue("enabled", m_enabled);

	settings.endGroup();
	settings.endGroup();
}

QString Device::name()
{
	return m_name;
}

QString Device::id()
{
	QString id = m_address + "_" + m_port;
	return QString(QCryptographicHash::hash(id.toStdString().c_str(), QCryptographicHash::Sha256).toHex());
}

QString Device::address()
{
	return m_address;
}

int Device::port()
{
	return m_port;
}

void Device::start()
{
	if (m_socket_bt || m_socket_tcp) {
		return;
	}
	if (m_port < 0) {
		m_socket_bt = new QBluetoothSocket(QBluetoothServiceInfo::RfcommProtocol);
		connect(m_socket_bt, SIGNAL(error(QBluetoothSocket::SocketError)), this, SLOT(connectionError(QBluetoothSocket::SocketError)));
		connect(m_socket_bt, SIGNAL(connected()), this, SLOT(connectionReady()));
		connect(m_socket_bt, SIGNAL(readyRead()), this, SLOT(readReady()));
		m_socket_bt->connectToService(QBluetoothAddress(m_address), QBluetoothUuid(QStringLiteral("00001101-0000-1000-8000-00805F9B34FB")));
	} else {
		m_socket_tcp = new QTcpSocket(this);
		connect(m_socket_tcp, SIGNAL(connected()), this, SLOT(connectionReady()));
		connect(m_socket_tcp, SIGNAL(readyRead()), this, SLOT(readReady()));
		m_socket_tcp->connectToHost(m_address, m_port);
	}
}

void Device::stop()
{
	if (m_socket_tcp) {
		m_socket_tcp->disconnect();
		m_socket_tcp->close();
		m_socket_tcp = NULL;
		emit disconnected();
	}
	if (m_socket_bt) {
		m_socket_bt->disconnect();
		m_socket_bt->close();
		m_socket_bt = NULL;
		emit disconnected();
	}
}

void Device::send(const QString &data)
{
	QByteArray line;
	line.append(data);
	line.append("\n");
	if (m_socket_tcp) {
		m_socket_tcp->write(line);
	}
	if (m_socket_bt) {
		m_socket_bt->write(line);
	}
}

QList<QObject *> Device::channels()
{
	return m_channels.values();
}

void Device::recv(const QString &data)
{
	/* check if this is a device property line */
	if (data.section(':', 0, 0) == "device") {
		/* extract value */
		QString value = data.section(':', 2);
		/* extract full key description */
		QString key = data.section(':', 1, 1);
		if (key.size() < 1 || value.size() < 1) {
			/* key or value was invalid */
			return;
		}
		/* extract options from key */
		QStringList options = key.split(',', QString::SkipEmptyParts);
		key = options.takeFirst();
		/* TODO: handle options */

		/* set device property */
		this->setProperty(key.toStdString().c_str(), value);
		return;
	}

	/* extract channel id and validate */
	char channel_id = data.at(0).toLatin1();
	if (channel_id < 'A' || channel_id > 'Z') {
		/* invalid channel, must be upper-case ascii letter */
		return;
	}

	/* check if this is a data line */
	if (data.at(1) == '=') {
		/* skip if this is not yet a valid channel */
		if (!m_channels.contains(channel_id)) {
			return;
		}
		Channel *channel = (Channel *)m_channels[channel_id];
		if (!channel->isValid()) {
			return;
		}
		/* valid channel data received */
		channel->recv(data.mid(2));
		return;
	}

	/* check if this is a configuration line */
	if (data.at(1) == ':') {
		/* extract value */
		QString value = data.section(':', 2);
		/* extract full key description */
		QString key = data.section(':', 1, 1);
		if (key.size() < 1 || value.size() < 1) {
			/* key was invalid */
			return;
		}
		/* extract options from key */
		QStringList options = key.split(',', QString::SkipEmptyParts);
		key = options.takeFirst();
		/* TODO: handle options */

		/* create new channel into list if it does not yet exist */
		Channel *channel = m_channels.contains(channel_id) ? (Channel *)m_channels[channel_id] : NULL;
		if (!channel) {
			channel = new Channel(this, channel_id);
			/* check if there are child channels, that were added before, waiting for this parent */
			foreach (QObject *o, m_channels) {
				if (((Channel *)o)->parentChannel() == channel_id) {
					channel->addChild((Channel *)o);
				}
			}
			/* append to device channels */
			m_channels[channel_id] = (QObject *)channel;
			/* connect to parent change */
			connect(channel, SIGNAL(parentChanged()), this, SLOT(channelChanged()));
			/* connect to children change */
			connect(channel, SIGNAL(childrenChanged()), this, SLOT(channelChanged()));
			/* inform that new channel has been added */
			emit channelsChanged();
		}
		/* set value */
		if (key == "parent") {
			char parent_id = value.toStdString().c_str()[0];
			channel->setParentChannel(parent_id);
			/* find parent channel, if it already exists, and add this child to it */
			foreach (QObject *o, m_channels) {
				Channel *c = (Channel *)o;
				if (c->id() == parent_id && c->id() != channel_id) {
					c->addChild(channel);
					break;
				}
			}
		} else {
			channel->setProperty(key.toStdString().c_str(), value);
		}
		return;
	}
}

void Device::connectionReady()
{
	/* send config request to device */
	send("get config");
	/* inform that we are now connected */
	emit connected();
}

void Device::connectionError(QBluetoothSocket::SocketError)
{
	stop();
	emit error();
}

void Device::readReady()
{
	/* read data from one of the connected sockets */
	if (m_socket_tcp) {
		/* tcp network socket */
		while (m_socket_tcp->canReadLine()) {
			QByteArray line = m_socket_tcp->readLine();
			QString data;
			data.append(line);
			data = data.trimmed();
			if (!data.isEmpty()) {
				recv(data);
				emit receive(data);
			}
		}
	}
	if (m_socket_bt) {
		/* bluetooth socket */
		while (m_socket_bt->canReadLine()) {
			QByteArray line = m_socket_bt->readLine();
			QString data;
			data.append(line);
			data = data.trimmed();
			if (!data.isEmpty()) {
				recv(data);
				emit receive(data);
			}
		}
	}
}

void Device::channelChanged()
{
	emit channelsChanged();
}
