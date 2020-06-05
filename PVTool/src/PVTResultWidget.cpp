#include "PVTResultWidget.h"
#include "ui_PVTResultWidget.h"

#include <QFileDialog>

PVTResultWidget::PVTResultWidget(QWidget *parent) :
	QWidget(parent),
	m_ui(new Ui::PVTResultWidget)
{
	m_ui->setupUi(this);
}

PVTResultWidget::~PVTResultWidget()
{
	delete m_ui;
}

void PVTResultWidget::setResultText(const std::vector<std::string> & resultLines) {

	//	ui->plainTextEditResults->appendPlainText(line);
	for (size_t i=0;i<resultLines.size();++i)
		m_ui->plainTextEditResults->appendPlainText(QString::fromStdString(resultLines[i]));

}
void PVTResultWidget::on_pushButtonClose_clicked()
{
	delete m_ui;
}

void PVTResultWidget::on_pushButtonSaveToFile_clicked()
{
	QString filename = QFileDialog::getSaveFileName(this,tr("Speichere Ergebnisdatei"), QString(),tr("Text File (*.txt)"));
//	QString fpath = QFileDialog::getOpenFileName(this,tr("Öffne Wetterdatei"),QString(),tr("EnergyPlus Weather Files (*.epw)"));
	QFile f( filename );
	f.open( QIODevice::WriteOnly );
	// store data in f
	f.close();
}
