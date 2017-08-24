
#ifndef __DEVICES_H__
#define __DEVICES_H__

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
#include <QQmlContext>
#include <QTimer>

#include "DeviceStream.h"
#include "DeviceGroup.h"


class Devices : public QObject
{
	Q_OBJECT
	Q_PROPERTY(bool scanInProgress READ isScanning NOTIFY scanStateChanged)

public:
	Devices(QObject *parent = NULL);
	void setRootContext(QQmlContext *ctxt);
	Q_INVOKABLE void scan();
	bool isScanning() const;

signals:
	void scanStarted();
	void scanFinished();
	void scanStateChanged();

public slots:
	void serviceDiscovered(const QBluetoothServiceInfo &);
	void deviceDiscovered(const QBluetoothDeviceInfo &);
	void deviceDiscoveryFinished();
	void deviceStreamConnected();
	void deviceStreamReceiveData();

	void testSlot();

protected:

private:
	bool xxx;

	int counter;
	QTimer timer;

	QList<DeviceStream *> streams;
	QList<QObject *> groups;

	QQmlContext *rootContext;

	void parseConfigLine(DeviceStream *stream, const QString &data);
	bool parseConfigLineChannel(DeviceStream *stream, const QStringList &data);

	void checkNewChannels();

	QBluetoothDeviceDiscoveryAgent *deviceAgent;
	QBluetoothServiceDiscoveryAgent *serviceAgent;
};


#endif
