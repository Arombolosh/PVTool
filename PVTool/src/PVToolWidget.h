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

public slots:

	void on_postProcPathChanged(QString);


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

	void on_pushButtonLoadProfile_clicked();

	void on_pushButtonResult_clicked();

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
							  const std::string & climateDataFileName,
							  const std::string & powerDrainFilePath, const std::string & inclination, const std::string & orientation);

	/*! Generate a Postproc2 session file. */
	void createPostProcSession(const std::string &templateFile, const IBK::Path &currentFolder);

	/*! Takes the next waiting job from m_waitingProjects and starts the process in slave mode.
	*/
	void startNextDELPHINSim();

	/*! Clear all result vectors. */
	void clearResultVecs();

	/*! Evaluate simulation results. */
	void evaluateResults();

	/*! Evaluate simulation results. */
	void evaluateResults(IBK::Path & filename);

	/*! Run PV energy calculation*/
	void runPVEnergy();

	void showResults();

	/*! */
	void startDiscProcess(const QString &cmdDiscPath,const QStringList &discCmdLine, QWidget *parent);

	/*! Import the load profile from file (filename) in m_loadProfile. */
	void readLoadProfile(const IBK::Path& filename);

	/*! Write results to the file. VectorIdx is the index of the vector element of calculation.*/
	void writeResults(const IBK::Path &filename, int vectorIdx);

	/*! Write results pv energy to the file. */
	void writeResultsPV(const IBK::Path &filename);

	/*! reset all. */
	void clearObjects();

	Ui::PVToolWidget	*m_ui;

	QProgressDialog		*m_progressDlg;
	QProcess			*m_cmdLineProcess;

	QTimer				m_simProgressTimer;

	/*! Contains list of all ready and discretized DELPHIN Projects to run (full file paths). */
	std::vector<double>	m_thicknessPCM;			///< thickness of PCM [m]
	QStringList			m_waitingProjects;
	QStringList			m_completedProjects;
	size_t				m_finishedProjects = 0;		///< finished projects to set progressBar

	IBK::Path			m_workingDirectory;
	IBK::Path			m_postProcSession;
	QString			m_postProcPath;

	/*! PV module data. */
	PVTOOL::Energy					m_pvtool;

	/*! Delphin results.*/
	std::vector<IBK::UnitVector>	m_temperature;
	std::vector<IBK::UnitVector>	m_radiation;
	std::vector<IBK::UnitVector>	m_pvEnergy;
	IBK::UnitVector					m_loadProfile;
	std::vector<IBK::UnitVector>	m_ownUseEnergy;
	std::vector<IBK::UnitVector>	m_purchaseEnergy;
	std::vector<IBK::UnitVector>	m_saleEnergy;
	double							m_tempProgress;
	int								m_moduleCount;

	/*! Cost Data*/
	double							m_costPvModule;
	double							m_costCasing;
	double							m_costPCM;
	double							m_increasePriceElectricity;
	double							m_costElectrEnergyPurchase;		///< €/kWh
	double							m_costElectrEnergySale;
	int								m_lccDuration;

	IBK::Path						m_filenameLoadProfile;



};

#endif // PVTOOLWIDGET_H
