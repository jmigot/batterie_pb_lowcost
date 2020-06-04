
/**
 * main function
 *
 * @date December 2010
 * @author Jean-Luc Migot (jl.migot@yahoo.fr)
 *
 * Copyright 2010-2011 Jean-Luc Migot
 */

#include "MainWindow.h"

#include <QApplication>
#include <QTextCodec>

int main(int argc, char* argv[])
{
	QApplication app(argc, argv);

	// QString must be UTF-8 (as all code is written in UTF-8 format)
	QTextCodec::setCodecForCStrings(QTextCodec::codecForName("UTF-8"));

	MainWindow w;

	return app.exec();
}
