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

#include "SymotesView.h"
#include "ViewClient.h"
#include "AVMuxDecode.h"

ViewClient::ViewClient(SymotesView *parent)
    : Endpoint(VIEWCLIENT_BACKGROUND_INTERVAL, "ViewClient")
{
    m_parent = parent;
}

QStringList ViewClient::streamSources()
{
	return m_sources;
}

void ViewClient::appClientInit()
{
	addStreams();
}

void ViewClient::appClientExit()
{
    deleteStreams();
}

void ViewClient::appClientReceiveMulticast(int servicePort, SYNTRO_EHEAD *multiCast, int len)
{
	int slot;

    for (slot = 0; slot < m_avmuxPorts.count(); slot++) {
        if (servicePort == m_avmuxPorts.at(slot)) {
            SYNTRO_RECORD_AVMUX *avmuxHeader = reinterpret_cast<SYNTRO_RECORD_AVMUX *>(multiCast + 1);
            int recordType = SyntroUtils::convertUC2ToUInt(avmuxHeader->recordHeader.type);

            if (recordType != SYNTRO_RECORD_TYPE_AVMUX) {
                qDebug() << "Got incorrect record type instead of avmux " << recordType;
                free(multiCast);
                return;
            }
            m_avmuxDecoders.at(slot)->newAVData(QByteArray((const char *)avmuxHeader, len));
            emit receiveData(slot, len);
			clientSendMulticastAck(servicePort);
			free(multiCast);
			return;
		}
	}

	logWarn(QString("Multicast received to invalid port %1").arg(servicePort));
}

void ViewClient::addStreams()
{
	int port;
    AVMuxDecode *decoder;

	loadStreamSources(SYNTRO_PARAMS_STREAM_SOURCES, SYNTRO_PARAMS_STREAM_SOURCE);
	for (int i = 0; i < m_sources.count(); i++) {
        port = clientAddService(SyntroUtils::insertStreamNameInPath(m_sources.at(i),SYNTRO_STREAMNAME_AVMUX), SERVICETYPE_MULTICAST, false);
        m_avmuxPorts.append(port);

        decoder = new AVMuxDecode(i);
        m_avmuxDecoders.append(decoder);
        connect(decoder, SIGNAL(newImage(int, QImage, qint64)),
                m_parent, SLOT(newImage(int, QImage, qint64)), Qt::QueuedConnection);

        connect(decoder, SIGNAL(newAudioSamples(int, QByteArray,qint64, int,int,int)),
                m_parent, SLOT(newAudioSamples(int, QByteArray,qint64, int,int,int)), Qt::QueuedConnection);

        decoder->resumeThread();

        emit setServiceName(i, m_sources.at(i));
    }
	emit newWindowLayout();
}

void ViewClient::deleteStreams()
{
    AVMuxDecode *decoder;

	for (int i = 0; i < m_sources.count(); i++) {
        clientRemoveService(m_avmuxPorts.at(i));

        decoder = m_avmuxDecoders.at(i);

        disconnect(decoder, SIGNAL(newImage(int, QImage, qint64)),
                m_parent, SLOT(newImage(int, QImage, qint64)));

        disconnect(decoder, SIGNAL(newAudioSamples(int, QByteArray,qint64,int,int,int)),
                m_parent, SLOT(newAudioSamples(int, QByteArray,qint64,int,int,int)));

        decoder->exitThread();
	}
    m_avmuxPorts.clear();
    m_avmuxDecoders.clear();
    m_sources.clear();
}


void ViewClient::loadStreamSources(QString group, QString src)
{
	QSettings *settings = SyntroUtils::getSettings();

	int count = settings->beginReadArray(group);

	for (int i = 0; i < count; i++) {
		settings->setArrayIndex(i);

		QString s = settings->value(src).toString();
		m_sources.append(s);
    }

	settings->endArray();
	delete settings;
}
