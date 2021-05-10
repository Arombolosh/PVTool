#include <QApplication>
#include <QFont>
#include <QStandardPaths>
#include <QIcon>

#include <IBK_Exception.h>
#include <IBK_messages.h>
#include <IBK_configuration.h>
#include <IBK_ArgParser.h>

#include "PVTDebugApplication.h"
#include "PVToolWidget.h"
#include "MainWindow.h"
#include "PVTConstants.h"

#include <memory>
#include <iostream>

void qDebugMsgHandler(QtMsgType type, const QMessageLogContext &context, const QString &msg) {
	(void) type;
	(void) context;
	std::cout << msg.toStdString() << std::endl;
}

void testDirk(){
	std::string txt = IBK::val2string(12345678.12, 12);
	txt = IBK::val2string(12345678.12, 10, 12, '0');
	txt= "";
}

int main(int argc, char *argv[]) {
//	const char * const FUNC_ID = "[main]";

	//testDirk();

	PVTDebugApplication a(argc, argv);
	a.setApplicationName(PROGRAM_NAME);
	a.setWindowIcon(QIcon(":/gfx/icon/icon_64.jpg"));

	qInstallMessageHandler(qDebugMsgHandler);

	// *** Locale setup for Unix/Linux ***
#if defined(Q_OS_UNIX)
	setlocale(LC_NUMERIC,"C");
#endif

	// *** Font size adjustment ***
#if defined(Q_OS_MAC)
//	QFont f = qApp->font();
//	f.setPointSize(10);
//	qApp->setFont(f);
//	qApp->setDesktopSettingsAware(false);
#elif defined(Q_OS_UNIX)
	QFont f = qApp->font();
	f.setPointSize(9);
	qApp->setFont(f);
//	qApp->setWindowIcon(QIcon(":/gfx/MasterSimulator_48x48.png"));
#elif defined(Q_OS_WIN)
	QFont f = qApp->font();
	f.setPointSize(8);
	qApp->setFont(f);
#endif

	// *** Initialize Command Line Argument Parser ***
	IBK::ArgParser argParser;

	// *** Apply command line arguments ***
	argParser.parse(argc, argv);
	// handle default arguments (--help)
	if (argParser.flagEnabled("help")) {
		argParser.printHelp(std::cout);
		return EXIT_SUCCESS;
	}

#if 0
	// *** Create log file directory and setup message handler ***
	QDir baseDir;
	baseDir.mkpath(MSIMDirectories::userDataDir());

	MSIMMessageHandler messageHandler;
	IBK::MessageHandlerRegistry::instance().setMessageHandler( &messageHandler );
	std::string errmsg;
	messageHandler.openLogFile(MSIMDirectories::globalLogFile().toUtf8().data(), false, errmsg);
	messageHandler.setConsoleVerbosityLevel( settings.m_userLogLevelConsole );
	messageHandler.setLogfileVerbosityLevel( settings.m_userLogLevelLogfile );

#endif

#if 0
	// *** Install translator ***
	if (argParser.hasOption("lang")) {
		std::string dummy = argParser.option("lang");
		QString langid = utf82QString(dummy);
		if (langid != MSIMLanguageHandler::instance().langId()) {
			IBK::IBK_Message( IBK::FormatString("Installing translator for language: '%1'.\n")
								.arg(langid.toUtf8().data()),
								IBK::MSG_PROGRESS, FUNC_ID, IBK::VL_STANDARD);
			MSIMLanguageHandler::instance().installTranslator(langid);
		}
	}
	else {
		MSIMLanguageHandler::instance().installTranslator(MSIMLanguageHandler::langId());
	}
#endif

#if 0
	// *** Create and show splash-screen ***
#if QT_VERSION >= 0x050000
	std::unique_ptr<QSplashScreen> splash;
#else
	std::auto_ptr<QSplashScreen> splash;
#endif
	if (!settings.m_flags[MSIMSettings::NoSplashScreen]) {
		QPixmap pixmap;
		pixmap.load(":/gfx/SplashScreen_MasterSim.png","PNG");
		splash.reset(new QSplashScreen(pixmap, Qt::WindowStaysOnTopHint | Qt::SplashScreen));
		splash->show();
		qApp->processEvents();
		QTimer::singleShot(2000, splash.get(), SLOT(close()));
	}

#endif

	// *** Setup and show MainWindow and start event loop ***
	int res;
	try { // open scope to control lifetime of main window, ensure that main window instance dies before settings or project handler

#if 0
		PVToolWidget w;
#else
		MainWindow w;
#endif



		// add user settings related window resize at program start
#if defined(Q_OS_WIN)
		w.show();//showMaximized();
#elif defined(Q_OS_LINUX)
		w.show();
#else
		w.show();
#endif

		// start event loop
		res = a.exec();
	} // here our mainwindow dies, main window goes out of scope and UI goes down -> destructor does ui and thread cleanup
	catch (IBK::Exception & ex) {
		ex.writeMsgStackToError();
		return EXIT_FAILURE;
	}

	// return exit code to environment
	return res;
}
