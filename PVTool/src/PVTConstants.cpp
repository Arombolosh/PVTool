#include "PVTConstants.h"

const char * const VERSION = "0.1";

const char * const LONG_VERSION = "0.1.0";

const char * const PROGRAM_NAME = "PVTool 0.1";

const char * const ORG_NAME				= "IBK";
const char * const DOT_FILE_EXTENSION	= ".pv";

#if defined(Q_OS_MAC) // Q_OS_UNIX

const int TABLE_FONT_SIZE = 10;
const char * const FIXED_FONT_FAMILY = "Monaco";

#elif defined(Q_OS_UNIX)

const int TABLE_FONT_SIZE = 10;
const char * const FIXED_FONT_FAMILY = "Monospace";

#else

const int TABLE_FONT_SIZE = 10;
const char * const FIXED_FONT_FAMILY = "Courier New";

#endif
