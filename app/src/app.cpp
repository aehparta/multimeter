#include <QApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QFontDatabase>

#include "scan.h"
#include "device.h"


int main(int argc, char *argv[])
{
	QCoreApplication::setOrganizationName("tldr.fi");
	QCoreApplication::setOrganizationDomain("tldr.fi");
	QCoreApplication::setApplicationName("Multimeter");
	QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);

	QApplication app(argc, argv);
	QQmlApplicationEngine app_engine;

	QFontDatabase::addApplicationFont(":/fonts/fa-solid-900.ttf");

	QFont font("monospace", 10);
	font.setStyleHint(QFont::SansSerif);
	app.setFont(font);
	
	Scan scan;
	app_engine.rootContext()->setContextProperty("scan", &scan);

	app_engine.load(QUrl(QStringLiteral("qrc:/qml/app.qml")));
	return app.exec();
}
