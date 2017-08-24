
#include <math.h>

#include "DeviceChannel.h"
#include "DeviceStream.h"


DeviceChannel::DeviceChannel(int index, QObject *parent) :
	QObject(parent), timer(this)
{
	connect(&timer, SIGNAL(timeout()), this, SLOT(timedValuePull()));

	chIndex = index;
	chParentChannel = -1;
	chIsNameStatic = true;

	chAveraging = "true rms";

	chMode = "source";
	chMethod = "push";
	chFormat = "hex";
	chInterval = 1.0;
	chResolution = 1.0;
	chZero = 0.0;
	chDivider = 1.0;

	chValue = QVariant::fromValue(0);
}

int DeviceChannel::chGetIndex() const
{
	return chIndex;
}

bool DeviceChannel::isConnected()
{
	if (chName.isEmpty())
	{
		return false;
	}
	if (chType.isEmpty())
	{
		return false;
	}
	return true;
}

DeviceChannel *DeviceChannel::chGetThis()
{
	return this;
}

QString DeviceChannel::chGetName() const
{
	return chName;
}

void DeviceChannel::chSetName(const QString &value, bool isStatic)
{
	if (chName != value)
	{
		chName = value;
		chIsNameStatic = isStatic;
		emit chNameChanged();
	}
}

bool DeviceChannel::chGetNameStatic() const
{
	return chIsNameStatic;
}

QVariant DeviceChannel::chGetValue() const
{
	return chValue;
}

void DeviceChannel::chSetValue(const QVariant &value)
{
	if (chValue != value)
	{
		chValue = value;
		if (chMethod == "push" && chMode == "sink")
		{
			DeviceStream *stream = (DeviceStream *)parent();
			char channel_name = chIndex + 'A';
			QString str;
			str += channel_name;

			if (chType == "datetime")
			{
				str += chValue.toDateTime().toString("yyyy-MM-ddThh:mm:ss");
			}
			else if (chType == "switch")
			{
				str += chValue.toBool() ? "1" : "0";
			}
			else if (chValue.canConvert<int>())
			{
				if (chFormat == "hex")
				{
					str += QString("%1").arg(chValue.toInt(), 0, 16);
				}
				else
				{
					str += chValue.toInt();
				}
			}
			else
			{
				str += chValue.toString();
			}

			if (str.length() > 1)
			{
				stream->setStringData(str);
			}
		}
		emit chValueChanged();
	}
}

QString DeviceChannel::chGetType() const
{
	return chType;
}

void DeviceChannel::chSetType(const QString &value)
{
	if (chType != value)
	{
		chType = value;
		emit chTypeChanged();
	}
}

QString DeviceChannel::chGetMode() const
{
	return chMode;
}

void DeviceChannel::chSetMode(const QString &value)
{
	if (chMode != value)
	{
		chMode = value;
		emit chModeChanged();
	}

	/* start timer in chSetMode() AND in chSetMethod(), since either
	 * can change and have impact on how data should be
	 * retrieved
	 */
	if (chMethod == "pull" && chMode == "source" && !timer.isActive())
	{
		int interval = chInterval * 1000;
		timer.start(interval > 0 ? interval : 1000);
	}
}

QString DeviceChannel::chGetMethod() const
{
	return chMethod;
}

void DeviceChannel::chSetMethod(const QString &value)
{
	if (chMethod != value)
	{
		chMethod = value;
		emit chMethodChanged();
	}

	/* start timer in chSetMode() AND in chSetMethod(), since either
	 * can change and have impact on how data should be
	 * retrieved
	 */
	if (chMethod == "pull" && chMode == "source" && !timer.isActive())
	{
		int interval = chInterval * 1000;
		timer.start(interval > 0 ? interval : 1000);
	}
}

QString DeviceChannel::chGetFormat() const
{
	return chFormat;
}

void DeviceChannel::chSetFormat(const QString &value)
{
	if (chFormat != value)
	{
		chFormat = value;
		emit chFormatChanged();
	}
}

double DeviceChannel::chGetInterval() const
{
	return chInterval;
}

void DeviceChannel::chSetInterval(const double &value)
{
	if (chInterval != value)
	{
		chInterval = value;
		emit chIntervalChanged();
	}
}

double DeviceChannel::chGetResolution() const
{
	return chResolution;
}

void DeviceChannel::chSetResolution(const double &value)
{
	if (chResolution != value)
	{
		chResolution = value;
		emit chResolutionChanged();
	}
}

double DeviceChannel::chGetZero() const
{
	return chZero;
}

void DeviceChannel::chSetZero(const double &value)
{
	if (chZero != value)
	{
		chZero = value;
		emit chZeroChanged();
	}
}

double DeviceChannel::chGetDivider() const
{
	return chDivider;
}

void DeviceChannel::chSetDivider(const double &value)
{
	if (chDivider != value)
	{
		chDivider = value;
		emit chDividerChanged();
	}
}

QStringList DeviceChannel::chGetColors() const
{
	return chColors;
}

int DeviceChannel::chGetParentChannel() const
{
	return chParentChannel;
}

void DeviceChannel::chSetParentChannel(const QString data)
{
	DeviceStream *stream = (DeviceStream *)parent();
	int channel_id = (int)data.at(0).toLatin1() - (int)'A';

	if (chParentChannel >= 0)
	{
		qDebug() << "parent channel already set";
		return;
	}

	DeviceChannel *channel = stream->getChannel(channel_id, false);
	if (!channel)
	{
		qDebug() << "invalid parent channel" << channel_id;
		return;
	}

	chParentChannel = channel_id;
	channel->addChildChannel(this);
	emit chParentChannelChanged();
}

bool DeviceChannel::chHasValidParentChannel() const
{
	if (chParentChannel < 0)
	{
		return false;
	}

	DeviceStream *stream = (DeviceStream *)parent();
	if (stream->getChannel(chParentChannel, false))
	{
		return true;
	}

	return false;
}

DeviceChannel *DeviceChannel::chGetFirstChild() const
{
	DeviceStream *stream = (DeviceStream *)parent();
	QList<DeviceChannel *> channels = stream->getChannelsByParent(chIndex);

	if (channels.count() > 0)
	{
		return channels.first();
	}

	return NULL;
}

void DeviceChannel::timedValuePull()
{
	DeviceStream *stream = (DeviceStream *)parent();
	char channel_name = chIndex + 'A';
	QString str;
	str += channel_name;
	stream->setStringData(str);
}

bool DeviceChannel::receiveConfigValue(const QStringList data)
{
	if (data[3] == "static" && data[4] == "name")
	{
		if (data.count() != 6)
		{
			/* invalid static name */
			return false;
		}
		/* special case of static name */
		chSetName(data[5], true);
		return true;
	}
	else if (data[3] == "name")
	{
		chSetName(data[4], false);
		return true;
	}
	else if (data[3] == "type")
	{
		chSetType(data[4]);
		return true;
	}
	else if (data[3] == "mode")
	{
		chSetMode(data[4]);
		return true;
	}
	else if (data[3] == "method")
	{
		chSetMethod(data[4]);
		return true;
	}
	else if (data[3] == "interval")
	{
		chSetInterval(data[4].toDouble());
		return true;
	}
	else if (data[3] == "resolution")
	{
		chSetResolution(data[4].toDouble());
		return true;
	}
	else if (data[3] == "divider")
	{
		chSetDivider(data[4].toDouble());
		return true;
	}
	else if (data[3] == "zero")
	{
		chSetZero(data[4].toDouble());
		return true;
	}
	else if (data[3] == "parent")
	{
		chSetParentChannel(data[4]);
		return true;
	}
	else if (data[3] == "color")
	{
		chColors.clear();
		for (int i = 4; i < data.count(); i++)
		{
			QColor color("#" + data[i]);
			if (color.isValid())
			{
				chColors.append(color.name());
			}
		}
		emit chColorsChanged();
		return true;
	}

	return false;
}

bool DeviceChannel::receiveData(QString data)
{
	/* special cases first */
	if (chType == "datetime")
	{
		chSetValue(QDateTime::fromString(data, Qt::ISODate));
		return true;
	}
	else if (chType == "switch")
	{
		bool value = (data.at(0) == QChar('0')) ? false : true;
		chSetValue(value);
		return true;
	}

	/* sample base */
	int base = 10;
	if (chFormat == "hex")
	{
		base = 16;
	}
	else if (chFormat == "octal")
	{
		base = 8;
	}
	else if (chFormat == "binary")
	{
		base = 2;
	}

	/* parse value */
	bool ok = false;
	qint64 value = data.toLongLong(&ok, base);
	if (!ok)
	{
		qDebug() << "invalid sample data (base:" << base << "):" << data;
		return false;
	}

	/* adjust to zero */
	value -= chZero;

	/* if shannel is sink, then no averaging etc is done */
	if (chMode == "sink")
	{
		chSetValue(QVariant::fromValue(value / chDivider));
		return true;
	}

	/* adjust value according to averaging */
	if (chAveraging == "true rms")
	{
		value *= value;
	}
	else
	{
		/* assume simple which is just the value itself */
	}

	samples.append(value);
	if (samples.count() < 800)
	{
		return false;
	}

	/* add up all the samples */
	qint64 sum = 0;
	for (int i = 0; i < samples.count(); i++)
	{
		sum += samples[i];
	}

	double result = sqrt(sum / samples.count()) / chDivider;
	chSetValue(QVariant::fromValue(result));

	samples.clear();

	return true;
}

void DeviceChannel::stop()
{
	timer.stop();
	chSetValue(QVariant());
}

void DeviceChannel::addChildChannel(DeviceChannel *channel)
{
	chChildChannels.append(channel);
	emit chChildChannelAdded(channel);
	emit chValuesChanged();
}

QList<QObject *> DeviceChannel::chGetChildren()
{
	return chChildChannels;
}

QList<QObject *> DeviceChannel::getGroupChannels()
{
	QList<QObject *> values;

	values.append(this);

	foreach (QObject *object, chChildChannels)
	{
		DeviceChannel *channel = (DeviceChannel *)object;

		/* some channel values are not displayed */
		if (channel->chGetMode() == "sink")
		{
			if (channel->chGetType() == "datetime")
			{
				continue;
			}
		}

		values.append(channel);
	}

	return values;
}
