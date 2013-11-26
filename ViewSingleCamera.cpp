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

#include "ViewSingleCamera.h"

#define	SYNTROVIEW_CAMERA_DEADTIME		(10 * SYNTRO_CLOCKS_PER_SEC)

ViewSingleCamera::ViewSingleCamera(QWidget *parent, QString sourceName)
	: QDialog(parent, Qt::WindowCloseButtonHint | Qt::WindowTitleHint)
{
	ui.setupUi(this);

	m_lastFrame = SyntroClock();

	restoreWindowState();
	setWindowTitle(sourceName);

	m_timeoutTimer = startTimer(SYNTROVIEW_CAMERA_DEADTIME);
}

void ViewSingleCamera::setSourceName(QString sourceName)
{
	setWindowTitle(sourceName);
}

void ViewSingleCamera::closeEvent(QCloseEvent *)
{
	saveWindowState();
	killTimer(m_timeoutTimer);
	emit closed();
}

void ViewSingleCamera::newImage(QImage image)
{
	m_lastFrame = SyntroClock();

	if (image.width() == 0)
		return;
    ui.cameraView->setPixmap(QPixmap::fromImage(image.scaled(size(), Qt::KeepAspectRatio)));
    repaint();
}

void ViewSingleCamera::timerEvent(QTimerEvent * /*event*/)
{
	if (SyntroUtils::syntroTimerExpired(SyntroClock(), m_lastFrame, SYNTROVIEW_CAMERA_DEADTIME)) {
		ui.cameraView->setText("No Image");
	}
}

void ViewSingleCamera::saveWindowState()
{
	QSettings *settings = SyntroUtils::getSettings();

	settings->beginGroup("SingleCameraView");
	settings->setValue("Geometry", saveGeometry());
	settings->endGroup();
	
	delete settings;
}

void ViewSingleCamera::restoreWindowState()
{
	QSettings *settings = SyntroUtils::getSettings();

	settings->beginGroup("SingleCameraView");
	restoreGeometry(settings->value("Geometry").toByteArray());
	settings->endGroup();
	
	delete settings;
}
