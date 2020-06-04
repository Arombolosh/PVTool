#!/bin/bash


# Build script for building 'PVTool' and all dependend libraries

# Command line options:
#   [reldeb|release|debug]		build type
#   [2 [1..n]]					cpu count
#   [off|gprof]					gprof (includes gcc)
#   [verbose]					enable cmake to call verbose makefiles
#   []

# path export for mac
export PATH=~/Qt/5.11.3/gcc_64/bin:~/Qt/5.11.3/clang_64/bin:$PATH

CMAKELISTSDIR=$(pwd)
BUILDDIR="bb"

# set defaults
CMAKE_BUILD_TYPE=" -DCMAKE_BUILD_TYPE:STRING=RelWithDebInfo"
MAKE_CPUCOUNT="2"
BUILD_DIR_SUFFIX="gcc"
COMPILER=""
SKIP_TESTS="false"

# parse parameters, except gprof
for var in "$@"
do

	if [[ $var = "--help" ]];
	then
		echo "Command line options:"
		echo "  [reldeb|release|debug]      build type"
		echo "  [2 [1..n]]                  cpu count"
		echo "  [off|gprof]                 gprof (includes gcc)"
		echo "  [verbose]                   enable cmake to call verbose makefiles"

		exit
	fi

    if [[ $var = *[[:digit:]]* ]];
    then
		MAKE_CPUCOUNT=$var
		echo "Using $MAKE_CPUCOUNT CPUs for compilation"
    fi

    if [[ $var = "debug"  ]];
    then
		CMAKE_BUILD_TYPE=" -DCMAKE_BUILD_TYPE:STRING=Debug"
		echo "Debug build..."
    fi

    if [[ $var = "release"  ]];
    then
		CMAKE_BUILD_TYPE=" -DCMAKE_BUILD_TYPE:STRING=Release"
		echo "Release build..."
    fi

    if [[ $var = "reldeb"  ]];
    then
		CMAKE_BUILD_TYPE=" -DCMAKE_BUILD_TYPE:STRING=RelWithDebInfo"
		echo "RelWithDebInfo build..."
    fi

    if [[ $var = "verbose"  ]];
  	then
		CMAKE_OPTIONS="$CMAKE_OPTIONS -DCMAKE_VERBOSE_MAKEFILE:BOOL=ON"
	fi

done


# override compiler options
for var in "$@"
do

    if [[ $var = "gprof" ]];
    then
		COMPILER="gcc"
		BUILD_DIR_SUFFIX="gcc"
		CMAKE_COMPILER_OPTIONS="-DCMAKE_CXX_FLAGS="'-pg'" -DCMAKE_EXE_LINKER_FLAGS="'-pg'""
		echo "Gprof build, forcing GCC build..."
    fi

done


# create build dir if not exists
BUILDDIR=$BUILDDIR-$BUILD_DIR_SUFFIX
if [ ! -d $BUILDDIR ]; then
    mkdir -p $BUILDDIR
fi

cd $BUILDDIR && cmake $CMAKE_OPTIONS $CMAKE_BUILD_TYPE $CMAKE_COMPILER_OPTIONS $CMAKELISTSDIR && make -j$MAKE_CPUCOUNT &&
cd $CMAKELISTSDIR &&
mkdir -p ../../bin/release &&
if [ -e $BUILDDIR/PVTool/PVTool ]; then
  echo "*** Copying PVTool to bin/release ***" &&
  cp $BUILDDIR/PVTool/PVTool ../../bin/release/PVTool
fi &&
if [ -e $BUILDDIR/PVTool/PVTool.app ]; then
  if [ -e ../../bin/release/PVTool.app ]; then
    rm -rf ../../bin/release/PVTool.app
  fi
fi 

