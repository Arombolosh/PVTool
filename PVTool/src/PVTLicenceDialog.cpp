#include "PVTLicenceDialog.h"
#include "ui_PVTLicenceDialog.h"

#include <QFileDialog>
#include <QFont>
#include <QMessageBox>

#include "PVTConstants.h"

PVTLicenceDialog::PVTLicenceDialog(QWidget *parent) :
	QDialog(parent),
	m_ui(new Ui::PVTLicenceDialog)
{
	m_ui->setupUi(this);

	QFont f;
	f.setFamily(FIXED_FONT_FAMILY);
	f.setPointSize(TABLE_FONT_SIZE);
	m_ui->plainTextEditLicence->setFont(f);
}

PVTLicenceDialog::~PVTLicenceDialog()
{
	delete m_ui;
}

void PVTLicenceDialog::setText() {

	//	ui->plainTextEditResults->appendPlainText(line);
	std::string	licence;
	licence = "Hier könnte Ihr Lizenztext stehen";
	m_ui->plainTextEditLicence->appendPlainText(QString::fromStdString(licence));

}
void PVTLicenceDialog::on_pushButtonClose_clicked()
{
	close();
}

