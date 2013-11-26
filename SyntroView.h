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

#ifndef SyntroVIEW_H
#define SyntroVIEW_H

#define	PRODUCT_TYPE "SyntroView"

#include <QtGui>

#include "ui_SyntroView.h"
#include "SyntroLib.h"
#include "DisplayStats.h"
#include "ViewClient.h"
#include "ImageWindow.h"
#include "ViewSingleCamera.h"

#ifdef Q_OS_UNIX
#include <alsa/asoundlib.h>
#else
#include <QAudioOutput>
#endif

class SyntroView : public QMainWindow
{
	Q_OBJECT

public:
    SyntroView();

public slots:
	void onStats();
	void onAbout();
	void onBasicSetup();
	void onSelectStreams();
    void onShowName();
	void onShowDate();
	void onShowTime();
	void onTextColor();
	void imageMousePress(int id);
	void imageDoubleClick(int id);
	void singleCameraClosed();
	void newStreams();
	void newWindowLayout();

    void newImage(int slot, QImage image, qint64 timestamp);
    void newAudioSamples(int slot, QByteArray dataArray, qint64 timestamp, int rate, int channels, int size);

#ifndef Q_OS_UNIX
    void handleAudioOutStateChanged(QAudio::State);
#endif

signals:
	void deleteAllServices();
	void deleteStreams();
	void addStreams();

protected:
	void closeEvent(QCloseEvent *event);
	void timerEvent(QTimerEvent *event);

private:
	void layoutGrid(QStringList sourceList);
	void deleteGrid();
	void initStatusBar();
	void initMenus();
	void saveWindowState();
	void restoreWindowState();

	Ui::SyntroViewClass ui;

	ViewClient *m_client;
	QGridLayout *m_grid;
	QList<ImageWindow *> m_windowList;
	DisplayStats *m_displayStats;
	QLabel *m_controlStatus;
	int m_statusTimer;
	bool m_showName;
	bool m_showDate;
	bool m_showTime;
	QColor m_textColor;
	int m_singleCameraId;
	ViewSingleCamera *m_singleCamera;

	int m_enableServicesTimer;

#ifndef Q_OS_UNIX
	QAudioOutput *m_audioOut;
	QIODevice *m_audioOutDevice;
#else
    snd_pcm_t *m_audioOutHandle;
    bool m_audioOutIsOpen;
    int m_audioOutSampleSize;

#endif

	bool audioOutOpen(int rate, int channels, int size);
	bool audioOutWrite(const QByteArray& audioData);

	int m_audioChannels;
	int m_audioSize;
	int m_audioRate;

    int m_activeAudioSlot;

	QString m_logTag;
};

#endif // SyntroVIEW_H
