# -------------------------------------------------
# Project for Materials library
# -------------------------------------------------
TARGET = Materials
TEMPLATE = lib

# this pri must be sourced from all our libraries,
# it contains all functions defined for casual libraries
include( ../../../IBK/projects/Qt/IBK.pri )


unix|mac {
	VER_MAJ = 1
	VER_MIN = 0
	VER_PAT = 0
	VERSION = $${VER_MAJ}.$${VER_MIN}.$${VER_PAT}
}

LIBS += -lIBK

INCLUDEPATH += ../../../IBK/src

SOURCES += \
	../../src/MM_AirTransport.cpp \
	../../src/MM_ThermalStorage.cpp \
	../../src/MM_ThermalTransport.cpp \
	../../src/MM_MoistureTransport.cpp \
	../../src/MM_ModelDataContainer.cpp \
	../../src/MM_MoistureStorage.cpp \
	../../src/MM_MaterialCategory.cpp \
	../../src/MM_Identification.cpp \
	../../src/MM_Functions.cpp \
	../../src/MM_KeywordList.cpp \
	../../src/MM_Utils.cpp \
	../../src/MM_Material.cpp \
	../../src/MM_Version.cpp \
	../../src/MM_MaterialDistances.cpp \
	../../src/MM_CreatingModelBase.cpp \
	../../src/MM_MeasuredData.cpp \
	../../src/MM_MaterialFunctionBase.cpp \
	../../src/MM_ModelHandler.cpp \
	../../src/MM_Foils.cpp \
    ../../src/MM_VaporContactResistanceModel.cpp

HEADERS += \
	../../src/MM_ThermalStorage.h \
	../../src/MM_Types.h \
	../../src/MM_Utils.h \
	../../src/MM_Material.h \
	../../src/MM_ThermalTransport.h \
	../../src/MM_MoistureTransport.h \
	../../src/MM_MoistureStorage.h \
	../../src/MM_Identification.h \
	../../src/MM_Functions.h \
	../../src/MM_Constants.h \
	../../src/MM_MaterialCategory.h \
	../../src/MM_AirTransport.h \
	../../src/MM_KeywordList.h \
	../../src/MM_ModelDataContainer.h \
	../../src/MM_Version.h \
	../../src/MM_MaterialDistances.h \
	../../src/MM_CreatingModelBase.h \
	../../src/MM_MaterialFunctionBase.h \
	../../src/MM_MeasuredData.h \
	../../src/MM_ModelHandler.h \
	../../src/MM_Foils.h \
    ../../src/MM_VaporContactResistanceModel.h
















