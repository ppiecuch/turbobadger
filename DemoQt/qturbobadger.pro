TEMPLATE = app

CONFIG += c++11 console debug
CONFIG -= app_bundle

DESTDIR = build-$$[QMAKE_SPEC]

CONFIG(debug, debug|release) {
	DIR = $$DESTDIR/$${TARGET}.debug
} else {
	DIR = $$DESTDIR/$${TARGET}.release
}

OBJECTS_DIR = $$DIR
MOC_DIR = $$DIR
RCC_DIR = $$DIR
UI_DIR = $$DIR

RESOURCES += \
	res/fonts.qrc \
	res/images.qrc \
	res/skin.qrc \
	res/resources.qrc \
	res/ui_resources.qrc

debug: DEFINES += DEBUG

DEMOROOT = ../Demo

SOURCES += \
	$$DEMOROOT/platform/Application.cpp \
	$$DEMOROOT/platform/port_qt.cpp \
	$$DEMOROOT/demo01/Demo01.cpp \
	$$DEMOROOT/demo01/ListWindow.cpp \
	$$DEMOROOT/demo01/ResourceEditWindow.cpp

HEADERS += \
	$$DEMOROOT/platform/Application.h \
	$$DEMOROOT/demo01/Demo.h \
	$$DEMOROOT/demo01/ListWindow.h \
	$$DEMOROOT/demo01/ResourceEditWindow.h

INCLUDEPATH += $$DEMOROOT

include("qturbobadger.pri")