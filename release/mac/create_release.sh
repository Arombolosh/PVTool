#!/bin/bash
export PATH=~/Qt/4.8.7/bin:$PATH
python ../scripts/createRelease.py --product PVTool --constants-path ../../PVTool/src  $*

