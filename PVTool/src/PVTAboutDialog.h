#ifndef PVTAboutDIALOG_H
#define PVTAboutDIALOG_H

#include <QDialog>

namespace Ui {
class PVTAboutDialog;
}

class PVTAboutDialog : public QDialog
{
	Q_OBJECT

public:
	explicit PVTAboutDialog(QWidget *parent = nullptr);
	~PVTAboutDialog();

	void setText();

private slots:
	void on_pushButtonClose_clicked();

private:
	Ui::PVTAboutDialog *m_ui;
};

#endif // PVTAboutDIALOG_H
