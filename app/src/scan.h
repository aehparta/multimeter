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
	Q_PROPERTY(bool btEnabled MEMBER m_scanner_bt_enabled WRITE btEnable NOTIFY btEnabledChanged)
	Q_PROPERTY(bool udpEnabled MEMBER m_scanner_udp_enabled WRITE udpEnable NOTIFY udpEnabledChanged)
	Q_PROPERTY(bool active READ isActive NOTIFY activeChanged)
	Q_PROPERTY(bool autostart MEMBER m_autostart NOTIFY autostartChanged)
	Q_PROPERTY(int selected MEMBER m_selected NOTIFY selectedChanged)
	Q_PROPERTY(QList<QObject *> channels READ channels NOTIFY channelsChanged)
	Q_PROPERTY(QList<QObject *> devices READ devices NOTIFY devicesChanged)

	const int BCAST_PORT = 17001;

public:
	Scan(QObject *parent = NULL);
	~Scan();

	void btEnable(bool value);
	void udpEnable(bool value);

	Q_INVOKABLE void start();
	bool isActive();
	QList<QObject *> channels();
	QList<QObject *> devices();

	Q_INVOKABLE void selectedRemove();
	
signals:
	void btEnabledChanged();
	void udpEnabledChanged();

	void started();
	void finished();
	void activeChanged();
	void discovered(Device *device);
	void autostartChanged();
	void channelsChanged();
	void devicesChanged();
	void selectedChanged();

private slots:
	void btDiscovered(const QBluetoothDeviceInfo &);
	void btFinished();
	void udpFinished();
	void udpHasData();
	void deviceChannelsChanged();
	void deviceSelectedChanged();

protected:

private:
	/* bluetooth scanner */
	QBluetoothDeviceDiscoveryAgent *m_scanner_bt;
	bool m_scanner_bt_enabled;
	/* udp scanner */
	QUdpSocket *m_scanner_udp;
	bool m_scanner_udp_enabled;
	QTimer m_scanner_udp_timer;
	/* autostart discovered devices */
	bool m_autostart;
	/* count of selected devices */
	int m_selected;
	/* list of devices found */
	QList<QObject *> m_devices;

	/* add new device */
	bool addDevice(const QString &address, int port, QString name);
	bool addDevice(Device *device);
};


#endif