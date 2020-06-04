
/**
 * Main window of the project
 *
 * @date December 2010
 * @author Jean-Luc Migot (jl.migot@yahoo.fr)
 *
 * Copyright 2010-2011 Jean-Luc Migot
 */

#ifndef MANAGE_STEPS_H
#define MANAGE_STEPS_H

#include <QWidget>

class QComboBox;
class QFormLayout;
class QVBoxLayout;
class QHBoxLayout;
class QPushButton;

class MainWindow;

class ManageSteps: public QWidget
{
	Q_OBJECT

	private:
		MainWindow*  _window;
		QVBoxLayout* _layout;
		QFormLayout* _formLayout;
		QComboBox*   _voltageLow;
		QComboBox*   _voltageHigh;
		QComboBox*   _flowLow;
		QComboBox*   _flowHigh;
		QComboBox*   _temperatureLow;
		QComboBox*   _temperatureHigh;
		QHBoxLayout* _buttonsLayout;
		QPushButton* _validate;
		QPushButton* _cancel;

	public:
		ManageSteps(MainWindow *window);
		~ManageSteps();

	public slots:
		void save();
};

#endif
