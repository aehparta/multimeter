

#include "Devices.h"


Devices::Devices(QObject *parent) :
	QObject(parent), timer(this)
{
	deviceAgent = NULL;

	counter = 0;
	connect(&timer, SIGNAL(timeout()), this, SLOT(testSlot()));
	timer.start(500);
}

void Devices::scan()
{
	if (!deviceAgent)
	{
		deviceAgent = new QBluetoothDeviceDiscoveryAgent(this);
		connect(deviceAgent, SIGNAL(deviceDiscovered(QBluetoothDeviceInfo)),
		        this, SLOT(deviceDiscovered(QBluetoothDeviceInfo)));
		connect(deviceAgent, SIGNAL(finished()), this, SLOT(deviceDiscoveryFinished()));
	}

	if (!deviceAgent->isActive())
	{
		deviceAgent->start();
		emit scanStarted();
		emit scanStateChanged();
	}
}

bool Devices::isScanning() const
{
	if (!deviceAgent)
	{
		return false;
	}
	return deviceAgent->isActive();
}

void Devices::serviceDiscovered(const QBluetoothServiceInfo &service)
{

}

void Devices::deviceDiscovered(const QBluetoothDeviceInfo &device)
{
	qDebug() << "device" << device.address();
	DeviceStream *stream = new DeviceStream(this);
	connect(stream, SIGNAL(connected()), this, SLOT(deviceStreamConnected()));
	connect(stream, SIGNAL(stringDataChanged()), this, SLOT(deviceStreamReceiveData()));
	stream->setAddress(device.address().toString());
	stream->start();
	streams.append(stream);
}

void Devices::deviceDiscoveryFinished()
{
	qDebug() << "device discovery finished";
	emit scanFinished();
	emit scanStateChanged();
}

void Devices::deviceStreamConnected()
{
	DeviceStream *stream = (DeviceStream *)sender();
	qDebug() << "device stream connected, address" << stream->address() << ", request configuration";
}

void Devices::deviceStreamReceiveData()
{
	DeviceStream *stream = (DeviceStream *)sender();
	QString data = stream->stringData();

	if (data.startsWith("config:"))
	{
		qDebug() << data;
		parseConfigLine(stream, data);
	}
	else
	{
		stream->receiveData(data);
	}
}

void Devices::parseConfigLine(DeviceStream *stream, const QString &data)
{
	QStringList parts = data.split(":");

	if (parts.count() < 5)
	{
		return;
	}

	if (parts[1] == "channel")
	{
		if (parseConfigLineChannel(stream, parts))
		{
			checkNewChannels();
		}
	}
}

bool Devices::parseConfigLineChannel(DeviceStream *stream, const QStringList &data)
{
	int channel_number = (int)data[2].at(0).toLatin1() - (int)'A';

	if (channel_number < 0 || channel_number > 31)
	{
		/* invalid channel number */
		return false;
	}

	DeviceChannel *channel = stream->getChannel(channel_number, true);
	return channel->receiveConfigValue(data);
}

void Devices::checkNewChannels()
{
	QList<QObject *> gs;

	foreach (DeviceStream *stream, streams)
	{
		QList<QObject *> scs = stream->getChannels();
		foreach (QObject *object, scs)
		{
			DeviceGroup *group = new DeviceGroup(object);
			gs.append(group);
		}
	}

	if (gs != groups)
	{
		groups = gs;
		rootContext->setContextProperty("channelsModel", QVariant::fromValue(groups));
	}
}

void Devices::setRootContext(QQmlContext *ctxt)
{
	rootContext = ctxt;
	//testSlot();
}

void Devices::testSlot()
{
	DeviceChannel *channel;
	return;

	if (counter > 0)
	{
		channel = (DeviceChannel *)streams.at(0)->getChannel(0, false);
		channel->chSetValue(QDateTime::currentDateTime());
	}
	else
	{
		DeviceStream *stream = new DeviceStream(this);

		channel = stream->getChannel(0, true);
		channel->chSetName(QString("channel 0"), true);
		channel->chSetType("datetime");

		channel = stream->getChannel(1, true);
		channel->chSetName(QString("channel 1"), true);
		channel->chSetValue(rand() % 100);
		channel->chSetType("slider");
		channel->chSetParentChannel("A");

		channel = stream->getChannel(10, true);
		channel->chSetName(QString("channel 10"), true);
		channel->chSetValue(0);
		channel->chSetType("switch");

		streams.append(stream);

		checkNewChannels();
	}

	counter++;
}

