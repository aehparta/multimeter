#include <QApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QFontDatabase>
#include <QCommandLineParser>

#include "scan.h"
#include "device.h"


QCommandLineOption opt_fullscreen("f", "Start as fullscreen");
QCommandLineOption opt_bt_disable("B", "Disable Bluetooth discovery");
QCommandLineOption opt_udp_disable("U", "Disable UDP discovery");

QString os()
{
#if defined(Q_OS_ANDROID)
	return QString("android");
#elif defined(Q_OS_IOS)
	return QString("ios");
#elif defined(Q_OS_MACOS)
	return QString("macos");
#elif defined(Q_OS_WIN)
	return QString("windows");
#elif defined(Q_OS_LINUX)
	return QString("linux");
#elif defined(Q_OS_UNIX)
	return QString("unix");
#else
	return QString("unknown");
#endif
}
 
int main(int argc, char *argv[])
{
	QApplication app(argc, argv);
	QQmlApplicationEngine engine;

	/* setup app information */
	QCoreApplication::setOrganizationName("tldr.fi");
	QCoreApplication::setOrganizationDomain("tldr.fi");
	QCoreApplication::setApplicationName("Multimeter");
	QCoreApplication::setApplicationVersion("2.0.0");
	//QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);

	/* command line options */
	QCommandLineParser parser;
	parser.setApplicationDescription("Multimeter application");
	parser.addHelpOption();
	parser.addVersionOption();

	/* add options */
	parser.addOption(opt_fullscreen);
	parser.addOption(opt_bt_disable);
	parser.addOption(opt_udp_disable);

	/* parse options */
	parser.process(app);
	const QStringList args = parser.positionalArguments();

	/* set fonts */
	QFont font("monospace", 10);
	font.setStyleHint(QFont::SansSerif);
	app.setFont(font);
	QFontDatabase::addApplicationFont(":/fonts/fa-solid-900.ttf");

	/* export scanner to qml */
	Scan scan;
	scan.btEnable(!parser.isSet(opt_bt_disable));
	scan.udpEnable(!parser.isSet(opt_udp_disable));
	engine.rootContext()->setContextProperty("scan", &scan);

	/* export platform to qml */
	engine.rootContext()->setContextProperty("os", os());

	/* export options to qml */
	engine.rootContext()->setContextProperty("fullscreen", parser.isSet(opt_fullscreen));

	/* run application */
	engine.load(QUrl(QStringLiteral("qrc:/qml/app.qml")));
	return app.exec();
}
