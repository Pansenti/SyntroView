//
//  Copyright (c) 2013 Pansenti, LLC.
//	
//  This file is part of Sentient Spaces
//

#ifndef STREAMDIALOG_H
#define STREAMDIALOG_H

#include <qdialog.h>
#include <qlabel.h>
#include <qpushbutton.h>
#include <qstringlist.h>
#include <qlistwidget.h>


class StreamDialog : public QDialog
{
	Q_OBJECT

public:
	StreamDialog(QWidget *parent, QStringList directory, QStringList currentStreams);
	
	QStringList newStreams();

private:
	void layoutWindow();
	void parseAvailableServices(QStringList directory);

	QStringList m_availableStreams;
	QStringList m_currentStreams;

	QListWidget *m_availableList;
	//bool m_wantLowResolutionFeeds;

	QPushButton *m_okButton;
	QPushButton *m_cancelButton;
};

#endif // STREAMDIALOG_H
