
#include <stdlib.h>
#include "device.h"
#include "channel.h"

Channel::Channel(QObject *parent, char id)
	: QObject(parent), m_timer(this)
{
	m_id = id;
	m_parent = 0;
	m_plain = false;

	m_base = 16;
	m_multiplier = 1;
	m_resolution = 0;
	m_divider = 0;

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

void Channel::recv(const QString &value)
{
	/* parse other types than number first */
	if (m_type == "switch") {
		QString v = value != "0" ? "1" : "0";
		if (m_value != v) {
			m_value = v;
			emit valueChanged();
		}
		return;
	} else if (m_type == "datetime" || m_type == "date" || m_type == "time" || m_type == "plain" || m_plain) {
		if (m_value != value) {
			m_value = value;
			emit valueChanged();
		}
		return;
	}

	/* as default parse number */
	double number = 0;
	bool ok = false;
	if (m_base == 10) {
		/* only base 10 can be parsed as floating point */
		number = value.toDouble(&ok);
	} else {
		/* other bases as long */
		number = value.toLong(&ok, m_base);
	}
	if (!ok) {
		return;
	}
	/* multiply */
	number *= m_multiplier;
	/* change value */
	m_value = QString::number(number, 'f', 12);
	emit valueChanged();
}

void Channel::send(const QString &value)
{
	/* as default send as number */
	bool ok = false;
	double number = value.toDouble(&ok);
	if (!ok) {
		return;
	}
	/* divide by multiplier */
	number /= m_multiplier;
	/* convert to correct base and send */
	Device *device = (Device *)parent();
	if (m_base == 10) {
		device->send(QString(m_id) + "=" + number);
	} else {
		device->send(QString(m_id) + "=" + QString("%1").arg((long)number, 0, m_base));
	}
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
	qDebug() << "valid" << m_name << m_type << m_mode << m_method << m_wasValid << isValid();
	if (isValid() != m_wasValid) {
		m_wasValid = isValid();
		emit validChanged();

		/* setup pull if channel value must be pulled */
		if (m_type == "datetime") {
			connect(&m_timer, SIGNAL(timeout()), this, SLOT(pull()));
			m_timer.setInterval(10);
			m_timer.setSingleShot(false);
			m_timer.start();
		}
	}
}

void Channel::deviceEnabled()
{
	emit enabledChanged();
}

void Channel::pull()
{
	// qDebug() << "pull";

	Device *device = (Device *)parent();
	device->send(QString(m_id));

	m_timer.setInterval(1000);
}