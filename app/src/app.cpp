#include <QApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QFontDatabase>

#include "scan.h"
#include "device.h"


int main(int argc, char *argv[])
{
	QApplication app(argc, argv);
	QQmlApplicationEngine app_engine;

	QFont font("monospace", 10);
	font.setStyleHint(QFont::SansSerif);
	app.setFont(font);

	Scan scan;
	app_engine.rootContext()->setContextProperty("scan", &scan);

	app_engine.load(QUrl(QStringLiteral("qrc:/qml/app.qml")));
	return app.exec();
}
