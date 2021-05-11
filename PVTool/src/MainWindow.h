#ifndef MainWindowH
#define MainWindowH

#include <QMainWindow>
#include "PVTPostProcDialog.h"


class MainWindow : public QMainWindow
{
	Q_OBJECT

public:
	MainWindow();

	PVTPostProcDialog	*ppDialog = new PVTPostProcDialog();

private:
	void createMenus();

	void openAboutDialog();
	void openLicenceDialog();
	void openTutorial();
	void openPostProcSettingsDailog();

	QMenu *m_fileMenu;

	QString m_postProcPath;
};

#endif // MainWindowH
