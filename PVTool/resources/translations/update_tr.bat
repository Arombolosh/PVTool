@echo off
set PATH=c:\Qt\5.11.3\msvc2015_64\bin;%PATH%
lupdate ..\..\projects\Qt\PVTool.pro

pause

linguist PVTool_de.ts

