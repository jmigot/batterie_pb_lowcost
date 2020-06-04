
/**
 * A plot managment class with mouse movement detection
 *
 * @date December 2010
 * @author Jean-Luc Migot (jl.migot@yahoo.fr)
 *
 * Copyright 2010-2011 Jean-Luc Migot
 */

#include "PlotTimes.h"
#include "MainWindow.h"

#include <Qwt/qwt_plot_zoomer.h>
#include <Qwt/qwt_array.h>
#include <Qwt/qwt_double_interval.h>
#include <Qwt/qwt_plot_layout.h>

#include <QDebug>
#include <QMouseEvent>

PlotTimes::PlotTimes(MainWindow *window, const QString &plotTitle, const QVector<double> &yValues)
	: QwtPlot(plotTitle), _window(window), _yValues(yValues)
{
	_yMax = 0.0;

	for (int i = 0; i < 8; i++)
		if (yValues.at(i) > _yMax)
			_yMax = yValues.at(i);

	this->setAxisScale(QwtPlot::xBottom, 0, 8);
	this->setAxisScale(QwtPlot::yLeft, 0, _yMax + (_yMax / 10.0));

	// Enable mouse tracking
	this->setMouseTracking(true);
	this->canvas()->setMouseTracking(true);

	// Zoom
	new QwtPlotZoomer(this->canvas());

	// Background-color
	this->setCanvasBackground(Qt::white);

	// Show axis
	this->setAxisTitle(this->xBottom, "Degrés celsius");
	this->setAxisTitle(this->yLeft, window->color("Nombre d'heures", Color::Blue));

	// Label to show real-time coordinates
	_coordinates = new QLabel(this);
	_coordinates->hide();
	_coordinates->resize(200, 100);
	_coordinates->setStyleSheet("color: brown; background-color: white;");
	_coordinates->installEventFilter(this);

	this->replot();
}

PlotTimes::~PlotTimes()
{
	#ifndef QT_NO_DEBUG
		qDebug() << "~PlotTimes()";
	#endif
}

void PlotTimes::mouseReleaseEvent(QMouseEvent *event)
{
	// Zoom to base
	if (event->button() == Qt::RightButton)
	{
		event->ignore();
		this->setAxisScale(this->xBottom, 0, 8);
		this->setAxisScale(this->yLeft, 0, _yMax + (_yMax / 10.0));
		this->replot();
	}

	else
		event->accept();
}

// This function is useful to capture mouse events on the label which follows the mouse (else events aren't captured by QwtPlot)
bool PlotTimes::eventFilter(QObject *obj, QEvent *event)
{
	if (dynamic_cast<QMouseEvent*>(event) != 0)
		this->updateCoordinates(dynamic_cast<QMouseEvent*>(event));

	 return false;
}

void PlotTimes::mouseMoveEvent(QMouseEvent *event)
{
	this->updateCoordinates(event);
}

void PlotTimes::updateCoordinates(QMouseEvent *event)
{
	// Get the plot coordinates
	double x = invTransform(QwtPlot::xBottom, event->pos().x()- plotLayout()->canvasRect().x());

	// Set label text
	QString xS;

	if (x < 1.0)
		xS = "&lt; 5°C";

	else if (x < 2.0)
		xS = "5 à 15°C";

	else if (x < 3.0)
		xS = "15 à 25°C";

	else if (x < 4.0)
		xS = "25 à 35°C";

	else if (x < 5.0)
		xS = "35 à 45°C";

	else if (x < 6.0)
		xS = "45 à 55°C";

	else if (x < 7.0)
		xS = "55 à 65°C";

	else
		xS = "&gt; 65°C";

	int y = _yValues.at((int)x);

	_coordinates->setText("<b>" + xS + "</b><br />" +
						  "<b>" + QString::number(y) + (y > 0 ? " heures" : " heure") + "</b>");

	// Move label
	_coordinates->adjustSize();
	_coordinates->move(event->pos().x() - (_coordinates->width() / 2), event->pos().y() + 10);

	if ( ! _coordinates->isVisible())
		_coordinates->show();
}
