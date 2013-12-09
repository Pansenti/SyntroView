# This file is part of Syntro
#
# Copyright (c) 2013 Pansenti, LLC. All rights reserved.
#

cache()

TEMPLATE = app
TARGET = SyntroView

win32* {
	DESTDIR = Release
}
else {
	DESTDIR = Output 
}

QT += core gui network
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += debug_and_release

unix {
	macx {
		LIBS += /usr/local/lib/libSyntroLib.dylib \
			/usr/local/lib/libSyntroGUI.dylib \
			/usr/local/lib/libSyntroControlLib.dylib

		INCLUDEPATH += /usr/local/include/syntro \
				/usr/local/include/syntro/SyntroControlLib \
				/usr/local/include/syntro/SyntroAV

		target.path = /usr/local/bin
	}
	else {
		CONFIG += link_pkgconfig
		PKGCONFIG += syntro

		LIBS += -lasound
		target.path = /usr/bin
	}

	INSTALLS += target
}

DEFINES += QT_NETWORK_LIB

INCLUDEPATH += GeneratedFiles

win32-g++:LIBS += -L"$(SYNTRODIR)/bin"

win32-msvc*:LIBS += -L"$(SYNTRODIR)/lib"

win32 {
	DEFINES += _CRT_SECURE_NO_WARNINGS
	INCLUDEPATH += $(SYNTRODIR)/include
	LIBS += -lSyntroLib -lSyntroGUI
}

MOC_DIR += GeneratedFiles/release

OBJECTS_DIR += release

UI_DIR += GeneratedFiles

RCC_DIR += GeneratedFiles

include(SyntroView.pri)
