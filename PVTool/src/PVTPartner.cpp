#include "PVTPartner.h"
#include "ui_PVTPartner.h"

#include <QFileDialog>
#include <QFont>
#include <QMessageBox>

#include "PVTConstants.h"

PVTPartner::PVTPartner(QWidget *parent) :
	QDialog(parent),
	m_ui(new Ui::PVTPartner)
{
	m_ui->setupUi(this);

	QFont f;
	f.setFamily(FIXED_FONT_FAMILY);
	f.setPointSize(TABLE_FONT_SIZE);
	m_ui->plainTextEditTUD->setFont(f);
	m_ui->plainTextEditRubitherm->setFont(f);
	m_ui->plainTextEditPazdera->setFont(f);
	m_ui->plainTextEditADOMetall->setFont(f);

	QString str = "TU Dresden Institut für Bauklimatik\nAnsprechpartner Dirk Weiß\nTel.: +49 351 463 35367\nEmail: dirk.weiss@tu-dresden.de\nWebseite: www.tu-dresden.de/bauklimatik\nwww.bauklimatik-dresden.de";
	str += "\n\nTU Dresden Institut für Baukonstruktion\nAnsprechpartner Christian Popp\nTel.: +49 351 463 32306\nEmail: christian.popp@tu-dresden.de\nWebseite: www.tu-dresden.de/bu/bauingenieurwesen/bauko";

	m_ui->plainTextEditTUD->appendPlainText(str);
	str = "PAZDERA AG\nAnsprechpartner: Axel Peschek\nHinterer Floßanger 14\nD-96450 Coburg\nTel.: +49 (0)9561 516-0\nEmail: APeschek@pazdera.de\nWebseite: www.pazdera.de";
	m_ui->plainTextEditPazdera->appendPlainText(str);
	str = "Rubitherm Technologies GmbH\nAnsprechpartner: Andreas Lärz\nImhoffweg 6\n12307 Berlin\nTel.:  +49 (30) 7109622-0\nEmail: andreas.laerz@rubitherm.com\nWebseite: www.rubitherm.eu";
	m_ui->plainTextEditRubitherm->appendPlainText(str);
	str = "ADO-Metall GmbH\nAnsprechpartner: Monika Domnick\nÖlwerkstrasse 66\nD-49744 Geeste\nTel.: +49(0)5937 - 8121\nEmail: info@ado-metall.de\nWebseite: www.ado-metall.de";
	m_ui->plainTextEditADOMetall->appendPlainText(str);
}

PVTPartner::~PVTPartner() {
	delete m_ui;
}

void PVTPartner::on_pushButtonClose_clicked() {
	close();
}

