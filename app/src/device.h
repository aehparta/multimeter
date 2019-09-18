#ifndef _DEVICE_H_
#define _DEVICE_H_

#include <QTcpSocket>
#include <QBluetoothSocket>

class Device : public QObject
{
	Q_OBJECT

public:
	Device(QObject *parent, QString address, int port = -1);

	Q_INVOKABLE QString id();
	Q_INVOKABLE QString address();
	Q_INVOKABLE int port();

	Q_INVOKABLE void start();
	Q_INVOKABLE void stop();
	Q_INVOKABLE void send(const QString &data);

signals:
	void connected();
	void disconnected();
	void error();
	void receive(QString data);

private slots:
	void connectionReady();
	void connectionError(QBluetoothSocket::SocketError error);
	void readReady();

private:
	QTcpSocket *m_socket_tcp;
	QBluetoothSocket *m_socket_bt;
	QString m_address;
	int m_port;
};

#endif
