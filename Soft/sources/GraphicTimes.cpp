
/**
 * Times histogram class
 *
 * @date July 2010
 * @author Jean-Luc Migot (jl.migot@yahoo.fr)
 *
 * Copyright 2010-2011 Jean-Luc Migot
 */

#include "GraphicTimes.h"
#include "MainWindow.h"
#include "PlotTimes.h"
#include "Histogram.h"

#include <Qwt/qwt_plot_curve.h>
#include <Qwt/qwt_plot_grid.h>
#include <Qwt/qwt_symbol.h>
#include <Qwt/qwt_curve_fitter.h>

#include <QDebug>
#include <QPen>

GraphicTimes::GraphicTimes(QObject *parent, MainWindow *window, const QVector<double> &yValues)
	: QObject(parent)
{
	_plot       = new PlotTimes(window, "Températures", yValues);
	_histogram  = new Histogram("Températures");

	QVector<double> xValues;
	xValues << 0 << 1 << 2 << 3 << 4 << 5 << 6 << 7;

	QStringList axesList;
	axesList << "&lt;" << "5" << "15" << "25" << "35" << "45" << "55" << "65" << ">";
	_plot->setAxisScaleDraw(QwtPlot::xBottom, new GraphicTimesScaleDraw(axesList));

	_histogram->setData(yValues);
	_histogram->attach(_plot);

	QPen pen(QColor(0, 0, 0, 50));
	pen.setWidth(1);

	QwtPlotGrid* grid = new QwtPlotGrid;
	grid->setPen(pen);
	grid->attach(_plot);

	_plot->replot();
}

GraphicTimes::~GraphicTimes()
{
	#ifndef QT_NO_DEBUG
		qDebug() << "~GraphicTimes()";
	#endif

	if (_plot != NULL)
		delete _plot;
}

QwtPlot* GraphicTimes::plot() const
{
	return _plot;
}
