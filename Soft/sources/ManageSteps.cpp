
/**
 * Widget to manage steps
 *
 * @date December 2010
 * @author Jean-Luc Migot (jl.migot@yahoo.fr)
 *
 * Copyright 2010-2011 Jean-Luc Migot
 */

#include "ManageSteps.h"
#include "MainWindow.h"

#include <QComboBox>
#include <QDebug>
#include <QDesktopWidget>
#include <QFormLayout>
#include <QLabel>
#include <QMessageBox>
#include <QPushButton>

ManageSteps::ManageSteps(MainWindow *window)
	: _window(window)
{
	// Members
	_layout          = new QVBoxLayout(this);
	_formLayout      = new QFormLayout;
	_voltageLow      = new QComboBox;
	_voltageHigh     = new QComboBox;
	_flowLow         = new QComboBox;
	_flowHigh        = new QComboBox;
	_temperatureLow  = new QComboBox;
	_temperatureHigh = new QComboBox;
	_buttonsLayout   = new QHBoxLayout;
	_validate        = new QPushButton(QIcon(":/images/apply"), "Valider");
	_cancel          = new QPushButton(QIcon(":/images/cross"), "Annuler");

	// Properties
	this->setWindowTitle("Réglage des seuils");
	this->setWindowModality(Qt::ApplicationModal);
	this->setAttribute(Qt::WA_DeleteOnClose);

	int v1 = _window->config()->voltages.at(0);

	for (int i = 1; i <= 99; i++)
	{
		_voltageHigh->addItem(QString::number(v1 * i / 100.0, 'f', 1) + " V");
		_voltageLow->addItem(QString::number(v1 * i / 100.0, 'f', 1) + " V");
		_flowHigh->addItem(QString::number(i) + "%");
		_flowLow->addItem(QString::number(i) + "%");
	}

	for (int i = 20; i <= 120; i++)
	{
		_temperatureHigh->addItem(QString::number(i) + "°C");
		_temperatureLow->addItem(QString::number(i) + "°C");
	}

	// Properties
	_voltageLow->setMaxVisibleItems(20);
	_voltageLow->setCurrentIndex(_voltageLow->findText(QString::number(v1 * _window->config()->voltageLow / 100.0, 'f', 1) + " V"));

	_voltageHigh->setMaxVisibleItems(20);
	_voltageHigh->setCurrentIndex(_voltageHigh->findText(QString::number(v1 * _window->config()->voltageHigh / 100.0, 'f', 1) + " V"));

	_flowLow->setMaxVisibleItems(20);
	_flowLow->setCurrentIndex(_flowLow->findText(QString::number(_window->config()->flowLow) + "%"));

	_flowHigh->setMaxVisibleItems(20);
	_flowHigh->setCurrentIndex(_flowHigh->findText(QString::number(_window->config()->flowHigh) + "%"));

	_temperatureLow->setMaxVisibleItems(20);
	_temperatureLow->setCurrentIndex(_temperatureLow->findText(QString::number(_window->config()->temperatureLow) + "°C"));

	_temperatureHigh->setMaxVisibleItems(20);
	_temperatureHigh->setCurrentIndex(_temperatureHigh->findText(QString::number(_window->config()->temperatureHigh) + "°C"));

	// Connects
	connect(_validate, SIGNAL(clicked()), this, SLOT(save()));
	connect(_cancel, SIGNAL(clicked()), this, SLOT(close()));

	// Layouts
	_layout->addLayout(_formLayout);

	if (_window->config()->nbVoltages > 0)
	{
		_formLayout->addRow("Seuil tension bas : ", _voltageLow);
		_formLayout->addRow("Seuil tension haut : ", _voltageHigh);
		_formLayout->addWidget(new QLabel("<hr />"));
	}

	if (_window->config()->nbFlows > 0)
	{
		_formLayout->addRow("Seuil courant bas : ", _flowLow);
		_formLayout->addRow("Seuil courant haut : ", _flowHigh);
		_formLayout->addWidget(new QLabel("<hr />"));
	}

	if (_window->config()->nbTemperatures > 0)
	{
		_formLayout->addRow("Seuil température bas : ", _temperatureLow);
		_formLayout->addRow("Seuil température haut : ", _temperatureHigh);
	}

	_layout->addLayout(_buttonsLayout);

	_buttonsLayout->addWidget(_validate);
	_buttonsLayout->addWidget(_cancel);

	// Show widget
	this->adjustSize();

	// Calculate coordinates of center of the screen
	QDesktopWidget desktop;
	int x = (desktop.screenGeometry().width() - this->width()) / 2;
	int y = (desktop.screenGeometry().height() - this->height()) / 2;

	// Move widget on center of the screen and show it
	this->move(x, y);
	this->show();
}

ManageSteps::~ManageSteps()
{
	#ifndef QT_NO_DEBUG
		qDebug() << "~ManageSteps()";
	#endif
}

void ManageSteps::save()
{
	this->setDisabled(true);
	bool result = true;

	// Voltages
	if (_window->config()->nbVoltages > 0)
	{
		result = result && (_window->tryRead("E003A" + _window->intToHex(_voltageLow->currentIndex() + 1), 3) == "E\r\n");
		result = result && (_window->tryRead("E003B" + _window->intToHex(_voltageHigh->currentIndex() + 1), 3) == "E\r\n");
	}

	// Flows
	if (_window->config()->nbFlows > 0)
	{
		result = result && (_window->tryRead("E003C" + _window->intToHex(_flowLow->currentText().remove("%").toInt()), 3) == "E\r\n");
		result = result && (_window->tryRead("E003D" + _window->intToHex(_flowHigh->currentText().remove("%").toInt()), 3) == "E\r\n");
	}

	// Temperatures
	if (_window->config()->nbTemperatures > 0)
	{
		result = result && (_window->tryRead("E003E" + _window->intToHex(_temperatureLow->currentText().remove("°C").toInt()), 3) == "E\r\n");
		result = result && (_window->tryRead("E003F" + _window->intToHex(_temperatureHigh->currentText().remove("°C").toInt()), 3) == "E\r\n");
	}

	// Apply changes hardwarly
	if (result)
		result = (_window->tryRead("S", 3) == "S\r\n");

	if (result)
	{
		_window->readEEPROMConfig();
		QMessageBox::information(this, "Seuils écrits", "Les seuils ont bien été écrits dans le boitier");
		this->close();
	}

	else
	{
		QMessageBox::critical(this, "Erreur", "Impossible d'écrire correctement les seuils dans le boitier");
		this->setEnabled(true);
	}
}
