@echo off
set PATH=c:\Python27;%PATH%
python ..\scripts\createRelease.py -p PVTool --constants-path ..\..\PVTool\src --x64 %*

