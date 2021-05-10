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
	../../src/MainWindow.cpp \
	../../src/PVTAboutDialog.cpp \
	../../src/PVTConstants.cpp \
	../../src/PVTDebugApplication.cpp \
	../../src/PVTDirectories.cpp \
	../../src/PVTLicenceDialog.cpp \
	../../src/PVTPostProcDialog.cpp \
		../../src/PVTResultDialog.cpp \
	../../src/PVToolWidget.cpp \
	../../src/main.cpp

HEADERS += \
	../../../PVEnergy/src/PVTool_Energy.h \
	../../src/MainWindow.h \
	../../src/PVTAboutDialog.h \
	../../src/PVTConstants.h \
	../../src/PVTDebugApplication.h \
	../../src/PVTDirectories.h \
	../../src/PVTLicenceDialog.h \
	../../src/PVTPostProcDialog.h \
		../../src/PVToolWidget.h \
		../../src/PVTResultDialog.h

FORMS += \
	../../src/PVTAboutDialog.ui \
	../../src/PVTLicenceDialog.ui \
	../../src/PVTPostProcDialog.ui \
		../../src/PVTResultDialog.ui \
	../../src/PVToolWidget.ui

