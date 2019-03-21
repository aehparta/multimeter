
#include <QApplication>
#include <QtQml>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QBluetoothServiceInfo>
#include <QBluetoothAddress>
#include <QBluetoothServiceDiscoveryAgent>
#include <QBluetoothDeviceDiscoveryAgent>
#include <QBluetoothLocalDevice>
#include <QBluetoothUuid>

#include "Devices.h"
#include "DeviceStream.h"


int main(int argc, char *argv[])
{
	QApplication app(argc, argv);
	QQmlApplicationEngine app_engine;

	// qmlRegisterType<Device>("Device", 1, 0, "Device");
	// DeviceStream *stream = new DeviceStream();
	// stream->list = &dataList;

	// QTimer *timer = new QTimer(stream);
	// stream->connect(timer, SIGNAL(timeout()), stream, SLOT(testSlot()));
	// timer->start(1000);

	QQmlContext *ctxt = app_engine.rootContext();

	QList<QObject *> empty;
	ctxt->setContextProperty("channelsModel", QVariant::fromValue(empty));

	Devices devices;
	devices.setRootContext(ctxt);
	ctxt->setContextProperty("devicesObject", &devices);

	app_engine.load(QUrl(QStringLiteral("qrc:/views/main.qml")));

	return app.exec();
}

