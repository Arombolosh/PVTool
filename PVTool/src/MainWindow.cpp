#include "MainWindow.h"

#include <QtWidgets>
#include <QDir>
#include <QDesktopServices>
#include "PVToolWidget.h"
#include "PVTAboutDialog.h"
#include "PVTLicenceDialog.h"

//! [0]
MainWindow::MainWindow() :
	ppDialog(new PVTPostProcDialog())
{
	PVToolWidget *widget = new PVToolWidget;

	readPostProcPath();

	connect(this, SIGNAL(postProcPathChanged(QString)), widget, SLOT(on_postProcPathChanged(QString)));

	setCentralWidget(widget);

	createMenus();
//! [0]

//! [1]
	//QWidget *topFiller = new QWidget;
	//topFiller->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

//    infoLabel = new QLabel(tr("<i>Choose a menu option, or right-click to "
//                              "invoke a context menu</i>"));
//    infoLabel->setFrameStyle(QFrame::StyledPanel | QFrame::Sunken);
//    infoLabel->setAlignment(Qt::AlignCenter);

//    QWidget *bottomFiller = new QWidget;
//    bottomFiller->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

//    QVBoxLayout *layout = new QVBoxLayout;
//    layout->setMargin(5);
//    layout->addWidget(topFiller);
//    layout->addWidget(infoLabel);
//    layout->addWidget(bottomFiller);
//    widget->setLayout(layout);
////! [1]

////! [2]
//    createActions();
//    createMenus();

//    QString message = tr("A context menu is available by right-clicking");
//    statusBar()->showMessage(message);

//    setWindowTitle(tr("Menus"));
//    setMinimumSize(160, 160);
	//    resize(480, 320);
}

MainWindow::~MainWindow(){
	writePostProcPath();
	delete ppDialog;
}

void MainWindow::readPostProcPath(){
	QFile file("config");

	if(!file.open(QIODevice::ReadOnly | QIODevice::Text))
		return;

	QTextStream in(&file);
	if(!in.atEnd())
		m_postProcPath = in.readLine();

	ppDialog->setText(m_postProcPath);

}

void MainWindow::writePostProcPath(){
	QFile file("config");

	if(!file.open(QIODevice::WriteOnly | QIODevice::Text))
		return;

	QTextStream out(&file);
	out << m_postProcPath;
}

void MainWindow::openAboutDialog(){
	PVTAboutDialog PVAbout;
	PVAbout.setText();
	PVAbout.setModal(true);
	PVAbout.exec();
}
void MainWindow::openLicenceDialog(){
	PVTLicenceDialog PVLic;
	PVLic.setText();
	PVLic.setModal(true);
	PVLic.exec();
}
void MainWindow::openTutorial(){
	QDesktopServices::openUrl(QUrl("../../doc/readMe.pdf"));
}

void MainWindow::openPostProcSettingsDailog(){
/* TODO Katja
	Label = PostProc2
	Textfeld
	Button ...
	*/

	//ppDialog->setModal(true);
	ppDialog->exec();
	m_postProcPath = QString::fromStdString(ppDialog->text());
	emit postProcPathChanged(m_postProcPath);
}

void MainWindow::createMenus(){
	/*
	 - A		Ebene1
	 -- A2		Ebene2
	 -- B2
	 -- C2
	*/
	QAction *aboutAct, *licenceAct, *tutorialAct, *postProcAct;

	aboutAct = new QAction("&Über", this);
	connect(aboutAct, &QAction::triggered, this, &MainWindow::openAboutDialog);
	licenceAct = new QAction("&Lizenz", this);
	connect(licenceAct, &QAction::triggered, this, &MainWindow::openLicenceDialog);
	tutorialAct = new QAction("&Tutorial", this);
	connect(tutorialAct, &QAction::triggered, this, &MainWindow::openTutorial);
	postProcAct = new QAction("&Einstellungen", this);
	connect(postProcAct, &QAction::triggered, this, &MainWindow::openPostProcSettingsDailog);


	m_fileMenu = menuBar()->addMenu("&Hilfe");
	m_fileMenu->addAction(aboutAct);
	m_fileMenu->addAction(licenceAct);
	m_fileMenu->addAction(tutorialAct);

	m_fileMenu = menuBar()->addMenu("&PostProc");
	m_fileMenu->addAction(postProcAct);

}
