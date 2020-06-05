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
	this->close();
}

void PVTResultWidget::on_pushButtonSaveToFile_clicked()
{
	QString filename = QFileDialog::getSaveFileName(this,tr("Speichere Ergebnisdatei"), QString(),tr("Text File (*.txt)"));
	QFile f( filename );
	f.open( QIODevice::WriteOnly ); // only saving possible
	// store data in f

	f.write(m_ui->plainTextEditResults->toPlainText().toStdString().c_str());

	f.close();
}
