
TEMPLATE = app

QT += core qml quick widgets bluetooth

linux:!android {
	LIBS += -lbluetooth
	DEFINES += USE_BLUEZ
}

android {
	DEFINES += USE_ANDROID
}

SOURCES += main.cpp \
	Devices.cpp \
	DeviceStream.cpp \
	DeviceChannel.cpp \
	DeviceGroup.cpp

HEADERS += Devices.h DeviceStream.h DeviceChannel.h DeviceGroup.h

RESOURCES += multimeter.qrc

# Additional import path used to resolve QML modules in Qt Creator's code model
QML_IMPORT_PATH = 

CONFIG(debug, debug|release) {
	DESTDIR = build/debug
}
CONFIG(release, debug|release) {
	DESTDIR = build/release
}

OBJECTS_DIR = $$DESTDIR/.obj
MOC_DIR = $$DESTDIR/.moc
RCC_DIR = $$DESTDIR/.qrc
UI_DIR = $$DESTDIR/.u


DISTFILES += \
    android/AndroidManifest.xml \
    android/gradle/wrapper/gradle-wrapper.jar \
    android/gradlew \
    android/res/values/libs.xml \
    android/build.gradle \
    android/gradle/wrapper/gradle-wrapper.properties \
    android/gradlew.bat

ANDROID_PACKAGE_SOURCE_DIR = $$PWD/android
