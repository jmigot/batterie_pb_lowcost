
/**
 * Graphic class
 *
 * @date December 2010
 * @author Jean-Luc Migot (jl.migot@yahoo.fr)
 *
 * Copyright 2010-2011 Jean-Luc Migot
 */

#include "Graphic.h"
#include "MainWindow.h"
#include "Plot.h"

#include <Qwt/qwt_plot_curve.h>
#include <Qwt/qwt_plot_grid.h>
#include <Qwt/qwt_symbol.h>

#include <QDebug>

Graphic::Graphic(QObject *parent, MainWindow *window, const QString &plotTitle, Config *config)
	: QObject(parent), _window(window), _config(config)
{
	_plot = new Plot(_window, plotTitle, _config);

	QwtSymbol symbol;
	symbol.setStyle(QwtSymbol::Rect);
	symbol.setSize(1, 1);

	QPen pen;
	pen.setWidth(2);

	for (int i = 0; i < 40; i++)
	{
		pen.setColor(QColor(_config->voltageColors.at(i)));

		_voltageCurves.append(new QwtPlotCurve("Tension " + QString::number(i + 1)));
		_voltageCurves.at(i)->setSymbol(symbol);
		_voltageCurves.at(i)->setPen(pen);

		if (_config->voltages[i] > 0)
			_voltageCurves.at(i)->attach(_plot);
	}

	for (int i = 0; i < 8; i++)
	{
		pen.setColor(_config->flowColors.at(i));

		_flowCurves.append(new QwtPlotCurve("Courant " + QString::number(i + 1)));
		_flowCurves.at(i)->setSymbol(symbol);
		_flowCurves.at(i)->setPen(pen);

		if (_config->flows[i] > 0)
			_flowCurves.at(i)->attach(_plot);
	}

	for (int i = 0; i < 8; i++)
	{
		pen.setColor(_config->temperatureColors.at(i));

		_temperatureCurves.append(new QwtPlotCurve("Température " + QString::number(i + 1)));
		_temperatureCurves.at(i)->setSymbol(symbol);
		_temperatureCurves.at(i)->setPen(pen);

		if (_config->temperatures[i] > 0)
			_temperatureCurves.at(i)->attach(_plot);
	}

	pen.setWidth(1);
	pen.setColor(QColor(0, 0, 0, 50));

	QwtPlotGrid* grid = new QwtPlotGrid;
	grid->setPen(pen);
	grid->attach(_plot);

	_plot->replot();
}

Graphic::~Graphic()
{
	#ifndef QT_NO_DEBUG
		qDebug() << "~Graphic()";
	#endif

	if (_plot != NULL)
		delete _plot;
}

void Graphic::setData(QVector<double> xValues,
					  const QList< QVector<double> > &voltageValues,
					  const QList< QVector<double> > &flowValues,
					  const QList< QVector<double> > &temperatureValues)
{
	_xValues            = xValues;
	_voltageValues      = voltageValues;
	_flowValues         = flowValues;
	_temperaturesValues = temperatureValues;

	QString xUnits(" minutes");
	QString xAxisTitle("Minutes");

	// If x values should be days
	if (xValues.count() > 1410)
	{
		xUnits = " jours";
		xAxisTitle = "Jours";

		for (int i = 0; i < xValues.count(); i++)
			xValues[i] = xValues[i] / 1440;
	}

	// If x values should be hours
	else if (xValues.count() > 60)
	{
		xUnits = " heures";
		xAxisTitle = "Heures";

		for (int i = 0; i < xValues.count(); i++)
			xValues[i] = xValues[i] / 60;
	}

	double xMax = 0;
	double yMax = 0;

	foreach (double value, xValues)
		if (value > xMax)
			xMax = value;

	for (int i = 0; i < 40; i++)
		if (_config->voltages[i] > 0)
			foreach (double value, voltageValues[i])
				if (value > yMax)
					yMax = value;

	for (int i = 0; i < 8; i++)
		if (_config->flows[i] > 0)
			foreach (double value, flowValues[i])
				if (value > yMax)
					yMax = value;

	for (int i = 0; i < 8; i++)
		if (_config->temperatures[i] > 0)
			foreach (double value, temperatureValues[i])
				if (value > yMax)
					yMax = value;

	_plot->setMaxs(ceil(xMax), xUnits, xAxisTitle, ceil(yMax));

	for (int i = 0; i < 40; i++)
		if (_config->voltages[i] > 0)
			_voltageCurves[i]->setData(xValues, voltageValues[i]);

	for (int i = 0; i < 8; i++)
		if (_config->flows[i] > 0)
			_flowCurves[i]->setData(xValues, flowValues[i]);

	for (int i = 0; i < 8; i++)
		if (_config->temperatures[i] > 0)
			_temperatureCurves[i]->setData(xValues, temperatureValues[i]);

	_plot->replot();
}

QVector<double> Graphic::xValues() const
{
	return _xValues;
}

QList< QVector<double> > Graphic::voltageValues() const
{
	return _voltageValues;
}

QList< QVector<double> > Graphic::flowValues() const
{
	return _flowValues;
}

QList< QVector<double> > Graphic::temperaturesValues() const
{
	return _temperaturesValues;
}

QwtPlot* Graphic::plot() const
{
	return _plot;
}
