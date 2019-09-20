#ifndef _CHANNEL_H_
#define _CHANNEL_H_

#include <QObject>
#include <QColor>
#include <QDebug>

class Channel : public QObject
{
	Q_OBJECT
	Q_PROPERTY(char id MEMBER m_id CONSTANT)
	Q_PROPERTY(QString name MEMBER m_name NOTIFY nameChanged)
	Q_PROPERTY(QString type MEMBER m_type NOTIFY typeChanged)
	Q_PROPERTY(QString mode MEMBER m_mode NOTIFY modeChanged)
	Q_PROPERTY(QString method MEMBER m_method NOTIFY methodChanged)
	Q_PROPERTY(char parent READ parentChannel WRITE setParentChannel NOTIFY parentChanged)
	Q_PROPERTY(QString value READ value WRITE setValue NOTIFY valueChanged)

	Q_PROPERTY(unsigned int base MEMBER m_base NOTIFY baseChanged)
	Q_PROPERTY(double multiplier MEMBER m_multiplier NOTIFY multiplierChanged)
	Q_PROPERTY(double resolution MEMBER m_resolution NOTIFY resolutionChanged)

	Q_PROPERTY(QStringList color MEMBER m_color NOTIFY colorChanged)

	Q_PROPERTY(bool enabled READ isEnabled WRITE setEnabled NOTIFY enabledChanged)

	Q_PROPERTY(bool valid READ isValid NOTIFY validChanged)
	Q_PROPERTY(QList<QObject *> children MEMBER m_children NOTIFY childrenChanged)

public:
	Channel(QObject *parent, char id);

	char id();
	char parentChannel();
	void setParentChannel(char parent_id);
	bool hasChild(char child_id);
	bool hasChild(Channel *channel);
	void addChild(Channel *channel);

	QString value();
	void setValue(QString value);

	bool isValid();
	void recv(const QString &value);
	void send(const QString &value);

	bool isEnabled();
	void setEnabled(bool value);
	
signals:
	void nameChanged();
	void typeChanged();
	void modeChanged();
	void methodChanged();
	void parentChanged();
	void valueChanged();

	void baseChanged();
	void multiplierChanged();
	void resolutionChanged();

	void colorChanged();

	void enabledChanged();

	void validChanged();
	void childrenChanged();

private slots:
	void checkValid();
	void deviceEnabled();

private:
	/* basic properties */
	char m_id;
	QString m_name;
	QString m_type;
	QString m_mode;
	QString m_method;
	char m_parent;
	QString m_value;

	/* number base (default is 16 (hex)) */
	unsigned int m_base;
	/* number multiplier */
	double m_multiplier;
	/* number resolution */
	double m_resolution;

	/* colors for rendering */
	QStringList m_color;

	/* device enabled */
	bool m_enabled;

	/* internals */
	bool m_wasValid;
	QList<QObject *> m_children;

};

#endif
