//
//  Copyright (c) 2012, 2013 Pansenti, LLC.
//	
//  This file is part of Syntro
//
//  Syntro is free software: you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation, either version 3 of the License, or
//  (at your option) any later version.
//
//  Syntro is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with Syntro.  If not, see <http://www.gnu.org/licenses/>.
//


#ifndef SELECTSTREAMSDLG_H
#define SELECTSTREAMSDLG_H

#include <qdialog.h>
#include <qtablewidget.h>
#include <qdialogbuttonbox.h>
#include <qpushbutton.h>

#include "SyntroValidators.h"
#include "SyntroLib.h"



class SelectStreamsDlg : public QDialog
{
	Q_OBJECT

public:
	SelectStreamsDlg(QWidget *parent);
	~SelectStreamsDlg();

public slots:
	void onOk();
	void onCancel();
	void buttonClicked(QAbstractButton * button);

signals:
	void newStreams();

private:
	void layoutWindow();
	void insertTableRow(int row, QString value);

	QTableWidget *m_streamTable;
	QDialogButtonBox *m_buttons0;
	QDialogButtonBox *m_buttons1;
	QPushButton *m_buttonAddRow;
	QPushButton *m_buttonAppendRow;
	QPushButton *m_buttonDeleteRow;
	ServicePathValidator *m_validator;

	bool m_changed;

};

#endif // SELECTSTREAMSDLG_H
