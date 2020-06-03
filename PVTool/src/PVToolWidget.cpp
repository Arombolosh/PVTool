#include "PVToolWidget.h"
#include "ui_PVToolWidget.h"

PVToolWidget::PVToolWidget(QWidget *parent) :
	QWidget(parent),
	ui(new Ui::PVToolWidget)
{
	ui->setupUi(this);

	ui->radioButton_WeatherComboBox->setChecked(true);
	on_radioButton_WeatherComboBox_toggled(true);

	//abschalten von updates
	ui->comboBox_WeatherFile->blockSignals(true);

	//todo schleife mit weather
	ui->comboBox_WeatherFile->addItem(tr("Dresden"));
	ui->comboBox_WeatherFile->addItem(tr("Potsdam"));
	//einschalten von updates
	ui->comboBox_WeatherFile->blockSignals(false);
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
