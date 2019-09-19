#ifndef _DEVICES_H_
#define _DEVICES_H_

#include <QUdpSocket>
#include <QBluetoothDeviceDiscoveryAgent>
#include <QJsonValue>
#include <QJsonObject>
#include "device.h"


class Scan : public QObject
{
	Q_OBJECT
	Q_PROPERTY(bool active READ isActive NOTIFY changedActive)
	Q_PROPERTY(bool autostart MEMBER m_autostart NOTIFY autostartChanged)
	Q_PROPERTY(QList<QObject *> channels READ channels NOTIFY channelsChanged)
	Q_PROPERTY(QList<QObject *> devices READ devices NOTIFY devicesChanged)

	const int BCAST_PORT = 17001;

public:
	Scan(QObject *parent = NULL);
	~Scan();

	Q_INVOKABLE void start();
	bool isActive();
	QList<QObject *> channels();
	QList<QObject *> devices();

signals:
	void started();
	void finished();
	void changedActive();
	void discovered(Device *device);
	void autostartChanged();
	void channelsChanged();
	void devicesChanged();

private slots:
	void btDiscovered(const QBluetoothDeviceInfo &);
	void btFinished();
	void udpHasData();
	void deviceChannelsChanged();

protected:

private:
	/* bluetooth scanner */
	QBluetoothDeviceDiscoveryAgent *m_scanner_bt;
	/* udp scanner */
	QUdpSocket *m_scanner_udp;
	/* autostart discovered devices */
	bool m_autostart;
	/* list of devices found */
	QList<QObject *> m_devices;

	/* add new device */
	bool addDevice(const QString &address, int port = -1);
	bool addDevice(Device *device);
};


#endif