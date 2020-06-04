
/**
 * Serial port connection
 *
 * @date Monday, 05th August 2010
 * @author Jean-Luc Migot (jl.migot@yahoo.fr)
 *
 * Copyright 2010-2011 Jean-Luc Migot
 */

#include "SerialPort.h"
#include "MainWindow.h"

#include <QextSerialPort/qextserialenumerator.h>
#include <QextSerialPort/qextserialport.h>

#include <QApplication>
#include <QDebug>
#include <QTime>

const int SerialPort::_TIMEOUT                 = 2; // Read/Write timeout for serial port (seconds)
const long SerialPort::_QEXTSERIALPORT_TIMEOUT = 10; // QextSerialPort internal Read/Write timeout for serial port (milliseconds)
const int SerialPort::_NB_ITERATIONS_CARD_FEED = 15; // Number of iterations for card feed (how many '?' to send to reset card buffers)

SerialPort::SerialPort(MainWindow *window)
{
	_window       = window;
	_port         = new QextSerialPort();
	_connected    = false;
}

SerialPort::~SerialPort()
{
	#ifndef QT_NO_DEBUG
		qDebug() << "~SerialPort()";
	#endif

	if (_port != NULL)
	{
		_port->close();
		delete _port;
	}
}

QList<QextPortInfo> SerialPort::getPorts()
{
	QList<QextPortInfo> ports;

	// Get all detected ports (then they are in top of the list)
	ports = QextSerialEnumerator::getPorts();

	QextPortInfo temp;
	bool add = true;

	// Add all ports from COM1 to COM256 that are not already in the list
	for (int i=1; i<=256; i++)
	{
		if (i<10)
		{
			temp.portName = "COM" + QString::number(i);
			temp.physName = "COM" + QString::number(i);
		}

		else
		{
			temp.portName = "\\\\.\\COM" + QString::number(i);
			temp.physName = "\\\\.\\COM" + QString::number(i);
		}

		temp.friendName = "Serial port COM" + QString::number(i);
		temp.enumName = "";

		add = true;

		for (int j = 0; j < ports.size(); j++)
		{
			QString portName1(ports.at(j).portName.toLocal8Bit().constData());
			QString portName2(temp.portName.toLocal8Bit().constData());

			if (portName1 == portName2)
			{
				add = false;
				break;
			}
		}

		if (add)
			ports << temp;
	}

	return ports;
}

void SerialPort::connect()
{
	if ( ! this->isConnected())
	{
		QList<QextPortInfo> ports = this->getPorts();
		QString response;
		int i                     = 0;

		_window->setConnection("Connexion sur port série en cours...", Color::Blue);

		// Test all serial ports detected
		while (i < ports.size() && ! this->isConnected())
		{
			_port->setPortName(ports.at(i).portName.toLocal8Bit().constData());
			_port->open(QIODevice::ReadWrite);
			_port->setFlowControl(FLOW_OFF);
			_port->setParity(PAR_NONE);
			_port->setDataBits(DATA_8);
			_port->setStopBits(STOP_1);
			_port->setBaudRate(BAUD38400);
			_port->setDtr(false);
			_port->setRts(false);
			_port->setTimeout(_QEXTSERIALPORT_TIMEOUT);

			if (_port->isOpen() && _port->isReadable() && _port->isWritable())
			{
				int j = 0;

				_port->write("????\r");

				// _NB_ITERATIONS_CARD_FEED + 1 tests
				while (j <= _NB_ITERATIONS_CARD_FEED && ! this->isConnected())
				{
					_port->write("?", 1);

					// Clear computer write/read buffers
					response = _port->readAll();

					// If response is Ok
					if (response == "#")
					{
						_connected = true;
						this->feedCardBuffer();
						_window->setConnection("Connecté sur " + ports.at(i).portName.simplified().remove("\\").remove("."), Color::Blue);
					}

					j++;
					QApplication::processEvents();
				}
			}

			if ( ! this->isConnected())
				_port->close();

			i++;
			QApplication::processEvents();
		}

		// If connected after ports scan, clear card and computer buffers
		if( ! this->isConnected())
			this->disconnect();
	}

	_window->updateConnectionDeconnectionButtons();
}

void SerialPort::feedCardBuffer()
{
	if (this->isConnected())
	{
		// Clear card buffers
		for (int i = 0; i < _NB_ITERATIONS_CARD_FEED; i++)
		{
			// Write command
			this->write("?");

			// Clear computer write/read buffers
			_port->flush();
			this->read();

			QApplication::processEvents();
		}
	}
}

bool SerialPort::isConnected()
{
	return _connected && _port != NULL && _port->isOpen();
}

void SerialPort::write(const QString &command)
{
	if (this->isConnected() && _port->isWritable())
	{
		// Clear PC read/write buffers
		_port->flush();
		_port->readAll();

		// Send each char of the command with a little time between them (card requirement)
		for (int i = 0; i < command.length(); i++)
		{
			// Send char
			QTime dieTime = QTime::currentTime().addSecs(_TIMEOUT);

			while ( ! _port->putChar(command[i].toAscii()) && QTime::currentTime() < dieTime)
				QCoreApplication::processEvents();

			// Wait 10 ms
			dieTime = QTime::currentTime().addMSecs(10);

			while (QTime::currentTime() <= dieTime)
				QCoreApplication::processEvents();
		}
	}
}

QByteArray SerialPort::read()
{
	QByteArray response;

	if (this->isConnected() && _port->isReadable())
	{
		QTime dieTime = QTime::currentTime().addSecs(_TIMEOUT);

		while (response.length() == 0 && QTime::currentTime() < dieTime)
		{
			response = _port->readAll();
			QCoreApplication::processEvents();
		}
	}

	return response;
}

void SerialPort::disconnect()
{
	_port->close();
	_connected = false;
	_window->setConnection("Déconnecté, boitier non trouvé", Color::Red);
	_window->updateConnectionDeconnectionButtons();
}
