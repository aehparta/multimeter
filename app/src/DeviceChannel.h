
#ifndef _DEVICE_CHANNEL_H_
#define _DEVICE_CHANNEL_H_

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

	Q_PROPERTY(DeviceChannel * this READ getThis)

	Q_PROPERTY(QString name READ getName WRITE setName NOTIFY nameChanged)
	Q_PROPERTY(bool isNameStatic READ isNameStatic)
	Q_PROPERTY(QVariant value READ getValue WRITE setValue NOTIFY valueChanged)

	Q_PROPERTY(QString type READ getType NOTIFY typeChanged)
	Q_PROPERTY(QString mode READ getMode NOTIFY modeChanged)
	Q_PROPERTY(QString method READ getMethod NOTIFY methodChanged)
	Q_PROPERTY(QString format READ getFormat NOTIFY formatChanged)
	Q_PROPERTY(double interval READ getInterval NOTIFY intervalChanged)
	Q_PROPERTY(double resolution READ getResolution NOTIFY resolutionChanged)
	Q_PROPERTY(double zero READ getZero NOTIFY zeroChanged)
	Q_PROPERTY(double divider READ getDivider NOTIFY dividerChanged)

	Q_PROPERTY(QStringList colors READ getColors NOTIFY colorsChanged)

	Q_PROPERTY(int parentChannel READ getParentChannel NOTIFY parentChannelChanged)
	Q_PROPERTY(DeviceChannel * firstChild READ getFirstChild)

	Q_PROPERTY(QList<QObject *> children READ getChildren)

public:
	DeviceChannel(int index, QObject *parent);

	int getIndex() const;

	bool isConnected();

	DeviceChannel *getThis();

	QString getName() const;
	void setName(const QString &name, bool isStatic = true);

	bool isNameStatic() const;

	QVariant getValue() const;
	void setValue(const QVariant &value);

	QString getType() const;
	void setType(const QString &value);
	QString getMode() const;
	void setMode(const QString &value);
	QString getMethod() const;
	void setMethod(const QString &value);
	QString getFormat() const;
	void setFormat(const QString &value);
	double getInterval() const;
	void setInterval(const double &value);
	double getResolution() const;
	void setResolution(const double &value);
	double getZero() const;
	void setZero(const double &value);
	double getDivider() const;
	void setDivider(const double &value);

	QStringList getColors() const;

	int getParentChannel() const;
	void setParentChannel(const QString data);
	bool hasValidParentChannel() const;

	DeviceChannel *getFirstChild() const;

	QList<QObject *> getChildren();
	QList<QObject *> getGroupChannels();

	bool receiveConfigValue(const QStringList data);
	bool receiveData(QString data);

	void stop();

	void addChildChannel(DeviceChannel *channel);

signals:
	void nameChanged();
	void valueChanged();

	void typeChanged();
	void modeChanged();
	void methodChanged();
	void formatChanged();
	void intervalChanged();
	void resolutionChanged();
	void zeroChanged();
	void dividerChanged();

	void colorsChanged();

	void parentChannelChanged();
	void childChannelAdded(DeviceChannel *channel);

	void valuesChanged();

public slots:
	void timedValuePull();

protected:

private:
	int index;
	int parentChannel;
	QList<QObject *> childChannels;

	QString name;
	bool nameStatic;
	QVariant value;

	/* default is true rms */
	QString averaging;

	QString type;
	QString mode;
	QString method;
	QString format;
	double interval;
	double resolution;
	double zero;
	double divider;

	QStringList colors;

	QList<double> samples;

	QTimer timer;
};

#endif
