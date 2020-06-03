#include "PVTButtonBar.h"

#include <QVBoxLayout>
#include <QToolButton>
#include <QIcon>
#include <QAction>

void setupToolButton(QToolButton * btn, const QString & iconFile, const QString & hint) {
	btn->setIcon(QIcon(iconFile));
	btn->setIconSize(QSize(32,32));
	btn->setAutoRaise(true);
	btn->setToolTip(hint);
}

PVTButtonBar::PVTButtonBar(QWidget * parent) :
	QWidget(parent)
{
	QVBoxLayout * lay  = new QVBoxLayout(this);

	// create tool buttons and assign resource files
	toolButtonAbout = new QToolButton(this); lay->addWidget(toolButtonAbout);

	toolButtonNew = new QToolButton(this); lay->addWidget(toolButtonNew);
	toolButtonLoad = new QToolButton(this); lay->addWidget(toolButtonLoad);
	toolButtonSave = new QToolButton(this); lay->addWidget(toolButtonSave);
	lay->addStretch(1);
	toolButtonQuit = new QToolButton(this); lay->addWidget(toolButtonQuit);

	setupToolButton(toolButtonAbout, ":/gfx/info_32x32.png", tr("Shows information about the program."));
	setupToolButton(toolButtonNew, ":/gfx/filenew_32x32.png", tr("Create a new default project (Ctrl + N)."));
	setupToolButton(toolButtonLoad, ":/gfx/fileopen_32x32.png", tr("Loads a project (Ctrl + O)."));
	setupToolButton(toolButtonSave, ":/gfx/filesave_32x32.png", tr("Saves current project (Ctrl + S)."));
	setupToolButton(toolButtonQuit, ":/gfx/exit_32x32.png", tr("Closes the program (asks for saving confirmation first)."));

	lay->setMargin(0);
	setLayout(lay);

	setFocusPolicy(Qt::StrongFocus);
}


PVTButtonBar::~PVTButtonBar() {
}


void PVTButtonBar::setEnabled(bool enabled) {
	toolButtonNew->setEnabled(enabled);
	toolButtonLoad->setEnabled(enabled);
	toolButtonSave->setEnabled(enabled);
}


