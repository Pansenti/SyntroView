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
#include <qapplication.h>

#include "SyntroUtils.h"

void loadSettings(QStringList arglist);


int main(int argc, char *argv[])
{
	QApplication a(argc, argv);

	loadSettings(a.arguments());

    SymotesView *w = new SymotesView();

	w->show();

	return a.exec();
}


void loadSettings(QStringList arglist)
{
	SyntroUtils::loadStandardSettings(PRODUCT_TYPE, arglist);

	//	check to see if the array of sources exists

	QSettings *settings = SyntroUtils::getSettings();

 	int	nSize = settings->beginReadArray(SYNTRO_PARAMS_STREAM_SOURCES);
	settings->endArray();

	if (nSize == 0) {
		settings->beginWriteArray(SYNTRO_PARAMS_STREAM_SOURCES);

		settings->setArrayIndex(0);
		settings->setValue(SYNTRO_PARAMS_STREAM_SOURCE, "App0");
		settings->endArray();
	}

	delete settings;
}
