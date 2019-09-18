
#include "channel.h"

Channel::Channel(QObject *parent) :	QObject(parent)
{
	m_base = 16;
	m_multiplier = 1;
	m_resolution = 1;

	m_wasValid = false;

	connect(this, SIGNAL(nameChanged()), this, SLOT(checkValid()));
	connect(this, SIGNAL(typeChanged()), this, SLOT(checkValid()));
	connect(this, SIGNAL(modeChanged()), this, SLOT(checkValid()));
	connect(this, SIGNAL(methodChanged()), this, SLOT(checkValid()));
}

bool Channel::isValid()
{
	return !m_name.isEmpty() && !m_type.isEmpty() && !m_mode.isEmpty() && !m_method.isEmpty();
}

void Channel::recv(QString data)
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

void Channel::checkValid()
{
	if (isValid() != m_wasValid) {
		m_wasValid = isValid();
		emit validChanged();
	}
}
