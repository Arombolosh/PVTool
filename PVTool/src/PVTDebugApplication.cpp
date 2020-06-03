#include "PVTDebugApplication.h"

#include <IBK_Exception.h>
#include <IBK_messages.h>

bool PVTDebugApplication::notify( QObject *recv, QEvent *e ) {
	const char * const FUNC_ID = "[PVTDebugApplication::notify]";

	try {
		return QApplication::notify( recv, e );
	}
	catch (IBK::Exception &ex) {
		ex.writeMsgStackToError();
		IBK::IBK_Message("IBK::Exception caught.", IBK::MSG_ERROR, FUNC_ID);
	}
	catch (std::exception &ex) {
		IBK::IBK_Message( ex.what(), IBK::MSG_ERROR, FUNC_ID);
		IBK::IBK_Message( "IBK::Exception caught.", IBK::MSG_ERROR, FUNC_ID);
	}

	return false;
}

