
/**
 * main() file of the project
 *
 * @date December 2010
 * @author Jean-Luc Migot (jl.migot@yahoo.fr)
 *
 * Copyright 2010-2011 Jean-Luc Migot
 */

#include "MainWindow.h"

#include <QApplication>
#include <QTextCodec>
#include <QTranslator>

int main(int argc, char *argv[])
{
	QApplication app(argc, argv);

	app.setApplicationName(MainWindow::APPLICATION_NAME);

	// QString must be UTF-8 (as all code is written in UTF-8 format)
	QTextCodec::setCodecForCStrings(QTextCodec::codecForName("UTF-8"));

	QTranslator translator;
	translator.load(":/lang/qt_fr");
	app.installTranslator(&translator);

	new MainWindow();

	return app.exec();
}
