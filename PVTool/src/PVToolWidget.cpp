#include "PVToolWidget.h"
#include "ui_PVToolWidget.h"

#include <QDebug>
#include <QFileDialog>
#include <QDoubleValidator>
#include <QIntValidator>
#include <QDir>
#include <QMessageBox>
#include <QSettings>
#include <QProcess>
#include <QProgressDialog>

#include <IBK_assert.h>

#include <fstream>

#include "PVTConstants.h"
#include "PVTDirectories.h"

PVToolWidget::PVToolWidget(QWidget *parent) :
	QWidget(parent),
	m_ui(new Ui::PVToolWidget),
	m_progressDlg(nullptr),
	m_cmdLineProcess(nullptr)
{
	m_ui->setupUi(this);

	// read last used working directory from user-settings
	QSettings settings(ORG_NAME, PROGRAM_NAME);
	QString initialWorkingDirectory = settings.value("WorkingDirectory", QDir::homePath()).toString();
	m_ui->lineEdit_Directory->setText(initialWorkingDirectory);

	m_ui->radioButton_WeatherComboBox->setChecked(true);
	on_radioButton_WeatherComboBox_toggled(true);

	//Data Weatherfiles
	//abschalten von updates
	m_ui->comboBox_WeatherFile->blockSignals(true);

	//todo schleife mit weather

	m_ui->comboBox_WeatherFile->addItem("01_Bremerhaven");
	m_ui->comboBox_WeatherFile->addItem("02_Rostock_Warnemuende");
	m_ui->comboBox_WeatherFile->addItem("03_Hamburg-Fuhlsbuettel");
	m_ui->comboBox_WeatherFile->addItem("04_Potsdam");
	m_ui->comboBox_WeatherFile->addItem("04_Potsdam_Sommer");
	m_ui->comboBox_WeatherFile->addItem("04_Potsdam_Winter");
	m_ui->comboBox_WeatherFile->addItem("05_Essen");
	m_ui->comboBox_WeatherFile->addItem("06_Bad_Marienberg_withoutRain");
	m_ui->comboBox_WeatherFile->addItem("07_Kassel");
	m_ui->comboBox_WeatherFile->addItem("08_Braunlage");
	m_ui->comboBox_WeatherFile->addItem("09_Chemnitz");
	m_ui->comboBox_WeatherFile->addItem("10_Hof");
	m_ui->comboBox_WeatherFile->addItem("11_Fichtelberg");
	m_ui->comboBox_WeatherFile->addItem("12_Mannheim");
	m_ui->comboBox_WeatherFile->addItem("13_Muehldorf-Inn");
	m_ui->comboBox_WeatherFile->addItem("14_Stoetten");
	m_ui->comboBox_WeatherFile->addItem("15_Garmisch_withoutRain");
	//einschalten von updates
	m_ui->comboBox_WeatherFile->blockSignals(false);

	m_ui->radioButton_PVDatabase->setChecked(true);
	on_radioButton_PVDatabase_toggled(true);

	//Data PV modules
	m_pvModule.push_back(PVTOOL::Energy::ManufactureData(31.4, 8.44, 38.3, 8.91, 0.05, -0.30, -0.43, 60, "aleo S19L265"));
	m_pvModule.push_back(PVTOOL::Energy::ManufactureData(31.5, 8.57, 38.3, 9.05, 0.05, -0.30, -0.43, 60, "aleo S19L270"));
	m_pvModule.push_back(PVTOOL::Energy::ManufactureData(32.24, 9.27, 39.75, 9.76, 0.043, -0.31, -0.41, 60, "NEMO 2.0 M295"));

	//abschalten von updates
	m_ui->comboBox_PVModule->blockSignals(true);

	//todo schleife mit weather
	for(auto &p : m_pvModule)
		m_ui->comboBox_PVModule->addItem(QString().fromStdString(p.m_name));
	//einschalten von updates
	m_ui->comboBox_PVModule->blockSignals(false);
	on_comboBox_PVModule_currentIndexChanged(0);

	QDoubleValidator *vali = new QDoubleValidator(0,100,3, this);
	vali->setNotation(QDoubleValidator::StandardNotation);
	m_ui->lineEdit_iSC->setValidator(vali);
	m_ui->lineEdit_uOC->setValidator(vali);
	m_ui->lineEdit_iMPP->setValidator(vali);
	m_ui->lineEdit_uMPP->setValidator(vali);
	m_ui->lineEdit_alpha->setValidator(new QDoubleValidator(0,1,8, this));
	m_ui->lineEdit_beta->setValidator(new QDoubleValidator(-1,0,8, this));
	m_ui->lineEdit_gamma->setValidator(new QDoubleValidator(-1,0,8, this));
	m_ui->lineEdit_nSer->setValidator(new QIntValidator(1,120, this));

	//Data PV modules
	//abschalten von updates
	m_ui->comboBox_PCMMaterials->blockSignals(true);

	//todo schleife mit weather
	m_ui->comboBox_PCMMaterials->addItem(tr("SP26"));
	m_ui->comboBox_PCMMaterials->addItem(tr("SP30"));
	//einschalten von updates
	m_ui->comboBox_PCMMaterials->blockSignals(false);
}


PVToolWidget::~PVToolWidget() {
	delete m_ui;
}


// *** protected

void PVToolWidget::closeEvent(QCloseEvent * event) {
	QWidget::closeEvent(event);

	// store currently used working directory in settings
	QSettings settings(ORG_NAME, PROGRAM_NAME);

	settings.setValue("WorkingDirectory", m_ui->lineEdit_Directory->text());
}


// *** slots

void PVToolWidget::on_radioButton_WeatherComboBox_toggled(bool checked) {
	m_ui->comboBox_WeatherFile->setEnabled(checked);
	m_ui->lineEdit_EPWFile->setEnabled(!checked);
	m_ui->pushButton_EPW->setEnabled(!checked);
}


void PVToolWidget::on_radioButton_PVDatabase_toggled(bool checked) {
	m_ui->comboBox_PVModule->setEnabled(checked);
	m_ui->lineEdit_iSC->setEnabled(!checked);
	m_ui->lineEdit_uOC->setEnabled(!checked);
	m_ui->lineEdit_iMPP->setEnabled(!checked);
	m_ui->lineEdit_uMPP->setEnabled(!checked);
	m_ui->lineEdit_alpha->setEnabled(!checked);
	m_ui->lineEdit_beta->setEnabled(!checked);
	m_ui->lineEdit_gamma->setEnabled(!checked);
	m_ui->lineEdit_nSer->setEnabled(!checked);
}


void PVToolWidget::on_pushButton_EPW_clicked() {
	QString fpath = QFileDialog::getOpenFileName(this,tr("Öffne Wetterdatei"),QString(),tr("EnergyPlus Weather Files (*.epw)"));
	if(fpath.isEmpty())
		return;
	m_ui->lineEdit_EPWFile->setText(fpath);
}


void PVToolWidget::on_comboBox_PVModule_currentIndexChanged(int index) {
	IBK_ASSERT(m_pvModule.size() > index);

	m_ui->lineEdit_iSC->setText(QString("%L1").arg(m_pvModule[index].m_isc));
	m_ui->lineEdit_uOC->setText(QString("%L1").arg(m_pvModule[index].m_voc));
	m_ui->lineEdit_beta->setText(QString("%L1").arg(m_pvModule[index].m_beta));
	m_ui->lineEdit_iMPP->setText(QString("%L1").arg(m_pvModule[index].m_imp));
	m_ui->lineEdit_nSer->setText(QString("%L1").arg(m_pvModule[index].m_nSer));
	m_ui->lineEdit_uMPP->setText(QString("%L1").arg(m_pvModule[index].m_vmp));
	m_ui->lineEdit_alpha->setText(QString("%L1").arg(m_pvModule[index].m_alpha));
	m_ui->lineEdit_gamma->setText(QString("%L1").arg(m_pvModule[index].m_gamma));
}


void PVToolWidget::on_pushButton_RunSimu_clicked() {
	// Climate data file
	// we need a project directory/working directory
	IBK::Path weatherDirectory( (PVTDirectories::resourcesRootDir() + "/DB_climate").toStdString());
	IBK::Path weatherPath;
	QString workingDir = m_ui->lineEdit_Directory->text();
	IBK::Path workingDirectory( workingDir.toStdString() );
	if (m_ui->radioButton_WeatherComboBox->isChecked()){
		weatherPath = weatherDirectory / m_ui->comboBox_WeatherFile->itemText(m_ui->comboBox_WeatherFile->currentIndex()).toStdString() + ".c6b";
		// Note: we can rely on the file to exist, since this is built-in stuff
	}
	else {
		// get path to user-defined climate data file
		weatherPath = IBK::Path(m_ui->lineEdit_EPWFile->text().toStdString());
		// check if file exists
		if (!weatherPath.exists()) {
			QMessageBox::critical(this, QString(), tr("Die ausgewählte Klimadatei '%1' existiert nicht.").arg(m_ui->lineEdit_EPWFile->text()));
			return;
		}
	}
	IBK::Path weatherName =  weatherPath.filename();
	IBK::Path weatherTargetDir = workingDirectory / "climate";
	if( !weatherTargetDir.exists())
		IBK::Path::makePath(weatherTargetDir);
	if( !weatherTargetDir.exists()){
		QMessageBox::critical(this, QString(), tr("Kann Zielklimaverzeichnis '%1' nicht erstellen.").arg(QString::fromStdString(weatherTargetDir.str())));
		return;
	}
	bool success = IBK::Path::copy(weatherPath, weatherTargetDir / weatherName);
	if (!success) {
		QMessageBox::critical(this, QString(), tr("Konnte Projektverzeichnis nicht ins Arbeitsverzeichnis '%1' kopieren.").arg(workingDir));
		return;
	}
	// TODO : check all the other input for meaningful values

	// populate manufacturing data with input values
	PVTOOL::Energy pvtool;
	if (m_ui->radioButton_PVDatabase->isChecked()){
		pvtool.m_manuData = m_pvModule[m_ui->comboBox_PVModule->currentIndex()];
	}
	else {
		// TODO : instead of toDouble() use locale-aware
		pvtool.m_manuData.m_isc = m_ui->lineEdit_iSC->text().toDouble();
		pvtool.m_manuData.m_imp = m_ui->lineEdit_iMPP->text().toDouble();
		pvtool.m_manuData.m_vmp = m_ui->lineEdit_uMPP->text().toDouble();
		pvtool.m_manuData.m_voc	 = m_ui->lineEdit_uOC->text().toDouble();
		pvtool.m_manuData.m_alpha = m_ui->lineEdit_alpha->text().toDouble();
		pvtool.m_manuData.m_beta = m_ui->lineEdit_beta->text().toDouble();
		pvtool.m_manuData.m_gamma = m_ui->lineEdit_gamma->text().toDouble();
		pvtool.m_manuData.m_nSer = m_ui->lineEdit_nSer->text().toInt();
		pvtool.m_manuData.m_name = "UserInput";
	}

	// now pre-calculate manufacturing parameter set
	try {
		pvtool.calcPhysicalParameterFromManufactureData();
	}
	catch (IBK::Exception & ex) {
		QMessageBox::critical(this, QString(), tr("Bei der Berechnung der PV-Panel-Kenngrößen aus den Eingabedaten ist ein Fehler aufgetreten."));
		return;
	}

	// we now have the input data to start our variation/optimization run
	// but we first need the paths to the external tools 'CmdDiscretize' and 'DelphinSolver.exe'

#ifdef Q_OS_LINUX
	const QString CMDDISCPATH = PVTDirectories::resourcesRootDir() + "/binaries/linux64/CmdDiscretise";
	const QString DELPHINPATH = PVTDirectories::resourcesRootDir() + "/binaries/linux64/DelphinSolver";
#endif

#ifdef Q_OS_WIN
	const QString CMDDISCPATH = PVTDirectories::resourcesRootDir() + "/binaries/win64/CmdDiscretise.exe";
	const QString DELPHINPATH = PVTDirectories::resourcesRootDir() + "/binaries/win64/DelphinSolver.exe";
#endif

#ifdef Q_OS_MAC
	const QString CMDDISCPATH = PVTDirectories::resourcesRootDir() + "/binaries/darwin64/CmdDiscretise";
	const QString DELPHINPATH = PVTDirectories::resourcesRootDir() + "/binaries/darwin64/DelphinSolver";
#endif

	if (!QFileInfo(CMDDISCPATH).exists()) {
		QMessageBox::critical(this, QString(), tr("Kann CmdDiscretise-Tool nicht in '%1' finden.").arg(CMDDISCPATH));
		return;
	}

	// TODO : Fehlermeldung DELPHIN

	// check, if the directory exists
	if (!QFileInfo(workingDir).exists()) {
		int res = QMessageBox::question(this, QString(), tr("Arbeitsverzeichnis existiert nicht, soll es erstellt werden?"),
							  QMessageBox::Ok | QMessageBox::Cancel);
		if (res == QMessageBox::Cancel)
			return;
		if (!QDir().mkpath(workingDir)) {
			QMessageBox::critical(this, QString(), tr("Konnte das Arbeitsverzeichnis nicht erstellen. Möglicherweise fehlen die Zugriffsrechte?"));
			return;
		}
	}

	// read the template files into memory
	// - project template
	// - insulation template

	IBK::Path d6pTemplatePath( (PVTDirectories::resourcesRootDir() + "/file_templates/template.d6p").toStdString());
	IBK::Path m6TemplatePath( (PVTDirectories::resourcesRootDir() + "/file_templates/InsulationMatTemplate.m6").toStdString());

	std::string d6Template, m6Template;
	{
		std::ifstream in(d6pTemplatePath.str());
		std::stringstream strm;
		strm << in.rdbuf();
		d6Template = strm.str();
	}
	{
		std::ifstream in(m6TemplatePath.str());
		std::stringstream strm;
		strm << in.rdbuf();
		m6Template = strm.str();
	}

	// copy entire working directory

	IBK::Path templateDirectory( (PVTDirectories::resourcesRootDir() + "/project_template").toStdString());

	success = IBK::Path::copy(templateDirectory, workingDirectory);
	if (!success) {
		QMessageBox::critical(this, QString(), tr("Konnte Projektverzeichnis nicht ins Arbeitsverzeichnis '%1' kopieren.").arg(workingDir));
		return;
	}

	// adjust template insulation material and write to target directory
	IBK::Path insulationM6Path( workingDirectory / "materials/InsulationMat.m6");

	createM6File(m6Template, insulationM6Path, m_ui->doubleSpinBox_Density->value(),
			   m_ui->doubleSpinBox_SpecHeatCapa->value(),
			   m_ui->doubleSpinBox_Conductivity->value());

	// *** start variation loop
	std::vector<double> pcmThick{0.01, 0.02, 0.03};	//thickness of pcm
	for (size_t i=0; i<pcmThick.size(); ++i) {
		IBK::Path d6ProjectPath(IBK::FormatString( "%1/project%2.d6p").arg(workingDirectory).arg(i).str());
		pcmThick[i] -= 0.005; // pcm has 3 cells, two non adjustable cells are set to 5 mm
	// - adjust PCM material layer thickness and write project template
		double insuThick = m_ui->doubleSpinBox_InsulationThickness->value()/100;
		createDelphinProject(d6Template, d6ProjectPath, pcmThick[i], insuThick,
							 m_ui->comboBox_PCMMaterials->currentText().toStdString(), weatherName.str());
		// now run CmdDiscretize to generate discretized project file
		QStringList discCmdLine;
		QDir::setCurrent(workingDir);
	//	QString projectFile = QString::fromStdString(d6ProjectPath.filename().str());
	//	QString projectDiscFile = QString::fromStdString(d6ProjectPath.filename().withoutExtension().str() + "-disc.d6p");
	//	discCmdLine << projectFile << "-o="+projectDiscFile;
		discCmdLine << QString::fromStdString("project" + IBK::val2string(i) + ".d6p") <<
					   QString::fromStdString("-o=project" + IBK::val2string(i) + "-disc.d6p") << "-q";
		QProcess p;
		int res = p.execute(CMDDISCPATH, discCmdLine);
		if (res != 0) {
			QMessageBox::critical(this, QString(), tr("Fehler bei der Ausführung des CmdDiscretize-Tools."));
			return;
		}
		m_waitingProjects.append(QString::fromStdString(d6ProjectPath.str()));
	}

	if (m_progressDlg == nullptr) {
		m_progressDlg = new QProgressDialog(tr("Simuliere Geometrievarianten..."), tr("Abbrechen"), 0, m_waitingProjects.count(), this);
	}
	else {
		m_progressDlg->setMaximum(m_waitingProjects.count());
	}
	m_progressDlg->setValue(0);
	m_progressDlg->setWindowModality(Qt::WindowModal);
	m_progressDlg->show();
	startNextDELPHINSim();
}


void PVToolWidget::on_pushButton_Directory_clicked() {
	//vorinitalisierung von dem
	QString fpath = QFileDialog::getExistingDirectory(this, tr("Setze Ausführungsordner"), QString());
	if(fpath.isEmpty())
		return;
	m_ui->lineEdit_Directory->setText(fpath);
}


void PVToolWidget::onSimulationJobFinished(int exitCode, QProcess::ExitStatus status) {
	if (status == QProcess::Crashed || exitCode != 0) {
		IBK::Path logFile(IBK::Path(m_completedProjects.back().toStdString()).withoutExtension() / "log/screenlog.txt");
		QMessageBox::critical(this, QString(), tr("Fehler bei der Ausführung der DELPHIN-Simulation, siehe Logdatei '%1'")
							  .arg(QString::fromStdString(logFile.str())));
		m_progressDlg->hide();
		return;
	}
//	m_progressDlg->setValue(m_finis)

}



void PVToolWidget::onSimulationJobAborted() {
	startNextDELPHINSim();
}



// *** private functions

void PVToolWidget::createM6File(const std::string & m6Template, const IBK::Path &targetFileName, double rho, double ce, double lambda) const {
	FUNCID(PVToolWidget::createM6File);
	std::string m6str = IBK::replace_string(m6Template, "${RHO}", IBK::val2string(rho));
	m6str = IBK::replace_string(m6str, "${CE}", IBK::val2string(ce));
	m6str = IBK::replace_string(m6str, "${LAMBDA}", IBK::val2string(lambda));

	// write file
	std::ofstream out(targetFileName.str());
	out << m6str << std::endl;
	if (!out)
		throw IBK::Exception("Error writing file.", FUNC_ID);
}


void PVToolWidget::createDelphinProject(const std::string & d6Template,
						  const IBK::Path & d6ProjectFilePath,
						  double pcmThickness,
						  double insulationThickness,
						  const std::string & pcmMaterialFileName,
						  const std::string & climateDataFileName)
{
	std::string d6str = IBK::replace_string(d6Template, "${PCMThick}", IBK::val2string(pcmThickness));
	d6str = IBK::replace_string(d6str, "${INSULATIONThick}", IBK::val2string(insulationThickness));
	d6str = IBK::replace_string(d6str, "${PCM}",pcmMaterialFileName);
	d6str = IBK::replace_string(d6str, "${CLIMATE}", climateDataFileName);

	// write file
	std::ofstream out(d6ProjectFilePath.str());
	out << d6str << std::endl;

}


void PVToolWidget::startNextDELPHINSim() {
	if (m_waitingProjects.isEmpty() || m_progressDlg->wasCanceled()) {
		m_progressDlg->hide();
		evaluateResults();
		return;
	}

	QString nextJobFile = m_waitingProjects.front();
	m_waitingProjects.pop_front();
	m_completedProjects.push_back(nextJobFile);

	// configure process
	if (m_cmdLineProcess == nullptr) {
		m_cmdLineProcess = new QProcess(this);
		connect(m_cmdLineProcess, SIGNAL(finished(int, QProcess::ExitStatus)), this, SLOT(onSimulationJobFinished(int, QProcess::ExitStatus)));
	}

#ifdef Q_OS_LINUX
	const QString DELPHINPATH = PVTDirectories::resourcesRootDir() + "/binaries/linux64/DelphinSolver";
#endif
#ifdef Q_OS_WIN
	const QString DELPHINPATH = PVTDirectories::resourcesRootDir() + "/binaries/win64/DelphinSolver.exe";
#endif
#ifdef Q_OS_MAC
	const QString DELPHINPATH = PVTDirectories::resourcesRootDir() + "/binaries/darwin64/DelphinSolver";
#endif
	// existence of DELPHIN solver file was already tested

	QStringList cmdLine;
	cmdLine << nextJobFile;
#ifdef Q_OS_WIN
	cmdLine << "-x";
#endif
	m_cmdLineProcess->start(DELPHINPATH, cmdLine);
}


void PVToolWidget::evaluateResults() {
	// process ready results
	//m_completedProjects.
}

