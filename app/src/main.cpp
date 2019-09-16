
#include <QApplication>
#include <QtQml>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QFontDatabase>

#include "Devices.h"
#include "DeviceStream.h"


int main(int argc, char *argv[])
{
	QCoreApplication::setOrganizationName("TL;DR");
	QCoreApplication::setOrganizationDomain("tldr.fi");
	QCoreApplication::setApplicationName("Multimeter");
	QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);

	QApplication app(argc, argv);
	// QFontDatabase::addApplicationFont(":/fa-solid-900.ttf");

	QFont font("monospace", 10);
	font.setStyleHint(QFont::SansSerif);
	app.setFont(font);

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

