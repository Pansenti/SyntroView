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


#ifndef VIEWCLIENT_H
#define VIEWCLIENT_H

#include "SyntroLib.h"

#define	VIEWCLIENT_BACKGROUND_INTERVAL (SYNTRO_CLOCKS_PER_SEC/100)

class AVMuxDecode;
class SyntroView;

class ViewClient : public Endpoint
{
	Q_OBJECT

public:
    ViewClient(SyntroView *parent);

	QStringList streamSources();

public slots:
	void deleteStreams();
	void addStreams();
	void requestDir();

signals:
    void newAVData(int slot, QByteArray avmuxArray);
	void newWindowLayout();
	void receiveData(int slot, int bytes);
	void setServiceName(int slot, QString name);
	void dirResponse(QStringList directory);

protected:
    void appClientInit();
    void appClientExit();
    void appClientReceiveMulticast(int servicePort, SYNTRO_EHEAD *multiCast, int len);
    void appClientReceiveDirectory(QStringList);

private:
	void loadStreamSources(QString group, QString src);

	QStringList m_sources;
    QList <int> m_avmuxPorts;
    QList <AVMuxDecode *> m_avmuxDecoders;

    SyntroView *m_parent;
};

#endif // VIEWCLIENT_H

