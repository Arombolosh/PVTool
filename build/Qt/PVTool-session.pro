# Project for PVTool session and all its libraries

TEMPLATE=subdirs

# SUBDIRS lists all subprojects
SUBDIRS += \
	PVTool \
	PVEnergy \
	IBK \
	DataIO \
	SAM

# where to find the sub projects
PVTool.file = ../../PVTool/projects/Qt/PVTool.pro
PVEnergy.file = ../../PVEnergy/projects/Qt/PVEnergy.pro

IBK.file = ../../externals/IBK/projects/Qt/IBK.pro
DataIO.file = ../../externals/DataIO/projects/Qt/DataIO.pro
SAM.file = ../../externals/SAM/projects/Qt/SAM.pro

# dependencies
PVTool.depends = DataIO IBK SAM
PVEnergy.depends = DataIO IBK SAM

DataIO.depends = IBK
