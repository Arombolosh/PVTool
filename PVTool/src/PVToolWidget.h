#ifndef PVTOOLWIDGET_H
#define PVTOOLWIDGET_H

#include <QWidget>

#include <IBK_Path.h>

#include <PVTool_Energy.h>

namespace Ui {
	class PVToolWidget;
}

class QProcess;

/*! Main widget. */
class PVToolWidget : public QWidget {
	Q_OBJECT

public:
	explicit PVToolWidget(QWidget *parent = nullptr);
	~PVToolWidget();

	std::vector<PVTOOL::Energy::ManufactureData>	m_pvModule;

protected:

	void closeEvent(QCloseEvent *event) override;

private slots:
	void on_radioButton_WeatherComboBox_toggled(bool checked);

	void on_radioButton_PVDatabase_toggled(bool checked);

	void on_pushButton_EPW_clicked();

	void on_comboBox_PVModule_currentIndexChanged(int index);

	void on_pushButton_RunSimu_clicked();

	void on_pushButton_Directory_clicked();

private:

	/*! Modifies the m6template file content with the given parameters and stores the new file in place of given
		target file name.
		\param m6template String containing template m6 file.
	*/
	void createM6File(const std::string & m6Template, const IBK::Path &targetFileName, double rho, double ce, double lambda) const;

	/*! Generates a DELPHIN project file. */
	void createDelphinProject(const std::string & d6Template,
							  const IBK::Path & d6ProjectPath,
							  double insulationThickness,
							  const std::string & pcmMaterialFileName,
							  const std::string & climateDataFileName);

	/*! Starts command in terminal and waits for job to finish. This is meant to be used for fast jobs, i.e. CmdDiscretize.
	*/
	int runInTerminal(const QString & executablePath, const QStringList & commandLineArgs);

	Ui::PVToolWidget *m_ui;

	QProcess			*m_cmdLineProcess;

};

#endif // PVTOOLWIDGET_H
