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

private:
	Ui::PVToolWidget *ui;
};

#endif // PVTOOLWIDGET_H
