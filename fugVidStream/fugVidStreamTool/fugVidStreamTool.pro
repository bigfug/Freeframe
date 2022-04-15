#-------------------------------------------------
#
# Project created by QtCreator 2013-06-02T18:26:54
#
#-------------------------------------------------

QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = fugVidStreamTool
TEMPLATE = app

SOURCES += main.cpp\
		mainwindow.cpp \
	fugstreamdecoder.cpp

HEADERS  += mainwindow.h \
	fugstreamdecoder.h

FORMS    += mainwindow.ui

CONFIG(debug,debug|release) {
	DESTDIR = ../../../freeframe-debug
} else {
	DESTDIR = ../../../freeframe-release
}

win32 {
	INCLUDEPATH += $$(LIBS)/msinttypes

	LIBS += -lshlwapi -lshell32
}

macx {
	INCLUDEPATH += /opt/local/include
	LIBS += -L/opt/local/lib

	CONFIG += x86_64
	CONFIG -= x86

	CONFIG(release,debug|release) {
		APP_NAME    = $$DESTDIR/$$TARGET".app"

		QMAKE_POST_LINK += macdeployqt $$APP_NAME &&

		QMAKE_POST_LINK += rm -rf $$APP_NAME/Contents/Frameworks/fugVidStreamLib.framework &&

		QMAKE_POST_LINK += install_name_tool -change @executable_path/../Frameworks/fugVidStreamLib.framework/Versions/2/fugVidStreamLib /Library/Frameworks/fugVidStreamLib.framework/Versions/2/fugVidStreamLib $$APP_NAME/Contents/MacOS/$$TARGET &&

		# QMAKE_POST_LINK += macdeployqt $$APP_NAME -no-plugins -dmg

		# Create ZIP

		ARCHIVE_NAME = /Users/alex/Documents/dev/freeframe/freeframe-deploy/$$TARGET"-OSX-latest.zip"
		README_NAME  = /Users/alex/Documents/dev/freeframe/freeframe/readme/$$TARGET".txt"

		QMAKE_POST_LINK += rm -rf $$ARCHIVE_NAME &&

		QMAKE_POST_LINK += cd /Library/Frameworks &&
		QMAKE_POST_LINK += zip -9r $$ARCHIVE_NAME fugVidStreamLib.framework &&
		QMAKE_POST_LINK += cd $$PWD &&

		exists( $$README_NAME ) {
			QMAKE_POST_LINK += cd /Users/alex/Documents/dev/freeframe/freeframe/readme/ &&
			QMAKE_POST_LINK += zip -9r $$ARCHIVE_NAME $$TARGET".txt" &&
			QMAKE_POST_LINK += cd $$PWD &&
		}

		QMAKE_POST_LINK += cd $$DESTDIR &&
		QMAKE_POST_LINK += zip -9r $$ARCHIVE_NAME $$TARGET".app" &&
		QMAKE_POST_LINK += cd $$PWD
	}
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

	CONFIG(debug,debug|release) {
		QMAKE_LFLAGS += -F../../../freeframe-debug/freeframe-debug

		LIBS += -L../../../freeframe-debug/freeframe-debug -framework fugVidStreamLib
	} else {
		QMAKE_LFLAGS += -F../../../freeframe-release

		LIBS += -L../../../freeframe-release -framework fugVidStreamLib
	}
}
