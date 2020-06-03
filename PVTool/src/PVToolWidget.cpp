#include "PVToolWidget.h"
#include "ui_PVToolWidget.h"

#include <QFileDialog>


PVToolWidget::PVToolWidget(QWidget *parent) :
	QWidget(parent),
	ui(new Ui::PVToolWidget)
{
	ui->setupUi(this);

	ui->radioButton_WeatherComboBox->setChecked(true);
	on_radioButton_WeatherComboBox_toggled(true);

	//Data Weatherfiles
	//abschalten von updates
	ui->comboBox_WeatherFile->blockSignals(true);

	//todo schleife mit weather
	ui->comboBox_WeatherFile->addItem(tr("Dresden"));
	ui->comboBox_WeatherFile->addItem(tr("Potsdam"));
	//einschalten von updates
	ui->comboBox_WeatherFile->blockSignals(false);

	ui->radioButton_PVDatabase->setChecked(true);
	on_radioButton_PVDatabase_toggled(true);

	//Data PV modules
	//abschalten von updates
	ui->comboBox_PVModule->blockSignals(true);

	//todo schleife mit weather
	ui->comboBox_PVModule->addItem(tr("alea S_19"));
	ui->comboBox_PVModule->addItem(tr("heckert solar"));
	//einschalten von updates
	ui->comboBox_PVModule->blockSignals(false);


	//Data PV modules
	//abschalten von updates
	ui->comboBox_PCMMaterials->blockSignals(true);

	//todo schleife mit weather
	ui->comboBox_PCMMaterials->addItem(tr("SP26"));
	ui->comboBox_PCMMaterials->addItem(tr("SP30"));
	//einschalten von updates
	ui->comboBox_PCMMaterials->blockSignals(false);
}

PVToolWidget::~PVToolWidget()
{
	delete ui;
}

void PVToolWidget::on_radioButton_WeatherComboBox_toggled(bool checked)
{
	ui->comboBox_WeatherFile->setEnabled(checked);
	ui->lineEdit_EPWFile->setEnabled(!checked);
	ui->pushButton_EPW->setEnabled(!checked);
}

void PVToolWidget::on_radioButton_PVDatabase_toggled(bool checked)
{
	ui->comboBox_PVModule->setEnabled(checked);
	ui->lineEdit_iSC->setEnabled(!checked);
	ui->lineEdit_uOC->setEnabled(!checked);
	ui->lineEdit_iMPP->setEnabled(!checked);
	ui->lineEdit_uMPP->setEnabled(!checked);
	ui->lineEdit_alpha->setEnabled(!checked);
	ui->lineEdit_beta->setEnabled(!checked);
	ui->lineEdit_gamma->setEnabled(!checked);
	ui->lineEdit_nSer->setEnabled(!checked);


}

void PVToolWidget::on_pushButton_EPW_clicked()
{
	QString fpath = QFileDialog::getOpenFileName(this,tr("Öffne Wetterdatei"),QString(),tr("EnergyPlus Weather Files (*.epw)"));
	if(fpath.isEmpty())
		return;
	ui->lineEdit_EPWFile->setText(fpath);
}

void PVToolWidget::on_comboBox_PVModule_currentIndexChanged(int index)
{
	double isc = 3.2;
	ui->lineEdit_iSC->setText(QString("%L1").arg(isc));
	ui->lineEdit_iSC->setText(QString("%L1").arg(isc));
}

void PVToolWidget::on_pushButton_RunSimu_clicked()
{
	//PCM-> Material (PCM) kopieren aus Vorgabedateien
	//Wärmedämmung (Insulation) erstellen aus Vorgaben
	//template datei bearbeiten und an richtige stelle kopieren alle weiteren dateien kopieren/erstellen
	//string ersetzen d6p datei ersetzen (klima insulation pcm dicken)

	// discre. aufrufen

	//d6 solver mit der neuen d6p datei starten

	//pvtool anwerfen mit pfadübergabe von delphin ausgaben und aus gui exportierten pv daten

	//ergebnisse sammeln


}
