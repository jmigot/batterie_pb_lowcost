
/**
* A plot managment class with mouse movement detection
*
* @date December 2010
* @author Jean-Luc Migot (jl.migot@yahoo.fr)
*
* Copyright 2010-2011 Jean-Luc Migot
*/

#ifndef PLOT_H
#define PLOT_H

#include <Qwt/qwt_plot.h>

class MainWindow;

struct Config;

class QwtPlotZoomer;

class QLabel;

class Plot: public QwtPlot
{
	private:
		MainWindow*    _window;
		QLabel*        _coordinates;
		int            _xMax;
		int            _yMax;
		int            _xStep;
		int            _yStep;
		QString        _xUnits;

	public:
		Plot(MainWindow *window, const QString &title, Config *config);
		~Plot();

		void mouseReleaseEvent(QMouseEvent *event);
		bool eventFilter(QObject *obj, QEvent *event);
		void mouseMoveEvent(QMouseEvent *event);
		void updateCoordinates(QMouseEvent *event);
		void setMaxs(int xMax, const QString &xUnits, const QString &xAxisTitle, int yMax);
};

#endif
