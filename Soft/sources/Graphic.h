
/**
 * Graphic class
 *
 * @date December 2010
 * @author Jean-Luc Migot (jl.migot@yahoo.fr)
 *
 * Copyright 2010-2011 Jean-Luc Migot
 */

#ifndef GRAPHIC_H
#define GRAPHIC_H

#include <QObject>
#include <QVector>

class MainWindow;
class Plot;

struct Config;

class QwtPlot;
class QwtPlotCurve;

class Graphic: public QObject
{
	private:
		MainWindow*              _window;
		Plot*                    _plot;
		QList<QwtPlotCurve*>     _voltageCurves;
		QList<QwtPlotCurve*>     _flowCurves;
		QList<QwtPlotCurve*>     _temperatureCurves;
		Config*                  _config;
		QVector<double>          _xValues;
		QList< QVector<double> > _voltageValues;
		QList< QVector<double> > _flowValues;
		QList< QVector<double> > _temperaturesValues;

	public:
		Graphic(QObject *parent, MainWindow *window, const QString &plotTitle, Config *config);
		~Graphic();

		void setData(QVector<double> xValues,
					 const QList< QVector<double> > &voltageValues,
					 const QList< QVector<double> > &flowValues,
					 const QList< QVector<double> > &temperaturesValues);

		QVector<double>          xValues() const;
		QList< QVector<double> > voltageValues() const;
		QList< QVector<double> > flowValues() const;
		QList< QVector<double> > temperaturesValues() const;

		QwtPlot* plot() const;
};

#endif
