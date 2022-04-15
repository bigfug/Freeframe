#-------------------------------------------------
#
# Project created by QtCreator 2013-11-29T18:02:00
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = fugScreenCapture
TEMPLATE = app

CONFIG(debug,debug|release) {
    DEFINES += _DEBUG
    DESTDIR = ../../../freeframe-debug
} else {
    DESTDIR = ../../../freeframe-release
}

contains(DEFINES,DEMO) {
    TARGET = fugScreenCaptureDemo
    DESTDIR = ../../../freeframe-demo
}

SOURCES += main.cpp\
        mainwindow.cpp \
    windowcapturebase.cpp \
    windowcapture.cpp \
    winprintwindow.cpp

HEADERS  += mainwindow.h \
    windowcapturebase.h \
    windowcapture.h \
    winprintwindow.h

FORMS    += mainwindow.ui

win32 {
    LIBS += -luser32 -lgdi32
}

win32 {
    SOURCES += wingetclientrect.cpp
    HEADERS += wingetclientrect.h
}

#------------------------------------------------------------------------------
# fugVidStreamLib

!contains(DEFINES,DEMO) {
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
}
