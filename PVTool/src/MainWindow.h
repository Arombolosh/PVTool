#ifndef MainWindowH
#define MainWindowH

#include <QMainWindow>


class MainWindow : public QMainWindow
{
	Q_OBJECT

public:
	MainWindow();

private:
	void createMenus();

	void openAboutDialog();
	void openLicenceDialog();
	void openTutorial();
	void openPostProcSettingsDailog();

	QMenu *m_fileMenu;
};

#endif // MainWindowH
