
VERSION = 2.0.0

TEMPLATE = lib
TARGET = fugVidStreamLib

CONFIG -= qt

CONFIG(debug,debug|release) {
    DEFINES += _DEBUG
    DESTDIR = ../../../freeframe-debug
} else {
    DESTDIR = ../../../freeframe-release
}

contains(DEFINES,DEMO) {
    DESTDIR = ../../../freeframe-demo
}

win32 {
	DEFINES += TARGET_OS_WIN

	INCLUDEPATH += $$(LIBS)/boost_1_63_0
	INCLUDEPATH += $$(LIBS)/libjpeg-turbo/include
	INCLUDEPATH += $$(LIBS)/msinttypes

	LIBS += -L$$(LIBS)/boost_1_63_0/lib32-msvc-14.0
	LIBS += -L$$(LIBS)/libjpeg-turbo/lib

	DEFINES += _WIN32_WINNT=_WIN32_WINNT_WINXP
	DEFINES += _CRT_SECURE_NO_WARNINGS

	QMAKE_LFLAGS += /DEF:"$$PWD/fugVidStream.def" /verbose:lib

	QMAKE_CFLAGS_RELEASE    = -O2 -MT
	QMAKE_CXXFLAGS_RELEASE  = -O2 -MT
	QMAKE_CFLAGS_DEBUG      = -Zi -MTd
	QMAKE_CXXFLAGS_DEBUG    = -Zi -MTd

	QMAKE_LFLAGS_DEBUG   += /NODEFAULTLIB:LIBCMT
	QMAKE_LFLAGS_RELEASE += /NODEFAULTLIB:LIBCMTD /NODEFAULTLIB:MSVCRT

	LIBS += -lturbojpeg-static

	LIBS += -llegacy_stdio_definitions

	#QMAKE_PRE_LINK += runas /env /user:BIGFUG0\\administrator \"copy $(DESTDIR_TARGET) C:\\WINDOWS\\SYSTEM32\\\"
}

macx {
	DEFINES += TARGET_OS_MAC

	CONFIG += lib_bundle x86 x86_64

        INCLUDEPATH += /opt/local/include
	INCLUDEPATH += /opt/libjpeg-turbo/include

        LIBS += -L/opt/local/lib

	QMAKE_LFLAGS += /opt/libjpeg-turbo/lib/libturbojpeg.a

        LIBS += -lboost_system-mt -lboost_thread-mt

	FRAMEWORK_NAME = $$DESTDIR/$$TARGET".framework"

	QMAKE_POST_LINK += mkdir -pv $$FRAMEWORK_NAME/Resources &&

	QMAKE_POST_LINK += rm -rf /Library/Frameworks/fugVidStreamLib.framework &&

	QMAKE_POST_LINK += cp -R $$FRAMEWORK_NAME /Library/Frameworks/ &&

	# Create ZIP

	ARCHIVE_NAME = /Users/alex/Documents/dev/freeframe/freeframe-deploy/$$TARGET"-OSX-latest.zip"
	README_NAME  = /Users/alex/Documents/dev/freeframe/freeframe/readme/$$TARGET".txt"

	QMAKE_POST_LINK += rm -rf $$ARCHIVE_NAME &&

	exists( $$README_NAME ) {
		QMAKE_POST_LINK += cd /Users/alex/Documents/dev/freeframe/freeframe/readme/ &&
		QMAKE_POST_LINK += zip -9r $$ARCHIVE_NAME $$TARGET".txt" &&
		QMAKE_POST_LINK += cd $$PWD &&
	}

	QMAKE_POST_LINK += cd $$DESTDIR &&
	QMAKE_POST_LINK += zip -9r $$ARCHIVE_NAME $$TARGET".framework" &&
	QMAKE_POST_LINK += cd $$PWD
}

HEADERS += \
	StreamThread.h \
	StreamMemory.h \
	StreamEncoderUDP.h \
	StreamEncoderTCP.h \
	StreamEncoderMemory.h \
	StreamEncoderFactory.h \
	StreamEncoderConfig.h \
	StreamEncoder.h \
	StreamDecoderUDP.h \
	StreamDecoderTCP.h \
	StreamDecoderMemory.h \
	StreamDecoderFactory.h \
	StreamDecoder.h \
	Stream.h \
	ImageConvert.h \
	ImageCompression.h \
	fugVidStream.h

SOURCES += \
	StreamThread.cpp \
	StreamEncoderUDP.cpp \
	StreamEncoderTCP.cpp \
	StreamEncoderMemory.cpp \
	StreamEncoderFactory.cpp \
	StreamEncoderConfig.cpp \
	StreamEncoder.cpp \
	StreamDecoderUDP.cpp \
	StreamDecoderTCP.cpp \
	StreamDecoderMemory.cpp \
	StreamDecoderFactory.cpp \
	StreamDecoder.cpp \
	Stream.cpp \
	ImageConvert.cpp \
	ImageCompression.cpp \
	fugVidStream.cpp

OTHER_FILES += \
	fugVidStream.def
