@echo off
set PATH=c:\Qt\5.7.0_VC14\5.7\msvc2015\bin\;%PATH%
lupdate ..\..\projects\Qt\MaterialsTranslations.pro

pause

linguist Materials_de.ts
::linguist Materials_es.ts
