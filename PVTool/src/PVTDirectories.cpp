#include "PVTDirectories.h"

#include <QCoreApplication>
#include <QDesktopServices>
#include <QDir>

#include <IBK_configuration.h>

QString PVTDirectories::resourcesRootDir() {
	QString installPath = qApp->applicationDirPath();

#ifdef IBK_DEPLOYMENT
	// deployment mode

#if defined(Q_OS_WIN)
	// in Deployment mode, resources are below install directory
	return installPath + "/resources";
#elif defined(Q_OS_MAC)
	// in deployment mode, we have them in PVTool.app/Contents/Resources
	return installPath + "/../Resources";
#elif defined(Q_OS_UNIX)

	// in deployment mode, we have them in "/usr/share/PVTool" or "/usr/local/share/PVTool"
	// unless otherwise specified in the settings

	QString resRootPath;
	if (installPath.indexOf("/usr/bin") == 0)
		resRootPath = "/usr/share/PVTool";
	else if (installPath.indexOf("/usr/local/bin") == 0)
		resRootPath = "/usr/local/share/PVTool";
	else
		resRootPath = installPath + "/../resources";

	return resRootPath;

#endif


#else // IBK_DEPLOYMENT

	// development (IDE) mode

#if defined(Q_OS_WIN)
	// in development mode, we have the resources in the data directory
	// executables are build in bin/debug or bin/release
	return installPath + "/../../data";
#elif defined(Q_OS_MAC)
	// in development mode, we have the resources outside the bundle
	return installPath + "/../../../../data";
#elif defined(Q_OS_UNIX)
	return installPath + "/../../data";
#endif

#endif // IBK_DEPLOYMENT
}


QString PVTDirectories::translationsDir() {
#ifdef IBK_DEPLOYMENT

	// deployment mode
	return resourcesRootDir() + "/translations";

#else // IBK_DEPLOYMENT
	// development (IDE) mode
	QString installPath = qApp->applicationDirPath();
#if defined(Q_OS_MAC)
	return installPath + "/../../../../../PVTool/resources/translations";
#else
	return installPath + "/../../PVTool/resources/translations";
#endif

#endif // IBK_DEPLOYMENT
}


QString PVTDirectories::userDataDir() {
	// we have different user data directories, based on OS
#if defined(Q_OS_WIN)
	return QDir::toNativeSeparators(QDir::home().absolutePath() + "/AppData/Roaming/PVTool");
#else
	// on Unix/Mac OS we store user data under home directory
	return QDir::toNativeSeparators(QDir::home().absolutePath() + "/.local/share/PVTool");
#endif // Q_OS_WIN
}


QString PVTDirectories::globalLogFile() {
	return userDataDir() + "/PVTool.log";
}

