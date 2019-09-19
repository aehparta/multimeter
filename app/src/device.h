#ifndef _DEVICE_H_
#define _DEVICE_H_

#include <QTcpSocket>
#include <QBluetoothSocket>
#include "channel.h"

class Device : public QObject
{
	Q_OBJECT
	Q_PROPERTY(QString name MEMBER m_name NOTIFY nameChanged)
	Q_PROPERTY(bool enabled MEMBER m_enabled NOTIFY enabledChanged)
	Q_PROPERTY(QList<QObject *> channels READ channels NOTIFY channelsChanged)

public:
	Device(QObject *parent, QString address, int port = -1);

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
	void connectionError(QBluetoothSocket::SocketError error);
	void readReady();

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

	/* receive data */
	void recv(const QString &data);
};

#endif
