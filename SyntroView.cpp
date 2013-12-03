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

#include "SyntroView.h"
#include "SyntroAboutDlg.h"
#include "BasicSetupDlg.h"
#include "SelectStreamsDlg.h"
#include <qcolordialog.h>

#define GRID_SPACING 3

SyntroView::SyntroView()
	: QMainWindow()
{
    m_logTag = "SyntroView";
	ui.setupUi(this);

	m_singleCameraId = -1;
	m_singleCamera = NULL;

	m_audioSize = -1;
	m_audioRate = -1;
	m_audioChannels = -1;
    m_activeAudioSlot = -1;

#ifndef Q_OS_MAC
#ifndef Q_OS_UNIX
	m_audioOut = NULL;
	m_audioOutDevice = NULL;
#else
    m_audioOutIsOpen = false;
#endif
#endif

	m_displayStats = new DisplayStats(this, true, false);

	SyntroUtils::syntroAppInit();
	startControlServer();

	m_client = new ViewClient(this);

	connect(ui.actionExit, SIGNAL(triggered()), this, SLOT(close()));

    connect(m_client, SIGNAL(setServiceName(int, QString)),
            m_displayStats, SLOT(setServiceName(int, QString)), Qt::QueuedConnection);

    connect(m_client, SIGNAL(receiveData(int, int)),
            m_displayStats, SLOT(receiveData(int, int)), Qt::DirectConnection);

    connect(this, SIGNAL(deleteAllServices()),
            m_displayStats, SLOT(deleteAllServices()), Qt::QueuedConnection);

    connect(this, SIGNAL(deleteStreams()),
            m_client, SLOT(deleteStreams()), Qt::QueuedConnection);

    connect(this, SIGNAL(addStreams()),
            m_client, SLOT(addStreams()), Qt::QueuedConnection);

    connect(m_client, SIGNAL(newWindowLayout()),
            this, SLOT(newWindowLayout()), Qt::QueuedConnection);

	m_client->resumeThread();

	m_statusTimer = startTimer(2000);

	restoreWindowState();
	initStatusBar();
	initMenus();

	setWindowTitle(QString("%1 - %2")
		.arg(SyntroUtils::getAppType())
		.arg(SyntroUtils::getAppName()));

	m_enableServicesTimer = -1;
}

void SyntroView::startControlServer()
{
	QSettings *settings = SyntroUtils::getSettings();

	if (settings->value(SYNTRO_PARAMS_LOCALCONTROL).toBool()) {
		m_controlServer = new SyntroServer();
		m_controlServer->resumeThread();
	} 
	else {
		m_controlServer = NULL;
	}

	delete settings;
}

void SyntroView::onStats()
{
	m_displayStats->activateWindow();
	m_displayStats->show();
}

void SyntroView::closeEvent(QCloseEvent *)
{
 	killTimer(m_statusTimer);

	disconnect(ui.actionExit, SIGNAL(triggered()), this, SLOT(close()));

    disconnect(m_client, SIGNAL(setServiceName(int, QString)),
            m_displayStats, SLOT(setServiceName(int, QString)));

    disconnect(m_client, SIGNAL(receiveData(int, int)),
            m_displayStats, SLOT(receiveData(int, int)));

    disconnect(this, SIGNAL(deleteAllServices()),
            m_displayStats, SLOT(deleteAllServices()));

    disconnect(this, SIGNAL(deleteStreams()),
            m_client, SLOT(deleteStreams()));

    disconnect(this, SIGNAL(addStreams()),
            m_client, SLOT(addStreams()));

    disconnect(m_client, SIGNAL(newWindowLayout()),
            this, SLOT(newWindowLayout()));

	if (m_singleCamera) {
		disconnect(m_singleCamera, SIGNAL(closed()), this, SLOT(singleCameraClosed()));
		m_singleCamera->close();
	}

	if (m_client) {
		m_client->exitThread(); 
		m_client = NULL;
	}

	if (m_controlServer) {
		m_controlServer->exitThread();
		m_controlServer = NULL;
	}

	saveWindowState();

	SyntroUtils::syntroAppExit();
}

void SyntroView::timerEvent(QTimerEvent *event)
{
	if (event->timerId() == m_enableServicesTimer) {
		emit addStreams();
		killTimer(m_enableServicesTimer);
		m_enableServicesTimer = -1;
	} else {
		m_controlStatus->setText(m_client->getLinkState());
	}
}


void SyntroView::singleCameraClosed()
{
	if (m_singleCamera) {
		delete m_singleCamera;

		m_windowList[m_singleCameraId]->setSelected(false);
		m_singleCamera = NULL;
		m_singleCameraId = -1;
		m_activeAudioSlot = -1;
	}
}

void SyntroView::imageMousePress(int id)
{
	if ((id == m_activeAudioSlot) && (m_singleCameraId < 0)) {
		m_windowList[id]->setSelected(false);
		m_activeAudioSlot = -1;
		return;
	}
    m_activeAudioSlot = id;
    for (int i = 0; i < m_windowList.count(); i++)
        m_windowList[i]->setSelected(i == id);

	if (m_singleCameraId < 0)
		return;

	m_singleCameraId = id;

	m_singleCamera->setSourceName(m_windowList[id]->sourceName());

	m_singleCamera->newImage(m_windowList[id]->m_image);
}

void SyntroView::imageDoubleClick(int id)
{
	if (m_singleCameraId >= 0)
		return;

	if (!m_singleCamera) {
		m_singleCamera = new ViewSingleCamera(NULL, m_windowList[id]->sourceName());

		if (!m_singleCamera)
			return;

		connect(m_singleCamera, SIGNAL(closed()), this, SLOT(singleCameraClosed()));
		m_singleCamera->show();
	}
	else {
		m_singleCamera->setSourceName(m_windowList[id]->sourceName());
	}

	m_singleCameraId = id;
	m_windowList[id]->setSelected(true);
    m_activeAudioSlot = id;
	m_singleCamera->newImage(m_windowList[id]->m_image);
}

void SyntroView::onShowName()
{
	m_showName = ui.actionShow_name->isChecked();

	for (int i = 0; i < m_windowList.count(); i++)
		m_windowList[i]->setShowName(m_showName);
}

void SyntroView::onShowDate()
{
	m_showDate = ui.actionShow_date->isChecked();

	for (int i = 0; i < m_windowList.count(); i++)
		m_windowList[i]->setShowDate(m_showDate);
}

void SyntroView::onShowTime()
{
	m_showTime = ui.actionShow_time->isChecked();

	for (int i = 0; i < m_windowList.count(); i++)
		m_windowList[i]->setShowTime(m_showTime);
}

void SyntroView::onTextColor()
{
	m_textColor = QColorDialog::getColor(m_textColor, this);

	for (int i = 0; i < m_windowList.count(); i++)
		m_windowList[i]->setTextColor(m_textColor);
}

void SyntroView::newStreams()
{
	if (m_enableServicesTimer != -1)
		return;												// already waiting to clear things
	deleteGrid();
	emit deleteStreams();
	emit deleteAllServices();
	m_enableServicesTimer = startTimer(500);				// give Endpoint time to clear up - finish off in timer event
	m_activeAudioSlot = -1;
}

void SyntroView::newWindowLayout()
{
	layoutGrid(m_client->streamSources());
}

void SyntroView::layoutGrid(QStringList sourceList)
{
	int rows;

	m_grid = new QGridLayout(ui.centralWidget);
	m_grid->setSpacing(GRID_SPACING);
	m_grid->setContentsMargins(2, 2, 2, 2);

	int numSources = sourceList.count();

	if (numSources > 30)
		numSources = 30;
	
	if (numSources < 4)
		rows = 1;
	else if (numSources < 9)
		rows = 2;
	else if (numSources < 13)
		rows = 3;
	else if (numSources < 23)
		rows = 4;
	else
		rows = 5;

	int cols = (numSources / rows);

	if (numSources % rows)
		cols++;

	for (int i = 0, id = 0; i < rows && id < numSources; i++) {
		for (int j = 0; j < cols && id < numSources; j++) {
			ImageWindow *iw = new ImageWindow(id, sourceList.at(id), m_showName, m_showDate, m_showTime, m_textColor);
			m_windowList.append(iw);
			m_grid->addWidget(iw, i, j);
			connect(iw, SIGNAL(imageMousePress(int)), this, SLOT(imageMousePress(int)));
			connect(iw, SIGNAL(imageDoubleClick(int)), this, SLOT(imageDoubleClick(int)));
			id++;
		}
	}

	for (int i = 0; i < rows; i++)
		m_grid->setRowStretch(i, 1);

	for (int i = 0; i < cols; i++)
		m_grid->setColumnStretch(i, 1);
}

void SyntroView::deleteGrid()
{
	ImageWindow *iw;

	for (int i = 0; i < m_windowList.count(); i++) {
		iw = m_windowList.at(i);
		disconnect(iw, SIGNAL(imageMousePress(int)), this, SLOT(imageMousePress(int)));
		disconnect(iw, SIGNAL(imageDoubleClick(int)), this, SLOT(imageDoubleClick(int)));
		delete m_windowList.at(i);
	}
	delete m_grid;
	m_windowList.clear();
}


void SyntroView::initStatusBar()
{
	m_controlStatus = new QLabel(this);
	m_controlStatus->setAlignment(Qt::AlignLeft);
	ui.statusBar->addWidget(m_controlStatus, 1);
}

void SyntroView::initMenus()
{
	connect(ui.actionAbout, SIGNAL(triggered()), this, SLOT(onAbout()));
	connect(ui.actionBasicSetup, SIGNAL(triggered()), this, SLOT(onBasicSetup()));
	connect(ui.actionSelectStreams, SIGNAL(triggered()), this, SLOT(onSelectStreams()));

	connect(ui.onStats, SIGNAL(triggered()), this, SLOT(onStats()));
	connect(ui.actionShow_name, SIGNAL(triggered()), this, SLOT(onShowName()));
	connect(ui.actionShow_date, SIGNAL(triggered()), this, SLOT(onShowDate()));
	connect(ui.actionShow_time, SIGNAL(triggered()), this, SLOT(onShowTime()));
	connect(ui.actionText_color, SIGNAL(triggered()), this, SLOT(onTextColor()));

	ui.actionShow_name->setChecked(m_showName);
	ui.actionShow_date->setChecked(m_showDate);
	ui.actionShow_time->setChecked(m_showTime);
}

void SyntroView::saveWindowState()
{
	QSettings *settings = SyntroUtils::getSettings();

	settings->beginGroup("Window");
	settings->setValue("Geometry", saveGeometry());
	settings->setValue("State", saveState());
	settings->setValue("showName", m_showName);
	settings->setValue("showDate", m_showDate);
	settings->setValue("showTime", m_showTime);
	settings->setValue("textColor", m_textColor);
	settings->endGroup();
	
	delete settings;
}

void SyntroView::restoreWindowState()
{
	QSettings *settings = SyntroUtils::getSettings();

	settings->beginGroup("Window");
	restoreGeometry(settings->value("Geometry").toByteArray());
	restoreState(settings->value("State").toByteArray());

	if (settings->contains("showName")) 
		m_showName = settings->value("showName").toBool();
	else
		m_showName = true;

	if (settings->contains("showDate"))
		m_showDate = settings->value("showDate").toBool();
	else
		m_showDate = true;

	if (settings->contains("showTime"))
		m_showTime = settings->value("showTime").toBool();
	else
		m_showTime = true;

	if (settings->contains("textColor"))
		m_textColor = settings->value("textColor").value<QColor>();
	else
		m_textColor = Qt::white;

	settings->endGroup();
	
	delete settings;
}

void SyntroView::onAbout()
{
	SyntroAbout *dlg = new SyntroAbout();
	dlg->show();
}

void SyntroView::onBasicSetup()
{
	BasicSetupDlg *dlg = new BasicSetupDlg(this);
	dlg->show();
}

void SyntroView::onSelectStreams()
{
	SelectStreamsDlg *dlg = new SelectStreamsDlg(this);
	connect(dlg, SIGNAL(newStreams()), this, SLOT(newStreams()), Qt::QueuedConnection);
	dlg->show();
}


void SyntroView::newImage(int slot, QImage image, qint64 timestamp)
{
    if (slot < 0 || slot >= m_windowList.count())
        return;

    m_windowList[slot]->newImage(image, timestamp);

    if (m_singleCameraId == slot)
        m_singleCamera->newImage(image);
}

void SyntroView::newAudioSamples(int slot, QByteArray dataArray, qint64 /*timestamp*/, 
	int rate, int channels, int size)
{
    if (slot != m_activeAudioSlot)
        return;

	if ((m_audioRate != rate) || (m_audioSize != size) || (m_audioChannels != channels)) {
		if (!audioOutOpen(rate, channels, size)) {
            qDebug() << "Failed to open audio out device";
            return;
        }
        m_audioRate = rate;
		m_audioSize = size;
		m_audioChannels = channels;
	}
	audioOutWrite(dataArray);
}

#ifndef Q_OS_UNIX
bool SyntroView::audioOutOpen(int rate, int channels, int size)
{
/*    foreach (const QAudioDeviceInfo& deviceInfo, QAudioDeviceInfo::availableDevices(QAudio::AudioOutput)) {
        qDebug() << "Device name: " << deviceInfo.deviceName();
        qDebug() << "    codec: " << deviceInfo.supportedCodecs();
        qDebug() << "    channels:" << deviceInfo.supportedChannelCounts();
        qDebug() << "    rates:" << deviceInfo.supportedSampleRates();
        qDebug() << "    sizes:" << deviceInfo.supportedSampleSizes();
        qDebug() << "    types:" << deviceInfo.supportedSampleTypes();
        qDebug() << "    order:" << deviceInfo.supportedByteOrders();
     }
*/
    QAudioFormat format;
    // Set up the format, eg.
    format.setSampleRate(rate);
    format.setChannelCount(channels);
    format.setSampleSize(size);
    format.setCodec("audio/pcm");
    format.setByteOrder(QAudioFormat::LittleEndian);
    format.setSampleType(QAudioFormat::SignedInt);

    QAudioDeviceInfo info(QAudioDeviceInfo::defaultOutputDevice());
    if (!info.isFormatSupported(format)) {
        qWarning() << "Cannot play audio.";
        return false;
    }

	if (m_audioOut != NULL) {
        delete m_audioOut;
		m_audioOut = NULL;
		m_audioOutDevice = NULL;
	}
    m_audioOut = new QAudioOutput(format, this);
    m_audioOut->setBufferSize(rate * channels * (size / 8));
//    qDebug() << "Buffer size: " << m_audioOut->bufferSize();
    connect(m_audioOut, SIGNAL(stateChanged(QAudio::State)), this, SLOT(handleAudioOutStateChanged(QAudio::State)));
	m_audioOutDevice = m_audioOut->start();
	return true;
}

bool SyntroView::audioOutWrite(const QByteArray& audioData)
{
	if (m_audioOutDevice == NULL)
		return false;

	return m_audioOutDevice->write(audioData) == audioData.length();
}

void SyntroView::handleAudioOutStateChanged(QAudio::State /* state */)
{
//	qDebug() << "Audio state " << state;
}

#else

bool SyntroView::audioOutOpen(int rate, int channels, int size)
{
#ifndef Q_OS_MAC
    int err;
    snd_pcm_hw_params_t *params;

    if (m_audioOutIsOpen) {
        snd_pcm_close(m_audioOutHandle);
        m_audioOutIsOpen = false;
    }
    if ((rate == 0) || (channels == 0) || (size == 0))
        return false;

    if ((err = snd_pcm_open(&m_audioOutHandle, "plughw:0", SND_PCM_STREAM_PLAYBACK, 0)) < 0) {
        return false;
    }
    snd_pcm_format_t sampleSize;

    switch (size) {
    case 8:
        sampleSize = SND_PCM_FORMAT_S8;
        break;

    case 32:
        sampleSize = SND_PCM_FORMAT_S32_LE;
        break;

    default:
        sampleSize = SND_PCM_FORMAT_S16_LE;
        break;

   }

    params = NULL;
    if (snd_pcm_hw_params_malloc(&params) < 0)
        goto openError;
    if (snd_pcm_hw_params_any(m_audioOutHandle, params) < 0)
        goto openError;
    if (snd_pcm_hw_params_set_access(m_audioOutHandle, params, SND_PCM_ACCESS_RW_INTERLEAVED) < 0)
        goto openError;
    if (snd_pcm_hw_params_set_format(m_audioOutHandle, params, sampleSize) < 0)
        goto openError;
    if (snd_pcm_hw_params_set_rate_near(m_audioOutHandle, params, (unsigned int *)&rate, 0) < 0)
        goto openError;
    if (snd_pcm_hw_params_set_channels(m_audioOutHandle, params, channels) < 0)
        goto openError;
    if (snd_pcm_hw_params(m_audioOutHandle, params) < 0)
        goto openError;
    if (snd_pcm_nonblock(m_audioOutHandle, 1) < 0)
        goto openError;
    snd_pcm_hw_params_free(params);
    params = NULL;

    if ((err = snd_pcm_prepare(m_audioOutHandle)) < 0)
        goto openError;

    m_audioOutSampleSize = channels * (size / 8);                       // bytes per sample

    m_audioOutIsOpen = true;
    return true;

openError:
    snd_pcm_close(m_audioOutHandle);
    if (params != NULL)
        snd_pcm_hw_params_free(params);
    m_audioOutIsOpen = false;
#endif
    return false;
}

bool SyntroView::audioOutWrite(const QByteArray& audioData)
{
#ifndef Q_OS_MAC
    int writtenLength;
    int samples = audioData.length() / m_audioOutSampleSize;

    writtenLength = snd_pcm_writei(m_audioOutHandle, audioData.constData(), samples);
    if (writtenLength == -EPIPE) {
        snd_pcm_prepare(m_audioOutHandle);
    }
    return writtenLength == samples;
#else
	return 0;
#endif
}
#endif
