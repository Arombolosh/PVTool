#include "PVTResultDialog.h"
#include "ui_PVTResultDialog.h"

#include <QFileDialog>
#include <QFont>
#include <QMessageBox>

#include "PVTConstants.h"

PVTResultDialog::PVTResultDialog(QWidget *parent) :
	QDialog(parent),
	m_ui(new Ui::PVTResultDialog)
{
	m_ui->setupUi(this);

	QFont f;
	f.setFamily(FIXED_FONT_FAMILY);
	f.setPointSize(TABLE_FONT_SIZE);
	m_ui->plainTextEditResults->setFont(f);
}

PVTResultDialog::~PVTResultDialog()
{
	delete m_ui;
}

void PVTResultDialog::setResultText(const std::vector<std::string> & resultLines) {

	//	ui->plainTextEditResults->appendPlainText(line);
	for (size_t i=0;i<resultLines.size();++i)
		m_ui->plainTextEditResults->appendPlainText(QString::fromStdString(resultLines[i]));

}
void PVTResultDialog::on_pushButtonClose_clicked()
{
	close();
}

void PVTResultDialog::on_pushButtonSaveToFile_clicked()
{
	QString filename = QFileDialog::getSaveFileName(this,tr("Speichere Ergebnisdatei"), QString(),tr("Text File (*.txt)"));
	QFile f( filename );
	f.open( QIODevice::WriteOnly ); // only saving possible
	// store data in f

	if (f.write(m_ui->plainTextEditResults->toPlainText().toStdString().c_str())==-1)
		QMessageBox::critical(this,"Error",QString("Ergebnisdatei konnte nicht geschrieben werden in:\n%1").arg(filename));
	else
		QMessageBox::information(this,QString(),"Ergebnisdatei wurde geschrieben.");
	f.close();
	close();
}
