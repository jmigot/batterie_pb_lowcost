
/**
 * Dialog class to show a small dialog box with a line edit
 *
 * @date December 2010
 * @author Jean-Luc Migot (jl.migot@yahoo.fr)
 *
 * @version 0.7
 *
 * Copyright 2010-2011 Jean-Luc Migot
 */

#ifndef DIALOG_H
#define DIALOG_H

#include <QDialog>

class QDialogButtonBox;
class QLineEdit;
class QVBoxLayout;

class Dialog: public QDialog
{
	Q_OBJECT

	private:
		QVBoxLayout*      _layout;
		QLineEdit*        _lineEdit;
		QDialogButtonBox* _buttons;

	public:
		Dialog(const QString &title, const QString &text, const QString &regex);
		~Dialog();

		QString lineEditText() const;

	public slots:
		void checkAndAccept();
};

#endif
