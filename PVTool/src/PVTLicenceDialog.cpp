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
	licence = "PVTool - A tool for optimizing PCM layer width below PV panels\n"
			"\n"
			"This software is free and open-source software, licensed under the BSD 3-Clause License.\n"
			"\n"
			"Copyright (c) 2020, IBK/TU Dresden\n"
			"All rights reserved.\n"
			"\n"
			"Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:\n"
			"\n"
			"1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.\n"
			"\n"
			"2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.\n"
			"\n"
			"3. Neither the name of the copyright holder nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.\n"
			"\n"
			"THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS 'AS IS'"
			"AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE"
			"IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE"
			"DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE"
			"FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL"
			"DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR"
			"SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER"
			"CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,"
			"OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE"
			"OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.\n";
	m_ui->plainTextEditLicence->appendPlainText(QString::fromStdString(licence));

}
void PVTLicenceDialog::on_pushButtonClose_clicked()
{
	close();
}

