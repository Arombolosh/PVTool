#ifndef PVTDebugApplicationH
#define PVTDebugApplicationH

#include <QApplication>

/*! This class catches all exceptions thrown during eventloop execution.
	It basically programmed for debug purposes.
*/
class PVTDebugApplication : public QApplication {
	Q_OBJECT
public:
	/*! ctor relay. */
	PVTDebugApplication( int & argc, char ** argv ) :
		QApplication( argc, argv )
	{
	}

	/*! We just reimplement QApplication::notify() to catch all exceptions and allow setting a breakpoint here. */
	bool notify( QObject *recv, QEvent *e );

};

#endif // PVTDebugApplicationH
