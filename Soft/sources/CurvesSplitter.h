
/**
 * A new tab with curves
 *
 * @date December 2010
 * @author Jean-Luc Migot (jl.migot@yahoo.fr)
 *
 * Copyright 2010-2011 Jean-Luc Migot
 */

#ifndef CURVES_SPLITTER_H
#define CURVES_SPLITTER_H

#include <QSplitter>

class Graphic;
class MainWindow;

class QListWidget;

class CurvesSplitter: public QSplitter
{
	Q_OBJECT

	private:
		MainWindow*     _window;
		QListWidget*    _graphicListWidget;
		QList<QObject*> _graphicList;
		struct Config*  curvesConfig;
		QString         _data;

	public:
		CurvesSplitter(MainWindow *window, int nbData, const QString &data, bool saveToFile, const QString &immat, QVector<double> yValues, const QString &config = QString());
		~CurvesSplitter();

		QList<Graphic*> graphicList();
		void            print();
		void            exportPDF();
		void            print(QPrinter *printer, int borneInf, int borneSup) const;

	public slots:
		void switchCurve(int i);
};

#endif
