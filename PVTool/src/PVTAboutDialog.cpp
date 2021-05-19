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

void PVTAboutDialog::setText() {

	//	ui->plainTextEditResults->appendPlainText(line);
	std::string	about;
	about = "Über Qt\n"
			"Dieses Programm verwendet Qt Version 5.12.8."
			"Qt is a C++ toolkit for cross-platform application development."
			"Qt is a C++ toolkit for cross-platform application development.\n"
			"\n"
			"Qt provides single-source portability across all major desktop operating systems. It is also available for embedded Linux and other embedded and mobile operating systems.\n"
			"\n"
			"Qt is available under multiple licensing options designed to accommodate the needs of our various users.\n"
			"\n"
			"Qt licensed under our commercial license agreement is appropriate for development of proprietary/commercial software where you do not want to share any source code with third parties or otherwise cannot comply with the terms of GNU (L)GPL.\n"
			"\n"
			"Qt licensed under GNU (L)GPL is appropriate for the development of Qt applications provided you can comply with the terms and conditions of the respective licenses.\n"
			"\n"
			"Please see qt.io/licensing for an overview of Qt licensing.\n"
			"Copyright (C) 2020 The Qt Company Ltd and other contributors.\n"
			"Qt and the Qt logo are trademarks of The Qt Company Ltd.\n"
			"Qt is The Qt Company Ltd product developed as an open source project. See qt.io for more information.\n"
			"\n"
			"Über SAM\n"
			"Dieses Programm verwendet die SAM-Bibliothek.\n"
			"Copyright (c) 2019, Alliance for Sustainable Energy, LLC\n"
			"All rights reserved.";
	m_ui->plainTextEditAbout->appendPlainText(QString::fromStdString(about));

}
void PVTAboutDialog::on_pushButtonClose_clicked()
{
	close();
}

