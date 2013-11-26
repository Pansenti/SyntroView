//
//  Copyright (c) 2013 symotes, LLC.
//
//  This file is part of symotesview.
//
//  symotesview is free software: you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation, either version 3 of the License, or
//  (at your option) any later version.
//
//  symotesview is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with symotesview.  If not, see <http://www.gnu.org/licenses/>.
//

#ifndef _AVMUXDECODE_H_
#define _AVMUXDECODE_H_

#include "SyntroLib.h"

#include <qmutex.h>

#define AVMUXDECODE_INTERVAL  (SYNTRO_CLOCKS_PER_SEC / 100)

class AVMuxDecode : public SyntroThread
{
    Q_OBJECT

public:
    AVMuxDecode(int slot);
    void newAVData(QByteArray avmuxArray);

    // gstreamer callbacks

    void processVideoSinkData();
    void processAudioSinkData();

public slots:

signals:
    void newImage(int slot, QImage image, qint64 timestamp);
    void newAudioSamples(int, QByteArray dataArray, qint64 timestamp, int rate, int channels, int size);

protected:
    void initThread();
    void timerEvent(QTimerEvent *event);
    void finishThread();

private:
    void processMJPPCM(SYNTRO_RECORD_AVMUX *avmux);
    void processAVData(QByteArray avmuxArray);

    void putVideoData(const unsigned char *data, int length);
    void putAudioData(const unsigned char *data, int length);

    int m_slot;

    int m_timer;

    QMutex m_avmuxLock;
    QQueue <QByteArray> m_avmuxQ;

	SYNTRO_AVPARAMS m_avParams;
};

#endif // _AVMUXDECODE_H_
