
/**
 * SerialPort connection abstract class
 *
 * @date December 2010
 * @author Jean-Luc Migot (jl.migot@yahoo.fr)
 *
 * Copyright 2010-2011 Jean-Luc Migot
 */

#ifndef SERIAL_PORT_H
#define SERIAL_PORT_H

#include <QextSerialPort/qextserialenumerator.h>
#include <QextSerialPort/qextserialport.h>

#include <QObject>

class MainWindow;

class SerialPort: public QObject
{
	Q_OBJECT

	private:
		MainWindow*     _window;
		QextSerialPort* _port;
		bool            _connected;

	public:
		static const int  TIMEOUT;
		static const long QEXTSERIALPORT_TIMEOUT;
		static const int  NB_ITERATIONS_CARD_FEED;

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
