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

#include <qlabel.h>
#include <qboxlayout.h>
#include <qmessagebox.h>
#include <qfiledialog.h>
#include <qdebug.h>

#include "SyntroDefs.h"
#include "DirectoryEntry.h"
#include "streamdialog.h"
#include "SyntroUtils.h"

StreamDialog::StreamDialog(QWidget *parent, QStringList directory, QStringList currentStreams) 
	: QDialog(parent, Qt::WindowCloseButtonHint | Qt::WindowTitleHint)
{
	m_currentStreams = currentStreams;

	parseAvailableServices(directory);

	layoutWindow();

	connect(m_okButton, SIGNAL(clicked()), this, SLOT(accept()));
	connect(m_cancelButton, SIGNAL(clicked()), this, SLOT(reject()));

	setWindowTitle("Choose Video Feeds");
} 

void StreamDialog::parseAvailableServices(QStringList directory)
{
	DirectoryEntry de;

	for (int i = 0; i < directory.count(); i++) {
		de.setLine(directory.at(i));

		if (!de.isValid())
			continue;

		if (m_currentStreams.contains(de.appName()))
			continue;
		
		QStringList services = de.multicastServices();

		for (int i = 0; i < services.count(); i++) {
			if (services.at(i) == SYNTRO_STREAMNAME_AVMUX) {
				m_availableStreams.append(de.appName());
				break;
			}
		}
	}		
}

QStringList StreamDialog::newStreams()
{
	QStringList list;

	QList<QListWidgetItem *> items = m_availableList->selectedItems();
	
	for (int i = 0; i < items.count(); i++)
		list << items.at(i)->text();

	return list;
}

void StreamDialog::layoutWindow()
{
	QVBoxLayout *vLayout = new QVBoxLayout();

	QVBoxLayout *streamLayout = new QVBoxLayout;

	m_availableList = new QListWidget();
	m_availableList->setSelectionMode(QAbstractItemView::MultiSelection);
	m_availableList->setEditTriggers(QAbstractItemView::NoEditTriggers);

	m_availableStreams.sort();

	for (int i = 0; i < m_availableStreams.count(); i++)
		m_availableList->addItem(new QListWidgetItem(m_availableStreams.at(i)));

	streamLayout->addStretch();	
	streamLayout->addWidget(new QLabel("Available Feeds"));
	streamLayout->addWidget(m_availableList, 1);
	streamLayout->addStretch();

	vLayout->addLayout(streamLayout, 1);
	vLayout->addStretch();

	QHBoxLayout *buttonLayout = new QHBoxLayout();

	m_okButton = new QPushButton("OK");
	m_cancelButton = new QPushButton("Cancel");

	buttonLayout->addStretch();
	buttonLayout->addWidget(m_okButton);
	buttonLayout->addWidget(m_cancelButton);
	buttonLayout->addStretch();

	vLayout->addSpacing(20);
	vLayout->addLayout(buttonLayout);
	setLayout(vLayout);
};
