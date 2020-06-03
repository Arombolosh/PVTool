
TARGET = PVEnergy
TEMPLATE = app
include(../../../externals/IBK/projects/Qt/IBK.pri )
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt

LIBS += \
		-lSAM \
		-lDataIO \
		#-lMaterials \
		-lIBK

INCLUDEPATH = \
		../../../externals/SAM/src	\
		../../../externals/IBK/src	\
		#../../../externals/Materials/src	\
		../../../externals/DataIO/src


SOURCES += \
		#../../src/PCM_Material.cpp \
		../../src/PVTool_Energy.cpp \
		../../src/main.cpp

HEADERS += \
	#../../src/PCM_Material.h \
	../../src/PVTool_Energy.h
