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
#include <QLocale>
#include <QToolButton>


#include <IBK_assert.h>
#include <IBK_FileReader.h>
#include <IBK_math.h>

#include <fstream>

#include <DataIO>

#include "PVTConstants.h"
#include "PVTDirectories.h"
#include "PVTResultDialog.h"

bool convertDoubleFromText(const QString & text, double & value) {
	// try converting using the current locale
	bool ok;
	double val = QLocale().toDouble(text, &ok);
	// if this didn't work, fall back to c-locale
	if (!ok) {
		val = text.toDouble(&ok);
		// still no success?
		if (!ok) {
			return false;
		}
	}
	value = val; // only store value if correctly parsed
	return true;
}


bool convertIntFromText(const QString & text, int & value) {
	// try converting using the current locale
	bool ok;
	int val = QLocale().toInt(text, &ok);
	// if this didn't work, fall back to c-locale
	if (!ok) {
		val = text.toInt(&ok);
		// still no success?
		if (!ok) {
			return false;
		}
	}
	value = val; // only store value if correctly parsed
	return true;
}


/*! takes Qwidget, Qlabel and QlineEdit object and tries to convert the read value from lineEdit into Double
	throws QMessageBox with caption from label and lineEdit value if conversion fails and
	sets color of text in lineEdit Object to red and the focus to it.
*/
bool convertLineEditintoDouble(QWidget * const widget, QLabel * const label, QLineEdit * const lineEdit, double &value) {
	QPalette palette;
	if (!convertDoubleFromText(lineEdit->text(),value)){
		lineEdit->setFocus(); // set focus to lineEdit Object
		palette.setColor(QPalette::Text,Qt::red);
		lineEdit->setPalette(palette); // set text color to red in lineEdit field
		QMessageBox::critical(widget, QString(), QMessageBox::tr("Ungültige Eingabe '%1' im Feld '%2'\nGleitkommazahl wird benötigt.")
								.arg(lineEdit->text())
								.arg(label->text())); // throws QMessageBox with Name of LineEdit Obj and Value
		return false;
	}
	else {
		palette.setColor(QPalette::Text,Qt::black);
		lineEdit->setPalette(palette); // set text color to black if value is valid
		return true;
	}
}

/*! takes Qwidget, Qlabel and QlineEdit object and tries to convert the read value from lineEdit into Integer
	throws QMessageBox with caption from label and lineEdit value if conversion fails and
	sets color of text in lineEdit Object to red and the focus to it.
*/
bool convertLineEditintoInt(QWidget * const widget, QLabel * const label, QLineEdit * const lineEdit, int &value) {
	QPalette palette;
	if (!convertIntFromText(lineEdit->text(),value)){
		lineEdit->setFocus(); // set focus to lineEdit Object
		palette.setColor(QPalette::Text,Qt::red);
		lineEdit->setPalette(palette); // set text color to red in lineEdit field
		QMessageBox::critical(widget, QString(), QMessageBox::tr("Ungültige Eingabe '%1' im Feld '%2'\nGleitkommazahl wird benötigt.")
								.arg(lineEdit->text())
								.arg(label->text())); // throws QMessageBox with Name of LineEdit Obj and Value
		return false;
	}
	else {
		palette.setColor(QPalette::Text,Qt::black);
		lineEdit->setPalette(palette); // set text color to black if value is valid
		return true;
	}
}


PVToolWidget::PVToolWidget(QWidget *parent) :
	QWidget(parent),
	m_ui(new Ui::PVToolWidget),
	m_progressDlg(nullptr),
	m_cmdLineProcess(nullptr)
{
	m_ui->setupUi(this);
	setWindowTitle(PROGRAM_NAME);

	// read last used working directory from user-settings
	QSettings settings(ORG_NAME, PROGRAM_NAME);
	QString initialWorkingDirectory = settings.value("WorkingDirectory", QDir::homePath()).toString();
	m_ui->lineEdit_Directory->setText(initialWorkingDirectory);

	m_ui->lineEdit_Inclination->setText(QString("90"));
	m_ui->lineEdit_Orientation->setText(QString("180"));

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
	//m_ui->comboBox_WeatherFile->addItem("04_Potsdam_Sommer");
	//m_ui->comboBox_WeatherFile->addItem("04_Potsdam_Winter");
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

	//Data PV modules
	m_pvModule.push_back(PVTOOL::Energy::ManufactureData(31.4, 8.44, 38.3, 8.91, 0.05, -0.30, -0.43, 60, "aleo S19L265", PVTOOL::Energy::ManufactureData::CellType::monoSi));				//Material monoSi
	m_pvModule.push_back(PVTOOL::Energy::ManufactureData(31.5, 8.57, 38.3, 9.05, 0.05, -0.30, -0.43, 60, "aleo S19L270", PVTOOL::Energy::ManufactureData::CellType::monoSi));				//Material monoSi
	m_pvModule.push_back(PVTOOL::Energy::ManufactureData(32.24, 9.27, 39.75, 9.76, 0.043, -0.31, -0.41, 60, "NEMO 2.0 M295", PVTOOL::Energy::ManufactureData::CellType::monoSi));			//Material monoSi
	m_pvModule.push_back(PVTOOL::Energy::ManufactureData(44.1, 1.84, 59.8, 2.16, 0.02, -0.24, -0.25, 156, "Calyxo CX 3 80", PVTOOL::Energy::ManufactureData::CellType::CdTe));			//Material CdTe
	m_pvModule.push_back(PVTOOL::Energy::ManufactureData(42.0, 1.81, 59.6, 2.15, 0.02, -0.24, -0.25, 156, "Calyxo CX 3 75", PVTOOL::Energy::ManufactureData::CellType::CdTe));			//Material CdTe
	m_pvModule.push_back(PVTOOL::Energy::ManufactureData(70.2, 1.6, 87.7, 1.75, 0.04, -0.29, -0.34, 213, "Firstsolar Dünnschicht", PVTOOL::Energy::ManufactureData::CellType::CdTe));	//Material CdTe
	m_pvModule.push_back(PVTOOL::Energy::ManufactureData(15.57, 8.25, 18.17, 8.71, 0.06, -0.30, -0.39, 28, "Solarglas Premium", PVTOOL::Energy::ManufactureData::CellType::monoSi));		//Material monoSi
	m_pvModule.push_back(PVTOOL::Energy::ManufactureData(19.4, 9.09, 23.8, 9.32, 0.05, -0.31, -0.39, 36, "Solarwatt", PVTOOL::Energy::ManufactureData::CellType::monoSi));	//Material monoSi
	m_pvModule.push_back(PVTOOL::Energy::ManufactureData(61.9, 2.34, 80.1, 2.59, 0.0, -0.287, -0.39, 136, "Avancis Powermax 145", PVTOOL::Energy::ManufactureData::CellType::CIS));		//Material CIS
	m_pvModule.push_back(PVTOOL::Energy::ManufactureData(30.4, 3.29, 40.9, 3.93, 0.08, -0.33, -0.2, 72, "Schott Protect ASI100", PVTOOL::Energy::ManufactureData::CellType::Amorphous));						//Material Amorphous
	m_pvModule.push_back(PVTOOL::Energy::ManufactureData(23.38,7.92, 29.23, 8.45, 0.05, -0.33, -0.45, 48, "Schott Protect 185", PVTOOL::Energy::ManufactureData::CellType::multiSi));						//Material Amorphous

	//Attention follow sort order in enum CellType { monoSi, multiSi, CdTe, CIS, CIGS, Amorphous };
	m_ui->comboBoxCellType->addItem("MonoSi", PVTOOL::Energy::ManufactureData::CellType::monoSi);
	m_ui->comboBoxCellType->addItem("multiSi", PVTOOL::Energy::ManufactureData::CellType::multiSi);
	m_ui->comboBoxCellType->addItem("CdTe", PVTOOL::Energy::ManufactureData::CellType::CdTe);
	m_ui->comboBoxCellType->addItem("CIS", PVTOOL::Energy::ManufactureData::CellType::CIS);
	m_ui->comboBoxCellType->addItem("CIGS", PVTOOL::Energy::ManufactureData::CellType::CIGS);
	m_ui->comboBoxCellType->addItem("Amorphous", PVTOOL::Energy::ManufactureData::CellType::Amorphous);

	for(auto &p : m_pvModule)
		m_ui->comboBox_PVModule->addItem(QString().fromStdString(p.m_name));

	m_ui->radioButton_PVDatabase->setChecked(true);
	on_radioButton_PVDatabase_toggled(true);

	//abschalten von updates
	m_ui->comboBox_PVModule->blockSignals(true);

	//einschalten von updates
	m_ui->comboBox_PVModule->blockSignals(false);
	on_comboBox_PVModule_currentIndexChanged(0);
#if 0
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
#endif

	//Data PV modules
	//abschalten von updates
	m_ui->comboBox_PCMMaterials->blockSignals(true);

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

	// TODO : ask user to save his/her input data to project file?

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

	QList<QLineEdit *> allLineEdits = m_ui->groupBox_PVModule->findChildren<QLineEdit *>();
	for (int i=0; i < allLineEdits.size(); i++)
	{
		allLineEdits.at(i)->setEnabled(!checked);
	}
	m_ui->comboBoxCellType->setEnabled(!checked);

	m_ui->comboBox_PVModule->setEnabled(checked);
	if(checked) {
		m_ui->comboBoxCellType->setCurrentIndex((m_pvModule[m_ui->comboBox_PVModule->currentIndex()].m_material));
		QPalette palette;
		palette.setColor(QPalette::Text, Qt::black);
		for (int i=0; i < allLineEdits.size(); i++)
		{
			allLineEdits.at(i)->setPalette(palette);
		}

		unsigned int index = m_ui->comboBox_PVModule->currentIndex();

		m_ui->lineEdit_iSC->setText(QString("%L1").arg(m_pvModule[index].m_isc));
		m_ui->lineEdit_uOC->setText(QString("%L1").arg(m_pvModule[index].m_voc));
		m_ui->lineEdit_beta->setText(QString("%L1").arg(m_pvModule[index].m_beta));
		m_ui->lineEdit_iMPP->setText(QString("%L1").arg(m_pvModule[index].m_imp));
		m_ui->lineEdit_nSer->setText(QString("%L1").arg(m_pvModule[index].m_nSer));
		m_ui->lineEdit_uMPP->setText(QString("%L1").arg(m_pvModule[index].m_vmp));
		m_ui->lineEdit_alpha->setText(QString("%L1").arg(m_pvModule[index].m_alpha));
		m_ui->lineEdit_gamma->setText(QString("%L1").arg(m_pvModule[index].m_gamma));
	}

	m_ui->lineEdit_Inclination->setEnabled(true);
	m_ui->lineEdit_Orientation->setEnabled(true);
	m_ui->lineEdit_numberOfModules->setEnabled(true);
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

	auto xxx = m_pvModule[index].m_material;
	auto xxx2 = static_cast<PVTOOL::Energy::ManufactureData::CellType>(xxx);
	m_ui->comboBoxCellType->setCurrentIndex((m_pvModule[index].m_material));
}

void PVToolWidget::startDiscProcess(const QString &cmdDiscPath,const QStringList &discCmdLine, QWidget *parent){
	QProcess p;
	int res = p.execute(cmdDiscPath, discCmdLine);
	if (res != 0) {
		QMessageBox::critical(parent, QString(), tr("Fehler bei der Ausführung des CmdDiscretize-Tools."));
		return;
	}
}

void PVToolWidget::readLoadProfile(const IBK::Path &filename) {
	FUNCID(PVToolWidget::readLoadProfile);
	IBK::FileReader fileReader(filename, 40960);
	std::vector<std::string> lines;

	fileReader.readAll(filename, lines, std::vector<std::string>{"\n"});

	if(lines.empty())
		return;

	//check header
	std::vector<std::string> elements;

	IBK::explode(lines[0], elements, "\t", IBK::ExplodeFlags::EF_TrimTokens);
	size_t pos1, pos2;
	if(elements.size()<2)
		throw IBK::Exception(IBK::FormatString("Load profile is not valid! Line: #%1").arg(0), FUNC_ID);
	pos1 = elements[1].find("[");
	pos2 = elements[1].find("]");
	std::string unit;
	if (pos1 != std::string::npos && pos2 != std::string::npos && pos1 < pos2)
		unit = elements[1].substr(pos1+1, pos2-pos1-1);

	m_loadProfile.m_unit = IBK::Unit(unit);
	m_loadProfile.m_name = "LoadProfile";

	//check unit
	IBK::Unit u1(unit);
	if(u1.base_id() != IBK::Unit("W").base_id())
		throw IBK::Exception(IBK::FormatString("Load profile has no valid unit! Line: #%1").arg(0), FUNC_ID);

	//Prüfe zeiteinheit TODO Mira
	if(elements[0].find("[h]") == std::string::npos)
		throw  IBK::Exception(IBK::FormatString("Load profile requires hourly time stamp."), FUNC_ID);

	//skip header line
	for(unsigned int i=1; i<lines.size(); ++i){
		//skip emtpy lines
		if(lines.empty())
			continue;

		IBK::explode(lines[i], elements, "\t", IBK::ExplodeFlags::EF_TrimTokens);
		if(elements.size()<2)
			throw IBK::Exception(IBK::FormatString("Load profile is not valid! Line: #%1").arg(i), FUNC_ID);
		double val;
		try {
			val = IBK::string2val<double>(elements[1]);
		}  catch (...) {
			throw IBK::Exception(IBK::FormatString("Load profile is not valid! No correct data value at line: #%1").arg(i), FUNC_ID);
		}
		m_loadProfile.m_data.push_back(val);
	}

	if(m_loadProfile.m_data.size() != 8761)
		throw IBK::Exception(IBK::FormatString("Load profile is not valid! There are not exactly 8760 value lines."), FUNC_ID);

}

void PVToolWidget::on_pushButton_RunSimu_clicked() {


	// Climate data file
	// we need a project directory/working directory
	IBK::Path weatherDirectory( (PVTDirectories::resourcesRootDir() + "/DB_Climate").toStdString());
	IBK::Path weatherPath;
	QString workingDir = m_ui->lineEdit_Directory->text();
	m_workingDirectory = IBK::Path( workingDir.toStdString() );

	std::vector<IBK::Path> deleteDirs;


	deleteDirs.push_back( m_workingDirectory + "/project1-0-disc" );
	deleteDirs.push_back( m_workingDirectory + "/project1-1-disc" );
	deleteDirs.push_back( m_workingDirectory + "/project1-2-disc" );
	deleteDirs.push_back( m_workingDirectory + "/project2-0-disc" );
	deleteDirs.push_back( m_workingDirectory + "/project2-1-disc" );
	deleteDirs.push_back( m_workingDirectory + "/project2-2-disc" );
	deleteDirs.push_back( m_workingDirectory + "/projectWithoutPCM-0-disc" );
	deleteDirs.push_back( m_workingDirectory + "/projectWithoutPCM-1-disc" );
	deleteDirs.push_back( m_workingDirectory + "/projectWithoutPCM-2-disc" );

	for (size_t i=0; i<deleteDirs.size(); ++i)
		m_workingDirectory.remove(deleteDirs[i]);

	if (m_ui->radioButton_WeatherComboBox->isChecked()){
		weatherPath = weatherDirectory / m_ui->comboBox_WeatherFile->itemText(m_ui->comboBox_WeatherFile->currentIndex()).toStdString() + ".c6b";
		// Note: we can relay on the file to exist, since this is built-in stuff
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
	IBK::Path weatherTargetDir = m_workingDirectory / "climate";
	if( !weatherTargetDir.exists())
		IBK::Path::makePath(weatherTargetDir);
	if( !weatherTargetDir.exists()){
		QMessageBox::critical(this, QString(), tr("Kann Zielklimaverzeichnis '%1' nicht erstellen.").arg(QString::fromStdString(weatherTargetDir.str())));
		return;
	}
	bool success = IBK::Path::copy(weatherPath, weatherTargetDir / weatherName);
	if (!success) {
		QMessageBox::critical(this, QString(), tr("Konnte Klimadatei '%2' nicht ins Arbeitsverzeichnis '%1' kopieren.")
								.arg(QString::fromStdString(weatherTargetDir.str())).arg(QString::fromStdString(weatherPath.str())) );
		return;
	}
	// TODO : check all the other input for meaningful values


	// populate manufacturing data with input values
	if (m_ui->radioButton_PVDatabase->isChecked()){
		m_pvtool.m_manuData = m_pvModule[m_ui->comboBox_PVModule->currentIndex()];
	}
	else {

		// set PV module data and throw Message Box if input is wrong
		if (!convertLineEditintoDouble(this, m_ui->label_uMPP, m_ui->lineEdit_uMPP, m_pvtool.m_manuData.m_vmp)) return;
		if (!convertLineEditintoDouble(this, m_ui->label_iMPP_2, m_ui->lineEdit_iMPP, m_pvtool.m_manuData.m_imp)) return;
		if (!convertLineEditintoDouble(this, m_ui->label_uOC, m_ui->lineEdit_uOC, m_pvtool.m_manuData.m_voc)) return;
		if (!convertLineEditintoDouble(this, m_ui->label_iSC, m_ui->lineEdit_iSC, m_pvtool.m_manuData.m_isc)) return;
		if (!convertLineEditintoDouble(this, m_ui->label_tempCoeffI, m_ui->lineEdit_alpha, m_pvtool.m_manuData.m_alpha)) return;
		if (!convertLineEditintoDouble(this, m_ui->label_tempCoeffU, m_ui->lineEdit_beta, m_pvtool.m_manuData.m_beta)) return;
		if (!convertLineEditintoDouble(this, m_ui->label_tempCoeffU, m_ui->lineEdit_gamma, m_pvtool.m_manuData.m_gamma)) return;
		if (!convertLineEditintoInt(this, m_ui->label_nSer, m_ui->lineEdit_nSer, m_pvtool.m_manuData.m_nSer)) return;
		m_pvtool.m_manuData.m_refTemp = 298.15;
		m_pvtool.m_manuData.m_material = static_cast<PVTOOL::Energy::ManufactureData::CellType>(m_ui->comboBoxCellType->currentData().toInt());


		m_pvtool.m_manuData.m_name = "UserGeneratedPVModule";
	}

	//Get cost and load profile data
	{
		if (!convertLineEditintoDouble(this, m_ui->label_AddCostPV, m_ui->lineEdit_AddCostPV, m_costPvModule)) return;
		if (!convertLineEditintoDouble(this, m_ui->label_CostPCM, m_ui->lineEdit_AddCostPCM, m_costPCM)) return;
		if (!convertLineEditintoDouble(this, m_ui->label_PCMCasing, m_ui->lineEdit_AddCostPCMCasing, m_costCasing)) return;
		if (!convertLineEditintoDouble(this, m_ui->label_CostElectricitySale, m_ui->lineEdit_CostElectrEnergySale, m_costElectrEnergySale)) return;
		if (!convertLineEditintoDouble(this, m_ui->label_CostElectricityPurchase, m_ui->lineEdit_CostElectrEnergyPurchase, m_costElectrEnergyPurchase)) return;
		if (!convertLineEditintoDouble(this, m_ui->label_PriceIncrease, m_ui->lineEdit_ElectrEnergyPriceIncrease, m_increasePriceElectricity)) return;
		if (!convertLineEditintoInt(this, m_ui->label_numberOfModules, m_ui->lineEdit_numberOfModules, m_moduleCount)) return;
		if (!convertLineEditintoInt(this, m_ui->label_LccDuration, m_ui->lineEdit_LccDuration, m_lccDuration)) return;
		//unit conversion

		m_costElectrEnergySale *= 0.01;
		m_costElectrEnergyPurchase *= 0.01;
		m_increasePriceElectricity *= 0.01;

		if(!m_ui->lineEdit_LoadProfile->text().isEmpty()){
			m_filenameLoadProfile = IBK::Path(m_ui->lineEdit_LoadProfile->text().toStdString());
			if(!m_filenameLoadProfile.exists()){
				QMessageBox::critical(this, QString(), tr("Die angegebenen Datei '%1' kann nicht gefunden werden.").arg(m_filenameLoadProfile.c_str()));
				return;
			}
		}
		//check read load profile
		readLoadProfile(IBK::Path(m_filenameLoadProfile));
		//m_moduleCount = 50;

	}

	// now pre-calculate manufacturing parameter set
	try {
		m_pvtool.calcPhysicalParameterFromManufactureData();
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
	// create "power" subdirectory if it doesn't exist yet
	if (!QFileInfo(workingDir + "/power").exists()) {
		if (!QDir().mkpath(workingDir + "/power")) {
			QMessageBox::critical(this, QString(), tr("Konnte das Arbeitsverzeichnis nicht erstellen. Möglicherweise fehlen die Zugriffsrechte?"));
			return;
		}
	}

	// read the template files into memory
	// - project template
	// - insulation template
	IBK::Path d6pTemplatePath( (PVTDirectories::resourcesRootDir() + "/file_templates/template_simple.d6p").toStdString());
	IBK::Path d6pTemplateWithoutPCMPath( (PVTDirectories::resourcesRootDir() + "/file_templates/templateWithoutPCM_simple.d6p").toStdString());
	IBK::Path m6TemplatePath( (PVTDirectories::resourcesRootDir() + "/file_templates/InsulationMatTemplate.m6").toStdString());

	// Wenn Radiobutton für Kamm eingebaut, dann hier Variable setzen ToDo Dirk
	bool withComb=false;
	if(withComb)
		 d6pTemplatePath= IBK::Path( (PVTDirectories::resourcesRootDir() + "/file_templates/template_simple_comb.d6p").toStdString());
	std::string d6Template, d6TemplateWithoutPCM, m6Template;
	{
		std::ifstream in(d6pTemplatePath.str());
		std::stringstream strm;
		strm << in.rdbuf();
		d6Template = strm.str();
	}
	{
		std::ifstream in(d6pTemplateWithoutPCMPath.str());
		std::stringstream strm;
		strm << in.rdbuf();
		d6TemplateWithoutPCM = strm.str();
	}
	{
		std::ifstream in(m6TemplatePath.str());
		std::stringstream strm;
		strm << in.rdbuf();
		m6Template = strm.str();
	}

	// copy entire working directory

	IBK::Path templateDirectory( (PVTDirectories::resourcesRootDir() + "/project_template").toStdString());

	success = IBK::Path::copy(templateDirectory, m_workingDirectory);
	if (!success) {
		QMessageBox::critical(this, QString(), tr("Konnte Projektverzeichnis nicht ins Arbeitsverzeichnis '%1' kopieren.").arg(workingDir));
		return;
	}
	// copy power drain template
	success = IBK::Path::copy( IBK::Path((PVTDirectories::resourcesRootDir() + "/file_templates/no_drain.tsv").toStdString()), m_workingDirectory / "power");
	if (!success) {
		QMessageBox::critical(this, QString(), tr("Konnte 'no_drain.tsv' nicht ins Arbeitsverzeichnis '%1' kopieren.").arg(workingDir));
		return;
	}

	// adjust template insulation material and write to target directory
	IBK::Path insulationM6Path( m_workingDirectory / "materials/InsulationMat.m6");

	createM6File(m6Template, insulationM6Path, m_ui->doubleSpinBox_Density->value(),
				 m_ui->doubleSpinBox_SpecHeatCapa->value(),
				 m_ui->doubleSpinBox_Conductivity->value());

	// *** start variation loop

	double insuThick = m_ui->doubleSpinBox_InsulationThickness->value()/100;
	const unsigned int MAX_WRM_ITERS = 3;
	for (unsigned int WRMCount=0; WRMCount<MAX_WRM_ITERS; ++WRMCount) {
		IBK::Path d6pWithoutPCM(IBK::FormatString( "%1/projectWithoutPCM-%2.d6p").arg(m_workingDirectory).arg(WRMCount).str());
		std::string powerDrainFilePath = "no_drain.tsv";
		// for all but first use counter
		if (WRMCount > 0) {
			powerDrainFilePath = IBK::FormatString("pv_power_drain_withoutPCM-%1.tsv").arg(WRMCount).str();
		}
		createDelphinProject(d6TemplateWithoutPCM, d6pWithoutPCM,0, insuThick,
							 "SP26", weatherName.str(), powerDrainFilePath,
							 m_ui->lineEdit_Inclination->text().toStdString(),
							 m_ui->lineEdit_Orientation->text().toStdString());
		QStringList discCmdLine0;
		QDir::setCurrent(workingDir);
		std::string discFilename = d6pWithoutPCM.filename().withoutExtension().str() + "-disc.d6p";
		discCmdLine0 << QString::fromStdString(d6pWithoutPCM.filename().str())
					<< QString::fromStdString("-o=" + discFilename)
					<< "-q"
					<< "-l=2";
		startDiscProcess(CMDDISCPATH,discCmdLine0,this);
		m_waitingProjects.append(QString::fromStdString((d6pWithoutPCM.parentPath() / discFilename).str()));
	}

	m_thicknessPCM = {0, 0.01, 0.02/*, 0.03*/};	//thickness of pcm

	for (size_t i=1; i<m_thicknessPCM.size(); ++i) {
		for (unsigned int WRMCount=0; WRMCount<MAX_WRM_ITERS; ++WRMCount) {

			IBK::Path d6ProjectPath(IBK::FormatString( "%1/project%2-%3.d6p").arg(m_workingDirectory).arg(i).arg(WRMCount).str());
			std::string powerDrainFilePath = "no_drain.tsv";
			// for all but first use counter
			if (WRMCount > 0) {
				powerDrainFilePath = IBK::FormatString("pv_power_drain_PCM%1-%2.tsv").arg(i).arg(WRMCount).str();
			}
			//for detailed model see description below...not required in the simple model
			//m_thicknessPCM[i] -= 0.004; // pcm has 3 cells, two non adjustable cells are set to 5 mm
			// - adjust PCM material layer thickness and write project template
			auto sss = m_ui->comboBox_PCMMaterials->currentText().toStdString();
			createDelphinProject(d6Template, d6ProjectPath, m_thicknessPCM[i], insuThick,
								 m_ui->comboBox_PCMMaterials->currentText().toStdString(), weatherName.str(), powerDrainFilePath,
								 m_ui->lineEdit_Inclination->text().toStdString(),
								 m_ui->lineEdit_Orientation->text().toStdString());
			// now run CmdDiscretize to generate discretized project file
			QStringList discCmdLine;
			//	QString projectFile = QString::fromStdString(d6ProjectPath.filename().str());
			//	QString projectDiscFile = QString::fromStdString(d6ProjectPath.filename().withoutExtension().str() + "-disc.d6p");
			//	discCmdLine << projectFile << "-o="+projectDiscFile;
			std::string discFilename = d6ProjectPath.filename().withoutExtension().str() + "-disc.d6p";
			discCmdLine << QString::fromStdString(d6ProjectPath.filename().str())
						<< QString::fromStdString("-o=" + discFilename)
						<< "-q"
						<< "-l=2";

			startDiscProcess(CMDDISCPATH,discCmdLine,this);
	//		QProcess p;
	//		int res = p.execute(CMDDISCPATH, discCmdLine);
	//		if (res != 0) {
	//			QMessageBox::critical(this, QString(), tr("Fehler bei der Ausführung des CmdDiscretize-Tools."));
	//			return;
	//		}
			m_waitingProjects.append(QString::fromStdString((d6ProjectPath.parentPath() / discFilename).str()));
		} // WRM loop
	}

	if (m_progressDlg == nullptr) {
		m_progressDlg = new QProgressDialog(tr("Simuliere Geometrievarianten..."), tr("Abbrechen"), 0, m_waitingProjects.count(), this);
		connect(&m_simProgressTimer, &QTimer::timeout, this, &PVToolWidget::onSimProgressTimerTimeout);
	}
	m_progressDlg->setMaximum(m_waitingProjects.count()*100);
	m_progressDlg->setValue(0);
	m_progressDlg->setWindowModality(Qt::WindowModal);
	m_progressDlg->show();
	m_simProgressTimer.setInterval(1000);
	m_simProgressTimer.start();
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
	if (status == QProcess::CrashExit || exitCode != 0) {
		// unless dlg was cancelled
		if (!m_progressDlg->wasCanceled()) {
			IBK::Path logFile(IBK::Path(m_completedProjects.back().toStdString()).withoutExtension() / "log/screenlog.txt");
			QMessageBox::critical(this, QString(), tr("Fehler bei der Ausführung der DELPHIN-Simulation, siehe Logdatei '%1'")
									.arg(QString::fromStdString(logFile.str())));
		}
		m_simProgressTimer.stop();
		m_progressDlg->hide();
		return;
	}

	// now read temperature from just completed D6 project and generate
	// power drain tsv file for next project
	IBK::Path filename("");
	evaluateResults(filename);


	startNextDELPHINSim();
}


void PVToolWidget::onSimProgressTimerTimeout() {
	// was the simulation aborted?
	if (m_progressDlg->wasCanceled()) {
		// kill solver process, if still running
		m_cmdLineProcess->kill();
		return;
	}
	// read currently processed simulation job's progress.tsv file, extract last line's percentage and update progress bar
	IBK::Path currentSimJob(m_completedProjects[m_finishedProjects].toStdString());
	IBK::Path path2Progress = currentSimJob.withoutExtension() / "log/progress.tsv"; //changed from tsv to txt (OS: win64)
	std::ifstream in(path2Progress.str());
	std::string lastLine, line;
	while (std::getline(in, line)) {
		if (line.find_first_not_of(" \t\r") == std::string::npos)
			continue; // skip empty lines
		lastLine = line;
	}
	// if all went well, lastLine is not empty and contains the corrent percentage
	if (lastLine.empty())
		return; // nope, some error
	// extract columns
	std::vector<double>  vals;
	try {
		IBK::string2valueVector(lastLine, vals);
		// we need the third number
		if (vals.size() > 2) {
			double percentage = vals[2];
			// compute progress bar value
			int progressBarValue = percentage + m_finishedProjects*100;
			m_progressDlg->setValue(progressBarValue);
			if (progressBarValue > m_tempProgress+20)
				qDebug() << progressBarValue;
			m_tempProgress = progressBarValue;
		}
	}
	catch (...) {
		// don't mind the error, just do nothing here
	}
}


void PVToolWidget::onButtonBarQuitClicked() {
	close();
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
										const std::string & climateDataFileName,
										const std::string & powerDrainFilePath,
										const std::string & inclination,
										const std::string & orientation)
{
	std::string d6str = IBK::replace_string(d6Template, "${PCMThick}", IBK::val2string(pcmThickness));
	d6str = IBK::replace_string(d6str, "${INSULATIONThick}", IBK::val2string(insulationThickness));
	d6str = IBK::replace_string(d6str, "${PCM}",pcmMaterialFileName);
	d6str = IBK::replace_string(d6str, "${CLIMATE}", climateDataFileName);
	d6str = IBK::replace_string(d6str, "${POWER_DRAIN_FILE_PATH}", powerDrainFilePath);
	d6str = IBK::replace_string(d6str, "${INCLINATION}", inclination);
	d6str = IBK::replace_string(d6str, "${ORIENTATION}", orientation);

	// write file
	std::ofstream out(d6ProjectFilePath.str());
	out << d6str << std::endl;

}


void PVToolWidget::startNextDELPHINSim() {
	if (m_waitingProjects.isEmpty() || m_progressDlg->wasCanceled()) {
		m_simProgressTimer.stop();
		//evaluateResults();
		showResults();
		m_progressDlg->hide();
		m_completedProjects.clear();
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

void PVToolWidget::clearResultVecs(){
	m_temperature.clear();
	m_radiation.clear();
	m_pvEnergy.clear();
}

void PVToolWidget::evaluateResults(IBK::Path &filename){
	m_completedProjects.back();

	IBK::Path tmp(m_completedProjects.back().toStdString());
	IBK::Path tmp2(tmp.parentPath() / tmp.filename().withoutExtension() / "results/");

	//read temperature and radiation data
	IBK::UnitVector temperature, radiation;
	try {
		DATAIO::DataIO temp;
		temp.read(tmp2 / "TMean.d6o");
		temperature.m_data = temp.columnValues(0);
		temperature.m_unit = IBK::Unit(temp.m_valueUnit);
		temperature.convert(IBK::Unit("K"));
	} catch (IBK::Exception &ex ) {
		QMessageBox::critical(this, QString(), tr("Fehler während des Lesens der Ergebnisse Temperatur von DELPHIN. \n %1").arg(ex.what()));
		return;
	}
	try {
		DATAIO::DataIO rad;
		rad.read(tmp2 / "GlobalRadiation.d6o");
		IBK::UnitVector unitVec;
		radiation.m_data = rad.columnValues(0);
		for (double &v : radiation.m_data)
			v/=0.7; //absorption coeficient from Delphin
		radiation.m_unit = IBK::Unit(rad.m_valueUnit);
		radiation.convert(IBK::Unit("W/m2"));
	} catch (IBK::Exception &ex) {
		QMessageBox::critical(this, QString(), tr("Fehler während des Lesens der Ergebnisse Strahlung von DELPHIN. \n %1").arg(ex.what()));
		return;
	}

	//run pvEnergy
	std::vector<double> energyRes;
	try {
//		int incrementValue = m_progressDlg->maximum();

		m_pvtool.calcPVEnergy(temperature.m_data,radiation.m_data, energyRes);
//		int progressValue = m_progressDlg->value();
//		progressValue += incrementValue;
//		m_progressDlg->setValue(progressValue);

	} catch (IBK::Exception &ex) {
		QMessageBox::critical(this, QString(), tr("%1").arg(ex.what()));
		return;
	}
	//create path variables (reading/writing)
	IBK::Path outFilename(tmp2.parentPath().parentPath() / "power" );
	std::string testStr = tmp2.parentPath().filename().str();
	std::string newFile = "pv_power_drain_";
	unsigned int fileNr;
	bool withPCM = false;
	if(testStr.find("WithoutPCM") != std::string::npos){
		std::string filenameOld = IBK::Path(m_completedProjects.back().toStdString()).filename().withoutExtension().str();
		std::string filenameNr = filenameOld.substr(filenameOld.find_last_of("-disc")-1-4,1);
		fileNr = IBK::string2val<unsigned int>(filenameNr)+1;
		newFile += "withoutPCM-" + IBK::val2string(fileNr) + ".tsv";
		outFilename.addPath(IBK::Path(newFile));
	}
	else {
		withPCM = true;
		std::string filenameOld = IBK::Path(m_completedProjects.back().toStdString()).filename().withoutExtension().str();
		std::string filenameNr = filenameOld.substr(filenameOld.find_last_of("-disc")-3-4,3);
		fileNr = IBK::string2val<unsigned int>(filenameNr.substr(2))+1;
		newFile += "PCM" + filenameNr.substr(0,2) + IBK::val2string(fileNr) + ".tsv";
		outFilename.addPath(IBK::Path(newFile));
	}
	//save last iteration vector for reporting
	if(fileNr == 3){
		IBK::UnitVector energy;
		energy.m_data = energyRes;
		energy.m_name = "PV Energy";
		energy.m_unit = IBK::Unit("W");
		m_pvEnergy.push_back(energy);
		//write file
		std::ofstream out2(outFilename.parentPath().str() + "/" + (withPCM ? "PCM" : "withoutPCM") + IBK::val2string(m_pvEnergy.size()-1) + ".tsv");
		out2 << "Time [h]\tPVEnergy[J/m3s]" << std::endl;
		//cyclic year: attention no time data value of 0 AND 8760 in one file
		size_t timeVal = 0;

		for(double &val : m_pvEnergy.back().m_data){

			out2 << timeVal++ << "\t" << IBK::val2string(val) << std::endl;
		}
		out2 << std::endl;
		out2.close();
	}
	//write file
	std::ofstream out(outFilename.str());
	out << "Time [h]\tPVEnergy[J/m3s]" << std::endl;
	//cyclic year: attention no time data value of 0 AND 8760 in one file
	size_t maxI = energyRes.size() <= 8760 ? energyRes.size() : 8760;
	for(size_t i=0; i<maxI; ++i){
		double val = energyRes[i]/(0.02*1.6);
		out << i << "\t" << (IBK::near_zero(val) ? "" : "-") << IBK::val2string(val,10) << std::endl;
	}
	out << std::endl;
	out.close();

	// set progress bar
	++m_finishedProjects;
}

void PVToolWidget::evaluateResults() {
	// process ready results
	//m_completedProjects.

	clearResultVecs();

	std::vector<IBK::Path> projectPaths;
	projectPaths.reserve(m_completedProjects.size());
	for(auto p : m_completedProjects){
		IBK::Path tmp (p.toStdString());
		IBK::Path tmp2(tmp.parentPath() / tmp.filename().withoutExtension() / "results/");
		//read temperature and radiation data

		try {
			DATAIO::DataIO temp;
			temp.read(tmp2 / "TMean.d6o");
			IBK::UnitVector unitVec;
			unitVec.m_data = temp.columnValues(0);
			unitVec.m_unit = IBK::Unit(temp.m_valueUnit);
			unitVec.convert(IBK::Unit("K"));
			m_temperature.push_back(unitVec);
		} catch (IBK::Exception &ex ) {
			QMessageBox::critical(this, QString(), tr("Fehler während des Lesens der Ergebnisse Temperatur von DELPHIN. \n %1").arg(ex.what()));
			return;
		}
		try {
			DATAIO::DataIO rad;
			rad.read(tmp2 / "GlobalRadiation.d6o");
			IBK::UnitVector unitVec;
			unitVec.m_data = rad.columnValues(0);
			for (double &v : unitVec.m_data)
				v/=0.7; //absorption coefficient from Delphin
			unitVec.m_unit = IBK::Unit(rad.m_valueUnit);
			unitVec.convert(IBK::Unit("W/m2"));
			m_radiation.push_back(unitVec);
		} catch (IBK::Exception &ex) {
			QMessageBox::critical(this, QString(), tr("Fehler während des Lesens der Ergebnisse Strahlung von DELPHIN. \n %1").arg(ex.what()));
			return;
		}
	}
	runPVEnergy();
}

void PVToolWidget::runPVEnergy()
{
	std::vector<std::vector<double>> energyRes(m_temperature.size());
	try {
		int incrementValue = m_progressDlg->maximum()/m_temperature.size()/2;
		for (	size_t i=0; i<m_temperature.size(); ++i){
			m_pvtool.calcPVEnergy(m_temperature[i].m_data,m_radiation[i].m_data, energyRes[i]);
			int progressValue = m_progressDlg->value();
			progressValue += incrementValue;
			m_progressDlg->setValue(progressValue);
		}
	} catch (IBK::Exception &ex) {
		QMessageBox::critical(this, QString(), tr("%1").arg(ex.what()));
		return;
	}

	//sum up all value of one vector
	std::vector<double> summedValues(m_temperature.size(),0);
	for(size_t i=0; i<energyRes.size();++i){
		for (size_t j=0; j<energyRes[i].size(); ++j)
			summedValues[i] += energyRes[i][j];
	}

	std::vector<std::string>	results;
	results.push_back(IBK::FormatString("Das Ergebnis jeder Variante wird dargestellt über die Schichtdicke in cm des PCM´s und dem erzeugten Stromertrag in kWh/a : \n %1 %2").arg("Dicke").arg("Ertrag").str());
	for(size_t i=0; i<summedValues.size(); ++i)
		results.push_back( IBK::FormatString("%1 %2").arg(m_thicknessPCM[i]*100,5)
							 .arg(summedValues[i]/1000,8, 'f', 2, ' ', std::ios_base::right).str() );

	PVTResultDialog * PVResults = new PVTResultDialog();
	PVResults->setResultText(results);
	PVResults->setModal(true);
	PVResults->exec();

//	// This loop will wait for the window is destroyed
//	QEventLoop loop;
//	connect(this, SIGNAL(PVResults.destroyed()), & loop, SLOT(quit()));
//	loop.exec();

	//m_progressDlg = new QProgressDialog(tr("Simuliere Geometrievarianten..."), tr("Abbrechen"), 0, m_waitingProjects.count(), this);
	//connect(&m_simProgressTimer, &QTimer::timeout, this, &PVToolWidget::onSimProgressTimerTimeout);

}

void PVToolWidget::writeResults(const IBK::Path &filename, int vectorIdx){
	FUNCID(PVToolWidget::writeResults);

	// write file
	std::ofstream out(filename.str());

	Q_ASSERT(!(vectorIdx<0 || vectorIdx >= m_saleEnergy.size()));

	//konvertierung der Unitvectoren?

	out << "Zeit [h]"<< "\t" << "PV (PCM 0 cm) Produktion [W]"<< "\t" <<
		   "Verbrauch (Lastpropfil) [W]"<< "\t" << "Eigennutzung [W]"<< "\t" <<
		   "Stromzukauf [W]"<< "\t" << "Stromüberschuss [W]" << "\n";
	for (unsigned int i=0; i<m_saleEnergy[vectorIdx].m_data.size(); ++i){
		out << i ;
		out << "\t" << m_pvEnergy[vectorIdx].m_data[i];
		out << "\t" << m_loadProfile.m_data[i];
		out << "\t" << m_ownUseEnergy[vectorIdx].m_data[i];
		out << "\t" << m_purchaseEnergy[vectorIdx].m_data[i];
		out << "\t" << m_saleEnergy[vectorIdx].m_data[i];
		out << "\n";
	}
	if (!out)
		throw IBK::Exception("Error writing file.", FUNC_ID);
}

void PVToolWidget::showResults(){
	FUNCID(PVToolWidget::showResults);
	//sum up all value of one vector
	std::vector<double> summedValues;//(m_pvEnergy.size(),0);
	std::vector<double> summedOwnUse, summedPurchase, summedSale;
	double summedLoad = 0;

	bool isOwnUse = !m_loadProfile.empty();
	//Wenn ein Lastprofil vorhanden ist werden Auswertungen vorgenommen
	if(isOwnUse){
		//Vorinitialsierung der Vektoren
		m_ownUseEnergy = std::vector<IBK::UnitVector> (m_pvEnergy.size());
		m_purchaseEnergy = std::vector<IBK::UnitVector> (m_pvEnergy.size());
		m_saleEnergy = std::vector<IBK::UnitVector> (m_pvEnergy.size());
		summedOwnUse = std::vector<double>(m_pvEnergy.size(),0);
		summedPurchase = std::vector<double>(m_pvEnergy.size(),0);
		summedSale = std::vector<double>(m_pvEnergy.size(),0);
		m_loadProfile.convert(IBK::Unit("W"));
	}

	//TODO Einheitenkonvertierung prüfen alle Vektoren
	//m_pvEnergy

	//pvE ist das Ertragsprofil
	for(unsigned int i=0; i<m_pvEnergy.size(); ++i){
		IBK::UnitVector &pvE =  m_pvEnergy[i];

		double res =0;
		for(unsigned int j=0; j<pvE.m_data.size(); ++j){
			double &val = pvE.m_data[j];
			val *= m_moduleCount;
			res += val;
			if(isOwnUse){
				double ownUse;
				/*Beispiel
				 * Hour		Last [kW]		Erzeugung [kW]		Eigennutzung [kW]
				 *	1			0.3			0					0
				 *	2			0.3			0					0
				 *	3			0.3			0					0
				 *	4			0.3			0					0
				 *	5			2.0			0					0
				 *	6			2.0			0.1					0.1
				 *	7			2.0			0.5					0.5
				 *	8			2.0			1.0					1.0
				 *	9			2.0			3.0					2.0
				 *	10			2.0			6.0					2.0
				 *	11			2.0			5.0					2.0
				*/
				//Eigennutzung

				/*
					if(erz <=0)
						eigen = 0
					else if(last - erz >=0)
						eigen = erz
					else
						eigen = last
				*/
				double load = m_loadProfile.m_data[j];
				if( val <= 0)
					ownUse = 0;
				//Einheiten?
				else if (load - val >= 0 )
					ownUse = val;
				else
					ownUse = load;

				m_ownUseEnergy[i].m_data.push_back(ownUse);
				m_purchaseEnergy[i].m_data.push_back(load - ownUse);
				m_saleEnergy[i].m_data.push_back(val - ownUse);

				summedOwnUse[i] += ownUse;
				summedPurchase[i] += m_purchaseEnergy[i].m_data.back();
				summedSale[i] += m_saleEnergy[i].m_data.back();
			}
		}
		summedValues.push_back(res);
	}
	if(isOwnUse)
		for(unsigned int j=0; j<m_loadProfile.size(); ++j)
			summedLoad += m_loadProfile.m_data[j];
	std::vector<std::string>	results;
	if(isOwnUse)
	{
		/* Cost functions */
		// write file
		std::ofstream out((m_workingDirectory / "cost.tsv").c_str());


		//konvertierung der Unitvectoren?

		results.push_back("Variantenname\tInvestkosten [€]\tBetriebskosten [€]\tGesamtkosten [€] nach "+ IBK::val2string(m_lccDuration) +" Jahren Betriebszeit\n");
		out << results.back();
		double operationCosts = summedLoad * m_costElectrEnergyPurchase * std::pow(1+m_increasePriceElectricity, m_lccDuration) * 0.01;
		results.push_back(IBK::FormatString("Ausgangsituation ohne PV\t0\t%1\t%2")
						  .arg(operationCosts, 8, 'f', 2, ' ', std::ios_base::right)
						  .arg(operationCosts, 8, 'f', 2, ' ', std::ios_base::right).str());

		out << results.back() <<"\n";
		//Invest
		for(unsigned int i=0; i<m_pvEnergy.size(); ++i){
			double invest = (m_costPvModule + (m_costPCM * i)+ (i>0 ? m_costCasing : 0)) * m_moduleCount;
			// kWh * €/kWh * (1+Preissteigerung)^Jahre
			//das sind die Kosten ohne eine Veränderung
			double costRed = summedOwnUse[i] * m_costElectrEnergyPurchase * std::pow(1+m_increasePriceElectricity, m_lccDuration) * 0.01;
			//8 ¢/kWh
			double salePV = summedSale[i] * m_costElectrEnergySale * 0.01;
			double operationCostsWithPV = operationCosts - costRed - salePV;
			std::string text ="PV ";
			text += (i==0 ? "ohne PCM" :"mit "+IBK::val2string(i)+" cm PCM");
			results.push_back(IBK::FormatString("%1\t%2\t%3\t%4").arg(text)
							  .arg(invest, 8, 'f', 2, ' ', std::ios_base::right)
							  .arg(operationCostsWithPV, 8, 'f', 2, ' ', std::ios_base::right)
							  .arg(invest + operationCostsWithPV, 8, 'f', 2, ' ', std::ios_base::right).str());

			out << results.back() << "\n";
		}
		results.push_back("\n");

		// Variante 1 operationCosts
		// Variante 2 operationCostsWithPV[0] + invest[0] (ohne PCM)
		// Variante 3 operationCostsWithPV[1] + invest[1] (1 cm PCM)
		// Variante 4 operationCostsWithPV[2] + invest[2] (2 cm PCM)

		if (!out)
			throw IBK::Exception("Error writing file.", FUNC_ID);

		writeResults(IBK::Path(m_workingDirectory / "pcm0.tsv"), 0);
		writeResults(IBK::Path(m_workingDirectory / "pcm1.tsv"), 1);
		writeResults(IBK::Path(m_workingDirectory / "pcm2.tsv"), 2);
	}

	results.push_back(IBK::FormatString("Das Ergebnis jeder Variante wird dargestellt über die Schichtdicke in cm des PCM´s und dem erzeugten Stromertrag in kWh/a : \n %1 %2").arg("Dicke").arg("Ertrag").str());
	for(size_t i=0; i<summedValues.size(); ++i)
		results.push_back( IBK::FormatString("%1 %2").arg(m_thicknessPCM[i]*100,5)
							 .arg(summedValues[i]/1000,8, 'f', 2, ' ', std::ios_base::right).str() );

	PVTResultDialog * PVResults = new PVTResultDialog();
	PVResults->setResultText(results);
	PVResults->setModal(true);
	PVResults->exec();
	clearResultVecs();
}




void PVToolWidget::on_pushButtonLoadProfile_clicked(){
	QString fpath = QFileDialog::getOpenFileName(this,tr("Öffne Lastprofil"),QString(),tr("(*.tsv)"));
	if(fpath.isEmpty())
		return;
	m_ui->lineEdit_LoadProfile->setText(fpath);
}


