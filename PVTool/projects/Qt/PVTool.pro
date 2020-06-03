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
	-lSAM \
	-lMaterials

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
	../../src/PCM_Material.cpp \
	../../src/PVT_DebugApplication.cpp \
	../../src/PVToolWidget.cpp \
	../../src/main.cpp

HEADERS += \
	../../src/PCM_Material.h \
	../../src/PVT_DebugApplication.h \
	../../src/PVToolWidget.h

FORMS += \
	../../src/PVToolWidget.ui

