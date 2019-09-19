
#include "device.h"
#include "channel.h"

Channel::Channel(QObject *parent, char id) : QObject(parent)
{
	m_id = id;
	m_parent = 0;

	m_base = 16;
	m_multiplier = 1;
	m_resolution = 1;

	m_enabled = true;

	m_wasValid = false;

	connect(this, SIGNAL(nameChanged()), this, SLOT(checkValid()));
	connect(this, SIGNAL(typeChanged()), this, SLOT(checkValid()));
	connect(this, SIGNAL(modeChanged()), this, SLOT(checkValid()));
	connect(this, SIGNAL(methodChanged()), this, SLOT(checkValid()));

	/* connect to device enabled changed signal */
	connect(parent, SIGNAL(enabledChanged()), this, SLOT(deviceEnabled()));
}

char Channel::id()
{
	return m_id;
}

char Channel::parentChannel()
{
	return m_parent;
}

void Channel::setParentChannel(char parent_id)
{
	if (parent_id != m_parent && 'A' <= parent_id && parent_id <= 'Z') {
		m_parent = parent_id;
		emit parentChanged();
	}
}

bool Channel::hasChild(char child_id)
{
	foreach (QObject *o, m_children) {
		if (((Channel *)o)->id() == child_id) {
			return true;
		}
	}
	return false;
}

bool Channel::hasChild(Channel *channel)
{
	foreach (QObject *o, m_children) {
		if (((Channel *)o)->id() == channel->id()) {
			return true;
		}
	}
	return false;
}

void Channel::addChild(Channel *channel)
{
	if (!hasChild(channel)) {
		m_children.append(channel);
		emit childrenChanged();
	}
}

QString Channel::value()
{
	return m_value;
}

void Channel::setValue(QString value)
{
	if (m_value != value) {
		m_value = value;
		send(m_value);
		emit valueChanged();
	}
}

bool Channel::isValid()
{
	return !m_name.isEmpty() && !m_type.isEmpty() && !m_mode.isEmpty() && !m_method.isEmpty();
}

void Channel::recv(const QString &data)
{
	/* as default parse number */
	double number = 0;
	bool ok = false;
	if (m_base == 10) {
		/* only base 10 can be parsed as floating point */
		number = data.toDouble(&ok);
	} else {
		/* other bases as long */
		number = data.toLong(&ok, m_base);
	}
	if (!ok) {
		return;
	}
	/* multiply */
	number *= m_multiplier;
	/* round up to resolution */
	number = double(int(number / m_resolution)) * m_resolution;
	/* cut decimals of the end manually, float value round ups can sometimes add small decimals */
	int decimals = 0;
	QString r = QString("%1").arg((m_resolution - double((int(m_resolution)))));
	if (r.mid(1, 1) == ".") {
		decimals = r.size() - 2;
	}
	/* change value */
	m_value = QString::number(number, 'f', decimals);
	emit valueChanged();
}

void Channel::send(const QString &data)
{
	Device *device = (Device *)parent();
	device->send(QString(m_id) + "=" + data);
}

bool Channel::isEnabled()
{
	return parent()->property("enabled").toBool() && m_enabled;
}

void Channel::setEnabled(bool value)
{
	if (m_enabled != value) {
		m_enabled = value;
		emit enabledChanged();
	}
}

void Channel::checkValid()
{
	if (isValid() != m_wasValid) {
		m_wasValid = isValid();
		emit validChanged();
	}
}

void Channel::deviceEnabled()
{
	emit enabledChanged();
}
