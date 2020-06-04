
/**
 * A new tab with curves
 *
 * @date December 2010
 * @author Jean-Luc Migot (jl.migot@yahoo.fr)
 *
 * Copyright 2010-2011 Jean-Luc Migot
 */

#include "CurvesSplitter.h"
#include "MainWindow.h"
#include "Graphic.h"
#include "GraphicTimes.h"

#include <Qwt/qwt_plot.h>

#include <QApplication>
#include <QDate>
#include <QDebug>
#include <QFile>
#include <QFileDialog>
#include <QListWidget>
#include <QMessageBox>
#include <QPrintDialog>
#include <QPrinter>
#include <QPainter>

CurvesSplitter::CurvesSplitter(MainWindow *window, int nbData, const QString &data, bool saveToFile, const QString &immat, QVector<double> yValues, const QString &config)
	: _window(window)
{
	_graphicListWidget = new QListWidget(this);
	connect(_graphicListWidget, SIGNAL(currentRowChanged(int)), this, SLOT(switchCurve(int)));

	curvesConfig = new Config;

	if ( ! config.isEmpty())
	{
		curvesConfig->nbVoltages     = 0;
		curvesConfig->nbFlows        = 0;
		curvesConfig->nbTemperatures = 0;

		for (int i = 0; i < 40; i++)
		{
			curvesConfig->voltages.append(_window->hexToInt(config.mid(i*2, 2)));

			if (curvesConfig->voltages[i] > 0)
				curvesConfig->nbVoltages++;
		}

		for (int i = 0; i < 8; i++)
		{
			curvesConfig->flows.append(_window->hexToInt(config.mid(80 + i*2, 2)));

			if (curvesConfig->flows[i] > 0)
				curvesConfig->nbFlows++;
		}

		for (int i = 0; i < 8; i++)
		{
			curvesConfig->temperatures.append(_window->hexToInt(config.mid(96 + i*2, 2)));

			if (curvesConfig->temperatures[i] > 0)
				curvesConfig->nbTemperatures++;
		}

		curvesConfig->specialCommand = _window->hexToInt(config.mid(112, 1));
		curvesConfig->eepromSize     = _window->hexToInt(config.mid(113, 1));
		curvesConfig->clientCode     = _window->hexToInt(config.mid(114, 2));

		curvesConfig->voltageLow      = _window->hexToInt(config.mid(116, 2));
		curvesConfig->voltageHigh     = _window->hexToInt(config.mid(118, 2));
		curvesConfig->flowLow         = _window->hexToInt(config.mid(120, 2));
		curvesConfig->flowHigh        = _window->hexToInt(config.mid(122, 2));
		curvesConfig->temperatureLow  = _window->hexToInt(config.mid(124, 2));
		curvesConfig->temperatureHigh = _window->hexToInt(config.mid(126, 2));
	}

	else
	{
		for (int i = 0; i < 40; i++)
			curvesConfig->voltages.append(_window->config()->voltages[i]);

		for (int i = 0; i < 8; i++)
			curvesConfig->flows.append(_window->config()->flows[i]);

		for (int i = 0; i < 8; i++)
			curvesConfig->temperatures.append(_window->config()->temperatures[i]);

		curvesConfig->nbVoltages     = _window->config()->nbVoltages;
		curvesConfig->nbFlows        = _window->config()->nbFlows;
		curvesConfig->nbTemperatures = _window->config()->nbTemperatures;

		curvesConfig->specialCommand = _window->config()->specialCommand;
		curvesConfig->eepromSize     = _window->config()->eepromSize;
		curvesConfig->clientCode     = _window->config()->clientCode;

		curvesConfig->voltageLow      = _window->config()->voltageLow;
		curvesConfig->voltageHigh     = _window->config()->voltageHigh;
		curvesConfig->flowLow         = _window->config()->flowLow;
		curvesConfig->flowHigh        = _window->config()->flowHigh;
		curvesConfig->temperatureLow  = _window->config()->temperatureLow;
		curvesConfig->temperatureHigh = _window->config()->temperatureHigh;
	}

	for (int i = 0; i < 40; i++)
		curvesConfig->voltageColors.append(_window->config()->voltageColors.at(i));

	for (int i = 0; i < 8; i++)
		curvesConfig->flowColors.append(_window->config()->flowColors.at(i));

	for (int i = 0; i < 8; i++)
		curvesConfig->temperatureColors.append(_window->config()->temperatureColors.at(i));

	QVector<double> xValues;
	QList< QVector<double> > voltageValues;
	QList< QVector<double> > flowsValues;
	QList< QVector<double> > temperaturesValues;

	int x = 0;
	int j = 0;
	int dataLen = curvesConfig->nbVoltages + curvesConfig->nbFlows + curvesConfig->nbTemperatures;
	Graphic *currentGraphic = NULL;

	for (int i = 0; i < 40; i++)
		voltageValues.append(QVector<double>());

	for (int i = 0; i < 8; i++)
		flowsValues.append(QVector<double>());

	for (int i = 0; i < 8; i++)
		temperaturesValues.append(QVector<double>());

	while (j < nbData * 2)
	{
		QString octet;

		// Header
		if (data.mid(j, 6) == "000000")
		{
			octet = data.mid(j, 16);

			// Date and time
			int yearAndDays = _window->hexToInt(octet.mid(6, 2));
			int year        = QString("20" + QString("0" + QString::number((yearAndDays & 0x7F))).right(2)).toInt();
			int days        = _window->hexToInt(octet.mid(8, 2));

			if (yearAndDays & 0x80)
				days += 255;

			int hours   = _window->hexToInt(octet.mid(10, 2));
			int minutes = _window->hexToInt(octet.mid(12, 2));
			int seconds = _window->hexToInt(octet.mid(14, 2));

			QDate date(year, 1, 1);
			date = date.addDays(days - 1);

			QTime time(hours, minutes, seconds);
			QString dateTime(date.toString("dd/MM/yy" ) + " à " + time.toString("hh:mm"));

			if (currentGraphic != NULL)
				currentGraphic->setData(xValues, voltageValues, flowsValues, temperaturesValues);

			currentGraphic = new Graphic(this, _window, immat + " débuté le " + dateTime, curvesConfig);

			// Create new curve
			_graphicList << currentGraphic;
			_graphicListWidget->addItem(dateTime);

			// Clear data vectors
			x = 0;
			xValues.clear();

			for (int i = 0; i < 40; i++)
				voltageValues[i].clear();

			for (int i = 0; i < 8; i++)
				flowsValues[i].clear();

			for (int i = 0; i < 8; i++)
				temperaturesValues[i].clear();

			j += 16;
		}

		// Data
		else
		{
			octet = data.mid(j, dataLen * 2);
			int k = 0;
			int l = curvesConfig->nbVoltages * 2;
			int m = (curvesConfig->nbVoltages + curvesConfig->nbFlows) * 2;

			// Add data to current curve
			xValues << x;

			for (int i = 0; i < 40; i++)
			{
				if (curvesConfig->voltages[i] > 0)
				{
					voltageValues[i].append((double)(_window->hexToInt(octet.mid(k, 2))) * (double)(curvesConfig->voltages[i]) / 255.0);
					k += 2;
				}
			}

			for (int i = 0; i < 8; i++)
			{
				if (curvesConfig->flows[i] > 0)
				{
					flowsValues[i].append((double)(_window->hexToInt(octet.mid(l, 2))) * (double)(curvesConfig->flows[i]) / 127.0);
					l += 2;
				}
			}

			for (int i = 0; i < 8; i++)
			{
				if (curvesConfig->temperatures[i] > 0)
				{
					temperaturesValues[i].append((double)(_window->hexToInt(octet.mid(m, 2))) / (double)(curvesConfig->temperatures[i]));
					m += 2;
				}
			}

			x += 4;
			j += dataLen * 2;
		}
	}

	if (currentGraphic != NULL)
		currentGraphic->setData(xValues, voltageValues, flowsValues, temperaturesValues);

	if (_window->config()->specialCommand == 1)
	{
		bool good = false;

		for (int i = 0; i < 8; i++)
		{
			if (yValues.at(i) > 0)
			{
				good = true;
				break;
			}
		}

		if (good)
		{
			_graphicList << new GraphicTimes(this, _window, yValues);
			_graphicListWidget->addItem("Températures");
		}
	}

	if (_graphicList.count() > 0)
		this->switchCurve(0);

	if (saveToFile)
	{
		// Save data to file
		QFile file(QApplication::applicationDirPath() + "/" + MainWindow::DEFAULT_DIR + immat + "_" + QDate::currentDate().toString("dd-MM-yy") + "_" + QTime::currentTime().toString(" hh-mm-ss") + MainWindow::DEFAULT_EXTENSION);

		if (file.open(QFile::WriteOnly | QFile::Text) && file.isWritable())
		{
			QString buffer(immat + "\n");

			for (int i=0; i<40; i++)
				buffer += _window->intToHex(_window->config()->voltages[i]);

			for (int i=0; i<8; i++)
				buffer += _window->intToHex(_window->config()->flows[i]);

			for (int i=0; i<8; i++)
				buffer += _window->intToHex(_window->config()->temperatures[i]);

			buffer += _window->intToHex(curvesConfig->specialCommand * 16 + curvesConfig->eepromSize);
			buffer += _window->intToHex(curvesConfig->clientCode);

			buffer += _window->intToHex(curvesConfig->voltageLow);
			buffer += _window->intToHex(curvesConfig->voltageHigh);
			buffer += _window->intToHex(curvesConfig->flowLow);
			buffer += _window->intToHex(curvesConfig->flowHigh);
			buffer += _window->intToHex(curvesConfig->temperatureLow);
			buffer += _window->intToHex(curvesConfig->temperatureHigh);

			buffer += "\n" + _window->intToHex(nbData, 6) + "\n" + data;

			// PC Date and Card date
			buffer += "\n" + _window->intToHex(QDate::currentDate().day()) +
							 _window->intToHex(QDate::currentDate().month()) +
							 _window->intToHex(QString::number(QDate::currentDate().year()).right(2).toInt()) +
							 _window->intToHex(QTime::currentTime().hour()) +
							 _window->intToHex(QTime::currentTime().minute()) +
							 _window->intToHex(QTime::currentTime().second()) +
							 _window->config()->date;

			if (_window->config()->specialCommand == 1)
			{
				buffer += "\n";

				for (int i = 0; i < 8; i++)
					buffer += _window->intToHex(yValues.at(i), 4);
			}

			if (file.write(buffer.toAscii()) == buffer.length())
				QMessageBox::information(_window, "Ok", "Les données ont bien été enregistrées sur le disque.");

			else
				QMessageBox::critical(_window, "Erreur", "Impossible d'écrire les données sur le disque.");

			file.close();
		}

		else
			QMessageBox::critical(_window, "Erreur", "Impossible d'ouvrir le fichier sur le disque.");
	}
}

CurvesSplitter::~CurvesSplitter()
{
	#ifndef QT_NO_DEBUG
		qDebug() << "~CurvesSplitter()";
	#endif

	if (curvesConfig != NULL)
		delete curvesConfig;

	if (this->widget(1) != NULL)
		this->widget(1)->setParent(NULL);

	for (int i = 0; i < _graphicList.count(); i++)
		delete _graphicList.at(i);
}

void CurvesSplitter::switchCurve(int i)
{
	int widthLeft  = _window->width() / 6;
	int widthRight = widthLeft * 5;

	if (this->widget(0) != NULL)
		widthLeft  = this->widget(0)->width();

	if (this->widget(1) != NULL)
		widthRight = this->widget(1)->width();

	if (this->widget(1) != NULL)
		this->widget(1)->setParent(NULL);

	if (dynamic_cast<Graphic*>(_graphicList[i]) != NULL)
	{
		this->addWidget(dynamic_cast<Graphic*>(_graphicList[i])->plot());
		dynamic_cast<Graphic*>(_graphicList[i])->plot()->show();
	}

	else if (dynamic_cast<GraphicTimes*>(_graphicList[i]) != NULL)
	{
		this->addWidget(dynamic_cast<GraphicTimes*>(_graphicList[i])->plot());
		dynamic_cast<GraphicTimes*>(_graphicList[i])->plot()->show();
	}

	_graphicListWidget->setCurrentRow(i);

	QList<int> sizes;
	sizes << widthLeft << widthRight;
	this->setSizes(sizes);
}

QList<Graphic*> CurvesSplitter::graphicList()
{
	QList<Graphic*> list;

	for (int i = 0; i < _graphicList.count(); i++)
	{
		Graphic* tmp = dynamic_cast<Graphic*>(_graphicList.at(i));

		if (tmp != NULL)
			list.append(tmp);
	}

	return list;
}

void CurvesSplitter::print()
{
	QPrinter printer;
	printer.setPageSize(QPrinter::A4);
	printer.setOrientation(QPrinter::Landscape);

	QPrintDialog printerDialog(&printer, _window);

	if (printerDialog.exec() == QDialog::Accepted)
		this->print(&printer, printer.fromPage(), printer.toPage());
}

void CurvesSplitter::exportPDF()
{
	QString fileName(QFileDialog::getSaveFileName(_window, "Exporter en PDF", "", "PDF (*.pdf)"));

	if ( ! fileName.isEmpty())
	{
		QPrinter printer;
		printer.setOutputFormat(QPrinter::PdfFormat);
		printer.setOutputFileName(fileName);
		printer.setPageSize(QPrinter::A4);
		printer.setOrientation(QPrinter::Landscape);

		this->print(&printer, 1, _graphicList.count());
		QMessageBox::information(this, "Ok", "Le fichier <b>" + fileName + "</b> a bien été enregistré.");
	}
}

void CurvesSplitter::print(QPrinter *printer, int borneInf, int borneSup) const
{
	QPainter painter;
	painter.begin(printer);

	QRect page = printer->pageRect();
	int w = page.width() - page.x();
	int h = page.height() - page.y();
	QRect rect(page.x(), page.y(), w, h);

	if (borneSup > _graphicList.count())
		borneSup = _graphicList.count();

	if (borneInf == 0 && borneSup == 0)
	{
		borneInf = 1;
		borneSup = _graphicList.count();
	}

	borneInf--;
	borneSup--;

	if (printer->pageOrder() == QPrinter::LastPageFirst)
	{
		for (int i = borneSup; i >= borneInf; i--)
		{
			if (dynamic_cast<Graphic*>(_graphicList[i]) != NULL)
				dynamic_cast<Graphic*>(_graphicList[i])->plot()->print(&painter, rect);

			else if (dynamic_cast<GraphicTimes*>(_graphicList[i]) != NULL)
				dynamic_cast<GraphicTimes*>(_graphicList[i])->plot()->print(&painter, rect);

			if (i > borneInf)
				printer->newPage();
		}
	}

	else
	{
		for (int i = borneInf; i <= borneSup; i++)
		{
			if (dynamic_cast<Graphic*>(_graphicList[i]) != NULL)
				dynamic_cast<Graphic*>(_graphicList[i])->plot()->print(&painter, rect);

			else if (dynamic_cast<GraphicTimes*>(_graphicList[i]) != NULL)
				dynamic_cast<GraphicTimes*>(_graphicList[i])->plot()->print(&painter, rect);

			if (i < borneSup)
				printer->newPage();
		}
	}

	painter.end();
}
