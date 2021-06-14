#ifndef PVTPartner_H
#define PVTPartner_H

#include <QDialog>

namespace Ui {
class PVTPartner;
}

class PVTPartner : public QDialog
{
	Q_OBJECT

public:
	explicit PVTPartner(QWidget *parent = nullptr);
	~PVTPartner();

private slots:
	void on_pushButtonClose_clicked();

private:
	Ui::PVTPartner *m_ui;
};

#endif // PVTPartner_H
