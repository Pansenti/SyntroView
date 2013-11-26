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

#include "DisplayStats.h"

#define STAT_UPDATE_INTERVAL_SECS 2

DisplayStats::DisplayStats(QWidget *parent, bool receive, bool transmit)
	: QDialog(parent, Qt::WindowCloseButtonHint | Qt::WindowTitleHint)
{
	m_logTag = "DisplayStats";
	m_RX = receive;
	m_TX = transmit;

	ui.setupUi(this);

	ui.statusCounts->setColumnCount(5);
    ui.statusCounts->setColumnWidth(0, 120);
    ui.statusCounts->setColumnWidth(1, 120);
    ui.statusCounts->setColumnWidth(2, 120);
    ui.statusCounts->setColumnWidth(3, 120);
    ui.statusCounts->setColumnWidth(4, 120);

    ui.statusCounts->setHorizontalHeaderLabels(
                QStringList() << tr("Service") << tr("RX records")
                << tr("RX bytes") << tr("RX record rate") << tr("RX byte rate"));


    ui.statusCounts->setSelectionMode(QAbstractItemView::NoSelection);

	ui.statusCounts->setMinimumSize(200, 60);

	m_timer = startTimer(STAT_UPDATE_INTERVAL_SECS * SYNTRO_CLOCKS_PER_SEC);
	hide();
}

DisplayStats::~DisplayStats()
{
	killTimer(m_timer);
}

void DisplayStats::showEvent(QShowEvent *)
{
	restoreDialogState();
}

void DisplayStats::closeEvent(QCloseEvent *event)
{
	saveDialogState();
	hide();
	event->ignore();
}

void DisplayStats::setServiceName(int slot, QString name)
{
	if (slot != m_data.count()) {
		logWarn(QString("Attempt to add service name in slot %1 not at end of table %2").arg(slot).arg(m_data.count()));
		return;
	}

	m_data.append(DisplayStatsData(name));

	ui.statusCounts->insertRow(slot);
	ui.statusCounts->setRowHeight(slot, DEFAULT_ROW_HEIGHT);

	for (int col = 0; col < 5; col++) {
		QTableWidgetItem *item = new QTableWidgetItem();
		item->setTextAlignment(Qt::AlignLeft | Qt::AlignBottom);
		item->setFlags(Qt::ItemIsEnabled);
		if (col == 0)
			item->setText(name);

		ui.statusCounts->setItem(slot, col, item);
	}
}

void DisplayStats::deleteAllServices()
{
	m_data.clear();
	ui.statusCounts->clear();
	ui.statusCounts->setRowCount(0);
    ui.statusCounts->setHorizontalHeaderLabels(
                QStringList() << tr("Service") << tr("RX records")
                << tr("RX bytes") << tr("RX record rate") << tr("RX byte rate"));
}

void DisplayStats::timerEvent(QTimerEvent *)
{
	QMutexLocker lock(&m_dataMutex);

	for (int i = 0; i < m_data.count(); i++) {
		if (m_RX){
			m_data[i].m_RXRecordRate = m_data[i].m_RXRecordTemp;
			m_data[i].m_RXByteRate = m_data[i].m_RXByteTemp;
			m_data[i].m_RXRecordTemp = 0;
			m_data[i].m_RXByteTemp = 0;
			ui.statusCounts->item(i, 1)->setText(QString::number(m_data[i].m_RXRecords));
			ui.statusCounts->item(i, 2)->setText(QString::number(m_data[i].m_RXBytes));
			ui.statusCounts->item(i, 3)->setText(QString::number(m_data[i].m_RXRecordRate / STAT_UPDATE_INTERVAL_SECS));
			ui.statusCounts->item(i, 4)->setText(QString::number(m_data[i].m_RXByteRate / STAT_UPDATE_INTERVAL_SECS));
		}

		if (m_TX) {
			m_data[i].m_TXRecordRate = m_data[i].m_TXRecordTemp;
			m_data[i].m_TXByteRate = m_data[i].m_TXByteTemp;
			m_data[i].m_TXRecordTemp = 0;
			m_data[i].m_TXByteTemp = 0;
		}
	}
}

void DisplayStats::receiveData(int slot, int bytes)
{
	QMutexLocker lock(&m_dataMutex);

	if (slot >= 0 && slot < m_data.count()) {
		m_data[slot].m_RXRecords++;
		m_data[slot].m_RXBytes += bytes;
		m_data[slot].m_RXRecordTemp++;
		m_data[slot].m_RXByteTemp += bytes;
	}
}

void DisplayStats::transmitData(int slot, int bytes)
{
	QMutexLocker lock(&m_dataMutex);

	if (slot >= 0 && slot < m_data.count()) {
		m_data[slot].m_TXRecords++;
		m_data[slot].m_TXBytes += bytes;
		m_data[slot].m_TXRecordTemp++;
		m_data[slot].m_TXByteTemp += bytes;
	}
}

void DisplayStats::saveDialogState()
{
	QSettings *settings = SyntroUtils::getSettings();

	settings->beginGroup("StatsDialog");
	settings->setValue("Geometry", saveGeometry());
	settings->endGroup();
	
	delete settings;
}

void DisplayStats::restoreDialogState()
{
	QSettings *settings = SyntroUtils::getSettings();

	settings->beginGroup("StatsDialog");
	restoreGeometry(settings->value("Geometry").toByteArray());
	settings->endGroup();
	
	delete settings;
}
