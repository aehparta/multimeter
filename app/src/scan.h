#ifndef _DEVICES_H_
#define _DEVICES_H_

#include <QUdpSocket>
#include <QBluetoothDeviceDiscoveryAgent>
#include "device.h"


class Scan : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool active READ isActive NOTIFY changedActive)

    const int BCAST_PORT = 17001;

public:
    Scan(QObject *parent = NULL);

    Q_INVOKABLE void start();
    bool isActive();

signals:
    void started();
    void finished();
    void changedActive();
    void discovered(Device *device);

private slots:
    void btDiscovered(const QBluetoothDeviceInfo &);
    void btFinished();
    void udpHasData();

protected:

private:
    QBluetoothDeviceDiscoveryAgent *m_scanner_bt;
    QUdpSocket *m_scanner_udp;
};


#endif