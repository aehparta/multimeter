
TEMPLATE = app

QT += core qml quick widgets bluetooth

linux:!android {
}

android {
	DEFINES += USE_ANDROID
}

SOURCES += src/app.cpp src/device.cpp src/scan.cpp

HEADERS += src/device.h src/scan.h

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
    android/gradlew.bat \
    android/AndroidManifest.xml \
    android/gradle/wrapper/gradle-wrapper.jar \
    android/gradlew \
    android/res/values/libs.xml \
    android/build.gradle \
    android/gradle/wrapper/gradle-wrapper.properties \
    android/gradlew.bat \
    android/AndroidManifest.xml \
    android/gradle/wrapper/gradle-wrapper.jar \
    android/gradlew \
    android/res/values/libs.xml \
    android/build.gradle \
    android/gradle/wrapper/gradle-wrapper.properties \
    android/gradlew.bat

ANDROID_PACKAGE_SOURCE_DIR = $$PWD/android
