#include "PVTPostProcDialog.h"
#include "ui_PVTPostProcDialog.h"

#include <QFileDialog>
#include <QFont>
#include <QMessageBox>

#include "PVTConstants.h"

PVTPostProcDialog::PVTPostProcDialog(QWidget *parent) :
	QDialog(parent),
	m_ui(new Ui::PVTPostProcDialog)
{
	m_ui->setupUi(this);

//	QFont f;
//	f.setFamily(FIXED_FONT_FAMILY);
//	f.setPointSize(TABLE_FONT_SIZE);
//	m_ui->plainTextEditPostProc->setFont(f);
}

PVTPostProcDialog::~PVTPostProcDialog()
{
	delete m_ui;
}

void PVTPostProcDialog::setText() {

	//	ui->plainTextEditResults->appendPlainText(line);


}



void PVTPostProcDialog::on_pushButtonDirectory_clicked()
{

}
