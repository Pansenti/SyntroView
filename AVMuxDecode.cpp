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

#include "AVMuxDecode.h"

#include <qimage.h>
#include <qbuffer.h>
#include <qbytearray.h>

AVMuxDecode::AVMuxDecode(int slot) : SyntroThread("AVMuxDecode", "AVMuxDecode")
{
    m_slot = slot;
}

void AVMuxDecode::initThread()
{
    m_timer = startTimer(AVMUXDECODE_INTERVAL);
}

void AVMuxDecode::finishThread()
{
    killTimer(m_timer);
}

void AVMuxDecode::timerEvent(QTimerEvent * /*event*/)
{
    m_avmuxLock.lock();
    while (!m_avmuxQ.empty())
        processAVData(m_avmuxQ.dequeue());
    m_avmuxLock.unlock();
}

void AVMuxDecode::newAVData(QByteArray avmuxArray)
{
    m_avmuxLock.lock();
    m_avmuxQ.enqueue(avmuxArray);
    m_avmuxLock.unlock();
}

void AVMuxDecode::processAVData(QByteArray avmuxArray)
{
    SYNTRO_RECORD_AVMUX *avmux = (SYNTRO_RECORD_AVMUX *)avmuxArray.constData();

    SyntroUtils::avmuxHeaderToAVParams(avmux, &m_avParams);
    switch (m_avParams.avmuxSubtype) {
        case SYNTRO_RECORD_TYPE_AVMUX_MJPPCM:
            processMJPPCM(avmux);
            break;

        default:
            printf("Unsupported avmux type %d\n", m_avParams.avmuxSubtype);
            break;
    }
}

void AVMuxDecode::processMJPPCM(SYNTRO_RECORD_AVMUX *avmux)
{
    unsigned char *ptr;
    int muxSize;
    int videoSize;
    int audioSize;
    int param;

    muxSize = SyntroUtils::convertUC4ToInt(avmux->muxSize);
    videoSize = SyntroUtils::convertUC4ToInt(avmux->videoSize);
    audioSize = SyntroUtils::convertUC4ToInt(avmux->audioSize);
    param = SyntroUtils::convertUC2ToInt(avmux->recordHeader.param);

    ptr = (unsigned char *)(avmux + 1) + muxSize;        // pointer to video area

    if ((videoSize != 0) || (param == SYNTRO_RECORDHEADER_PARAM_NOOP)) {
        // there is video data present

        if ((videoSize < 0) || (videoSize > 300000)) {
            printf("Illegal video data size %d\n", videoSize);
            return;
        }
        QImage image;
        image.loadFromData((const uchar *)ptr, videoSize, "JPEG");
        emit newImage(m_slot, image, SyntroUtils::convertUC8ToInt64(avmux->recordHeader.timestamp));

        ptr += videoSize;                                   // move to next data area
    }

    if (audioSize != 0) {
        // there is audio data present

        if ((audioSize < 0) || (audioSize > 300000)) {
            printf("Illegal audio data size %d\n", audioSize);
            return;
        }
        emit newAudioSamples(m_slot, QByteArray((const char *)ptr, audioSize), 
            SyntroUtils::convertUC8ToInt64(avmux->recordHeader.timestamp),
			m_avParams.audioSampleRate, m_avParams.audioChannels, m_avParams.audioSampleSize);
        ptr += audioSize;                        // move to next data area
    }
}

