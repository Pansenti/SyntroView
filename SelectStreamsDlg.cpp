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

#include <QLineEdit>
#include <QCheckBox>

#include "SyntroView.h"
#include "SelectStreamsDlg.h"

SelectStreamsDlg::SelectStreamsDlg(QWidget *parent)
	: QDialog(parent)
{
	layoutWindow();
	setWindowTitle("Stream selection");
	connect(m_buttons1, SIGNAL(accepted()), this, SLOT(onOk()));
    connect(m_buttons1, SIGNAL(rejected()), this, SLOT(onCancel()));
	m_changed = false;
}

SelectStreamsDlg::~SelectStreamsDlg()
{

}

void SelectStreamsDlg::onOk()
{
	int settingsRow;

	QSettings *settings = SyntroUtils::getSettings();

	if (!m_changed) {
		int	size = settings->beginReadArray(SYNTRO_PARAMS_STREAM_SOURCES);

		m_changed = size != m_streamTable->rowCount();		// entry must have been added or deleted
		if (!m_changed) {
			for (int row = 0; row < size; row++) {
				settings->setArrayIndex(row);
				if (((QLineEdit *)(m_streamTable->cellWidget(row, 1)))->text() != settings->value(SYNTRO_PARAMS_STREAM_SOURCE).toString()) {
					m_changed = true;
					break;
				}
			}
		}
		settings->endArray();
	}
	if (m_changed) {
		settings->remove(SYNTRO_PARAMS_STREAM_SOURCES);	// clear old entries
		settingsRow = 0;
		settings->beginWriteArray(SYNTRO_PARAMS_STREAM_SOURCES);
		for (int row = 0; row < m_streamTable->rowCount(); row++) {
			settings->setArrayIndex(settingsRow);
			if (((QLineEdit *)(m_streamTable->cellWidget(row, 1)))->text().length() > 0) {
				settings->setValue(SYNTRO_PARAMS_STREAM_SOURCE, ((QLineEdit *)(m_streamTable->cellWidget(row, 1)))->text());
				settingsRow++;
			}
		}
		settings->endArray();
		delete settings;
		accept();
		emit newStreams();
		return;
	} else {
		delete settings;
		reject();
	}

}

void SelectStreamsDlg::onCancel()
{
	reject();
}

void SelectStreamsDlg::layoutWindow()
{
	m_validator = new ServicePathValidator();
	QWidget *centralWidget = new QWidget(this);
	QVBoxLayout *verticalLayout = new QVBoxLayout(centralWidget);
	setMinimumSize(560, 500);
	centralWidget->setMinimumSize(550, 490);
	verticalLayout->setSpacing(6);
	verticalLayout->setContentsMargins(10, 5, 10, 5);


	m_streamTable = new QTableWidget();
	m_streamTable->setColumnCount(2);
	m_streamTable->setColumnWidth(0, 50);
	m_streamTable->setColumnWidth(1, 450);

    m_streamTable->setHorizontalHeaderLabels(QStringList() << "Select" << tr("Stream path"));
    m_streamTable->setSelectionMode(QAbstractItemView::NoSelection);

	verticalLayout->addWidget(m_streamTable);

	QSettings *settings = SyntroUtils::getSettings();

	int	size = settings->beginReadArray(SYNTRO_PARAMS_STREAM_SOURCES);
	for (int row = 0; row < size; row++) {
		settings->setArrayIndex(row);
		insertTableRow(row, settings->value(SYNTRO_PARAMS_STREAM_SOURCE).toString());
	}
	settings->endArray();

	m_buttons0 = new QDialogButtonBox(Qt::Horizontal);
	m_buttons0->setCenterButtons(true);

	m_buttonAddRow = m_buttons0->addButton("Insert", QDialogButtonBox::ActionRole);
	m_buttonAppendRow = m_buttons0->addButton("Append", QDialogButtonBox::ActionRole);
	m_buttonDeleteRow = m_buttons0->addButton("Delete", QDialogButtonBox::ActionRole);
	verticalLayout->addWidget(m_buttons0);

	m_buttons1 = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, Qt::Horizontal);
	m_buttons1->setCenterButtons(true);
	verticalLayout->addWidget(m_buttons1);

	connect(m_buttons0, SIGNAL(clicked(QAbstractButton *)), this, SLOT(buttonClicked(QAbstractButton *))); 
	connect(m_buttons1, SIGNAL(clicked(QAbstractButton *)), this, SLOT(buttonClicked(QAbstractButton *))); 

	delete settings;
}

void SelectStreamsDlg::buttonClicked(QAbstractButton *button)
{
	int row;
	int newRow;

	if (button == m_buttonDeleteRow) {
		row = 0;

		while (row < m_streamTable->rowCount()) {
			if (((QCheckBox *)(m_streamTable->cellWidget(row, 0)))->checkState() == Qt::Checked) {
				for (newRow = row; newRow < (m_streamTable->rowCount() - 1); newRow++) {	// move other entries up
					((QCheckBox *)(m_streamTable->cellWidget(newRow, 0)))->setCheckState(
							((QCheckBox *)(m_streamTable->cellWidget(newRow + 1, 0)))->checkState());
					((QLineEdit *)(m_streamTable->cellWidget(newRow, 1)))->setText(
							((QLineEdit *)(m_streamTable->cellWidget(newRow + 1, 1)))->text());
				}
				m_streamTable->setRowCount(m_streamTable->rowCount() - 1);
			} else {
				row++;
			}
		}

	} else if (button == m_buttonAppendRow) {
		insertTableRow(m_streamTable->rowCount(), "");
	} else if (button == m_buttonAddRow) {
		row = 0;
		while (row < m_streamTable->rowCount()) {
			if (((QCheckBox *)(m_streamTable->cellWidget(row, 0)))->checkState() == Qt::Checked) {
				insertTableRow(row, "");
				row++;
			}
			row++;
		}
	}
}

void SelectStreamsDlg::insertTableRow(int row, QString value)
{
	m_streamTable->insertRow(row);
	m_streamTable->setRowHeight(row, 20);

	QCheckBox *checkBox = new QCheckBox(m_streamTable);
	m_streamTable->setCellWidget(row, 0, checkBox);

	QLineEdit *lineEdit = new QLineEdit(value, this);
	lineEdit->setValidator(m_validator);
	m_streamTable->setCellWidget(row, 1, lineEdit);
}

