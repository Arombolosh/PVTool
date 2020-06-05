#include "PVTResultWidget.h"
#include "ui_PVTResultWidget.h"

PVTResultWidget::PVTResultWidget(QWidget *parent) :
	QWidget(parent),
	ui(new Ui::PVTResultWidget)
{
	ui->setupUi(this);
}

PVTResultWidget::~PVTResultWidget()
{
	delete ui;
}

void PVTResultWidget::setResultText(const std::vector<std::string> & resultLines) {

//	ui->plainTextEditResults->appendPlainText(line);

}

