
TEMPLATE = lib

win32:TARGET = $$qtLibraryTarget(fugVidSnd2GL)
macx:TARGET = fugVidSnd2GL

CONFIG -= qt

DEFINES += FF_VERSION_15

INCLUDEPATH += $$_PRO_FILE_PWD_

CONFIG(debug,debug|release) {
	DESTDIR = ../../../../freeframe-debug/plugins
} else {
	DESTDIR = ../../../../freeframe-release/plugins
}

win32 {
	DEFINES += TARGET_OS_WIN
	DEFINES += _CRT_SECURE_NO_WARNINGS
	DEFINES += USE_MMX

	QMAKE_LFLAGS += /DEF:"..\\..\\..\\..\\freeframe\\freeframe\\shared\\Library.def"
	LIBS += -ladvapi32 -luser32 -lopengl32
	INCLUDEPATH += $$(LIBS)/boost_1_63_0
	INCLUDEPATH += $$(LIBS)/msinttypes
	INCLUDEPATH += ../../../ini

	LIBS += -lshlwapi -lshell32
	LIBS += -L$$(LIBS)/boost_1_63_0/lib32-msvc-14.0

	QMAKE_CFLAGS_RELEASE    = -O2 -MT
	QMAKE_CXXFLAGS_RELEASE  = -O2 -MT
	QMAKE_CFLAGS_DEBUG      = -Zi -MTd
	QMAKE_CXXFLAGS_DEBUG    = -Zi -MTd

	QMAKE_LFLAGS_DEBUG   += /NODEFAULTLIB:LIBCMT  /NODEFAULTLIB:MSVCRT
	QMAKE_LFLAGS_RELEASE += /NODEFAULTLIB:LIBCMTD /NODEFAULTLIB:MSVCRT
}

macx {
	DEFINES += TARGET_OS_MAC
	LIBS += -L/opt/local/lib -framework OpenGL
	CONFIG -= x86_64
	CONFIG += lib_bundle

	INCLUDEPATH += ../../../ini
	INCLUDEPATH += ../../../../../boost_1_53_0

	LIBS += -L../../../../../boost_1_53_0/stage/lib

	BUNDLE_NAME    = $$DESTDIR/$$TARGET".bundle"
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
	QMAKE_POST_LINK += zip -9r $$ARCHIVE_NAME $$TARGET".bundle" &&
	QMAKE_POST_LINK += cd $$PWD
}

INCLUDEPATH += ../../include
INCLUDEPATH += ../../shared

HEADERS += \
	Plugin.h \
	Instance.h \
	../../include/FreeFrame.h \
	../../include/FFGL.h \
	../../shared/PluginBase.h \
	../../shared/InstanceBase.h \
	../../../ini/INIReader.h \
	../../../ini/ini.h \
	../../shared/TexToBuf.h

SOURCES += \
	Plugin.cpp \
	Instance.cpp \
	../../shared/PluginBase.cpp \
	../../shared/Library.cpp \
	../../shared/InstanceBase.cpp \
	../../../ini/INIReader.cpp \
	../../../ini/ini.c \
	../../shared/TexToBuf.cpp

#------------------------------------------------------------------------------
# glew/glu

DEFINES += GLEW_STATIC

win32 {
	LIBS += -L$$(LIBS)/glew.32.2015/lib/Release -llibglew32
	INCLUDEPATH += $$(LIBS)/glew-2.0.0/include
}

macx {
	SOURCES += ../../../../../glew-1.8.0/src/glew.c
	INCLUDEPATH += ../../../../../glew-1.8.0/include
}

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
		QMAKE_LFLAGS += -F../../../../freeframe-debug

		LIBS += -L../../../../freeframe-debug -framework fugVidStreamLib
	} else {
		QMAKE_LFLAGS += -F../../../../freeframe-release

		LIBS += -L../../../../freeframe-release -framework fugVidStreamLib
	}
}
