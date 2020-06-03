#include "MM_Version.h"

#include <cstdlib>

namespace MM {

void toASCIIEncoding( unsigned int version, unsigned int &first, unsigned int &second ){

	union exchangeType {
		unsigned int	dword;
		char			byte[4];
	};

	// split numbers
	char versionArray[ 2*sizeof(unsigned int) ];

	// extract version numbers
	unsigned int major = majorFromVersionASCII( version );
	unsigned int minor = minorFromVersionASCII( version );

	// initialize
	first = 0;
	second = 0;

	// create hex array to base 10
	unsigned int counter = major;
	unsigned int rest = 0;
	unsigned int position = 0;
	unsigned int spacer = 3;
	while ( counter ) {

		// a mod 10 =>
		// a div a
		std::div_t divresult = std::div (counter,10);
		counter = divresult.quot;
		rest = divresult.rem;

		versionArray[ position ] = '0' + rest;
		++position;
		--spacer;

	}

	while( spacer ){
		versionArray[position] = '0';
		--spacer;
		++position;
	}


	spacer = 3;
	counter = minor;
	while ( counter ) {

		// a mod 10 =>
		// a div a
		std::div_t divresult = std::div (counter,10);
		counter = divresult.quot;
		rest = divresult.rem;

		versionArray[ position ] = '0' + rest;
		++position;
		--spacer;

	}

	while( spacer ){
		versionArray[position] = '0';
		--spacer;
		++position;
	}

	exchangeType temp;
	temp.byte[0] = 32;
	temp.byte[1] = versionArray[2];
	temp.byte[2] = versionArray[1];
	temp.byte[3] = versionArray[0];

	first = temp.dword;

	temp.byte[0] = 46;
	temp.byte[1] = versionArray[5];
	temp.byte[2] = versionArray[4];
	temp.byte[3] = versionArray[3];

	second = temp.dword;
}



unsigned int fromASCIIEncoding( unsigned int first, unsigned int second ){
	union exchangeType {
		unsigned int	dword;
		char			byte[4];
	};

	exchangeType tmp;
	tmp.dword = first;
	unsigned int major = (tmp.byte[1] - '0') * 100 + (tmp.byte[2] - '0') * 10 + (tmp.byte[3] - '0');

	tmp.dword = second;
	unsigned int minor = (tmp.byte[1] - '0') * 100 + (tmp.byte[2] - '0') * 10 + (tmp.byte[3] - '0');

	return ((major << 8) | minor);
}


} // namespace MM

/*! \file MM_Version.cpp
	\brief Implementation for version encoding/decoding functions.
*/

