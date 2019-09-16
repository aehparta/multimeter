
#include <math.h>

#include "DeviceChannel.h"
#include "DeviceStream.h"


DeviceChannel::DeviceChannel(int ind, QObject *parent) :
	QObject(parent), timer(this)
{
	connect(&timer, SIGNAL(timeout()), this, SLOT(timedValuePull()));

	index = ind;
	parentChannel = -1;
	nameStatic = true;

	averaging = "true rms";

	mode = "source";
	method = "push";
	format = "hex";
	interval = 1.0;
	resolution = 1.0;
	zero = 0.0;
	divider = 1.0;

	value = QVariant::fromValue(0);
}

int DeviceChannel::getIndex() const
{
	return index;
}

bool DeviceChannel::isConnected()
{
	if (name.isEmpty()) {
		return false;
	}
	if (type.isEmpty()) {
		return false;
	}
	return true;
}

DeviceChannel *DeviceChannel::getThis()
{
	return this;
}

QString DeviceChannel::getName() const
{
	return name;
}

void DeviceChannel::setName(const QString &value, bool isStatic)
{
	if (name != value) {
		name = value;
		nameStatic = isStatic;
		emit nameChanged();
	}
}

bool DeviceChannel::isNameStatic() const
{
	return nameStatic;
}

QVariant DeviceChannel::getValue() const
{
	return value;
}

void DeviceChannel::setValue(const QVariant &v)
{
	if (value != v) {
		value = v;
		if (method == "push" && mode == "sink") {
			DeviceStream *stream = (DeviceStream *)parent();
			char channel_name = index + 'A';
			QString str;
			str += channel_name;

			if (type == "datetime") {
				str += value.toDateTime().toString("yyyy-MM-ddThh:mm:ss");
			} else if (type == "switch") {
				str += value.toBool() ? "1" : "0";
			} else if (value.canConvert<int>()) {
				if (format == "hex") {
					str += QString("%1").arg(value.toInt(), 0, 16);
				} else {
					str += value.toInt();
				}
			} else {
				str += value.toString();
			}

			if (str.length() > 1) {
				stream->send(str);
			}
		}
		emit valueChanged();
	}
}

QString DeviceChannel::getType() const
{
	return type;
}

void DeviceChannel::setType(const QString &value)
{
	if (type != value) {
		type = value;
		emit typeChanged();
	}
}

QString DeviceChannel::getMode() const
{
	return mode;
}

void DeviceChannel::setMode(const QString &value)
{
	if (mode != value) {
		mode = value;
		emit modeChanged();
	}

	/* start timer in setMode() AND in setMethod(), since either
	 * can change and have impact on how data should be
	 * retrieved
	 */
	if (method == "pull" && mode == "source" && !timer.isActive()) {
		int interval = interval * 1000;
		timer.start(interval > 0 ? interval : 1000);
	}
}

QString DeviceChannel::getMethod() const
{
	return method;
}

void DeviceChannel::setMethod(const QString &value)
{
	if (method != value) {
		method = value;
		emit methodChanged();
	}

	/* start timer in setMode() AND in setMethod(), since either
	 * can change and have impact on how data should be
	 * retrieved
	 */
	if (method == "pull" && mode == "source" && !timer.isActive()) {
		int interval = interval * 1000;
		timer.start(interval > 0 ? interval : 1000);
	}
}

QString DeviceChannel::getFormat() const
{
	return format;
}

void DeviceChannel::setFormat(const QString &value)
{
	if (format != value) {
		format = value;
		emit formatChanged();
	}
}

double DeviceChannel::getInterval() const
{
	return interval;
}

void DeviceChannel::setInterval(const double &value)
{
	if (interval != value) {
		interval = value;
		emit intervalChanged();
	}
}

double DeviceChannel::getResolution() const
{
	return resolution;
}

void DeviceChannel::setResolution(const double &value)
{
	if (resolution != value) {
		resolution = value;
		emit resolutionChanged();
	}
}

double DeviceChannel::getZero() const
{
	return zero;
}

void DeviceChannel::setZero(const double &value)
{
	if (zero != value) {
		zero = value;
		emit zeroChanged();
	}
}

double DeviceChannel::getDivider() const
{
	return divider;
}

void DeviceChannel::setDivider(const double &value)
{
	if (divider != value) {
		divider = value;
		emit dividerChanged();
	}
}

QStringList DeviceChannel::getColors() const
{
	return colors;
}

int DeviceChannel::getParentChannel() const
{
	return parentChannel;
}

void DeviceChannel::setParentChannel(const QString data)
{
	DeviceStream *stream = (DeviceStream *)parent();
	int channel_id = (int)data.at(0).toLatin1() - (int)'A';

	if (parentChannel >= 0) {
		return;
	}

	DeviceChannel *channel = stream->getChannel(channel_id, false);
	if (!channel) {
		return;
	}

	parentChannel = channel_id;
	channel->addChildChannel(this);
	emit parentChannelChanged();
}

bool DeviceChannel::hasValidParentChannel() const
{
	if (parentChannel < 0) {
		return false;
	}

	DeviceStream *stream = (DeviceStream *)parent();
	if (stream->getChannel(parentChannel, false)) {
		return true;
	}

	return false;
}

DeviceChannel *DeviceChannel::getFirstChild() const
{
	DeviceStream *stream = (DeviceStream *)parent();
	QList<DeviceChannel *> channels = stream->getChannelsByParent(index);

	if (channels.count() > 0) {
		return channels.first();
	}

	return NULL;
}

void DeviceChannel::timedValuePull()
{
	DeviceStream *stream = (DeviceStream *)parent();
	char channel_name = index + 'A';
	QString str;
	str += channel_name;
	stream->send(str);
}

bool DeviceChannel::receiveConfigValue(const QStringList data)
{
	if (data[3] == "static" && data[4] == "name") {
		if (data.count() != 6) {
			/* invalid static name */
			return false;
		}
		/* special case of static name */
		setName(data[5], true);
		return true;
	} else if (data[3] == "name") {
		setName(data[4], false);
		return true;
	} else if (data[3] == "type") {
		setType(data[4]);
		return true;
	} else if (data[3] == "mode") {
		setMode(data[4]);
		return true;
	} else if (data[3] == "method") {
		setMethod(data[4]);
		return true;
	} else if (data[3] == "interval") {
		setInterval(data[4].toDouble());
		return true;
	} else if (data[3] == "resolution") {
		setResolution(data[4].toDouble());
		return true;
	} else if (data[3] == "divider") {
		setDivider(data[4].toDouble());
		return true;
	} else if (data[3] == "zero") {
		setZero(data[4].toDouble());
		return true;
	} else if (data[3] == "format") {
		setFormat(data[4]);
		return true;
	} else if (data[3] == "parent") {
		setParentChannel(data[4]);
		return true;
	} else if (data[3] == "color") {
		colors.clear();
		for (int i = 4; i < data.count(); i++) {
			QColor color("#" + data[i]);
			if (color.isValid()) {
				colors.append(color.name());
			}
		}
		emit colorsChanged();
		return true;
	}

	return false;
}

bool DeviceChannel::receiveData(QString data)
{
	/* special cases first */
	if (type == "datetime") {
		setValue(QDateTime::fromString(data, Qt::ISODate));
		return true;
	} else if (type == "switch") {
		bool value = (data.at(0) == QChar('0')) ? false : true;
		setValue(value);
		return true;
	}

	/* parse value based on format */
	bool ok = false;
	double value;
	if (format == "decimal") {
		/* parse float */
		value = data.toDouble(&ok);
	} else {
		/* parse integer value by base */
		int base = 10;
		if (format == "hex") {
			base = 16;
		} else if (format == "octal") {
			base = 8;
		} else if (format == "binary") {
			base = 2;
		}
		value = (double)data.toLongLong(&ok, base);
	}
	if (!ok) {
		qDebug() << "invalid sample data (format:" << format << "):" << data;
		return false;
	}

	/* adjust to zero */
	value -= zero;

	/* if shannel is sink, then no averaging etc is done */
	if (mode == "sink") {
		setValue(QVariant::fromValue(value / divider));
		return true;
	}

	/* adjust value according to averaging */
	if (averaging == "true rms") {
		value *= value;
	} else {
		/* assume simple which is just the value itself */
	}

	samples.append(value);
	if (samples.count() < 1) {
		return false;
	}

	/* add up all the samples */
	double sum = 0;
	for (int i = 0; i < samples.count(); i++) {
		sum += samples[i];
	}

	double result = sqrt(sum / (double)samples.count()) / divider;
	setValue(QVariant::fromValue(result));

	samples.clear();

	return true;
}

void DeviceChannel::stop()
{
	timer.stop();
	setValue(QVariant());
}

void DeviceChannel::addChildChannel(DeviceChannel *channel)
{
	childChannels.append(channel);
	emit childChannelAdded(channel);
	emit valuesChanged();
}

QList<QObject *> DeviceChannel::getChildren()
{
	return childChannels;
}

QList<QObject *> DeviceChannel::getGroupChannels()
{
	QList<QObject *> values;

	values.append(this);

	foreach (QObject *object, childChannels) {
		DeviceChannel *channel = (DeviceChannel *)object;

		/* some channel values are not displayed */
		if (channel->getMode() == "sink") {
			if (channel->getType() == "datetime") {
				continue;
			}
		}

		values.append(channel);
	}

	return values;
}
