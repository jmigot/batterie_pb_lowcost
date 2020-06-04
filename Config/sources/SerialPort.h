
/**
 * Header of SerialPort.cpp
 *
 * @date Monday, 05th August 2010
 * @author Jean-Luc Migot (jl.migot@yahoo.fr)
 *
 * Copyright 2010-2011 Jean-Luc Migot
 */

#ifndef SERIAL_PORT_H
#define SERIAL_PORT_H

#include <QObject>

class QextPortInfo;
class QextSerialPort;

class MainWindow;

class SerialPort: public QObject
{
	Q_OBJECT

	private:
		MainWindow*     _window;
		QextSerialPort* _port;
		bool            _connected;

	public:
		static const int  _TIMEOUT;
		static const long _QEXTSERIALPORT_TIMEOUT;
		static const int  _NB_ITERATIONS_CARD_FEED;

		SerialPort(MainWindow *window);
		~SerialPort();

		QList<QextPortInfo> getPorts();
		bool                isConnected();
		void                write(const QString &command);
		QByteArray          read();
		void                feedCardBuffer();

	public slots:
		void                connect();
		void                disconnect();
};

#endif
