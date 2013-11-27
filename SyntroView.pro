TEMPLATE = app
TARGET = SyntroView

win32* {
	DESTDIR = Release
}
else {
	DESTDIR = Output 
}

include(version.pri)

QT += core gui network
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets multimedia

CONFIG += debug_and_release

unix {
	CONFIG += link_pkgconfig
	macx {
		CONFIG -= app_bundle
	} else {
        LIBS += -lasound
	}
	PKGCONFIG += syntro
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
