#ifndef MM_VersionH
#define MM_VersionH

#include <cmath>
#include <string>

namespace MM {

/*! Major revision number */
inline unsigned int majorNumber(){ return 0x06; }

/*! Minor revision number */
inline unsigned int minorNumber(){ return 0x01; }

/*! Formatted version number. */
inline const char * MM_LIB_VERSION() { return "6.1.1"; }

/*! Magic header for the binary isotropic delphin 6 data material file. */
inline unsigned int magicFirstBinaryMaterialDataFile(){ return 0x424D3644; }

/*! Magic header for isotropic delphin 6 data material file. */
inline unsigned int magicFirstMaterialDataFile(){ return 0x414D3644; }

/*! Secondary magic header. */
inline unsigned int magicSecondFile(){ return 0x215A4C52; }


/*! Mask for the major version number. */
inline unsigned int majorMaskBinary(){ return 0x000000ff; }

/*! Mask for the minor version number. */
inline unsigned int minorMaskBinary(){ return 0x0000ff00; }

/*! Returns the complete version number as unsigned int formated for a binary material little endian. */
inline unsigned int versionNumberBinary(){ return ( majorNumber() | (minorNumber() << 8 ) ); }


/*! Extracts major number from version id
	\param version The version number read from an material file.
*/
inline unsigned int majorFromVersionBinary( unsigned int version ){
		return ( version & majorMaskBinary() );
}


/*! Extracts major number from version id
	\param version The version number read from an material file.
*/
inline unsigned int minorFromVersionBinary( unsigned int version ){
		return (( version & minorMaskBinary() ) >> 8 );
}



// *** ASCII format crap ***


/*! mask the major version number. */
inline unsigned int majorMaskASCII() { return 0x0000ff00; }


/*! mask the minor version number. */
inline unsigned int minorMaskASCII() { return 0x000000ff; }


/*! returns the version number as ascii encoded string */
inline unsigned int versionNumberASCII() { return ( (majorNumber() << 8) | minorNumber() ); }


/*! Extracts major number from version id.
	\param version is the version number read from an material file.
*/
inline unsigned int majorFromVersionASCII( unsigned int version ) {
	return (( version & majorMaskASCII() ) >> 8 );
}


/*! Extracts minor number from version id.
	\param version is the version number read from an material file.
*/
inline unsigned int minorFromVersionASCII( unsigned int version ) {
	return ( version & minorMaskASCII() );
}

/*! Decodes a version number that is stored in ASCII format.
	\param version	takes version for encoding in stupid ASCII encoded file.
	\param first	returns first version number part from an ASCII encoded file.
	\param second	returns second version number part from an ASCII encoded file.
*/
void toASCIIEncoding( unsigned int version, unsigned int &first, unsigned int &second );

/*! Encodes a version number from major and minor versions.
	\param first	takes first version number part from an ASCII encoded file.
	\param second	takes second version number part from an ASCII encoded file.
	\returns version in ASCII Encoding format.
*/
unsigned int fromASCIIEncoding( unsigned int first, unsigned int second );


/*! Date format string. Can be used for Qt QDate class and more.
	Creates a date like this "01.05.2012" with two digit day, two digit numerical month and 4 digit year.
*/
inline std::string dateFormatStandard() { return "dd.MM.yyyy"; }

/*! Date format string. Can be used for ISO-C function strftime.
	Creates a date like this "01.05.2012" with two digit day, two digit numerical month and 4 digit year.
*/
inline std::string dateFormatStrftime() { return "%d.%m.%y"; }

} // namespace MM

/*! \file MM_Version.h
	\brief Contains version constants.
*/

#endif // MM_VersionH
