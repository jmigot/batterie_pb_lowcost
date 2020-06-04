
/**
 * MainWindow
 *
 * @date December 2010
 * @author Jean-Luc Migot (jl.migot@yahoo.fr)
 *
 * Copyright 2010-2011 Jean-Luc Migot
 */

#include "MainWindow.h"
#include "SerialPort.h"

#include <QApplication>
#include <QCheckBox>
#include <QComboBox>
#include <QDateEdit>
#include <QDebug>
#include <QDesktopServices>
#include <QDesktopWidget>
#include <QFile>
#include <QFileDialog>
#include <QFormLayout>
#include <QGridLayout>
#include <QGroupBox>
#include <QLabel>
#include <QLineEdit>
#include <QMessageBox>
#include <QPushButton>
#include <QTextEdit>
#include <QTimeEdit>
#include <QToolBar>
#include <QUrl>

#include <qmath.h>

const QString MainWindow::VERSION             = "0.9";
const int MainWindow::MAX_RETRY_WRITE_TO_FILE = 3;
const int MainWindow::MAX_RETRY_READ_EEPROM   = 3;

MainWindow::MainWindow()
{
	this->setWindowTitle("Batterie PB Lowcost - Configuration 0.9");

	// --------------------------------------------------
	//		Toolbar
	// --------------------------------------------------
	_connection        = new QLabel("<h4>Connexion : </h4>");
	_connectDisconnect = new QPushButton(" Connecter ");
	_testConnection    = new QPushButton(" Tester connexion ");
	_testConnection->setDisabled(true);
	connect(_connectDisconnect, SIGNAL(clicked()), this, SLOT(connectDisconnect()));
	connect(_testConnection, SIGNAL(clicked()), this, SLOT(testConnection()));

	QWidget* separator = new QWidget;
	separator->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);

	QToolBar* toolBar = this->addToolBar("Barre d'outils");
	toolBar->setMovable(false);
	toolBar->setMinimumHeight(50);
	toolBar->addWidget(_connection);
	toolBar->addWidget(separator);
	toolBar->addWidget(_connectDisconnect);
	toolBar->addWidget(_testConnection);

	// --------------------------------------------------
	//		Config
	// --------------------------------------------------
	_configLayout  = new QGridLayout;
	_configLayout2 = new QGridLayout;
	_configLayout3 = new QGridLayout;
	_configLayout4 = new QHBoxLayout;

	for (int i = 0; i < 40; i++)
		_voltages.append(new QComboBox);

	for (int i = 0; i < 8; i++)
	{
		_flows.append(new QComboBox);
		_temperatures.append(new QComboBox);
	}

	_eepromSize       = new QComboBox;
	_specialCommand   = new QComboBox;
	_clientCode       = new QComboBox;
	_clientName       = new QLineEdit;
	_voltageLow       = new QComboBox;
	_voltageHigh      = new QComboBox;
	_flowLow          = new QComboBox;
	_flowHigh         = new QComboBox;
	_temperatureLow   = new QComboBox;
	_temperatureHigh  = new QComboBox;

	_date             = new QDateEdit;
	_time             = new QTimeEdit;
	_updateDateTime   = new QPushButton(" Date/Heure du PC ");

	_resetDataPointer = new QCheckBox(" Reset pointeur data ");
	_resetTemperaturesPointer = new QCheckBox("Reset pointeur températures");

	// Properties
	for (int i = 0; i < 40; i++)
	{
		for (int j = 0; j < 256; j++)
			_voltages.at(i)->addItem(QString::number(j));

		_voltages.at(i)->setEditable(true);
		_voltages.at(i)->setMaxVisibleItems(20);
		_voltages.at(i)->setValidator(new QIntValidator(0, 255, _voltages.at(i)));
	}

	for (int i = 0; i < 8; i++)
	{
		for (int j = 0; j < 256; j++)
			_flows.at(i)->addItem(QString::number(j));

		_temperatures.at(i)->addItem("0");
		_temperatures.at(i)->addItem("1");
		_temperatures.at(i)->addItem("2");

		_flows.at(i)->setEditable(true);
		_flows.at(i)->setMaxVisibleItems(20);
		_flows.at(i)->setValidator(new QIntValidator(0, 255, _flows.at(i)));

		_temperatures.at(i)->setEditable(true);
		_temperatures.at(i)->setValidator(new QIntValidator(0, 255, _temperatures.at(i)));
	}

	_eepromSize->addItem("0 Demo");

	for (int i = 1; i <= 7; i++)
		_eepromSize->addItem(QString::number(i) + " LC" + QString::number(qPow(2, i+5)));

	_eepromSize->setEditable(true);
	_eepromSize->setValidator(new QIntValidator(0, 9, _eepromSize));

	for (int i = 0; i <= 15; i++)
		_specialCommand->addItem(QString::number(i));

	_specialCommand->setEditable(true);
	_specialCommand->setMaxVisibleItems(20);
	_specialCommand->setValidator(new QIntValidator(0, 15, _specialCommand));

	for (int i = 0; i <= 99; i++)
		_clientCode->addItem(QString::number(i));

	_clientCode->setEditable(true);
	_clientCode->setMaxVisibleItems(20);
	_clientCode->setValidator(new QIntValidator(0, 99, _clientCode));

	_clientName->setMaxLength(40);

	for (int i = 1; i <= 99; i++)
	{
		//_voltageHigh->addItem(QString::number(i) + "%");
		//_voltageLow->addItem(QString::number(i) + "%");
		_flowHigh->addItem(QString::number(i) + "%");
		_flowLow->addItem(QString::number(i) + "%");
	}

	_voltageLow->setEditable(true);
	_voltageLow->setMaxVisibleItems(20);
	_voltageHigh->setEditable(true);
	_voltageHigh->setMaxVisibleItems(20);
	_flowLow->setEditable(true);
	_flowLow->setMaxVisibleItems(20);
	_flowHigh->setEditable(true);
	_flowHigh->setMaxVisibleItems(20);
	_voltageLow->setValidator(new QIntValidator(50, 95, _voltageLow));
	_voltageHigh->setValidator( new QIntValidator(50, 95, _voltageHigh));
	_flowLow->setValidator( new QIntValidator(50, 95, _flowLow));
	_flowHigh->setValidator( new QIntValidator(50, 95, _flowHigh));

	for (int i = 20; i <= 150; i++)
	{
		_temperatureHigh->addItem(QString::number(i) + "°C");
		_temperatureLow->addItem(QString::number(i) + "°C");
	}

	_temperatureLow->setEditable(true);
	_temperatureLow->setMaxVisibleItems(20);
	_temperatureHigh->setEditable(true);
	_temperatureHigh->setMaxVisibleItems(20);
	_temperatureLow->setValidator(new QIntValidator(20, 150, _temperatureLow));
	_temperatureHigh->setValidator(new QIntValidator(20, 150, _temperatureHigh));

	connect(_updateDateTime, SIGNAL(clicked()), this, SLOT(updateDateTime()));
	connect(_voltages.at(0), SIGNAL(currentIndexChanged(int)), this, SLOT(updateVoltagesValues()));

	// --------------------------------------------------
	//		Direct commands
	// --------------------------------------------------
	_commandsLayout = new QHBoxLayout;
	_output         = new QTextEdit;
	_input          = new QLineEdit;
	_cr             = new QPushButton(" CR ");
	_lf             = new QPushButton(" LF ");
	_clearConsole   = new QPushButton(" Effacer console ");

	_commandsLayout->addWidget(_input);
	_commandsLayout->addWidget(_cr);
	_commandsLayout->addWidget(_lf);
	_commandsLayout->addWidget(_clearConsole);

	connect(_input, SIGNAL(returnPressed()), this, SLOT(directCommand()));
	connect(_cr, SIGNAL(clicked()), this, SLOT(cr()));
	connect(_lf, SIGNAL(clicked()), this, SLOT(lf()));

	// --------------------------------------------------
	//		Buttons
	// --------------------------------------------------
	_buttonsLayout = new QHBoxLayout;
	_openFile      = new QPushButton(QIcon(":/images/open"), "Ouvrir");
	_writeFile     = new QPushButton(QIcon(":/images/save"), "Enregistrer");
	_readEEPROM    = new QPushButton(QIcon(":/images/apply_blue"), "Lire EEPROM");
	_writeEEPROM   = new QPushButton(QIcon(":/images/apply_green"), "Ecrire EEPROM");
	_clearData     = new QPushButton(QIcon(":/images/delete"), "Effacer data");
	_clearTemperatures = new QPushButton(QIcon(":/images/delete"), "Effacer temperatures");
	_help          = new QPushButton(QIcon(":/images/help"), "Aide");

	_readEEPROM->setEnabled(false);
	_writeEEPROM->setEnabled(false);
	_clearData->setEnabled(false);
	_clearTemperatures->setEnabled(false);

	_buttonsLayout->setAlignment(Qt::AlignTop);
	_buttonsLayout->addWidget(_openFile);
	_buttonsLayout->addWidget(_writeFile);
	_buttonsLayout->addWidget(_readEEPROM);
	_buttonsLayout->addWidget(_writeEEPROM);
	_buttonsLayout->addWidget(_clearData);
	_buttonsLayout->addWidget(_clearTemperatures);
	_buttonsLayout->addWidget(_help);

	connect(_openFile, SIGNAL(clicked()), this, SLOT(openFile()));
	connect(_writeFile, SIGNAL(clicked()), this, SLOT(writeFile()));
	connect(_readEEPROM, SIGNAL(clicked()), this, SLOT(readEEPROM()));
	connect(_writeEEPROM, SIGNAL(clicked()), this, SLOT(writeEEPROM()));
	connect(_clearData, SIGNAL(clicked()), this, SLOT(clearData()));
	connect(_clearTemperatures, SIGNAL(clicked()), this, SLOT(clearTemperatures()));
	connect(_clearConsole, SIGNAL(clicked()), _output, SLOT(clear()));
	connect(_help, SIGNAL(clicked()), this, SLOT(help()));

	// --------------------------------------------------
	//		Main widget
	// --------------------------------------------------
	_widget           = new QWidget;
	_layout           = new QVBoxLayout(_widget);

	this->setCentralWidget(_widget);

	_layout->addLayout(_configLayout);
	_layout->addLayout(_configLayout2);
	_layout->addLayout(_configLayout3);
	_layout->addLayout(_configLayout4);
	_layout->addWidget(_output);
	_layout->addLayout(_commandsLayout);
	_layout->addLayout(_buttonsLayout);

	for (int i = 0; i < 20; i++)
	{
		_configLayout->addWidget(new QLabel("V" + QString::number(i+1)), 0, i);
		_configLayout->addWidget(_voltages.at(i), 1, i);
	}

	for (int i = 20; i < 40; i++)
	{
		_configLayout->addWidget(new QLabel("V" + QString::number(i+1)), 2, i - 20);
		_configLayout->addWidget(_voltages.at(i), 3, i - 20);
	}

	for (int i = 0; i < 8; i++)
	{
		_configLayout->addWidget(new QLabel("I" + QString::number(i+1)), 4, i);
		_configLayout->addWidget(_flows.at(i), 5, i);
	}

	for (int i = 0; i < 8; i++)
	{
		_configLayout->addWidget(new QLabel("T" + QString::number(i+1)), 6, i);
		_configLayout->addWidget(_temperatures.at(i), 7, i);
	}

	_configLayout2->addWidget(new QLabel("Taille EEPROM"), 0, 0);
	_configLayout2->addWidget(_eepromSize, 1, 0);
	_configLayout2->addWidget(new QLabel("Commande spéciale"), 0, 1);
	_configLayout2->addWidget(_specialCommand, 1, 1);
	_configLayout2->addWidget(new QLabel("Code client"), 0, 2);
	_configLayout2->addWidget(_clientCode, 1, 2 );
	_configLayout2->addWidget(new QLabel("Nom client"), 0, 3);
	_configLayout2->addWidget(_clientName, 1, 3);

	_configLayout3->addWidget(new QLabel("Seul tension bas"), 0, 0);
	_configLayout3->addWidget(_voltageLow, 1, 0);
	_configLayout3->addWidget(new QLabel("Seuil tension haut"), 0, 1);
	_configLayout3->addWidget(_voltageHigh, 1, 1);
	_configLayout3->addWidget(new QLabel("Seuil courant bas"), 0, 2);
	_configLayout3->addWidget(_flowLow, 1, 2);
	_configLayout3->addWidget(new QLabel("Seuil courant haut"), 0, 3);
	_configLayout3->addWidget(_flowHigh, 1, 3);
	_configLayout3->addWidget(new QLabel("Seuil température bas"), 0, 4);
	_configLayout3->addWidget(_temperatureLow, 1, 4);
	_configLayout3->addWidget(new QLabel("Seuil température haut"), 0, 5);
	_configLayout3->addWidget(_temperatureHigh, 1, 5);

	_configLayout4->addWidget(_date);
	_configLayout4->addWidget(_time);
	_configLayout4->addWidget(_updateDateTime);
	_configLayout4->addWidget(_resetDataPointer);
	_configLayout4->addWidget(_resetTemperaturesPointer);

	// --------------------------------------------------
	//		Size and placement of main window
	// --------------------------------------------------
	QDesktopWidget desktop;

	int screenWidth  = desktop.screenGeometry().width();
	int screenHeight = desktop.screenGeometry().height();

	// If screen is too small, we show application in maximized mode "full screen"
	if (screenWidth <= 1024 || screenHeight <= 768)
		this->showMaximized();

	// Else application is adjusted
	else
	{
		this->adjustSize();

		int x = (screenWidth - this->width()) / 2;
		int y = (screenHeight - this->height()) / 2;
		this->move(x, y);
		this->show();
	}

	// --------------------------------------------------
	//		Serial port
	// --------------------------------------------------
	_port = new SerialPort(this);
	this->connectDisconnect();
}

MainWindow::~MainWindow()
{
	#ifndef QT_NO_DEBUG
		qDebug() << "~MainWindow()";
	#endif

	if (_port != NULL)
		delete _port;
}

void MainWindow::write(const QString &command)
{
	_output->append("[Envoi] " + command);
	_port->write(command);
}

QByteArray MainWindow::read()
{
	QByteArray response = _port->read();
	_output->append(response);

	return response;
}

void MainWindow::updateVoltagesValues(int voltageLow, int voltageHigh)
{
	if (voltageLow == -1)
		voltageLow = _voltageLow->currentIndex() + 1;

	if (voltageHigh == -1)
		voltageHigh = _voltageHigh->currentIndex() + 1;

	int v1 = _voltages.at(0)->currentIndex();

	_voltageLow->clear();
	_voltageHigh->clear();

	for (int i = 1; i < 99; i++)
	{
		_voltageLow->addItem(QString::number(v1 * i / 100.0, 'f', 1) + " V");
		_voltageHigh->addItem(QString::number(v1 * i / 100.0, 'f', 1) + " V");
	}

	_voltageLow->setCurrentIndex(voltageLow - 1);
	_voltageHigh->setCurrentIndex(voltageHigh - 1);
}

void MainWindow::updateDateTime()
{
	_date->setDate(QDate::currentDate());
	_time->setTime(QTime::currentTime());
}

QString MainWindow::color(const QString &str, Color::Color color) const
{
	if (color == Color::Blue)
		return "<span style='color: #0066CC'>" + str + "</span>";

	else if (color == Color::Red)
		return "<span style='color: #FF0000'>" + str + "</span>";

	return str;
}

void MainWindow::updateConnectionDeconnectionButtons()
{
	_connectDisconnect->setEnabled(true);

	if (_port->isConnected())
	{
		_connectDisconnect->setText(" Deconnecter ");
		_testConnection->setEnabled(true);
		_readEEPROM->setEnabled(true);
		_writeEEPROM->setEnabled(true);
		_clearData->setEnabled(true);
		_clearTemperatures->setEnabled(true);
	}

	else
	{
		_connectDisconnect->setText(" Connecter ");
		_testConnection->setDisabled(true);
		_readEEPROM->setEnabled(false);
		_writeEEPROM->setEnabled(false);
		_clearData->setEnabled(false);
		_clearTemperatures->setEnabled(false);
	}
}

void MainWindow::setConnection(const QString &text, Color::Color color)
{
	_connection->setText("<h4>&nbsp;Connexion : " + this->color(text, color) + "&nbsp;</h4>");
}

void MainWindow::directCommand()
{
	if (_port->isConnected())
	{
		this->write(_input->text().replace("\\r", "\r").replace("\\n", "\n"));
		this->read();
	}

	else
		QMessageBox::critical(this, "Erreur", "Le boitier n'est pas connecté");
}

void MainWindow::cr()
{
	_input->setText(_input->text() + "\\r");
}

void MainWindow::lf()
{
	_input->setText(_input->text() + "\\n");
}

QString MainWindow::extract(const QString &line)
{
	QRegExp regex("^(.*) ");
	regex.setMinimal(true);

	if (line.contains(regex))
		return regex.cap(1);

	return QString();
}

void MainWindow::openFile()
{
	QString fileName = QFileDialog::getOpenFileName(this, "Ouvrir un fichier", "", "Fichiers texte (*.txt)");
	QStringList datas;

	if ( ! fileName.isEmpty())
	{
		QFile file(fileName);

		if (file.open(QFile::ReadOnly | QFile::Text) && file.isReadable())
		{
			datas = QString(file.readAll()).split("\n");
			file.close();

			if (datas.count() >= 68)
			{
				// Parse data
				_clientName->setText(this->extract(datas.at(0)));

				for (int i = 0; i < 40; i++)
					_voltages.at(i)->setCurrentIndex(this->extract(datas.at(i + 1)).toInt());

				for (int i = 0; i < 8; i++)
					_flows.at(i)->setCurrentIndex(this->extract(datas.at(40 + i + 1)).toInt());

				for (int i = 0; i < 8; i++)
					_temperatures.at(i)->setCurrentIndex(this->extract(datas.at(48 + i + 1)).toInt());

				_specialCommand->setCurrentIndex(this->extract(datas.at(57)).toInt());
				_eepromSize->setCurrentIndex(this->extract(datas.at(58)).toInt());
				_clientCode->setCurrentIndex(this->extract(datas.at(59)).toInt());
				this->updateVoltagesValues(this->extract(datas.at(60)).remove("%").toInt(), this->extract(datas.at(61)).remove("%").toInt());
				_flowLow->setCurrentIndex(_flowLow->findText(this->extract(datas.at(62))));
				_flowHigh->setCurrentIndex(_flowHigh->findText(this->extract(datas.at(63))));
				_temperatureLow->setCurrentIndex(_temperatureLow->findText(this->extract(datas.at(64))));
				_temperatureHigh->setCurrentIndex(_temperatureHigh->findText(this->extract(datas.at(65))));

				_resetDataPointer->setChecked(this->extract(datas.at(66)) == "true");
				_resetTemperaturesPointer->setChecked(this->extract(datas.at(67)) == "true");

				QMessageBox::information(this, "Fichier lu", "Le fichier a bien été lu");
			}

			else
				QMessageBox::critical(this, "Erreur", "Il manque des informations dans le fichier");
		}

		else
			QMessageBox::critical(this, "Erreur lecture d'un fichier", "Le fichier n'a pas pu être ouvert (" + file.errorString() + ")");
	}
}

void MainWindow::writeFile()
{
	QString fileName = QFileDialog::getSaveFileName(this, "Enregistrer un fichier", "", "Fichiers texte (*.txt)");
	QString data     = _clientName->text() + " // Nom du client\n";

	if ( ! fileName.isEmpty())
	{
		// Prepare data
		for (int i = 0; i < 40; i++)
			data += _voltages.at(i)->currentText() + " // Tension " + QString::number(i + 1) + "\n";

		for (int i = 0; i < 8; i++)
			data += _flows.at(i)->currentText() + " // Courant " + QString::number(i + 1) + "\n";

		for (int i = 0; i < 8; i++)
			data += _temperatures.at(i)->currentText() + " // Température " + QString::number(i + 1) + "\n";

		data += _specialCommand->currentText() + " // Commande spéciale\n";
		data += _eepromSize->currentText().split(" ").at(0) + " // Taille EEPROM\n";
		data += _clientCode->currentText() + " // Code client\n";
		data += QString::number(_voltageLow->currentIndex() + 1) + "% // Tension bas\n";
		data += QString::number(_voltageHigh->currentIndex() + 1) + "% // Tension haut\n";
		data += _flowLow->currentText() + " // Courant bas\n";
		data += _flowHigh->currentText() + " // Courant haut\n";
		data += _temperatureLow->currentText() + " // Température bas\n";
		data += _temperatureHigh->currentText() + " // Température haut\n";

		data += (_resetDataPointer->isChecked() ? "true" : "false");
		data += " // Reset pointeur data\n";

		data += (_resetTemperaturesPointer->isChecked() ? "true" : "false");
		data += " // Reset pointeur températures";

		// Write file
		QFile file(fileName);

		if (file.open(QFile::WriteOnly | QFile::Text) && file.isWritable())
		{
			int tries = 0;

			while (file.write(data.toAscii()) == -1 && tries < MAX_RETRY_WRITE_TO_FILE)
			{
				tries++;
				QApplication::processEvents();
			}

			file.close();

			if (tries != MAX_RETRY_WRITE_TO_FILE)
				QMessageBox::information(this, "Fichier enregistré", "Le fichier a bien été enregistré");
		}

		else
			QMessageBox::critical(this, "Erreur enregistrement d'un fichier", "Le fichier n'a pas pu être ouvert (" + file.errorString() + ")");
	}
}

void MainWindow::readEEPROM()
{
	int tries    = 0;
	bool ok      = false;
	bool resultC = false;
	bool resultH = false;
	QString response;

	do
	{
		this->write("C");
		response = this->read();

		if (response.length() == 128 + 2)
		{
			for (int i = 0; i < 40; i++)
				_voltages.at(i)->setCurrentIndex(_voltages.at(i)->findText(QString::number(this->hexToInt(response.mid(i * 2, 2)))));

			for (int i = 0; i < 8; i++)
				_flows.at(i)->setCurrentIndex(_flows.at(i)->findText(QString::number(this->hexToInt(response.mid(80 + i * 2, 2)))));

			for (int i = 0; i < 8; i++)
				_temperatures.at(i)->setCurrentIndex(_temperatures.at(i)->findText(QString::number(this->hexToInt(response.mid(96 + i * 2, 2)))));

			_specialCommand->setCurrentIndex(this->hexToInt(response.mid(112, 1)));
			_eepromSize->setCurrentIndex(this->hexToInt(response.mid(113, 1)));
			_clientCode->setCurrentIndex(this->hexToInt(response.mid(114, 2)));
			this->updateVoltagesValues(this->hexToInt(response.mid(116, 2)), this->hexToInt(response.mid(118, 2)));
			_flowLow->setCurrentIndex(_flowLow->findText(QString::number(this->hexToInt(response.mid(120, 2))) + "%"));
			_flowHigh->setCurrentIndex(_flowHigh->findText(QString::number(this->hexToInt(response.mid(122, 2))) + "%"));
			_temperatureLow->setCurrentIndex(_temperatureLow->findText(QString::number(this->hexToInt(response.mid(124, 2))) + "°C"));
			_temperatureHigh->setCurrentIndex(_temperatureHigh->findText(QString::number(this->hexToInt(response.mid(126, 2))) + "°C"));

			resultC = true;
		}

		else
			tries++;
	} while (response.length() != 130 && tries < MAX_RETRY_READ_EEPROM);

	if (tries == MAX_RETRY_READ_EEPROM)
		QMessageBox::critical(this, "Erreur de lecture EEPROM", "Impossible de lire la configuration (C) dans l'EEPROM après " + QString::number(MAX_RETRY_READ_EEPROM) + " essais");

	tries    = 0;
	response = "";

	do
	{
		this->write("H");
		response = this->read();

		if (response.length() == 18)
		{
			// --------------------------------------------------
			//		Date and time
			// --------------------------------------------------
			int yearAndDays = response.mid(6, 2).toInt(&ok, 16);
			int year        = QString("20" + QString("0" + QString::number((yearAndDays & 0x7F))).right(2)).toInt();
			int days        = response.mid(8, 2).toInt(&ok, 16);

			if (yearAndDays & 0x80)
				days += 255;

			int hours       = response.mid(10, 2).toInt(&ok, 16);
			int minutes     = response.mid(12, 2).toInt(&ok, 16);
			int seconds     = response.mid(14, 2).toInt(&ok, 16);

			_date->setDate(QDate(year, 1, 1).addDays(days - 1));
			_time->setTime(QTime(hours, minutes, seconds));

			resultH = true;
		}

		else
			tries++;
	} while (response.length() != 18 && tries < MAX_RETRY_READ_EEPROM);

	if (tries == MAX_RETRY_READ_EEPROM)
		QMessageBox::critical(this, "Erreur de lecture EEPROM", "Impossible de lire l'heure (H) dans l'EEPROM après " + QString::number(MAX_RETRY_READ_EEPROM) + " essais");

	if (resultC && resultH)
		QMessageBox::information(this, "Valeurs lues dans l'EEPROM", "Les valeurs ont bien été lues dans l'EEPROM");
}

void MainWindow::writeEEPROM()
{
	QString config   = "W";
	QString dateTime = "M";

	// Prepare config
	for (int i = 0; i < 40; i++)
		config += this->intToHex(_voltages.at(i)->currentText().toInt());

	for (int i = 0; i < 8; i++)
		config += this->intToHex(_flows.at(i)->currentText().toInt());

	for (int i = 0; i < 8; i++)
		config += this->intToHex(_temperatures.at(i)->currentText().toInt());

	config += this->intToHex(_specialCommand->currentText().toInt() * 16 | _eepromSize->currentText().split(" ").at(0).toInt());
	config += this->intToHex(_clientCode->currentText().toInt());
	config += this->intToHex(_voltageLow->currentIndex() + 1);
	config += this->intToHex(_voltageHigh->currentIndex() + 1);
	config += this->intToHex(_flowLow->currentText().remove("%").toInt());
	config += this->intToHex(_flowHigh->currentText().remove("%").toInt());
	config += this->intToHex(_temperatureLow->currentText().remove("°C").toInt());
	config += this->intToHex(_temperatureHigh->currentText().remove("°C").toInt());

	// Prepare date and time
	dateTime += this->intToHex(QString::number(_date->date().year()).right(2).toInt() | ((_date->date().dayOfYear() > 255) ? 128 : 0));
	dateTime += this->intToHex((_date->date().dayOfYear() > 255) ? _date->date().dayOfYear() - 255 : _date->date().dayOfYear());
	dateTime += this->intToHex(_time->time().hour());
	dateTime += this->intToHex(_time->time().minute());

	int tries   = 0;
	bool result = true;

	do
	{
		// Send config
		this->write(config + "\r");

		if (this->read() != "W\r\n")
			result = false;

		// Reset data pointer
		if (_resetDataPointer->isChecked())
		{
			this->write("R");

			if (this->read() != "R\r\n")
				result = false;
		}

		// Reset temperatures pointer
		if (_resetTemperaturesPointer->isChecked())
		{
			this->write("B");

			if (this->read() != "B\r\n")
				result = false;
		}

		// Send date and time
		this->write(dateTime);

		if (this->read() != "M\r\n")
			result = false;

		// If all responses were received, we can check EEPROM
		if (result)
		{
			QString response;

			// Pointer
			if (_resetDataPointer->isChecked())
			{
				this->write("P");
				response = this->read();

				if (response != "000080\r\n")
					result = false;
			}

			if (_resetTemperaturesPointer->isChecked())
			{
				this->write("A");
				response = this->read();

				if ( ! response.contains(QRegExp("0{32}.{64}\r\n")))
					result = false;
			}


			// Configuration
			this->write("C");
			response = this->read();

			if (response.length() == 130 && response.mid(0, 128) != config.mid(1, 128))
				result = false;

			// Date and time
			this->write("H");
			response = this->read();

			if (response.length() == 18 && response.mid(6, 8) != dateTime.mid(1, 8))
				result = false;
		}

		if( ! result)
			tries++;
	} while ( ! result && tries < MAX_RETRY_READ_EEPROM);

	if (tries == MAX_RETRY_READ_EEPROM)
		QMessageBox::critical(this, "Erreur d'écriture EEPROM", "Impossible d'écrire correctement la configuration et/ou l'heure dans l'EEPROM après " + QString::number(MAX_RETRY_READ_EEPROM) + " essais");

	else
	{
		this->write("Z");
		QMessageBox::information(this, "Valeurs écrites dans l'EEPROM", "Les valeurs ont bien été écrites dans l'EEPROM");
	}
}

void MainWindow::clearData()
{
	this->write("R");

	if (this->read() == "R\r\n")
	{
		this->write("P");

		if (this->read() == "000080\r\n")
			QMessageBox::information(this, "Ok", "La remise à 0 est effective");

		else
			QMessageBox::critical(this, "Erreur", "Le pointeur n'a pas été remis à 0");
	}

	else
		QMessageBox::critical(this, "Erreur", "Aucune réponse du boitier");
}

void MainWindow::clearTemperatures()
{
	this->write("B");

	if (this->read() == "B\r\n")
	{
		this->write("A");

		if (QString(this->read()).contains(QRegExp("0{32}[A-Za-z0-9]{64}\r\n")))
			QMessageBox::information(this, "Ok", "La remise à 0 est effective");

		else
			QMessageBox::critical(this, "Erreur", "Les temperatures n'ont pas été remises à 0");
	}

	else
		QMessageBox::critical(this, "Erreur", "Aucune réponse du boitier");
}

void MainWindow::help()
{
	QFile file(QApplication::applicationDirPath() + "/Documentation.pdf");

	if ( ! file.exists())
		QMessageBox::critical(this, "Erreur", "Le fichier <b>Documentation.pdf</b> n'existe pas");

	else if ( ! QDesktopServices::openUrl(QUrl("file:///" + QApplication::applicationDirPath() + "/Documentation.pdf")))
		QMessageBox::critical(this, "Erreur", "Impossible de lancer le fichier <b>" + QApplication::applicationDirPath() + "/Documentation.pdf</b> dans le lecteur PDF par défaut");
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
			this->readEEPROM();
	}
}

void MainWindow::testConnection()
{
	this->write("?");

	if (this->read() != "#")
	{
		_port->disconnect();
		QMessageBox::critical(this, "Boitier déconnecté", "Le boitier est déconnecté");
	}

	else
		QMessageBox::information(this, "Boitier présent", "Le boitier est toujours présent");
}

QString MainWindow::intToHex(int octet)
{
	return QString("0" + QString::number(octet, 16).toUpper()).right(2);
}

int MainWindow::hexToInt(const QString &hex)
{
	bool conv;
	return hex.toInt(&conv, 16);
}
