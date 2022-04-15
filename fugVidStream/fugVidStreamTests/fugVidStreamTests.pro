#-------------------------------------------------
#
# Project created by QtCreator 2013-05-27T22:07:36
#
#-------------------------------------------------

QT += core gui testlib

TARGET   = fugVidStreamTests

TEMPLATE = app

SOURCES += teststream.cpp \
	fugVidStreamTests.cpp

HEADERS += \
	teststream.h

win32 {
	INCLUDEPATH += $$(LIBS)/msinttypes

	DEFINES += _WIN32_WINNT=0x0501
	DEFINES += _CRT_SECURE_NO_WARNINGS

	QMAKE_LFLAGS += /NODEFAULTLIB:LIBCMT /NODEFAULTLIB:LIBCMTD

	QMAKE_POST_LINK += echo $(DESTDIR_TARGET)
}

CONFIG(debug,debug|release) {
	DEFINES += _DEBUG
}

#------------------------------------------------------------------------------
# fugVidStreamLib

INCLUDEPATH += ../fugVidStreamLib

win32 {
	DEFINES += TARGET_OS_WIN

	CONFIG(debug,debug|release) {
		LIBS += -L../../../freeframe-debug -lfugVidStreamLib2
	} else {
		LIBS += -L../../../freeframe-release -lfugVidStreamLib2
	}
}

macx {
	DEFINES += TARGET_OS_MAC

	CONFIG += x86_64
	CONFIG -= x86

	CONFIG(debug,debug|release) {
		QMAKE_LFLAGS += -F/Users/alex/Documents/dev/freeframe-debug

		LIBS += -L/Users/alex/Documents/dev/freeframe-debug -framework fugVidStreamLib
	} else {
		QMAKE_LFLAGS += -F/Users/alex/Documents/dev/freeframe-release

		LIBS += -L/Users/alex/Documents/dev/freeframe-release -framework fugVidStreamLib
	}

	CONFIG(release,debug|release) {
		QMAKE_POST_LINK += macdeployqt ../../../freeframe-release/fugVidStreamTester.app -dmg
	}
}
