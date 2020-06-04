#-------------------------------------------------
#
# Project created by QtCreator 2010-11-11T11:43:10
#
#-------------------------------------------------

QT       += core gui

TARGET = Configuration
TEMPLATE = app

HEADERS += \
	sources/MainWindow.h \
	sources/SerialPort.h

SOURCES += \
	sources/SerialPort.cpp \
	sources/MainWindow.cpp \
	sources/main.cpp
LIBS += -lqextserialport
unix {
	DEFINES = _TTY_POSIX_
}
win32 {
	DEFINES = _TTY_WIN_
}
TARGET = BPLC1210_Config

RESOURCES += \
	Resources.qrc
