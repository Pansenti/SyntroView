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
	DisplayStatsData();

	void update(int bytes);
	void updateRates(int secs);
	void clear();

	int m_totalRecords;
	int m_totalBytes;
	int m_records;
	int m_bytes;
	qreal m_recordRate;
	qreal m_byteRate;
};

#endif // DISPLAYSTATSDATA_H
