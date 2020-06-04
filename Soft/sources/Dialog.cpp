
/**
 * Dialog class to show a small dialog box with a line edit
 *
 * @date December 2010
 * @author Jean-Luc Migot (jl.migot@yahoo.fr)
 *
 * Copyright 2010-2011 Jean-Luc Migot
 */

#include "Dialog.h"

#include <QDialogButtonBox>
#include <QDebug>
#include <QLabel>
#include <QLineEdit>
#include <QMessageBox>
#include <QRegExpValidator>
#include <QVBoxLayout>

Dialog::Dialog(const QString &title, const QString &text, const QString &regex)
{
	// Members
	_layout   = new QVBoxLayout(this);
	_lineEdit = new QLineEdit;
	_buttons  = new QDialogButtonBox(QDialogButtonBox::Cancel | QDialogButtonBox::Ok);

	// Properties
	this->setWindowTitle(title);

	_lineEdit->setMinimumWidth(300);
	_lineEdit->setValidator(new QRegExpValidator(QRegExp(regex), _lineEdit));

	// Layouts
	_layout->addWidget(new QLabel(text));
	_layout->addWidget(_lineEdit);
	_layout->addWidget(_buttons);

	// Signals for buttons
	connect(_buttons, SIGNAL(accepted()), this, SLOT(checkAndAccept()));
	connect(_buttons, SIGNAL(rejected()), this, SLOT(reject()));
}

Dialog::~Dialog()
{
	#ifndef QT_NO_DEBUG
		qDebug() << "~Dialog()";
	#endif
}

void Dialog::checkAndAccept()
{
	if (_lineEdit->text().isEmpty())
		QMessageBox::critical(this, "Erreur", "Vous devez remplir le champ.");

	else
		this->accept();
}

QString Dialog::lineEditText() const
{
	return _lineEdit->text();
}
