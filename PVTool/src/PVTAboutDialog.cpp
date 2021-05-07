#include "PVTAboutDialog.h"
#include "ui_PVTAboutDialog.h"

#include <QFileDialog>
#include <QFont>
#include <QMessageBox>

#include "PVTConstants.h"

PVTAboutDialog::PVTAboutDialog(QWidget *parent) :
	QDialog(parent),
	m_ui(new Ui::PVTAboutDialog)
{
	m_ui->setupUi(this);

	QFont f;
	f.setFamily(FIXED_FONT_FAMILY);
	f.setPointSize(TABLE_FONT_SIZE);
	m_ui->plainTextEditAbout->setFont(f);
}

PVTAboutDialog::~PVTAboutDialog()
{
	delete m_ui;
}

void PVTAboutDialog::setAboutText() {

	//	ui->plainTextEditResults->appendPlainText(line);
	std::string	about;
	about = "Hier könnte Ihr Abouttext stehen";
	m_ui->plainTextEditAbout->appendPlainText(QString::fromStdString(about));

}
void PVTAboutDialog::on_pushButtonClose_clicked()
{
	close();
}

