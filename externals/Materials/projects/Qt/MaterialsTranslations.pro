# -------------------------------------------------
# Project for Materials Translations library
# -------------------------------------------------
TARGET = MaterialsTranslations
TEMPLATE = lib

# this pri must be sourced from all our libraries,
# it contains all functions defined for casual libraries
include( ../../../IBK/projects/Qt/IBK.pri )

QT += core gui

unix|mac {
	VER_MAJ = 1
	VER_MIN = 0
	VER_PAT = 0
	VERSION = $${VER_MAJ}.$${VER_MIN}.$${VER_PAT}
}

LIBS += -lMaterials \
		-lIBK

INCLUDEPATH += ../../src

TRANSLATIONS += ../../resources/translations/Materials_de.ts \
	../../resources/translations/Materials_fr.ts \
	../../resources/translations/Materials_it.ts \
	../../resources/translations/Materials_es.ts


SOURCES += \
	../../srcTranslations/MM_KeywordListQt.cpp

HEADERS += \
	../../srcTranslations/MM_KeywordListQt.h


