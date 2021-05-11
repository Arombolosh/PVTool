#ifndef PVTPostProcDIALOG_H
#define PVTPostProcDIALOG_H

#include <QDialog>

namespace Ui {
class PVTPostProcDialog;
}

class PVTPostProcDialog : public QDialog
{
	Q_OBJECT

public:
	explicit PVTPostProcDialog(QWidget *parent = nullptr);
	~PVTPostProcDialog();

	void setText();

	std::string text();

private slots:


	void on_pushButtonDirectory_clicked();

private:
	Ui::PVTPostProcDialog *m_ui;
};

#endif // PVTPostProcDIALOG_H
