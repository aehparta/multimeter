
#include "DeviceGroup.h"


DeviceGroup::DeviceGroup(QObject *parent) :
	QObject(parent)
{
	connect(parent, SIGNAL(chNameChanged()), this, SLOT(forwardNameChanged()));
	connect(parent, SIGNAL(chTypeChanged()), this, SLOT(forwardTypeChanged()));
	connect(parent, SIGNAL(chValueChanged()), this, SLOT(forwardValueChanged()));
}

QString DeviceGroup::getGroupName() const
{
	DeviceChannel *channel = (DeviceChannel *)parent();
	return channel->chGetName();
}

void DeviceGroup::setGroupName(const QString &name)
{
	DeviceChannel *channel = (DeviceChannel *)parent();
	channel->chSetName(name);
}

bool DeviceGroup::isGroupNameStatic() const
{
	DeviceChannel *channel = (DeviceChannel *)parent();
	return channel->chGetNameStatic();
}

QString DeviceGroup::getGroupType() const
{
	DeviceChannel *channel = (DeviceChannel *)parent();
	return channel->chGetType();
}

QVariant DeviceGroup::getGroupValue() const
{
	DeviceChannel *channel = (DeviceChannel *)parent();
	return channel->chGetValue();
}

void DeviceGroup::setGroupValue(const QVariant &value)
{
	DeviceChannel *channel = (DeviceChannel *)parent();
	channel->chSetValue(value);
}

QList<QObject *> DeviceGroup::getGroupChannels()
{
	DeviceChannel *channel = (DeviceChannel *)parent();
	return channel->getGroupChannels();
}
