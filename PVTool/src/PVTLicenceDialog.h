#ifndef PVTLicenceDIALOG_H
#define PVTLicenceDIALOG_H

#include <QDialog>

namespace Ui {
class PVTLicenceDialog;
}

class PVTLicenceDialog : public QDialog
{
	Q_OBJECT

public:
	explicit PVTLicenceDialog(QWidget *parent = nullptr);
	~PVTLicenceDialog();

	void setLicenceText();

private slots:
	void on_pushButtonClose_clicked();

private:
	Ui::PVTLicenceDialog *m_ui;
};

#endif // PVTLicenceDIALOG_H
