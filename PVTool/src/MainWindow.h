#ifndef MainWindowH
#define MainWindowH

#include <QMainWindow>
#include "PVTPostProcDialog.h"


class MainWindow : public QMainWindow
{
	Q_OBJECT

public:
	MainWindow();
	~MainWindow() override;

	PVTPostProcDialog	*ppDialog;// = new PVTPostProcDialog();

signals:
	void postProcPathChanged(QString );

private:

	void readPostProcPath();

	void writePostProcPath();

	void createMenus();

	void openAboutDialog();
	void openLicenceDialog();
	void openTutorial();
	void openPostProcSettingsDailog();

	QMenu *m_fileMenu;

	QString m_postProcPath;
};

#endif // MainWindowH
