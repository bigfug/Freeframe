TEMPLATE = lib

win32:TARGET = $$qtLibraryTarget(fugVidRcv2)
macx:TARGET = fugVidRcv2

CONFIG -= qt

DEFINES += FF_VERSION_10
DEFINES += FF_IS_SOURCE

INCLUDEPATH += $$_PRO_FILE_PWD_

CONFIG(debug,debug|release) {
	DESTDIR = ../../../../freeframe-debug/plugins
} else {
	DESTDIR = ../../../../freeframe-release/plugins
}

win32 {
	INCLUDEPATH += $$(LIBS)/boost_1_55_0
	INCLUDEPATH += $$(LIBS)/msinttypes
	DEFINES += _CRT_SECURE_NO_WARNINGS
	QMAKE_LFLAGS += /DEF:"..\\..\\..\\..\\freeframe\\freeframe\\shared\\Library.def"

	QMAKE_CFLAGS_RELEASE    = -O2 /MT
	QMAKE_CXXFLAGS_RELEASE  = -O2 /MT
	QMAKE_CFLAGS_DEBUG      = -Zi -MTd
	QMAKE_CXXFLAGS_DEBUG    = -Zi -MTd

	QMAKE_LFLAGS_DEBUG   += /NODEFAULTLIB:LIBCMT
	QMAKE_LFLAGS_RELEASE += /NODEFAULTLIB:LIBCMTD /NODEFAULTLIB:MSVCRT
}

macx {
	DEFINES += TARGET_OS_MAC
	LIBS += -L/opt/local/lib
	CONFIG -= x86_64
	CONFIG += lib_bundle

	INCLUDEPATH += ../../../ini
	INCLUDEPATH += ../../../../../boost_1_53_0

	LIBS += -L../../../../../boost_1_53_0/stage/lib

	BUNDLE_NAME    = $$DESTDIR/$$TARGET".frf"
	FRAMEWORK_NAME = $$DESTDIR/$$TARGET".framework"

	QMAKE_POST_LINK += rm -rf $$BUNDLE_NAME &&

	QMAKE_POST_LINK += mkdir -pv $$BUNDLE_NAME/Contents/MacOS &&
	QMAKE_POST_LINK += cp $$FRAMEWORK_NAME/Contents/Info.plist $$BUNDLE_NAME/Contents/ &&
	QMAKE_POST_LINK += cp $$FRAMEWORK_NAME/$$TARGET $$BUNDLE_NAME/Contents/MacOS/ &&

	QMAKE_POST_LINK += install_name_tool -change fugVidStreamLib.framework/Versions/2/fugVidStreamLib /Library/Frameworks/fugVidStreamLib.framework/Versions/2/fugVidStreamLib $$BUNDLE_NAME/Contents/MacOS/$$TARGET &&

	QMAKE_POST_LINK += rm -rf $$FRAMEWORK_NAME &&

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
	QMAKE_POST_LINK += zip -9r $$ARCHIVE_NAME $$TARGET".frf" &&
	QMAKE_POST_LINK += cd $$PWD
}

INCLUDEPATH += ../../include
INCLUDEPATH += ../../shared

CONFIG(debug,debug|release) {
	DESTDIR = ../../../../freeframe-debug/plugins
} else {
	DESTDIR = ../../../../freeframe-release/plugins
}

HEADERS += \
	Plugin.h \
	Instance.h \
	../../include/FreeFrame.h \
	../../include/FFGL.h \
	../../shared/PluginBase.h \
	../../shared/InstanceBase.h

SOURCES += \
	Plugin.cpp \
	Instance.cpp \
	../../shared/PluginBase.cpp \
	../../shared/Library.cpp \
	../../shared/InstanceBase.cpp

INCLUDEPATH += ../../../fugVidStream/fugVidStreamLib

#------------------------------------------------------------------------------
# fugVidStreamLib

INCLUDEPATH += ../../../fugVidStream/fugVidStreamLib

win32 {
	CONFIG(debug,debug|release) {
		LIBS += -L../../../../freeframe-debug -lfugVidStreamLib2
	} else {
		LIBS += -L../../../../freeframe-release -lfugVidStreamLib2
	}
}

macx {
	DEFINES += TARGET_OS_MAC

	CONFIG(debug,debug|release) {
		QMAKE_LFLAGS += -F/Users/alex/Documents/dev/freeframe-debug

		LIBS += -L/Users/alex/Documents/dev/freeframe-debug -framework fugVidStreamLib
	} else {
		QMAKE_LFLAGS += -F/Users/alex/Documents/dev/freeframe-release

		LIBS += -L/Users/alex/Documents/dev/freeframe-release -framework fugVidStreamLib
	}
}
