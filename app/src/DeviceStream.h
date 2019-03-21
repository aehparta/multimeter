
#ifndef _DEVICE_STREAM_H_
#define _DEVICE_STREAM_H_

#include <QDebug>
#include <QObject>
#include <QAbstractListModel>
#include <QQmlListProperty>
#include <QBluetoothSocket>
#include <QSocketNotifier>
#include <QBluetoothServiceInfo>
#include <QBluetoothDeviceInfo>
#include <QDateTime>

#include "DeviceChannel.h"


class DeviceStream : public QObject
{
	Q_OBJECT

	Q_PROPERTY(QString address READ address WRITE setAddress NOTIFY addressChanged)
	Q_PROPERTY(QString stringData READ stringData WRITE setStringData NOTIFY stringDataChanged)

public:
	int start();
	void stop();

	DeviceStream(QObject *parent = NULL);

	QString address() const;
	void setAddress(const QString &address);

	QString stringData();
	void setStringData(const QString &data);

	DeviceChannel *getChannel(int id, bool create);
	QList<QObject *> getChannels();
	QList<DeviceChannel *> getChannelsByParent(int parent);

	bool receiveData(QString data);

signals:
	void connected();
	void disconnected();

	void addressChanged();
	void typeChanged();
	void labelChanged();
	void stringDataChanged();

public slots:
	void connectionReady();
	void connectionError(QBluetoothSocket::SocketError);
	void dataReadReady();
	void reconnectTry();
	void waitConfigCheck();

protected:

private:
	QList<DeviceChannel *> channels;

	bool m_connected;
	QString m_address;

	QList<QString> m_stringDataWrite;
	QList<QString> m_stringDataRead;
	QString m_stringDataReadBuffer;

	QBluetoothSocket *m_socket;

	/* connection error handling */
	static const int reconnectCountMax = 5;
	static const int waitConfigCountMax = 3;
	int reconnectCount = 0;
	QTimer reconnectTimer;
	int waitConfigCount;
	QTimer waitConfigTimer;
};

#endif
