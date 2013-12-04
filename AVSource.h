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

#ifndef AVSOURCE_H
#define AVSOURCE_H

#include <qmutex.h>

#include "AVMuxDecode.h"

class AVSource : public QObject
{
	Q_OBJECT

public:
	AVSource();
	AVSource(QString streamName);
	~AVSource();

	QString name() const;

	int servicePort() const;
	void setServicePort(int port);

	qint64 lastAVUpdate() const;

	QImage image();
	qint64 imageTimestamp();

	void setAVData(int servicePort, QByteArray rawData);
	void stopBackgroundProcessing();

public slots:
	void newImage(int slot, QImage image, qint64 timestamp);
	void newAudioSamples(int slot, QByteArray data, qint64 timestamp, int rate, int channels, int size);

private:
	QString m_name;
	int m_servicePort;

	qint64 m_lastAVUpdate;

	QMutex m_imageMutex;
	QImage m_image;
	qint64 m_imageTimestamp;

	QMutex m_audioMutex;
	QByteArray m_latestAudio;
	qint64 m_audioTimestamp;
	int m_audioRate;
	int m_audioChannels;
	int m_audioSize;

	QMutex m_decoderMutex;
	AVMuxDecode *m_decoder;
};

#endif // AVSOURCE_H
