#ifndef PVTOOLWIDGET_H
#define PVTOOLWIDGET_H

#include <QWidget>

namespace Ui {
class PVToolWidget;
}

/*! Main widget. */
class PVToolWidget : public QWidget {
	Q_OBJECT

public:
	explicit PVToolWidget(QWidget *parent = nullptr);
	~PVToolWidget();

private slots:
	void on_radioButton_WeatherComboBox_toggled(bool checked);

	void on_radioButton_PVDatabase_toggled(bool checked);

private:
	Ui::PVToolWidget *ui;
};

#endif // PVTOOLWIDGET_H
