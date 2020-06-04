
/**
 * Times histogram class
 *
 * @date July 2011
 * @author Jean-Luc Migot (jl.migot@yahoo.fr)
 *
 * Copyright 2010-2011 Jean-Luc Migot
 */

#ifndef GRAPHIC_TIMES_H
#define GRAPHIC_TIMES_H

#include <QObject>
#include <QStringList>

#include <Qwt/qwt_scale_draw.h>

class Config;
class Histogram;
class MainWindow;

class PlotTimes;
class QwtPlot;
class QwtPlotCurve;

class GraphicTimes: public QObject
{
	private:
		PlotTimes*    _plot;
		Histogram*    _histogram;

	public:
		GraphicTimes(QObject *parent, MainWindow *window, const QVector<double> &yValues);
		~GraphicTimes();

		QwtPlot* plot() const;
};

class GraphicTimesScaleDraw: public QwtScaleDraw
{
	private:
		QStringList _axesList;

	public:
		GraphicTimesScaleDraw(QStringList axesList)
		{
			_axesList = axesList;
		}

		virtual QwtText label(double value) const
		{
			if ((int)value < _axesList.count())
				return _axesList[(int)value];

			return QString();
		}
};

#endif
