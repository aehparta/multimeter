#ifndef _CHANNEL_H_
#define _CHANNEL_H_

#include <QObject>
#include <QDebug>

class Channel : public QObject
{
	Q_OBJECT
	Q_PROPERTY(QString name MEMBER m_name NOTIFY nameChanged)
	Q_PROPERTY(QString type MEMBER m_type NOTIFY typeChanged)
	Q_PROPERTY(QString mode MEMBER m_mode NOTIFY modeChanged)
	Q_PROPERTY(QString method MEMBER m_method NOTIFY methodChanged)
	Q_PROPERTY(QString value MEMBER m_value NOTIFY valueChanged)

	Q_PROPERTY(unsigned int base MEMBER m_base NOTIFY baseChanged)
	Q_PROPERTY(double multiplier MEMBER m_multiplier NOTIFY multiplierChanged)
	Q_PROPERTY(double resolution MEMBER m_resolution NOTIFY resolutionChanged)

	Q_PROPERTY(bool enabled READ isEnabled WRITE setEnabled NOTIFY enabledChanged)

	Q_PROPERTY(bool valid READ isValid NOTIFY validChanged)
	Q_PROPERTY(QList<QObject *> children MEMBER m_children NOTIFY childrenChanged)

public:
	Channel(QObject *parent = NULL);
	bool isValid();
	void recv(QString data);

	bool isEnabled();
	void setEnabled(bool value);
	
signals:
	void nameChanged();
	void typeChanged();
	void modeChanged();
	void methodChanged();
	void valueChanged();

	void baseChanged();
	void multiplierChanged();
	void resolutionChanged();

	void enabledChanged();

	void validChanged();
	void childrenChanged();

private slots:
	void checkValid();
	void deviceEnabled();

private:
	/* basic properties */
	QString m_name;
	QString m_type;
	QString m_mode;
	QString m_method;
	QString m_value;
	/* number base (default is 16 (hex)) */
	unsigned int m_base;
	/* number multiplier */
	double m_multiplier;
	/* number resolution */
	double m_resolution;

	/* device enabled */
	bool m_enabled;

	/* internals */
	bool m_wasValid;
	QList<QObject *> m_children;

};

#endif
