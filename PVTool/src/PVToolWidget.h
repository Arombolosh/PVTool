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

protected:

	void closeEvent(QCloseEvent *event) override;

private slots:
	void on_radioButton_WeatherComboBox_toggled(bool checked);

	void on_radioButton_PVDatabase_toggled(bool checked);

	void on_pushButton_EPW_clicked();

	void on_comboBox_PVModule_currentIndexChanged(int index);

	void on_pushButton_RunSimu_clicked();

	void on_pushButton_Directory_clicked();

private:

	/*! Modifies the m6template file content with the given parameters and stores the new file in place of given
		target file name.
		\param m6template String containing template m6 file - will be modified in function, since we only need it once per simulation
	*/
	void createM6File(std::string & m6Template, const IBK::Path &targetFileName, double rho, double ce, double lambda) const;

	Ui::PVToolWidget *m_ui;

};

#endif // PVTOOLWIDGET_H
