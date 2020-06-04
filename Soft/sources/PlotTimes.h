
/**
* A plot managment class with mouse movement detection
*
* @date December 2010
* @author Jean-Luc Migot (jl.migot@yahoo.fr)
*
* Copyright 2010-2011 Jean-Luc Migot
*/

#ifndef PLOT_TIMES_H
#define PLOT_TIMES_H

#include <QLabel>
#include <Qwt/qwt_plot.h>

class MainWindow;

class QwtPlotZoomer;

class PlotTimes: public QwtPlot
{
	private:
		MainWindow*    _window;
		QVector<double> _yValues;
		double _yMax;
		QLabel*        _coordinates;

	public:
		PlotTimes(MainWindow* window, const QString &title, const QVector<double> &yValues);
		~PlotTimes();

		void setItemColor(QColor color);
		void mouseReleaseEvent(QMouseEvent *event);
		bool eventFilter(QObject *obj, QEvent *event);
		void mouseMoveEvent(QMouseEvent *event);
		void updateCoordinates(QMouseEvent *event);
};

#endif
