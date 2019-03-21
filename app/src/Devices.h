
#ifndef _DEVICES_H_
#define _DEVICES_H_

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
#include <QUdpSocket>
#include <QQmlContext>
#include <QTimer>

#include "DeviceStream.h"
#include "DeviceGroup.h"


class Devices : public QObject
{
	Q_OBJECT
	Q_PROPERTY(bool scanInProgress READ isScanning NOTIFY scanStateChanged)

	const int BCAST_PORT = 17001;

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
	void scannerBtFoundDevice(const QBluetoothDeviceInfo &);
	void scannerBtFinished();
	void deviceStreamReceiveData(QString data);
	void scannerUdpHasData();

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

	QBluetoothDeviceDiscoveryAgent *scannerBt;
	QUdpSocket *scannerUdp;
};


#endif
