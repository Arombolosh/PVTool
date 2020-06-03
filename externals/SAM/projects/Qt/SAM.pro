# -----------------------
# Project for SAM library
# -----------------------

# first we define what we are
TARGET = SAM
TEMPLATE = lib

# this pri must be sourced from all our libraries,
# it contains all functions defined for casual libraries
include( ../../../IBK/projects/Qt/IBK.pri )

# finally we setup our custom library specfic things
# like version number etc., we also may reset all
#
unix|mac {
	VER_MAJ = 1
	VER_MIN = 0
	VER_PAT = 0
	VERSION = $${VER_MAJ}.$${VER_MIN}.$${VER_PAT}
}

HEADERS += \
	../../src/6par_gamma.h \
	../../src/6par_jacobian.h \
	../../src/6par_lu.h \
	../../src/6par_newton.h \
	../../src/6par_search.h \
	../../src/6par_solve.h \
	../../src/lib_util.h

SOURCES += \
	../../src/lib_util.cpp
