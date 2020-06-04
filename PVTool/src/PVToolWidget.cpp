#include "PVToolWidget.h"
#include "ui_PVToolWidget.h"

#include <QFileDialog>
#include <QDoubleValidator>
#include <QIntValidator>
#include <QDir>
#include <QMessageBox>
#include <QSettings>
#include <QProcess>

#include <IBK_assert.h>

#include <fstream>

#include "PVTConstants.h"
#include "PVTDirectories.h"

PVToolWidget::PVToolWidget(QWidget *parent) :
	QWidget(parent),
	m_ui(new Ui::PVToolWidget),
	m_cmdLineProcess(new QProcess(this))
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
	std::string weatherName;
	if (m_ui->radioButton_WeatherComboBox->isChecked()){
		weatherName = m_ui->comboBox_WeatherFile->itemText(m_ui->comboBox_WeatherFile->currentIndex()).toStdString() + ".c6b";
		// Note: we can rely on the file to exist, since this is built-in stuff
	}
	else {
		// get path to user-defined climate data file
		IBK::Path weatherPath(m_ui->lineEdit_EPWFile->text().toStdString());
		// check if file exists
		if (!weatherPath.exists()) {
			QMessageBox::critical(this, QString(), tr("Die ausgewählte Klimadatei '%1' existiert nicht.").arg(m_ui->lineEdit_EPWFile->text()));
			return;
		}
		weatherName = weatherPath.filename().str();
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

	// for now hard-coded, same directory as executable PVTool.exe
	const QString CMDDISCPATH = qApp->applicationDirPath() + "/CmdDiscretize";
	const QString DELPHINPATH = qApp->applicationDirPath() + "/DelphinSolver";

	// we need a project directory/working directory
	QString workingDir = m_ui->lineEdit_Directory->text();
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
	IBK::Path workingDirectory( workingDir.toStdString() );
	bool success = IBK::Path::copy(templateDirectory, workingDirectory);
	if (!success) {
		QMessageBox::critical(this, QString(), tr("Konnte Projektverzeichnis nicht ins Arbeitsverzeichnis '%1' kopieren.").arg(workingDir));
		return;
	}

	// adjust template insulation material and write to target directory
	IBK::Path insulationM6Path( workingDirectory / "materials/InsulationMat.m6");

	createM6File(m6Template, insulationM6Path, m_ui->doubleSpinBox_Density->value(),
			   m_ui->doubleSpinBox_SpecHeatCapa->value(),
			   m_ui->doubleSpinBox_Conductivity->value());

	IBK::Path d6ProjectPath( workingDirectory / "project.d6p");

	// *** start variation loop
	//
	// in each loop we:
	// - adjust PCM material layer thickness and write project template

	double pcmThick = 0.03 - 0.005; // in m -> muss mit den variationen angepasst werden
	double insuThick = m_ui->doubleSpinBox_InsulationThickness->value()/100;
    createDelphinProject(d6Template, d6ProjectPath, pcmThick, insuThick, m_ui->comboBox_PCMMaterials->currentText().toStdString(), weatherName);


	// now run CmdDiscretize to generate discretized project file
	QStringList discCmdLine;
	discCmdLine << QString::fromStdString(d6ProjectPath.str()) <<

#if 0
	IBK::Path filenameD6p(""), filenameM6("");
	//pcm thickness total - 2 cells of pcm
	//total cells for pcm is 3
	//variance 0.01 to 0.03



	//PCM-> Material (PCM) kopieren aus Vorgabedateien
	//Wärmedämmung (Insulation) erstellen aus Vorgaben

	MM::Material insulation;
	insulation.m_identification.m_name.setString("Insulation", "de");
	insulation.m_identification.m_productId.setString("PVTool", "de");
	insulation.m_identification.m_producer.setString("PVTool", "de");
	insulation.m_identification.m_flags =
			MM::MaterialIdentification::AIR_TIGHT &
			MM::MaterialIdentification::WATER_TIGHT &
			MM::MaterialIdentification::VAPOR_TIGHT;

	insulation.m_identification.m_cat = MM::MaterialCategory::Insulations;
	insulation.m_id = 1001002;
	insulation.m_paraTransport[MM::Material::MP_LAMBDA] = IBK::Parameter("LAMBDA", m_ui->doubleSpinBox_Conductivity->text().toDouble(), IBK::Unit("W/mK"));
	insulation.m_paraStorage[MM::Material::MP_RHO] = IBK::Parameter("RHO", m_ui->doubleSpinBox_Density->text().toDouble(), IBK::Unit("kg/m3"));
	insulation.m_paraStorage[MM::Material::MP_CE] = IBK::Parameter("CE", m_ui->doubleSpinBox_SpecHeatCapa->text().toDouble(), IBK::Unit("J/kgK"));

	//IBK::Path insuFilename (path / ("Insulation_" + IBK::val2string(insulation.m_id) + ".m6"));

#endif
	//template datei bearbeiten und an richtige stelle kopieren alle weiteren dateien kopieren/erstellen
	//string ersetzen d6p datei ersetzen (klima insulation pcm dicken)

	// discre. aufrufen

	//d6 solver mit der neuen d6p datei starten

	//pvtool anwerfen mit pfadübergabe von delphin ausgaben und aus gui exportierten pv daten

	//ergebnisse sammeln
}


void PVToolWidget::on_pushButton_Directory_clicked() {
	//vorinitalisierung von dem
	QString fpath = QFileDialog::getExistingDirectory(this, tr("Setze Ausführungsordner"), QString());
	if(fpath.isEmpty())
		return;
	m_ui->lineEdit_Directory->setText(fpath);
}



// *** private functions

void PVToolWidget::createM6File(const std::string & m6Template, const IBK::Path &targetFileName, double rho, double ce, double lambda) const {
	std::string m6str = IBK::replace_string(m6Template, "${RHO}", IBK::val2string(rho));
	m6str = IBK::replace_string(m6str, "${CE}", IBK::val2string(ce));
	m6str = IBK::replace_string(m6str, "${LAMBDA}", IBK::val2string(lambda));

	// write file
	std::ofstream out(targetFileName.str());
	out << m6str << std::endl;
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

//int PVToolWidget::runInTerminal(const QString & executablePath, const QStringList & commandLineArgs) {

//	std::unique_ptr<QProcess> myProcess (new QProcess(this));
//	return myProcess->execute(executablePath, commandLineArgs);
//	if (res != 0)
//		throw IBK::Exception("Error running job.")
//}



#if 0
void readD6pFile(const IBK::Path &filename, double pcmThick, double insuThick,
				 const std::string &namePCM,const std::string &nameInsu,const std::string &nameClimate){

	std::ifstream in;
	in.open(filename.c_str());
	std::string line;
	unsigned int counter=0;
	while (std::getline(in, line)) {

		std::string nameToReplace = "${CLIMATE}";
		size_t len = nameToReplace.length();
		size_t pos = line.find(nameToReplace);
		if(pos != std::string::npos){	//name with extension
			line.replace(pos, len, nameClimate);
			++counter;
		}
		nameToReplace = "${PCM}";
		len = nameToReplace.length();
		pos = line.find(nameToReplace);
		if(pos != std::string::npos){	//name without extension
			line.replace(pos, len, namePCM + ".m6");
			++counter;
		}
		nameToReplace = "${INSULATION}";
		len = nameToReplace.length();
		pos = line.find(nameToReplace);
		if(pos != std::string::npos){	//name without extension
			line.replace(pos, len, nameInsu + ".m6");
			++counter;
		}
		nameToReplace = "${PCMThick}";
		len = nameToReplace.length();
		pos = line.find(nameToReplace);
		if(pos != std::string::npos){
			line.replace(pos, len, IBK::val2string(pcmThick));
			++counter;
		}
		nameToReplace = "${INSULATIONThick}";
		len = nameToReplace.length();
		pos = line.find(nameToReplace);
		if(pos != std::string::npos){
			line.replace(pos, len, IBK::val2string(insuThick));
			++counter;
		}

		if(counter==5)
			break;
	}
	//schreiben der datei
}
#endif


