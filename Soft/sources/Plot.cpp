
/**
 * A plot managment class with mouse movement detection
 *
 * @date December 2010
 * @author Jean-Luc Migot (jl.migot@yahoo.fr)
 *
 * Copyright 2010-2011 Jean-Luc Migot
 */

#include "Plot.h"
#include "MainWindow.h"

#include <Qwt/qwt_legend.h>
#include <Qwt/qwt_legend_item.h>
#include <Qwt/qwt_plot_layout.h>
#include <Qwt/qwt_plot_zoomer.h>

#include <QDebug>
#include <QLabel>
#include <QMouseEvent>

Plot::Plot(MainWindow *window, const QString &plotTitle, Config *config)
	: QwtPlot(plotTitle), _window(window)
{
	_xMax = 1000;
	_yMax = 50;
	_xUnits = " min";

	// Zoom
	new QwtPlotZoomer(this->canvas());

	// Enable mouse tracking
	this->setMouseTracking(true);
	this->canvas()->setMouseTracking(true);

	// Background-color
	this->setCanvasBackground(Qt::white);

	// Show axis
	this->setAxisTitle(this->xBottom, "Minutes");

	QString yAxisTitle;

	if (config->nbVoltages > 0)
		yAxisTitle.append(_window->color("Volts", Color::Blue));

	if (config->nbFlows > 0)
		yAxisTitle.append("/" + _window->color("Ampères", Color::Green));

	if (config->nbTemperatures > 0)
		yAxisTitle.append("/" + _window->color("Degrés", Color::Red));

	this->setAxisTitle(this->yLeft, yAxisTitle);

	// Legend
	if (config->nbVoltages + config->nbFlows + config->nbTemperatures > 1)
		this->insertLegend(new QwtLegend, this->RightLegend);

	// Label to show real-time coordinates
	_coordinates = new QLabel(this);
	_coordinates->hide();
	_coordinates->resize(200, 100);
	_coordinates->setStyleSheet("color: brown; background-color: white;");
	_coordinates->installEventFilter(this);

	this->replot();
}

Plot::~Plot()
{
	#ifndef QT_NO_DEBUG
		qDebug() << "~Plot()";
	#endif
}

void Plot::mouseReleaseEvent(QMouseEvent *event)
{
	// Zoom to base
	if (event->button() == Qt::RightButton)
	{
		event->ignore();
		this->setAxisScale(this->xBottom, 0, _xMax);
		this->setAxisScale(this->yLeft, 0, _yMax + (_yMax / 10));
		this->replot();
	}
}

// This function is useful to capture mouse events on the label which follows the mouse (else events aren't captured by QwtPlot)
bool Plot::eventFilter(QObject *obj, QEvent *event)
{
	if (dynamic_cast<QMouseEvent*>(event) != 0)
		this->updateCoordinates(dynamic_cast<QMouseEvent*>(event));

	return false;
}

void Plot::mouseMoveEvent(QMouseEvent *event)
{
	this->updateCoordinates(event);
}

void Plot::updateCoordinates(QMouseEvent *event)
{
	// Get the plot coordinates
	double x = invTransform(QwtPlot::xBottom, (event->pos()).x() - this->plotLayout()->canvasRect().left());
	double y = invTransform(QwtPlot::yLeft,   (event->pos()).y() - this->plotLayout()->canvasRect().top());

	// Set label text
	QString xS;

	// If units are days
	if (_xUnits == " jours")
		xS = QString("0" + QString::number((int)floor(x))).right(2) + "jr " + QString("0" + QString::number((int)((x - floor(x)) * 24.0))).right(2) + "h";

	// If units are hours
	else if (_xUnits == " heures")
		xS = QString("0" + QString::number((int)floor(x))).right(2) + "h " + QString("0" + QString::number((int)((x - floor(x)) * 60.0))).right(2) + "min";

	// if units are minutes
	else
		xS = QString::number(x, 'f', 0) + " minutes";

	_coordinates->setText("<b>" + xS + "</b><br />" +
						  "<b>Valeur : " + QString::number(y, 'f', 2) + "</b>" );

	// Move label
	_coordinates->adjustSize();
	_coordinates->move(event->pos().x() - (_coordinates->width() / 2), event->pos().y() + 10);

	if ( ! _coordinates->isVisible())
		_coordinates->show();
}

void Plot::setMaxs(int xMax, const QString &xUnits, const QString &xAxisTitle, int yMax)
{
	_xMax = xMax;
	_xUnits = xUnits;
	this->setAxisTitle(this->xBottom, xAxisTitle);
	this->setAxisScale(this->xBottom, 0, _xMax);

	_yMax = yMax;
	this->setAxisScale(this->yLeft, 0, _yMax + (_yMax / 10));
}
