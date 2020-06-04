
/**
 * MainWindow
 *
 * @date December 2010
 * @author Jean-Luc Migot (jl.migot@yahoo.fr)
 *
 * Copyright 2010-2011 Jean-Luc Migot
 */

#ifndef MAIN_WINDOW_H
#define MAIN_WINDOW_H

#include <QMainWindow>

class SerialPort;

class QCheckBox;
class QComboBox;
class QDateEdit;
class QGridLayout;
class QHBoxLayout;
class QLabel;
class QLineEdit;
class QPushButton;
class QTextEdit;
class QTimeEdit;
class QVBoxLayout;

namespace Color
{
	enum Color {Red, Blue};
}

class MainWindow: public QMainWindow
{
	Q_OBJECT

	private:
		// Toolbar
		QLabel*           _connection;
		QPushButton*      _connectDisconnect;
		QPushButton*      _testConnection;

		// Main widget
		QWidget*          _widget;
		QVBoxLayout*      _layout;
		QHBoxLayout*      _firstLineLayout;
		QHBoxLayout*      _secondLineLayout;

		// Config
		QGridLayout*      _configLayout;
		QGridLayout*      _configLayout2;
		QGridLayout*      _configLayout3;
		QHBoxLayout*      _configLayout4;
		QList<QComboBox*> _voltages;
		QList<QComboBox*> _flows;
		QList<QComboBox*> _temperatures;

		QComboBox*       _eepromSize;
		QComboBox*       _specialCommand;
		QComboBox*       _clientCode;
		QLineEdit*       _clientName;

		QComboBox*       _voltageLow;
		QComboBox*       _voltageHigh;
		QComboBox*       _flowLow;
		QComboBox*       _flowHigh;
		QComboBox*       _temperatureLow;
		QComboBox*       _temperatureHigh;

		QDateEdit*       _date;
		QTimeEdit*       _time;
		QPushButton*     _updateDateTime;
		QCheckBox*       _resetDataPointer;
		QCheckBox*       _resetTemperaturesPointer;

		// Direct commands
		QHBoxLayout*     _commandsLayout;
		QTextEdit*       _output;
		QLineEdit*       _input;
		QPushButton*     _cr;
		QPushButton*     _lf;
		QPushButton*     _clearConsole;

		// Buttons
		QHBoxLayout*     _buttonsLayout;
		QPushButton*     _openFile;
		QPushButton*     _writeFile;
		QPushButton*     _readEEPROM;
		QPushButton*     _writeEEPROM;
		QPushButton*     _clearData;
		QPushButton*     _clearTemperatures;

		QPushButton*     _help;

		// Serial port
		SerialPort*      _port;

	public:
		static const QString VERSION;
		static const int MAX_RETRY_WRITE_TO_FILE;
		static const int MAX_RETRY_READ_EEPROM;

		MainWindow();
		~MainWindow();

		QString    color(const QString &str, Color::Color color) const;
		void       setConnection(const QString &text, Color::Color color);
		void       updateConnectionDeconnectionButtons();
		QString    intToHex(int octet);
		int        hexToInt(const QString &hex);
		QString    extract(const QString &line);
		void       write(const QString &command);
		QByteArray read();

	public slots:
		void connectDisconnect();
		void testConnection();

		void updateVoltagesValues(int voltageLow = -1, int voltageHigh = -1);
		void updateDateTime();

		void directCommand();
		void cr();
		void lf();

		void openFile();
		void writeFile();
		void readEEPROM();
		void writeEEPROM();
		void clearData();
		void clearTemperatures();
		void help();
};

#endif
