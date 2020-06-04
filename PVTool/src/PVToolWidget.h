#ifndef PVTOOLWIDGET_H
#define PVTOOLWIDGET_H

#include <QWidget>

#include <IBK_Path.h>

#include <PVTool_Energy.h>

namespace Ui {
	class PVToolWidget;
}

/*! Main widget. */
class PVToolWidget : public QWidget {
	Q_OBJECT

public:
	explicit PVToolWidget(QWidget *parent = nullptr);
	~PVToolWidget();

	std::vector<PVTOOL::Energy::ManufactureData>	m_pvModule;

private slots:
	void on_radioButton_WeatherComboBox_toggled(bool checked);

	void on_radioButton_PVDatabase_toggled(bool checked);

	void on_pushButton_EPW_clicked();

	void on_comboBox_PVModule_currentIndexChanged(int index);

	void on_pushButton_RunSimu_clicked();

	void on_pushButton_Directory_clicked();

private:
	Ui::PVToolWidget *m_ui;

};

#endif // PVTOOLWIDGET_H
