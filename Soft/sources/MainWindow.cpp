
/**
 * Main window of the project
 *
 * @date December 2010
 * @author Jean-Luc Migot (jl.migot@yahoo.fr)
 *
 * Copyright 2010-2011 Jean-Luc Migot
 */

#include "MainWindow.h"
#include "CurvesSplitter.h"
#include "Dialog.h"
#include "Graphic.h"
#include "ManageSteps.h"
#include "Plot.h"
#include "SerialPort.h"

#include <QApplication>
#include <QDate>
#include <QDebug>
#include <QDesktopWidget>
#include <QDesktopServices>
#include <QFileDialog>
#include <QLabel>
#include <QMenuBar>
#include <QMessageBox>
#include <QProgressBar>
#include <QPushButton>
#include <QtCore/qmath.h>
#include <QToolBar>
#include <QUrl>
#include <QVBoxLayout>

#define AddressPointer "AddressPointer00"

const QString MainWindow::APPLICATION_NAME        = "BPLC1210";
const QString MainWindow::VERSION                 = "1.5";
const int     MainWindow::MAX_RETRY_EEPROM        = 3;
const int     MainWindow::MAX_RETRY_WRITE_TO_FILE = 3;
const QString MainWindow::DEFAULT_DIR             = "courbes/";
const QString MainWindow::DEFAULT_EXTENSION       = ".pbl";
const QString MainWindow::COLORS_FILE             = "Colors.cfg";

MainWindow::MainWindow()
{
	this->setWindowTitle(APPLICATION_NAME);
	this->setAttribute(Qt::WA_DeleteOnClose);

	// --------------------------------------------------
	//		Menu
	// --------------------------------------------------
	_fileMenu = this->menuBar()->addMenu("Fichier");
	_fileMenu->addAction(QIcon(":/images/open"), "Ouvrir", this, SLOT(open()));
	_fileMenu->addAction(QIcon(":/images/export"), "Exporter les données en texte", this, SLOT(exportData()));
	_fileMenu->addAction(QIcon(":/images/pdf"), "Exporter les courbes en PDF", this, SLOT(exportPDF()));
	_fileMenu->addAction(QIcon(":/images/printer"), "Imprimer les courbes", this, SLOT(print()));
	_fileMenu->addAction(QIcon(":/images/quit" ), "Quitter", this, SLOT(close()));

	_cardMenu = this->menuBar()->addMenu("Boitier");
	_cardMenu->addAction(QIcon(":/images/read"), "Lire les valeurs", this, SLOT(read()));
	_cardMenu->addAction(QIcon(":/images/dateTime"), "Mettre à jour la date et l'heure du boitier", this, SLOT(updateDateTime()));
	_cardMenu->addAction(QIcon(":/images/steps"), "Réglage des seuils", this, SLOT(manageSteps()));
	_cardMenu->addAction(QIcon(":/images/delete"), "Effacer les données du boitier", this, SLOT(resetData()));

	QMenu* helpMenu = this->menuBar()->addMenu("Aide");
	helpMenu->addAction(QIcon(":/images/help"), "Aide", this, SLOT(help()));
	helpMenu->addAction(QIcon(":/images/about"), "A propos", this, SLOT(about()));

	foreach (QAction *action, _cardMenu->actions())
		action->setDisabled(true);

	// --------------------------------------------------
	//		Toolbar
	// --------------------------------------------------
	_connection  = new QLabel("<h4>&nbsp;Connexion : </h4>");
	_dateTime    = new QLabel("<h4>&nbsp;Date et heure : </h4>");
	_memoryUsage = new QProgressBar;

	_connectDisconnect = new QPushButton(" Connecter ");
	_testConnection    = new QPushButton(" Tester connexion ");
	_testConnection->setDisabled(true);
	connect(_connectDisconnect, SIGNAL(clicked()), this, SLOT(connectDisconnect()));
	connect(_testConnection, SIGNAL(clicked()), this, SLOT(testConnection()));

	QToolBar* toolBar = this->addToolBar("Barre d'outils");
	toolBar->setMovable(false);
	toolBar->setMinimumHeight(50);
	toolBar->addWidget(_connection);
	toolBar->addSeparator();
	toolBar->addWidget(_dateTime);
	toolBar->addSeparator();
	toolBar->addWidget(new QLabel("<h4>&nbsp;Mémoire utilisée : </h4>"));
	toolBar->addWidget(_memoryUsage);
	toolBar->addWidget(_connectDisconnect);
	toolBar->addWidget(_testConnection);

	// --------------------------------------------------
	//		Central widget
	// --------------------------------------------------
	_tabWidget = new QTabWidget;
	_tabWidget->setTabsClosable(true);
	connect(_tabWidget, SIGNAL(tabCloseRequested(int)), this, SLOT(closeTab(int)));
	this->setCentralWidget(_tabWidget);

	this->updatePrintMenu();

	// --------------------------------------------------
	//		Default save dir for curves files
	// --------------------------------------------------
	QDir dir;

	if ( ! dir.exists(QApplication::applicationDirPath() + "/" + DEFAULT_DIR))
		dir.mkdir(QApplication::applicationDirPath() + "/" + DEFAULT_DIR);

	// --------------------------------------------------
	//		Size and placement of main window
	// --------------------------------------------------
	QDesktopWidget desktop;

	int screenWidth  = desktop.screenGeometry().width();
	int screenHeight = desktop.screenGeometry().height();

	// If screen is too small, we show application in maximized mode "full screen"
	if( screenWidth <= 1024 || screenHeight <= 768 )
		this->showMaximized();

	// Else application takes 2/3 of screen
	else
	{
		this->resize((screenWidth / 3) * 2, (screenHeight / 3) * 2);

		int x = (screenWidth  - this->width()) / 2;
		int y = (screenHeight - this->height()) / 2;
		this->move(x, y);
		this->show();
	}

	QApplication::processEvents();

	// --------------------------------------------------
	//		Usual
	// --------------------------------------------------
	_config = new Config;
	this->readConfigColors();
	_port   = new SerialPort(this);
	this->connectDisconnect();
}

MainWindow::~MainWindow()
{
	#ifndef QT_NO_DEBUG
		qDebug() << "~MainWindow()";
	#endif

	if (_port != NULL)
		delete _port;

	if (_config != NULL)
		delete _config;
}

void MainWindow::readConfigColors()
{
	// First we declare all colors to prevent program bugs
	for (int i = 0; i < 40; i++)
		_config->voltageColors.append("#0066CC");

	for (int i = 0; i < 8; i++)
	{
		_config->flowColors.append("#00FF00");
		_config->temperatureColors.append("#FF0000");
	}

	QFile file(COLORS_FILE);
	QString buffer;

	// If file exists, read colors from it
	if (file.exists())
	{
		if (file.open(QFile::ReadOnly | QFile::Text) &&file.isReadable())
		{
			buffer = file.readAll();
			file.close();
			bool integrity = true;

			// If we have all colors
			if (buffer.count("\n") >= 56)
			{
				QList<QString> args(buffer.split("\n"));

				// File integrity
				for (int i = 0; i < 56; i++)
				{
					if ( ! args.at(i).contains(QRegExp("=(\\s)*\\#[a-zA-Z0-9]{6}(\\s)*$")))
					{
						integrity = false;
						break;
					}
				}

				if (integrity)
				{
					for (int i = 0; i < 40; i++)
						_config->voltageColors[i] = args.at(i).split("=").at(1).simplified();

					for (int i = 0; i < 8; i++)
						_config->flowColors[i] = args.at(40+i).split("=").at(1).simplified();

					for (int i = 0; i < 8; i++)
						_config->temperatureColors[i] = args.at(48+i).split("=").at(1).simplified();
				}
			}

			else
				integrity = false;

			if ( ! integrity)
				QMessageBox::critical(this, "Fichiers couleurs corrompu", "Le fichier " + COLORS_FILE + " est corrompu, les couleurs par défaut seront utilisées.");
		}
	}

	// Else, write file
	else
	{
		// Prepare data
		for (int i = 0; i < 40; i++)
			buffer += "Voltage" + QString("0" + QString::number(i+1)).right(2) + " = " + _config->voltageColors.at(i) + "\n";

		for (int i = 0; i < 8; i++)
			buffer += "Flow" + QString("0" + QString::number(i+1)).right(2) + " = " + _config->flowColors.at(i) + "\n";

		for (int i = 0; i < 8; i++)
			buffer += "Temperature" + QString("0" + QString::number(i+1)).right(2) + " = " + _config->temperatureColors.at(i) + "\n";

		// Write it to file
		if (file.open(QFile::WriteOnly | QFile::Text) && file.isWritable())
		{
			file.write(buffer.toAscii());
			file.close();
		}
	}
}

void MainWindow::closeTab(int index)
{
	_tabWidget->widget(index)->deleteLater();
	_tabWidget->removeTab(index);
	this->updatePrintMenu();
}

bool MainWindow::readEEPROMConfig()
{
	if ( ! this->testConnection(false))
		return false;

	QString response(this->tryRead("C", 130));

	if (response.isEmpty())
	{
		QMessageBox::critical(this, "Erreur", "Impossible de lire la configuration dans l'EEPROM. La connexion n'est pas utilisable.");
		return false;
	}

	_config->nbVoltages     = 0;
	_config->nbFlows        = 0;
	_config->nbTemperatures = 0;

	_config->voltages.clear();
	_config->flows.clear();
	_config->temperatures.clear();

	for (int i = 0; i < 40; i++)
	{
		_config->voltages.append(this->hexToInt(response.mid(i * 2, 2)));

		if (_config->voltages[i] > 0)
			_config->nbVoltages++;
	}

	for (int i = 0; i < 8; i++)
	{
		_config->flows.append(this->hexToInt(response.mid(80 + i * 2, 2)));

		if (_config->flows[i] > 0)
			_config->nbFlows++;
	}

	for (int i = 0; i < 8; i++)
	{
		_config->temperatures.append(this->hexToInt(response.mid(96 + i * 2, 2)));

		if (_config->temperatures[i] > 0)
			_config->nbTemperatures++;
	}

	_config->specialCommand = this->hexToInt(response.mid(112, 1));
	_config->eepromSize     = this->hexToInt(response.mid(113, 1));
	_config->clientCode     = this->hexToInt(response.mid(114, 2));

	_config->voltageLow      = this->hexToInt(response.mid(116, 2));
	_config->voltageHigh     = this->hexToInt(response.mid(118, 2));
	_config->flowLow         = this->hexToInt(response.mid(120, 2));
	_config->flowHigh        = this->hexToInt(response.mid(122, 2));
	_config->temperatureLow  = this->hexToInt(response.mid(124, 2));
	_config->temperatureHigh = this->hexToInt(response.mid(126, 2));

	return true;
}

void MainWindow::readEEPROMUsual()
{
	if ( ! this->testConnection(false))
		return;

	// --------------------------------------------------
	//		Date and time
	// --------------------------------------------------
	QString response(this->tryRead("H", 18));

	if (response.length() == 18)
	{
		int yearAndDays = this->hexToInt(response.mid(6, 2));
		int year        = QString("20" + QString("0" + QString::number((yearAndDays & 0x7F))).right(2)).toInt();
		int days        = this->hexToInt(response.mid(8, 2));

		if (yearAndDays & 0x80)
			days += 255;

		int hours   = this->hexToInt(response.mid(10, 2));
		int minutes = this->hexToInt(response.mid(12, 2));
		int seconds = this->hexToInt(response.mid(14, 2));

		QDate date = QDate(year, 1, 1).addDays(days - 1);
		QTime time(hours, minutes, seconds);

		_config->date = this->intToHex(date.day()) +
						this->intToHex(date.month()) +
						this->intToHex(QString::number(date.year()).right(2).toInt()) +
						this->intToHex(time.hour()) +
						this->intToHex(time.minute()) +
						this->intToHex(time.second());

		_dateTime->setText("<h4>&nbsp;Date et heure : " + this->color(date.toString("dd/MM/yy") + " à " + time.toString("hh:mm"), Color::Blue) + "&nbsp;</h4>");
	}

	// --------------------------------------------------
	//		Memory
	// --------------------------------------------------
	response = this->tryRead("P", 8);

	if (response.contains(QRegExp("[A-Z0-9]{6}\r\n")))
	{
		int eepromSize = qPow(2, _config->eepromSize + 12) - 384;

		_memoryUsage->setRange(0, eepromSize);
		int value = this->hexToInt(response.left(6)) - 128;

		// Value can be a bit more than 100%
		if (value < 0)
			value = 0;

		if (value > eepromSize)
			value = eepromSize;

		_memoryUsage->setValue(value);
		_memoryUsage->update();
		QApplication::processEvents();
	}
}

void MainWindow::open()
{
	QString fileName(QFileDialog::getOpenFileName(this, "Ouvrir un fichier", QApplication::applicationDirPath() + "/" + DEFAULT_DIR, "Fichiers Batterie PB Lowcost (*" + DEFAULT_EXTENSION + ")"));

	if ( ! fileName.isEmpty())
	{
		QString data;
		QFile dataFile(fileName);

		if (dataFile.open(QFile::ReadOnly | QFile::Text) && dataFile.isReadable())
		{
			data = dataFile.readAll();
			dataFile.close();
		}

		if (data.count("\n") >= 4)
		{
			QList<QString> args(data.split("\n"));
			QString immat(args.at(0));
			QString config(args.at(1));
			int nbData = this->hexToInt(args.at(2));
			data = args.at(3).simplified();
			QVector<double> yValues;
			QString timeValues;

			if (_config->specialCommand == 1 && args.count() == 6)
			{
				timeValues = args.at(5).simplified();

				for (int i = 0; i < 8; i++)
					yValues.append(this->hexToInt(timeValues.mid(i*4, 4)));
			}

			if ((nbData > 0 && data.length() > 0) || ( ! timeValues.contains("0{16}")))
			{
				if (data.length() == nbData * 2)
				{
					CurvesSplitter *curvesSplitter = new CurvesSplitter(this, nbData, data, false, immat, yValues, config);
					_tabWidget->addTab(curvesSplitter, "Courbes " + immat);
					_tabWidget->setCurrentIndex(_tabWidget->indexOf(curvesSplitter));
					this->updatePrintMenu();
				}

				else
					QMessageBox::critical(this, "Erreur de données", "Mauvais nombre de données, le fichier est probablement corrompu.");
			}

			else
				QMessageBox::critical(this, "Erreur de données", "Le fichier ne comporte aucune donnée.");
		}

		else
			QMessageBox::critical(this, "Erreur de données", "Le fichier est corrompu et illisible.");
	}
}

QByteArray MainWindow::tryRead(const QString &command, int lenExpected, int maxRetry)
{
	this->setButtonsEnabled(false);

	QByteArray response;
	int tries = 0;

	do
	{
		_port->write(command);
		response = _port->read();
		tries++;
	} while(response.length() != lenExpected && tries < maxRetry);

	if (tries == maxRetry)
		response = "";

	this->setButtonsEnabled(true);

	return response;
}

void MainWindow::read()
{
	if ( ! this->testConnection(false))
		return;

	QString response(this->tryRead("P", 8));
	int nbData = 0;
	QString data;

	if (response.contains(QRegExp("[A-Z0-9]{6}\r\n")))
	{
		nbData = this->hexToInt(response.left(6)) - 128;

		if (nbData > 0)
		{
			QWidget *widget(new QWidget);
			QVBoxLayout *layout(new QVBoxLayout(widget));

			widget->setWindowModality(Qt::ApplicationModal);
			widget->setWindowFlags(Qt::Popup | Qt::WindowTitleHint);

			layout->addWidget(new QLabel(this->color("<h1>Lecture des données en cours...</h1>", Color::Blue)));

			widget->adjustSize();

			// Place widget on center of the screen
			QDesktopWidget desktop;
			int x = (desktop.screenGeometry().width() - widget->width()) / 2;
			int y = (desktop.screenGeometry().height() - widget->height()) / 2;

			widget->move(x, y);
			widget->show();

			QByteArray dataK(this->tryRead("K", nbData + 2));

			if (widget != NULL)
				delete widget;

			if (dataK.isEmpty())
			{
				QMessageBox::critical(this, "Erreur", "Impossible de lire les données.");
				return;
			}

			for (int i = 0; i < dataK.length() - 2; i++)
				data += this->intToHex(dataK[i]);

			// Response if nbData * 2 + CR + LF
			if (data.length() != nbData * 2)
			{
				nbData = 0;
				data.clear();
				QMessageBox::critical(this, "Erreur", "Mauvais nombre d'octets lus.");
			}
		}
	}

	else
		QMessageBox::critical(this, "Erreur", "Impossible de trouver le nombre d'enregistrements.");

	QVector<double> values;
	bool good = false;

	if (_config->specialCommand == 1)
	{
		QString response(this->tryRead("A", 98));

		if (response.contains(QRegExp("[A-Z0-9]{96}\r\n")))
		{
			for (int i = 0; i < 8; i++)
			{
				int value = this->hexToInt(response.mid(i*4, 4));

				if (value > 0)
					good = true;

				values.append(value);
			}
		}

		else
			QMessageBox::critical(this, "Erreur", "Erreur de lecture des valeurs de temps.");
	}

	if ( ! data.isEmpty() || good)
	{
		Dialog dialog("Immatriculation véhicule/batterie", "Saisir référence véhicule/batterie", "[^/\\\\\"*?<>|:]{1,30}");

		if (dialog.exec() == QDialog::Accepted)
		{
			CurvesSplitter *curvesSplitter = new CurvesSplitter(this, nbData, data.left(nbData * 2), true, dialog.lineEditText(), values);
			_tabWidget->addTab(curvesSplitter, "Courbes " + dialog.lineEditText());
			_tabWidget->setCurrentIndex(_tabWidget->indexOf(curvesSplitter));
			this->updatePrintMenu();
		}
	}

	else
		QMessageBox::information(this, "Erreur", "Il n'y a aucune donnée dans le boitier");
}

void MainWindow::updateDateTime()
{
	if ( ! this->testConnection(false))
		return;

	QDate date(QDate::currentDate());
	QTime time(QTime::currentTime());
	QString dateTime("M");

	dateTime += this->intToHex(QString::number(date.year()).right(2).toInt() | ((date.dayOfYear() > 255) ? 128 : 0));
	dateTime += this->intToHex((date.dayOfYear() > 255) ? date.dayOfYear() - 255 : date.dayOfYear());
	dateTime += this->intToHex(time.hour());
	dateTime += this->intToHex(time.minute());

	if (this->tryRead(dateTime, 3) != "M\r\n")
	{
		QMessageBox::critical(this, "Erreur", "Erreur lors de la réponse du boitier.");
		return;
	}

	// Wait 100 ms for this order
	QTime dieTime = QTime::currentTime().addMSecs(100);

	while (QTime::currentTime() <= dieTime)
		QCoreApplication::processEvents(QEventLoop::AllEvents, 50);

	QString response = this->tryRead("H", 18);

	if (response.length() != 18)
	{
		QMessageBox::critical(this, "Erreur", "Impossible de relire l'heure.");
		return;
	}

	if (response.mid(6, 8) != dateTime.mid(1, 8))
	{
		QMessageBox::critical(this, "Erreur", "L'heure relue est incorrecte.");
		return;
	}

	this->readEEPROMUsual();
	QMessageBox::information(this, "Ok", "Le boitier a bien été mis à l'heure");
}

void MainWindow::manageSteps()
{
	new ManageSteps(this);
}

void MainWindow::resetData()
{
	if ( ! this->testConnection(false))
		return;

	if (QMessageBox::question(this, "Confirmation", "Etes-vous certain de vouloir effacer les données ?<br />Si vous n'avez pas lancé <b>Lire les données</b> via le menu <b>Boitier</b>, elles n'ont pas été enregistrées sur le disque et seront perdues.", QMessageBox::Ok, QMessageBox::Cancel) == QMessageBox::Ok)
	{
		if (this->tryRead("R", 3) != "R\r\n")
			QMessageBox::critical(this, "Erreur", "Mauvaise réponse du boitier.");

		else
		{
			// Wait 100 ms for this order
			QTime dieTime = QTime::currentTime().addMSecs(100);

			while (QTime::currentTime() <= dieTime)
				QCoreApplication::processEvents(QEventLoop::AllEvents, 50);

			if (this->tryRead("P", 8) != "000080\r\n")
				QMessageBox::critical(this, "Erreur", "Les données n'ont pas pu être effacées.");

			else
			{
				if (_config->specialCommand == 1)
				{
					if (this->tryRead("B", 3) != "B\r\n")
						QMessageBox::information(this, "Erreur", "Les données ont bien été effacées mais les valeurs de temps sont toujours présentes. Réessayez.");

					else
						QMessageBox::information(this, "Ok", "Les données ont bien été effacées.");
				}

				else
					QMessageBox::information(this, "Ok", "Les données ont bien été effacées.");
			}
		}

		this->readEEPROMUsual();
	}
}

void MainWindow::help()
{
	QFile file(QApplication::applicationDirPath() + "/Documentation.pdf");

	if ( ! file.exists())
		QMessageBox::critical(this, "Erreur", "Le fichier <b>Documentation.pdf</b> n'existe pas");

	else if ( ! QDesktopServices::openUrl(QUrl("file:///" + QApplication::applicationDirPath() + "/Documentation.pdf")))
		QMessageBox::critical(this, "Erreur", "Impossible de lancer le fichier <b>" + QApplication::applicationDirPath() + "/Documentation.pdf</b> dans le lecteur PDF par défaut");
}

void MainWindow::exportData()
{
	CurvesSplitter *tab = dynamic_cast<CurvesSplitter*>(_tabWidget->currentWidget());

	if (tab != NULL)
	{
		QString fileName(QFileDialog::getSaveFileName(this, "Exporter les données", "", "Fichier texte (*.txt)"));

		if ( ! fileName.isEmpty())
		{
			// Save data to file
			QFile file(fileName);

			if (file.open(QFile::WriteOnly | QFile::Text) && file.isWritable())
			{
				QString buffer;

				QList<Graphic*> list = tab->graphicList();

				for (int i = 0; i < list.count(); i++)
				{
					// Title
					buffer += list.at(i)->plot()->title().text() + "\n";

					// Legend
					buffer += "Minutes";

					for (int k = 0; k < 40; k++)
						if (_config->voltages[k] > 0)
							buffer += ",Tension voie " + QString::number(k+1);

					for (int k = 0; k < 8; k++)
						if (_config->flows[k] > 0)
							buffer += ",Courant voie " + QString::number(k+1);

					for (int k = 0; k < 8; k++)
						if (_config->temperatures[k] > 0)
							buffer += ",Température voie " + QString::number(k+1);

					buffer += "\n";

					QVector<double> xValues = list.at(i)->xValues();
					QList< QVector<double> > voltageValues = list.at(i)->voltageValues();
					QList< QVector<double> > flowValues = list.at(i)->flowValues();
					QList< QVector<double> > temperaturesValues = list.at(i)->temperaturesValues();

					// Data
					for (int j = 0; j < xValues.count(); j++)
					{
						buffer += QString::number(xValues.at(j));

						for (int k = 0; k < 40; k++)
							if (_config->voltages[k] > 0)
								buffer += "," + QString::number(voltageValues.at(k).at(j));

						for (int k = 0; k < 8; k++)
							if (_config->flows[k] > 0)
								buffer += "," +  QString::number(flowValues.at(k).at(j));

						for (int k = 0; k < 8; k++)
							if (_config->temperatures[k] > 0)
								  buffer += "," + QString::number(temperaturesValues.at(k).at(j));

						buffer += "\n";
					}

					buffer += "\n";
				}

				file.write(buffer.toAscii());
				file.close();

				QMessageBox::information(this, "Ok", "Le fichier <b>" + fileName + "</b> a bien été enregistré.");
			}

			else
				QMessageBox::critical(this, "Erreur", "Impossible d'ouvrir le fichier sur le disque.");
		}
	}

	else
		QMessageBox::critical(this, "Erreur", "Il n'y a rien à exporter");
}

void MainWindow::exportPDF()
{
	CurvesSplitter *tab = dynamic_cast<CurvesSplitter*>(_tabWidget->currentWidget());

	if (tab != NULL)
		tab->exportPDF();

	else
		QMessageBox::critical(this, "Erreur", "Il n'y a rien à exporter.");
}

void MainWindow::print()
{
	CurvesSplitter *tab = dynamic_cast<CurvesSplitter*>(_tabWidget->currentWidget());

	if (tab != NULL)
		tab->print();

	else
		QMessageBox::critical(this, "Erreur", "Il n'y a rien à imprimer.");
}

void MainWindow::about()
{
	QString text = "<h3>" + APPLICATION_NAME + " " + VERSION + "</h3>"
				   "Boitier développé par Etienne Migot<br />"
				   "Programme développé par Jean-Luc Migot";

	QMessageBox::information(this, "A propos", text);
}

QString MainWindow::color(const QString &str, Color::Color color) const
{
	if (color == Color::Blue)
		return "<span style='color: #0066CC'>" + str + "</span>";

	else if (color == Color::Red)
		return "<span style='color: #FF0000'>" + str + "</span>";

	else if (color == Color::Green)
		return "<span style='color: #00FF00'>" + str + "</span>";

	return str;
}

void MainWindow::setButtonsEnabled(bool enable)
{
	if (enable)
		this->updateConnectionDeconnectionButtons();

	else
	{
		// Disable buttons and card menu items
		_connectDisconnect->setDisabled(true);
		_testConnection->setDisabled(true);

		foreach (QAction *action, _cardMenu->actions())
			action->setDisabled(true);
	}
}

void MainWindow::updatePrintMenu()
{
	bool enable = _tabWidget->count() > 0;

	_fileMenu->actions()[1]->setEnabled(enable);
	_fileMenu->actions()[2]->setEnabled(enable);
	_fileMenu->actions()[3]->setEnabled(enable);
}

void MainWindow::updateConnectionDeconnectionButtons()
{
	_connectDisconnect->setEnabled(true);

	if (_port->isConnected())
	{
		// Fill toolBar infos
		_connectDisconnect->setText(" Deconnecter ");
		_testConnection->setEnabled(true);

		// Enable card menu items
		foreach (QAction *action, _cardMenu->actions())
			action->setEnabled(true);
	}

	else
	{
		// Clear toolBar infos
		_connectDisconnect->setText(" Connecter ");
		_testConnection->setDisabled(true);
		_dateTime->setText("<h4>&nbsp;Date et heure : </h4>");
		_memoryUsage->reset();

		// Disable card menu items
		foreach (QAction *action, _cardMenu->actions())
			action->setDisabled(true);
	}
}

void MainWindow::setConnection(const QString &text, Color::Color color)
{
	_connection->setText("<h4>&nbsp;Connexion : " + this->color(text, color) + "&nbsp;</h4>");
}

void MainWindow::connectDisconnect()
{
	if (_port->isConnected())
		_port->disconnect();

	else
	{
		_connectDisconnect->setDisabled(true);
		_port->connect();

		if (_port->isConnected())
		{
			this->setConnection("Lecture de la configuration...", Color::Blue);

			if (this->readEEPROMConfig())
			{
				QString addressPointer(AddressPointer);
				int clientCode = addressPointer.right(2).toInt();

				if (clientCode != 0 && clientCode != _config->clientCode)
				{
					QMessageBox::critical(this, "Erreur", "Ce boitier n'est pas autorisé. La connection au boitier n'est pas utilisable.");
					_port->disconnect();
				}

				else
				{
					this->readEEPROMUsual();
					this->setConnection("Boitier prêt", Color::Blue);
				}
			}

			else
				_port->disconnect();
		}
	}
}

bool MainWindow::testConnection(bool showOk)
{
	if ( ! _port->isConnected())
	{
		QMessageBox::critical(this, "Erreur", "Boitier déconnecté.");
		return false;
	}

	// Test presence of card with "?" -> "#" question/answer
	_port->write("?");

	if (_port->read() != "#")
	{
		_port->disconnect();
		QMessageBox::critical(this, "Erreur", "Boitier déconnecté.");
		return false;
	}

	if (showOk)
		QMessageBox::information(this, "Ok", "Boitier présent");

	return true;
}

QString MainWindow::intToHex(int octet, int nbChars) const
{
	// Can convert an int to hex with "0" lef-padding depending of nbChars wanted
	return QString(QString("0").repeated(nbChars - 1) + QString::number(octet, 16).toUpper()).right(nbChars);
}

int MainWindow::hexToInt(const QString &hex) const
{
	bool conv;
	return hex.toInt(&conv, 16);
}

Config* MainWindow::config() const
{
	return _config;
}
