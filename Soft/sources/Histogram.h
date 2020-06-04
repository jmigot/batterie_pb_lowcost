
#ifndef HISTOGRAM_H
#define HISTOGRAM_H

#include <QColor>

#include <Qwt/qwt_plot_item.h>
#include <Qwt/qwt_interval_data.h>

class Histogram: public QwtPlotItem
{
	private:
		QwtIntervalData _data;
		QColor _color;
		double _reference;

	public:
		Histogram(const QString &title);
		virtual ~Histogram();

		void                  setData(const QVector<double> &data);
		virtual QwtDoubleRect boundingRect() const;
		virtual int           rtti() const;
		virtual void          draw(QPainter *painter, const QwtScaleMap &xMap, const QwtScaleMap &yMap, const QRect &rect) const;

	protected:
		virtual void drawBar(QPainter *painter, Qt::Orientation, const QRect &rect) const;


};

#endif
