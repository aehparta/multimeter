
#ifndef __DEVICE_CHANNEL_H__
#define __DEVICE_CHANNEL_H__

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


class DeviceChannel : public QObject
{
	Q_OBJECT

	Q_PROPERTY(DeviceChannel * chThis READ chGetThis)

	Q_PROPERTY(QString chName READ chGetName WRITE chSetName NOTIFY chNameChanged)
	Q_PROPERTY(bool chIsNameStatic READ chGetNameStatic)
	Q_PROPERTY(QVariant chValue READ chGetValue WRITE chSetValue NOTIFY chValueChanged)

	Q_PROPERTY(QString chType READ chGetType NOTIFY chTypeChanged)
	Q_PROPERTY(QString chMode READ chGetMode NOTIFY chModeChanged)
	Q_PROPERTY(QString chMethod READ chGetMethod NOTIFY chMethodChanged)
	Q_PROPERTY(QString chFormat READ chGetFormat NOTIFY chFormatChanged)
	Q_PROPERTY(double chInterval READ chGetInterval NOTIFY chIntervalChanged)
	Q_PROPERTY(double chResolution READ chGetResolution NOTIFY chResolutionChanged)
	Q_PROPERTY(double chZero READ chGetZero NOTIFY chZeroChanged)
	Q_PROPERTY(double chDivider READ chGetDivider NOTIFY chDividerChanged)

	Q_PROPERTY(QStringList chColors READ chGetColors NOTIFY chColorsChanged)

	Q_PROPERTY(int chParentChannel READ chGetParentChannel NOTIFY chParentChannelChanged)
	Q_PROPERTY(DeviceChannel * chFirstChild READ chGetFirstChild)

	Q_PROPERTY(QList<QObject *> chChildren READ chGetChildren)

public:
	DeviceChannel(int index, QObject *parent);

	int chGetIndex() const;

	bool isConnected();

	DeviceChannel *chGetThis();

	QString chGetName() const;
	void chSetName(const QString &name, bool isStatic = true);

	bool chGetNameStatic() const;

	QVariant chGetValue() const;
	void chSetValue(const QVariant &value);

	QString chGetType() const;
	void chSetType(const QString &value);
	QString chGetMode() const;
	void chSetMode(const QString &value);
	QString chGetMethod() const;
	void chSetMethod(const QString &value);
	QString chGetFormat() const;
	void chSetFormat(const QString &value);
	double chGetInterval() const;
	void chSetInterval(const double &value);
	double chGetResolution() const;
	void chSetResolution(const double &value);
	double chGetZero() const;
	void chSetZero(const double &value);
	double chGetDivider() const;
	void chSetDivider(const double &value);

	QStringList chGetColors() const;

	int chGetParentChannel() const;
	void chSetParentChannel(const QString data);
	bool chHasValidParentChannel() const;

	DeviceChannel *chGetFirstChild() const;

	QList<QObject *> chGetChildren();
	QList<QObject *> getGroupChannels();

	bool receiveConfigValue(const QStringList data);
	bool receiveData(QString data);

	void stop();

	void addChildChannel(DeviceChannel *channel);

signals:
	void chNameChanged();
	void chValueChanged();

	void chTypeChanged();
	void chModeChanged();
	void chMethodChanged();
	void chFormatChanged();
	void chIntervalChanged();
	void chResolutionChanged();
	void chZeroChanged();
	void chDividerChanged();

	void chColorsChanged();

	void chParentChannelChanged();
	void chChildChannelAdded(DeviceChannel *channel);

	void chValuesChanged();

public slots:
	void timedValuePull();

protected:

private:
	int chIndex;
	int chParentChannel;
	QList<QObject *> chChildChannels;

	QString chName;
	bool chIsNameStatic;
	QVariant chValue;

	/* default is true rms */
	QString chAveraging;

	QString chType;
	QString chMode;
	QString chMethod;
	QString chFormat;
	double chInterval;
	double chResolution;
	double chZero;
	double chDivider;

	QStringList chColors;

	QList<qint64> samples;

	QTimer timer;
};

// "config:channel:A:static:name:Ihmevekotin"
// "config:channel:A:type:datetime"
// "config:channel:A:mode:source"
// "config:channel:A:method:pull"
// "config:channel:B:static:name:Ihmevekotin-sink"
// "config:channel:B:type:datetime"
// "config:channel:B:mode:sink"

#endif
