#include "PVToolWidget.h"
#include "ui_PVToolWidget.h"

PVToolWidget::PVToolWidget(QWidget *parent) :
	QWidget(parent),
	ui(new Ui::PVToolWidget)
{
	ui->setupUi(this);
}

PVToolWidget::~PVToolWidget()
{
	delete ui;
}
