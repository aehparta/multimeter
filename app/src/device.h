#ifndef _DEVICE_H_
#define _DEVICE_H_

#include <QTcpSocket>
#include <QBluetoothSocket>
#include <QTimer>
#include <QSettings>
#include <QCryptographicHash>
#include <QHostAddress>
#include "channel.h"

class Device : public QObject
{
	Q_OBJECT
	Q_PROPERTY(QString name MEMBER m_name NOTIFY nameChanged)
	Q_PROPERTY(bool enabled MEMBER m_enabled NOTIFY enabledChanged)
	Q_PROPERTY(QList<QObject *> channels READ channels NOTIFY channelsChanged)

public:
	Device(QObject *parent, bool enabled, QString address, int port = -1);
	Device(QObject *parent, QString settings_group);
	~Device();

	QString name();
	Q_INVOKABLE QString id();
	Q_INVOKABLE QString address();
	Q_INVOKABLE int port();

	Q_INVOKABLE void start();
	Q_INVOKABLE void stop();
	Q_INVOKABLE void send(const QString &data);

	QList<QObject *> channels();

signals:
	void nameChanged();
	void enabledChanged();

	void connected();
	void disconnected();
	void error();
	void receive(QString data);
	void channelsChanged();

private slots:
	void connectionReady();
	void btConnectionError(QBluetoothSocket::SocketError);
	void tcpConnectionError(QAbstractSocket::SocketError);
	void readReady();
	void channelChanged();
	void getConfig();
	
private:
	/* device name, if given */
	QString m_name;
	/* device enabled */
	bool m_enabled;

	/* tcp network socket */
	QTcpSocket *m_socket_tcp;
	/* bluetooth socket */
	QBluetoothSocket *m_socket_bt;
	/* device address */
	QString m_address;
	/* device port, -1 if not used */
	int m_port;
	/* channels */
	QMap<char, QObject *> m_channels;

	/* main timer to be used in different states */
	QTimer timer;

	/* receive data */
	void recv(const QString &data);
};

#endif
