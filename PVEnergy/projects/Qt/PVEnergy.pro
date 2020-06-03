
TARGET = PVEnergy
TEMPLATE = app
include(../../../externals/IBK/projects/Qt/IBK.pri )
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt

LIBS += \
		-lSAM \
		-lDataIO \
		-lMaterials \
		-lIBK

INCLUDEPATH = \
		../../../externals/SAM/src	\
		../../../externals/IBK/src	\
		../../../externals/Materials/src	\
		../../../externals/DataIO/src


SOURCES += \
		../../src/PCM_Material.cpp \
		../../src/PVTool_Energy.cpp \
		../../src/lib_util.cpp \
		../../src/main.cpp

HEADERS += \
	../../src/6par_gamma.h \
	../../src/6par_jacobian.h \
	../../src/6par_lu.h \
	../../src/6par_newton.h \
	../../src/6par_search.h \
	../../src/6par_solve.h \
	../../src/PCM_Material.h \
	../../src/PVTool_Energy.h \
	../../src/lib_util.h
