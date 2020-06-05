# remember to set DYLD_FALLBACK_LIBRARY_PATH on MacOSX
# set LD_LIBRARY_PATH on Linux

TARGET = PVTool
TEMPLATE = app

# this pri must be sourced from all our applications
include( ../../../externals/IBK/projects/Qt/IBK.pri )

QT += xml core gui network printsupport widgets

LIBS += -L../../../lib$${DIR_PREFIX} \
	-lIBK \
	-lDataIO \
        -lSAM


INCLUDEPATH = \
	../../src \
	../../../externals/IBK/src \
	../../../externals/DataIO/src \
	../../../externals/SAM/src \
	../../../externals/Materials/src \
	../../../PVEnergy/src


TRANSLATIONS += ../../resources/translations/PVTool_de.ts
CODECFORSRC = UTF-8

RESOURCES += \
	../../resources/PVTool.qrc

SOURCES += \
	../../../PVEnergy/src/PVTool_Energy.cpp \
	../../src/PVTButtonBar.cpp \
	../../src/PVTConstants.cpp \
	../../src/PVTDebugApplication.cpp \
	../../src/PVTDirectories.cpp \
	../../src/PVTResultWidget.cpp \
	../../src/PVToolWidget.cpp \
	../../src/main.cpp

HEADERS += \
	../../../PVEnergy/src/PVTool_Energy.h \
	../../src/PVTButtonBar.h \
	../../src/PVTConstants.h \
	../../src/PVTDebugApplication.h \
	../../src/PVTDirectories.h \
	../../src/PVTResultWidget.h \
	../../src/PVToolWidget.h

FORMS += \
	../../src/PVTResultWidget.ui \
	../../src/PVToolWidget.ui

