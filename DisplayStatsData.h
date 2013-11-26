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

#ifndef DISPLAYSTATSDATA_H
#define DISPLAYSTATSDATA_H

#include <qstring.h>

class DisplayStatsData
{
public:
	DisplayStatsData(QString serviceName);
	DisplayStatsData(const DisplayStatsData &rhs);

	DisplayStatsData& operator=(const DisplayStatsData &rhs);
	bool operator==(DisplayStatsData &rhs) const; 

	QString m_serviceName;
	qint64	m_RXRecords;			// received record count
	qint64	m_RXBytes;				// received byte count
	qint64	m_TXRecords;			// transmitted record count
	qint64	m_TXBytes;				// transmitted byte count

	int	m_RXRecordTemp;				// for rate calculation
	int	m_RXByteTemp;				// for rate calculation
	unsigned m_RXRecordRate;		// records per second
	unsigned m_RXByteRate;			// bytes per second

	int	m_TXRecordTemp;				// for rate calculation
	int	m_TXByteTemp;				// for rate calculation
	unsigned int m_TXRecordRate;	// records per second
	unsigned int m_TXByteRate;		// bytes per second
};


#endif // DISPLAYSTATSDATA_H
