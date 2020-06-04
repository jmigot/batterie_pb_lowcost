# -------------------------------------------------
# Project created by QtCreator 2010-11-12T13:36:00
# -------------------------------------------------
QT += core \
	gui
TARGET = Soft
TEMPLATE = app

HEADERS += sources/MainWindow.h \
	sources/SerialPort.h \
	sources/CurvesSplitter.h \
	sources/Graphic.h \
	sources/Plot.h \
	sources/Dialog.h \
	sources/ManageSteps.h \
	sources/GraphicTimes.h \
	sources/PlotTimes.h \
	sources/Histogram.h
SOURCES += sources/MainWindow.cpp \
	sources/main.cpp \
	sources/SerialPort.cpp \
	sources/CurvesSplitter.cpp \
	sources/Graphic.cpp \
	sources/Plot.cpp \
	sources/Dialog.cpp \
	sources/ManageSteps.cpp \
	sources/GraphicTimes.cpp \
	sources/PlotTimes.cpp \
	sources/Histogram.cpp
LIBS += -lqextserialport
unix {
	DEFINES = _TTY_POSIX_
	LIBS += -lqwt
}
win32 {
	DEFINES = _TTY_WIN_
	LIBS += -lqwt5
}
TARGET = BPLC1210
RESOURCES += Resources.qrc
