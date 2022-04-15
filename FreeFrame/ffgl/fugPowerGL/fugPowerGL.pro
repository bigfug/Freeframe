
include( ../../../freeframe-global.pri )

TEMPLATE = lib

TARGET_BASE = fugPowerGL

contains( QT_ARCH, i386 ) {
	TARGET_BASE = "$${TARGET_BASE}32"
	DEFINES += FF_VERSION_15
	CONFIG -= x86_64
} else {
	TARGET_BASE = "$${TARGET_BASE}64"
	DEFINES += FF_VERSION_16
	CONFIG -= x86
}

TARGET = $$qtLibraryTarget( $$TARGET_BASE )

CONFIG -= qt

DEFINES += FF_IS_SOURCE

INCLUDEPATH += $$_PRO_FILE_PWD_

win32 {
	DEFINES += TARGET_OS_WIN

	INCLUDEPATH += $$(LIBS)/msinttypes

	DEFINES += _CRT_SECURE_NO_WARNINGS
	QMAKE_LFLAGS += /DEF:"..\\..\\..\\..\\freeframe\\freeframe\\shared\\Library.def"
	LIBS += -ladvapi32 -luser32 -lopengl32

	QMAKE_CFLAGS_RELEASE    = -O2 /MT
	QMAKE_CXXFLAGS_RELEASE  = -O2 /MT
	QMAKE_CFLAGS_DEBUG      = -Zi -MTd
	QMAKE_CXXFLAGS_DEBUG    = -Zi -MTd

	QMAKE_LFLAGS_DEBUG   += /NODEFAULTLIB:LIBCMT  /NODEFAULTLIB:MSVCRT
	QMAKE_LFLAGS_RELEASE += /NODEFAULTLIB:LIBCMTD /NODEFAULTLIB:MSVCRT

	LIBS += -llegacy_stdio_definitions
}

macx {
	DEFINES += TARGET_OS_MAC
	LIBS    += -framework OpenGL
	CONFIG  += lib_bundle

	CONFIG(debug,debug|release) {
		DEFINES += _DEBUG
	}

	BUNDLE_NAME    = $$DESTDIR/$$TARGET".bundle"
	RESOURCES_NAME = $$BUNDLE_NAME/Contents/Resources
	FRAMEWORK_NAME = $$DESTDIR/$$TARGET_BASE".framework"
	INSTALLDEST    = $$INSTALLBASE/plugins
	ARCHIVE_NAME   = $$INSTALLBASE/$$TARGET"-OSX-latest.zip"
	README_NAME    = $$DESTDIR/../freeframe/readme/$$TARGET".txt"

	QMAKE_POST_LINK  = echo

	QMAKE_POST_LINK += && rm -rf $$BUNDLE_NAME

	QMAKE_POST_LINK += && mkdir -pv $$BUNDLE_NAME/Contents/MacOS
	QMAKE_POST_LINK += && cp $$FRAMEWORK_NAME/Resources/Info.plist $$BUNDLE_NAME/Contents/
	QMAKE_POST_LINK += && cp $$FRAMEWORK_NAME/$$TARGET $$BUNDLE_NAME/Contents/MacOS/

	QMAKE_POST_LINK += && mkdir -pv $$RESOURCES_NAME
	QMAKE_POST_LINK += && cp /usr/local/lib/libportaudio.2.dylib $$RESOURCES_NAME/

	QMAKE_POST_LINK += && install_name_tool -change /Users/bigfug/dev/libs/portaudio-x64/lib/libportaudio.2.dylib @loader_path/../Resources/libportaudio.2.dylib $$BUNDLE_NAME/Contents/MacOS/$$TARGET

	QMAKE_POST_LINK += && rm -rf $$FRAMEWORK_NAME

	CONFIG(debug,debug|release) {
		QMAKE_POST_LINK += && rm -rf $$FRAMEWORK_NAME
	}

	plugin.path  = $$INSTALLDEST
	plugin.files = $$BUNDLE_NAME

	INSTALLS += plugin

	zip.path  = $$INSTALLDEST
	zip.files = $$BUNDLE_NAME

	INSTALLS += zip

	exists( $$README_NAME ) {
		zip.files += $$README_NAME
	}

	zip.extra = zip -9 $$ARCHIVE_NAME $$zip.files
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
	../../shared/FFT.h \
	../../shared/AudioWindow.h

SOURCES += \
	Plugin.cpp \
	Instance.cpp \
	../../shared/PluginBase.cpp \
	../../shared/Library.cpp \
	../../shared/InstanceBase.cpp \
	../../shared/AudioWindow.cpp

#------------------------------------------------------------------------------
# glew/glu

DEFINES += GLEW_STATIC

win32 {
	contains( QT_ARCH, i386 ) {
		LIBS += -L$$(LIBS)/glew.32.2015/lib/Release -llibglew32
		INCLUDEPATH += $$(LIBS)/glew-2.0.0/include
	} else {
		LIBS += -L$$(LIBS)/glew.64.2015/lib/Release -llibglew32
		INCLUDEPATH += $$(LIBS)/glew-2.0.0/include
	}
}

macx {
	INCLUDEPATH += $$(LIBS)/glew-2.0.0/include
	LIBS += -L$$(LIBS)/glew-2.0.0 -lGLEW
}

#------------------------------------------------------------------------------
# portaudio

windows {
	INCLUDEPATH += $$(LIBS)/portaudio/include

	contains( QT_ARCH, i386 ) {
		CONFIG(debug,debug|release) {
			LIBS += -L$$(LIBS)/portaudio.32.2015/Debug
		} else {
			LIBS += -L$$(LIBS)/portaudio.32.2015/Release
		}

		LIBS += -lportaudio_static_x86

		LIBS += -L"C:\Program Files (x86)\Microsoft SDKs\Windows\v7.1A\Lib"
	} else {
		CONFIG(debug,debug|release) {
			LIBS += -L$$(LIBS)/portaudio.64.2015/Debug
		} else {
			LIBS += -L$$(LIBS)/portaudio.64.2015/Release
		}

		LIBS += -lportaudio_static_x64

		LIBS += -L"C:\Program Files (x86)\Microsoft SDKs\Windows\v7.1A\Lib\x64"
	}

	LIBS += -lksguid
}

macx {
	LIBS += -L$$(LIBS)/portaudio-x64/lib -lportaudio

	INCLUDEPATH += $$(LIBS)/portaudio-x64/include
}
