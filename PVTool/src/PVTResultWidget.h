#ifndef PVTRESULTWIDGET_H
#define PVTRESULTWIDGET_H

#include <QWidget>
#include <vector>
#include <string>

namespace Ui {
class PVTResultWidget;
}

class PVTResultWidget : public QWidget {
	Q_OBJECT

public:
	explicit PVTResultWidget(QWidget *parent = nullptr);
	~PVTResultWidget();

	void setResultText(const std::vector<std::string> & resultLines);

private slots:
	void on_pushButtonClose_clicked();

	void on_pushButtonSaveToFile_clicked();

private:
	Ui::PVTResultWidget *m_ui;
};

#endif // PVTRESULTWIDGET_H
