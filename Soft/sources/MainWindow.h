
/**
 * Main window of the project
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

class QLabel;
class QProgressBar;
class QPushButton;

namespace Color
{
	enum Color { Red, Blue, Green };
}

struct Config
{
	QList<int> voltages;
	QList<int> flows;
	QList<int> temperatures;

	int nbVoltages;
	int nbFlows;
	int nbTemperatures;

	int specialCommand;
	int eepromSize;
	int clientCode;

	int voltageLow;
	int voltageHigh;
	int flowLow;
	int flowHigh;
	int temperatureLow;
	int temperatureHigh;

	QString date;

	QStringList voltageColors;
	QStringList flowColors;
	QStringList temperatureColors;
};

class MainWindow: public QMainWindow
{
	Q_OBJECT

	private:
		// Menu
		QMenu*         _fileMenu;
		QMenu*         _cardMenu;

		// Toolbar
		QLabel*        _connection;
		QLabel*        _dateTime;
		QProgressBar*  _memoryUsage;
		QPushButton*   _connectDisconnect;
		QPushButton*   _testConnection;

		// Central widget
		QTabWidget*    _tabWidget;

		// Usual
		Config*        _config;
		SerialPort*    _port;

	public:
		static const QString   APPLICATION_NAME;
		static const QString   VERSION;
		static const int       MAX_RETRY_EEPROM;
		static const int       MAX_RETRY_WRITE_TO_FILE;
		static const QString   DEFAULT_DIR;
		static const QString   DEFAULT_EXTENSION;
		static const QString   COLORS_FILE;

		MainWindow();
		~MainWindow();

		void       readConfigColors();
		QByteArray tryRead(const QString &command, int lenExpected, int maxRetry = MAX_RETRY_EEPROM);
		bool       readEEPROMConfig();
		void       readEEPROMUsual();
		QString    color(const QString &str, Color::Color color) const;
		void       updateConnectionDeconnectionButtons();
		void       updatePrintMenu();
		void       setButtonsEnabled(bool enable);
		void       setConnection(const QString &text, Color::Color color);
		QString    intToHex(int octet, int nbChars = 2) const;
		int        hexToInt(const QString &hex) const;
		Config*    config() const;

	public slots:
		void   closeTab(int index);
		void   open();
		void   read();
		void   updateDateTime();
		void   manageSteps();
		void   resetData();
		void   exportData();
		void   exportPDF();
		void   print();
		void   help();
		void   about();

		void   connectDisconnect();
		bool   testConnection(bool showOk = true);
};

#endif
