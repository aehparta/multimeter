
#ifndef __DEVICE_GROUP_H__
#define __DEVICE_GROUP_H__

#include <QDebug>
#include <QObject>
#include <QAbstractListModel>
#include <QQmlListProperty>
#include <QBluetoothSocket>
#include <QSocketNotifier>
#include <QBluetoothServiceInfo>
#include <QBluetoothDeviceInfo>
#include <QBluetoothServiceDiscoveryAgent>
#include <QBluetoothDeviceDiscoveryAgent>
#include <QVariant>
#include <QTimer>
#include <QColor>

#include "DeviceChannel.h"


class DeviceGroup : public QObject
{
	Q_OBJECT

	Q_PROPERTY(QString groupName READ getGroupName WRITE setGroupName NOTIFY groupNameChanged)
	Q_PROPERTY(bool groupNameIsStatic READ isGroupNameStatic)

	Q_PROPERTY(QString groupType READ getGroupType NOTIFY groupTypeChanged)
	Q_PROPERTY(QVariant groupValue READ getGroupValue WRITE setGroupValue NOTIFY groupValueChanged)
	Q_PROPERTY(QList<QObject *> groupChannels READ getGroupChannels NOTIFY groupChannelsChanged)

public:
	DeviceGroup(QObject *parent);

	QString getGroupName() const;
	void setGroupName(const QString &name);
	bool isGroupNameStatic() const;
	QString getGroupType() const;
	QVariant getGroupValue() const;
	void setGroupValue(const QVariant &value);
	QList<QObject *> getGroupChannels();

signals:
	void groupNameChanged();
	void groupTypeChanged();
	void groupValueChanged();
	void groupChannelsChanged();

public slots:

private slots:
	void forwardNameChanged() { emit groupNameChanged(); }
	void forwardTypeChanged() { emit groupTypeChanged(); }
	void forwardValueChanged() { emit groupValueChanged(); }

protected:

private:
};


#endif
