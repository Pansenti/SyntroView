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

#include "AVSource.h"

AVSource::AVSource()
{
	m_decoder = NULL;
	m_servicePort = -1;
}

AVSource::AVSource(QString streamName)
{
	m_name = streamName;
	m_decoder = NULL;
	m_servicePort = -1;
}

AVSource::~AVSource()
{
	stopBackgroundProcessing();
}

QString AVSource::name() const
{
	return m_name;
}

int AVSource::servicePort() const
{
	return m_servicePort;
}

void AVSource::setServicePort(int port)
{
	m_servicePort = port;

	if (port == -1) {
		stopBackgroundProcessing();
	}
	else if (!m_decoder) {
		m_decoderMutex.lock();

		m_decoder = new AVMuxDecode(port);

		connect(m_decoder, SIGNAL(newImage(int, QImage, qint64)), 
			this, SLOT(newImage(int, QImage, qint64)));
	
		connect(m_decoder, SIGNAL(newAudioSamples(int, QByteArray, qint64, int, int, int)), 
			this, SLOT(newAudioSamples(int, QByteArray, qint64, int, int, int)));

		m_decoder->resumeThread();
		m_decoderMutex.unlock();
	}
}

qint64 AVSource::lastUpdate() const
{
	return m_lastUpdate;
}

void AVSource::setLastUpdate(qint64 timestamp)
{
	m_lastUpdate = timestamp;
}

QImage AVSource::image()
{
	QMutexLocker lock(&m_imageMutex);
	return m_image;
}

qint64 AVSource::imageTimestamp()
{
	return m_imageTimestamp;
}

void AVSource::stopBackgroundProcessing()
{
	QMutexLocker lock(&m_decoderMutex);

	if (m_decoder) {
		disconnect(m_decoder, SIGNAL(newImage(int, QImage, qint64)), 
			this, SLOT(newImage(int, QImage, qint64)));

		disconnect(m_decoder, SIGNAL(newAudioSamples(int, QByteArray, qint64, int, int, int)), 
			this, SLOT(newAudioSamples(int, QByteArray, qint64, int, int, int)));

		m_decoder->exitThread();
		m_decoder = NULL;
	}
}

// feed new raw data to the decoder
void AVSource::setAVData(int servicePort, QByteArray rawData)
{
	if (servicePort != m_servicePort)
		return;

	if (!m_decoder)
		return;

	m_decoder->newAVData(rawData);
	m_lastUpdate = SyntroClock();
}

// signal from the decoder, processed image
void AVSource::newImage(int /*slot*/, QImage image, qint64 timestamp)
{
	QMutexLocker lock(&m_imageMutex);

	m_image = image;
	m_imageTimestamp = timestamp;
}

// signal from the decoder, processed sound
void AVSource::newAudioSamples(int /*slot*/, QByteArray data, qint64 timestamp, int rate, int channels, int size)
{
	QMutexLocker lock(&m_audioMutex);

	m_latestAudio = data;
	m_audioTimestamp = timestamp;
	m_audioRate = rate;
	m_audioChannels = channels;
	m_audioSize = size;
}
