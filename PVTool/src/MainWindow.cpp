#include "MainWindow.h"

#include <QtWidgets>
#include "PVToolWidget.h"
#include "PVTAboutDialog.h"
#include "PVTLicenceDialog.h"

//! [0]
MainWindow::MainWindow()
{
	PVToolWidget *widget = new PVToolWidget;

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

void MainWindow::openAboutDialog(){
	PVTAboutDialog * PVAbout = new PVTAboutDialog();
	PVAbout->setText();
	PVAbout->setModal(true);
	PVAbout->exec();
}
void MainWindow::openLicenceDialog(){
	PVTLicenceDialog * PVLic = new PVTLicenceDialog();
	PVLic->setText();
	PVLic->setModal(true);
	PVLic->exec();
}
void MainWindow::openTutorial(){
// TODO Katja hier muss dann das pdf rein
}
void MainWindow::openPostProcSettingsDailog(){
/* TODO Katja
	Label = PostProc2
	Textfeld
	Button ...
	*/
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
