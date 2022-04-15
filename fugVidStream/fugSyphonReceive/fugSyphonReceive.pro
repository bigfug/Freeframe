#-------------------------------------------------
#
# Project created by QtCreator 2013-07-07T12:59:50
#
#-------------------------------------------------

QT       += core gui opengl

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = fugSyphonReceive
TEMPLATE = app

CONFIG -= x86_64
CONFIG += x86

CONFIG(debug,debug|release) {
	DESTDIR = ../../../freeframe-debug
} else {
	DESTDIR = ../../../freeframe-release
}

SOURCES += main.cpp\
		mainwindow.cpp \
	syphonoutput.cpp \
	../../FreeFrame/shared/TexToBuf.cpp \
	../fugVidStreamTool/fugstreamdecoder.cpp

HEADERS  += mainwindow.h \
	syphonoutput.h \
	../../FreeFrame/shared/TexToBuf.h \
	../fugVidStreamTool/fugstreamdecoder.h

FORMS    += mainwindow.ui

QMAKE_LFLAGS += -framework Syphon
QMAKE_LFLAGS += -framework OpenGL
QMAKE_LFLAGS += -framework Cocoa

OBJECTIVE_SOURCES += \
	syphonsend.mm

OBJECTIVE_HEADERS += \
	syphonsend.h

QMAKE_MACOSX_DEPLOYMENT_TARGET = 10.6

DEFINES += FF_VERSION_15

INCLUDEPATH += ../../FreeFrame/include
INCLUDEPATH += ../../FreeFrame/shared
INCLUDEPATH += ../fugVidStreamTool

CONFIG(release,debug|release) {
	APP_NAME    = $$DESTDIR/$$TARGET".app"

	QMAKE_POST_LINK += macdeployqt $$APP_NAME &&

	QMAKE_POST_LINK += cp -R /Library/Frameworks/Syphon.framework $$APP_NAME/Contents/Frameworks/ &&

	QMAKE_POST_LINK += install_name_tool -change @loader_path/../Frameworks/Syphon.framework/Versions/A/Syphon @executable_path/../Frameworks/Syphon.framework/Versions/A/Syphon $$APP_NAME/Contents/MacOS/$$TARGET &&

	QMAKE_POST_LINK += rm -rf $$APP_NAME/Contents/Frameworks/fugVidStreamLib.framework &&

	QMAKE_POST_LINK += install_name_tool -change @executable_path/../Frameworks/fugVidStreamLib.framework/Versions/2/fugVidStreamLib /Library/Frameworks/fugVidStreamLib.framework/Versions/2/fugVidStreamLib $$APP_NAME/Contents/MacOS/$$TARGET &&

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

#------------------------------------------------------------------------------

DEFINES += GLEW_STATIC

SOURCES += ../../../../glew-1.8.0/src/glew.c
INCLUDEPATH += ../../../../glew-1.8.0/include

#------------------------------------------------------------------------------
# fugVidStreamLib

INCLUDEPATH += ../fugVidStreamLib

win32 {
	DEFINES += TARGET_OS_WIN

	CONFIG(debug,debug|release) {
		LIBS += -L../../../freeframe-debug -lfugVidStreamLib2
	} else {
		LIBS += -L../../../freeframe-release -lfugVidStreamLib
	}
}

macx {
	DEFINES += TARGET_OS_MAC

	CONFIG(debug,debug|release) {
		QMAKE_LFLAGS += -F../../../freeframe-debug

		LIBS += -L../../../freeframe-debug -framework fugVidStreamLib
	} else {
		QMAKE_LFLAGS += -F../../../freeframe-release

		LIBS += -L../../../freeframe-release -framework fugVidStreamLib
	}
}
