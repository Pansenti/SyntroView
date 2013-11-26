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

#include "DisplayStatsData.h"


DisplayStatsData::DisplayStatsData(QString serviceName)
{
	m_serviceName = serviceName;
	m_RXRecords = 0;
	m_RXBytes = 0;
	m_TXRecords = 0;
	m_TXBytes = 0;

	m_RXRecordTemp = 0;
	m_RXByteTemp = 0;
	m_RXRecordRate = 0;
	m_RXByteRate = 0;

	m_TXRecordTemp = 0;
	m_TXByteTemp = 0;
	m_TXRecordRate = 0;
	m_TXByteRate = 0;
}

DisplayStatsData::DisplayStatsData(const DisplayStatsData &rhs)
{
	*this = rhs;
}

DisplayStatsData& DisplayStatsData::operator=(const DisplayStatsData &rhs)
{
	if (this != &rhs) {
		m_serviceName = rhs.m_serviceName;
		m_RXRecords = rhs.m_RXRecords;
		m_RXBytes = rhs.m_RXBytes;
		m_TXRecords = rhs.m_TXRecords;
		m_TXBytes = rhs.m_TXBytes;

		m_RXRecordTemp = rhs.m_RXRecordTemp;
		m_RXByteTemp = rhs.m_RXByteTemp;
		m_RXRecordRate = rhs.m_RXRecordRate;
		m_RXByteRate = rhs.m_RXByteRate;

		m_TXRecordTemp = rhs.m_TXRecordTemp;
		m_TXByteTemp = rhs.m_TXByteTemp;
		m_TXRecordRate = rhs.m_TXRecordRate;
		m_TXByteRate = rhs.m_TXByteRate;
	}

	return *this;
}

bool DisplayStatsData::operator==(DisplayStatsData &rhs) const
{
	return (m_serviceName == rhs.m_serviceName);
}
