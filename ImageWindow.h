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

#ifndef IMAGEWINDOW_H
#define IMAGEWINDOW_H

#include <QLabel>
#include "SyntroLib.h"


class ImageWindow : public QLabel
{
	Q_OBJECT

public:
	ImageWindow(int id, QString sourceName, bool showName, bool showDate, bool showTime, QColor textColor, QWidget *parent = 0);
	virtual ~ImageWindow();

	QString sourceName();

    void newImage(QImage image, qint64 timestamp);

	void setShowName(bool enable);
	void setShowDate(bool enable);
	void setShowTime(bool enable);
	void setTextColor(QColor color);

	bool selected();
	void setSelected(bool select);

	QImage m_image;

signals:
	void imageMousePress(int id);
	void imageDoubleClick(int id);

protected:
	void paintEvent(QPaintEvent *event);
	void timerEvent(QTimerEvent *event);
	void mousePressEvent(QMouseEvent *event);
	void mouseDoubleClickEvent(QMouseEvent *event);

private:
	QRect drawingRect();

	int m_id;
	QString m_sourceName;
	bool m_showName;
	bool m_showDate;
	bool m_showTime;
	QColor m_textColor;
	bool m_selected;
	bool m_idle;
	int m_timeoutTimer;
    QDate m_displayDate;
    QTime m_displayTime;
	qint64 m_lastFrame;
};

#endif // IMAGEWINDOW_H
