#ifndef PVTButtonBarH
#define PVTButtonBarH

#include <QWidget>

class QToolButton;
class QMenu;

/*!	\brief Declaration for class ButtonBar

	The button bar is the the container of the master control buttons to
	the right of the main window.
*/
class PVTButtonBar : public QWidget {
	Q_OBJECT
public:
	/*! Default constructor. */
	PVTButtonBar(QWidget * parent);
	/*! Default destructor. */
	~PVTButtonBar();

	/*! Enables/disables certain buttons during simulation. */
	void setEnabled(bool enabled);

	// *** PUBLIC MEMBER VARIABLES ***
	QToolButton  * toolButtonAbout;
	QToolButton  * toolButtonNew;
	QToolButton  * toolButtonSave;
	QToolButton  * toolButtonLoad;

	QToolButton  * toolButtonQuit;
};

#endif // ButtonBarH
