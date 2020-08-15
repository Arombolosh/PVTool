#ifndef PVTOOLWIDGET_H
#define PVTOOLWIDGET_H

#include <QWidget>
#include <QProcess>
#include <QTimer>

#include <IBK_Path.h>

#include <PVTool_Energy.h>

namespace Ui {
	class PVToolWidget;
}


class QProgressDialog;

/*! Main widget. */
class PVToolWidget : public QWidget {
	Q_OBJECT

public:
	explicit PVToolWidget(QWidget *parent = nullptr);
	~PVToolWidget() override;

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

	void onSimulationJobFinished(int status, QProcess::ExitStatus);

	/*! Triggered in repeated intervals during the simulation to update the progress bar. */
	void onSimProgressTimerTimeout();

	void onButtonBarQuitClicked();

private:

	/*! Modifies the m6template file content with the given parameters and stores the new file in place of given
		target file name.
		\param m6Template String containing template m6 file.
	*/
	void createM6File(const std::string & m6Template, const IBK::Path &targetFileName, double rho, double ce, double lambda) const;

	/*! Generates a DELPHIN project file. */
	void createDelphinProject(const std::string & d6Template,
							  const IBK::Path & d6ProjectFilePath,
							  double pcmThickness,
							  double insulationThickness,
							  const std::string & pcmMaterialFileName,
							  const std::string & climateDataFileName);

	/*! Takes the next waiting job from m_waitingProjects and starts the process in slave mode.
	*/
	void startNextDELPHINSim();

	/*! Clear all result vectors. */
	void clearResultVecs();

	/*! Evaluate simulation results. */
	void evaluateResults();

	/*! Run PV energy calculation*/
	void runPVEnergy();

	/*! */
	void startDiscProcess(const QString &cmdDiscPath,const QStringList &discCmdLine, QWidget *parent);

	Ui::PVToolWidget	*m_ui;

	QProgressDialog		*m_progressDlg;
	QProcess			*m_cmdLineProcess;

	QTimer				m_simProgressTimer;

	/*! Contains list of all ready and discretized DELPHIN Projects to run (full file paths). */
	std::vector<double>	m_thicknessPCM;			///< thickness of PCM [m]
	QStringList			m_waitingProjects;
	QStringList			m_completedProjects;

	/*! PV module data. */
	PVTOOL::Energy					m_pvtool;

	/*! Delphin results.*/
	std::vector<IBK::UnitVector>	m_temperature;
	std::vector<IBK::UnitVector>	m_radiation;

};

#endif // PVTOOLWIDGET_H
