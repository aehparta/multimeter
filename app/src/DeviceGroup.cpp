
#include "DeviceGroup.h"


DeviceGroup::DeviceGroup(QObject *parent) :
	QObject(parent)
{
	connect(parent, SIGNAL(nameChanged()), this, SLOT(forwardNameChanged()));
	connect(parent, SIGNAL(typeChanged()), this, SLOT(forwardTypeChanged()));
	connect(parent, SIGNAL(valueChanged()), this, SLOT(forwardValueChanged()));
}

QString DeviceGroup::getGroupName() const
{
	DeviceChannel *channel = (DeviceChannel *)parent();
	return channel->getName();
}

void DeviceGroup::setGroupName(const QString &name)
{
	DeviceChannel *channel = (DeviceChannel *)parent();
	channel->setName(name);
}

bool DeviceGroup::isGroupNameStatic() const
{
	DeviceChannel *channel = (DeviceChannel *)parent();
	return channel->isNameStatic();
}

QString DeviceGroup::getGroupType() const
{
	DeviceChannel *channel = (DeviceChannel *)parent();
	return channel->getType();
}

QVariant DeviceGroup::getGroupValue() const
{
	DeviceChannel *channel = (DeviceChannel *)parent();
	return channel->getValue();
}

void DeviceGroup::setGroupValue(const QVariant &value)
{
	DeviceChannel *channel = (DeviceChannel *)parent();
	channel->setValue(value);
}

QList<QObject *> DeviceGroup::getGroupChannels()
{
	DeviceChannel *channel = (DeviceChannel *)parent();
	return channel->getGroupChannels();
}
