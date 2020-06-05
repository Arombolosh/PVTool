#ifndef PVTRESULTDIALOG_H
#define PVTRESULTDIALOG_H

#include <QDialog>

namespace Ui {
class PVTResultDialog;
}

class PVTResultDialog : public QDialog
{
    Q_OBJECT

public:
	explicit PVTResultDialog(QWidget *parent = nullptr);
	~PVTResultDialog();

	void setResultText(const std::vector<std::string> & resultLines);

private slots:
	void on_pushButtonClose_clicked();

	void on_pushButtonSaveToFile_clicked();
private:
	Ui::PVTResultDialog *m_ui;
};

#endif // PVTRESULTDIALOG_H
