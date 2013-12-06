# This file is part of Syntro
#
# Copyright (c) 2013 Pansenti, LLC. All rights reserved.
#

INCLUDEPATH += $$PWD
DEPENDPATH += $$PWD

HEADERS += AVMuxDecode.h \
    AVSource.h \
    DisplayStats.h \
    DisplayStatsData.h \
    ImageWindow.h \
    StreamDialog.h \
    SyntroView.h \
    ViewClient.h \
	ViewSingleCamera.h

SOURCES += AVMuxDecode.cpp \
    AVSource.cpp \
    DisplayStats.cpp \
    DisplayStatsData.cpp \
    ImageWindow.cpp \
    main.cpp \
    StreamDialog.cpp \
    SyntroView.cpp \
    ViewClient.cpp \
    ViewSingleCamera.cpp

FORMS += SyntroView.ui \
    ViewSingleCamera.ui \
    DisplayStats.ui
