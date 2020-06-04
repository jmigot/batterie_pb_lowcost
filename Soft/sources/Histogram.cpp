
/**
 * Times histogram class
 *
 * @date July 2010
 * @author Jean-Luc Migot (jl.migot@yahoo.fr)
 *
 * Copyright 2010-2011 Jean-Luc Migot
 *
 * ----------------------------------
 * Adapted from QwtRegularHistogram.h
 *
 * Qwt Widget Library
 * Copyright (C) 1997   Josef Wilgen
 * Copyright (C) 2002   Uwe Rathmann
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the Qwt License, Version 1.0
 * -------------------------------------------------------------
 */

#include "Histogram.h"

#include <QColor>
#include <QPen>
#include <QPainter>
#include <QDebug>

#include <Qwt/qwt_plot.h>
#include <Qwt/qwt_plot_grid.h>
#include <Qwt/qwt_interval_data.h>
#include <Qwt/qwt_plot_item.h>
#include <Qwt/qwt_painter.h>
#include <Qwt/qwt_plot_zoomer.h>

Histogram::Histogram(const QString &title)
	: QwtPlotItem(title)
{
	_color = QColor(0, 0, 128, 127);
	_reference = 0.0;
	this->setZ(20.0);
}

Histogram::~Histogram()
{
	#ifndef QT_NO_DEBUG
		qDebug() << "~Histogram()";
	#endif
}

void Histogram::setData(const QVector<double> &data)
{
	QwtArray<QwtDoubleInterval> intervals(data.size());

	for (int i=0; i<data.size(); i++)
		intervals[i] = QwtDoubleInterval(i, i+1);

	_data = QwtIntervalData(intervals, data);
	this->itemChanged();
}

QwtDoubleRect Histogram::boundingRect() const
{
	QwtDoubleRect rect = _data.boundingRect();

	if ( ! rect.isValid())
		return rect;

	if ( rect.bottom() < _reference )
		rect.setBottom(_reference);

	else if ( rect.top() > _reference)
		rect.setTop(_reference);

	return rect;
}

int Histogram::rtti() const
{
	return QwtPlotItem::Rtti_PlotHistogram;
}

void Histogram::draw(QPainter *painter, const QwtScaleMap &xMap, const QwtScaleMap &yMap, const QRect &) const
{
	painter->setPen(QPen(_color));
	const int y0 = yMap.transform(_reference);

	for (int i = 0; i < (int)_data.size(); i++)
	{
		const int y2 = yMap.transform(_data.value(i));

		// Si valeur vaut 0, on passe à la suivante
		if (_data.value(i) == 0)
			continue;

		int x1 = xMap.transform(_data.interval(i).minValue());
		int x2 = xMap.transform(_data.interval(i).maxValue());

		// On inverse x1 et x2 pour que x2 > x1
		if (x1 > x2)
			qSwap(x1, x2);

		if (i < (int)_data.size() - 2)
		{
			const int xx1 = xMap.transform(_data.interval(i+1).minValue());
			const int xx2 = xMap.transform(_data.interval(i+1).maxValue());

			if (x2 == qwtMin(xx1, xx2))
			{
				const int yy2 = yMap.transform(_data.value(i+1));

				// One pixel distance between neighboured bars
				if (yy2 != y0 && ((yy2 < y0 && y2 < y0) || (yy2 > y0 && y2 > y0)))
					x2--;
			}
		}

		// On dessine la barre
		drawBar(painter, Qt::Vertical, QRect(x1, y0, x2 - x1, y2 - y0));
	}
}

void Histogram::drawBar(QPainter *painter,Qt::Orientation, const QRect &rect) const
{
   painter->save();

   const QColor color(painter->pen().color());
   const QRect r = rect.normalized();
   const int factor = 125;
   const QColor light(color.light(factor));
   const QColor dark(color.dark(factor));

   painter->setBrush(color);
   painter->setPen(Qt::NoPen);
   QwtPainter::drawRect(painter, r.x() + 1, r.y() + 1, r.width() - 2, r.height() - 2);
   painter->setBrush(Qt::NoBrush);
   painter->setPen(QPen(light, 2));
   QwtPainter::drawLine(painter, r.left() + 1, r.top() + 2, r.right() + 1, r.top() + 2);
   painter->setPen(QPen(dark, 2));
   QwtPainter::drawLine(painter, r.left() + 1, r.bottom(), r.right() + 1, r.bottom());
   painter->setPen(QPen(light, 1));
   QwtPainter::drawLine(painter, r.left(), r.top() + 1, r.left(), r.bottom());
   QwtPainter::drawLine(painter, r.left() + 1, r.top() + 2, r.left() + 1, r.bottom() - 1);
   painter->setPen(QPen(dark, 1));
   QwtPainter::drawLine(painter, r.right() + 1, r.top() + 1, r.right() + 1, r.bottom());
   QwtPainter::drawLine(painter, r.right(), r.top() + 2, r.right(), r.bottom() - 1);
   painter->restore();
}
