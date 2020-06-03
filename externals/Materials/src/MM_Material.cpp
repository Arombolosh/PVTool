#include "MM_Material.h"

#include <cstdlib>
#include <numeric>
#include <cmath>
#include <memory>
#include <cstring>
#include <iterator>
#include <sstream>
#include <list>

#include <IBK_FileUtils.h>
#include <IBK_InputOutput.h>
#include <IBK_FormatString.h>
#include <IBK_Exception.h>
#include <IBK_StopWatch.h>
#include <IBK_NotificationHandler.h>
#include <IBK_MultiLanguageString.h>
#include <IBK_physics.h>
#include <IBK_StopWatch.h>
#include <IBK_FileReader.h>

#include "MM_Constants.h"
#include "MM_KeywordList.h"
#include "MM_Types.h"
#include "MM_Utils.h"

#if _WIN32
  #include <windows.h>
#endif

#include "MM_Version.h"


namespace MM {

bool Material::m_useStrictConsistencyChecking = false;

void Material::clear() {

	m_filename.clear();

	m_type = NUM_MaterialFormat;
	m_id = 0;
	m_used = false;
	m_isBinary = false;

	m_identification.clear();

	IBK::clear(m_paraStorage, m_paraStorage + NUM_ParaStorage);
	IBK::clear(m_paraTransport, m_paraTransport + NUM_ParaTransport);
	IBK::clear(m_paraMechanical, m_paraMechanical + NUM_ParaMechanical);

	m_moistureStorage.clear();

	IBK::clear(m_moistureTransport, m_moistureTransport + NUM_MaterialDirection);
	IBK::clear(m_thermalTransport, m_thermalTransport + NUM_MaterialDirection);
	IBK::clear(m_airTransport, m_airTransport + NUM_MaterialDirection);

	m_measuredData.clear();

	m_majorFileVersion = 0;
	m_minorFileVersion = 0;
	m_capabilities = 0;
}
// ----------------------------------------------------------------------------

void Material::readOld(const IBK::Path & fname) {
	const char * const FUNC_ID = "[Material::read]";

//	IBK::StopWatch stopWatch1;
//	std::ofstream timeOut("D:/Programming/Material/MaterialView/trunk/readTimes.txt", std::ios_base::app);
//	stopWatch1.start();
//	timeOut << "Start time measurement in: " << fname << "\n";
	if (!fname.isFile()) {
		throw IBK::Exception(IBK::FormatString("Cannot open file: '%1'").arg(fname), FUNC_ID);
	}


	// extract filename
	IBK::IBK_Message(IBK::FormatString("Reading material file '%1'.\n").arg(fname.filename()), IBK::MSG_PROGRESS, FUNC_ID, IBK::VL_STANDARD);
	IBK::MessageIndentor indent; (void)indent;

	// Empty data in object
	clear();

	// open file in binary mode and determine file size
	std::ifstream in;
	in.rdbuf()->pubsetbuf(0, 0); // switch off buffering in order to improve performance

#if defined(_WIN32)
	#if defined(_MSC_VER)
		std::wstring filenameClean = fname.wstr();
	#else
		std::string filenameClean = IBK::WstringToANSI(fname.wstr(), false);
	#endif
#else // _WIN32
		std::string filenameClean = fname.str();
#endif
	in.open(filenameClean.c_str(), std::ios_base::binary);
	if (!in) {
		throw IBK::Exception(IBK::FormatString("Cannot open file: '%1'").arg(fname), FUNC_ID);
	}
	// since we opened it we save the filename
	m_filename = fname;

	// extract ID number of filename
	m_id = IBK::extract_ID_from_filename(fname);
	if ( m_id == 0 ) {
		// only warn about malformed filename for V6 and newer materials
		if (fname.extension() != "mat") {
			IBK::IBK_Message( IBK::FormatString("Invalid format for material file, '<name>_xxx.m6' expected, "
				"with xxx as ID number. Filename was '%1'.").arg(m_filename), IBK::MSG_WARNING, FUNC_ID, 2);
		}
	}

	// read magic header and determine file format and version
	uint32_t test = 0;

	try {
		// the magic header is always 4 integers wide

		// read first unsigned int from stream
		IBK::read_uint32_binary( in, test);

		// check if this is a binary data file (DELPHIN 6 and later)
		if ( test == magicFirstBinaryMaterialDataFile()) {

			// stream is positioned right after magic header, so we are ready to call
			// the binary read function, and store our format as binary read in.
			readBinary(in);

			m_isBinary = true;

		}

		// must be an ASCII data file

		bool preV6Format = ( test != magicFirstMaterialDataFile() );
		if (!preV6Format) {
			// if reading in D6 format, continue evaluating magic numbers and file version
			unsigned int test = 0;
			// test magic header
			IBK::read_uint32_binary( in, test);
			if ( test != magicSecondFile() ){
				throw IBK::Exception("Wrong second magic number in V6 - ASCII material file!", FUNC_ID);
			}

			// read file version and split it
			IBK::read_uint32_binary( in, test); // 3rd uint
			unsigned int second;
			IBK::read_uint32_binary( in, second); // 4th uint
			unsigned int version = fromASCIIEncoding( test, second );

			// extract file version
			m_majorFileVersion = majorFromVersionASCII( version );
			m_minorFileVersion = minorFromVersionASCII( version );
		}
		else {
			// D5 format, set file version manually
			m_majorFileVersion = 5;
			m_minorFileVersion = 6;
		}

		// the stream had been opened in binary mode, we close the file and
		// reopen the stream again in ASCII mode

//		timeOut << "Version tests: " << stopWatch1.difference() << "\n";
//		stopWatch1.start();

		in.close();
		const int BufferSize = 2048;
		char readBuffer[BufferSize];
		in.rdbuf()->pubsetbuf(readBuffer, BufferSize); // switch off buffering in order to improve performance
#ifdef _MSC_VER
		in.open(m_filename.wstr().c_str()); // open in ASCII mode
#else
		in.open(m_filename.str().c_str()); // open in ASCII mode
#endif
		std::string line;
		getline(in, line);
		// in Version 6 format, simply skip the first line, which must contain the magic header
		// for older version test first line for [MATERIAL] keyword
		if (!in) {
			// didn't find material section
			throw IBK::Exception("Error opening material file (again) in ASCII mode.", FUNC_ID);
		}
		if (preV6Format) {
			// skip empty lines or comments
			unsigned int commentsOrEmpty = 0;
			IBK::trim(line);
			while( line.empty() || line[0] == ';') {
				getline(in, line);
				IBK::trim(line);
				++commentsOrEmpty;
			}
			// if [MATERIAL] keyword not found reopen the file in order to get the first line again
			if( line.find("[MATERIAL]") == std::string::npos) {
				in.close();
#ifdef _MSC_VER
				in.open(m_filename.wstr().c_str()); // open in ASCII mode
#else
				in.open(m_filename.str().c_str()); // open in ASCII mode
#endif
				// skip comments or empty lines
				for( unsigned int i=0; i<commentsOrEmpty; ++i)
					getline(in, line);
			}
		}

//		timeOut << "Read from file: " << stopWatch1.difference() << "\n";
//		stopWatch1.start();

		// ASCII stream is positioned after [MATERIAL] section header om case of V5 format, or in case of V6 format,
		// after the line with the magic header.
		// In either case we can call readASCII.
		readASCII(in, preV6Format);

//		timeOut << "Read material: " << stopWatch1.difference() << "\n";
//		stopWatch1.start();

	}
	catch (IBK::Exception & ex) {
		throw IBK::Exception(ex, IBK::FormatString("Error reading material from file '%1'.").arg(m_filename), FUNC_ID);
	}
}
// ----------------------------------------------------------------------------

void Material::read(const IBK::Path & fname) {
	const char * const FUNC_ID = "[Material::read]";

//	IBK::StopWatch stopWatch1;
//	std::ofstream timeOut("D:/Programming/Material/MaterialView/trunk/readTimes.txt", std::ios_base::app);
//	stopWatch1.start();
//	timeOut << "Start time measurement in: " << fname << "\n";


	// extract filename
	IBK::IBK_Message(IBK::FormatString("Reading material file '%1'.\n").arg(fname.filename()), IBK::MSG_PROGRESS, FUNC_ID, IBK::VL_STANDARD);
	IBK::MessageIndentor indent; (void)indent;

	// Empty data in object
	clear();

	// open file in binary mode and determine file size
	IBK::FileReader reader(fname, 4096);
	if (!reader.valid()) {
		throw IBK::Exception(IBK::FormatString("Cannot open file: '%1'").arg(fname), FUNC_ID);
	}
	// since we opened it we save the filename
	m_filename = fname;

	// extract ID number of filename
	m_id = IBK::extract_ID_from_filename(fname);
	if ( m_id == 0 ) {
		// only warn about malformed filename for V6 and newer materials
		if ( fname.extension() != "mat") {
			IBK::IBK_Message( IBK::FormatString("Invalid format for material file, '<name>_xxx.m6' expected, "
				"with xxx as ID number. Filename was '%1'.").arg(m_filename), IBK::MSG_WARNING, FUNC_ID, 2);
		}
	}

	// read magic header and determine file format and version
	uint32_t test = 0;

	try {
		// the magic header is always 4 integers wide
		std::vector<unsigned char> firstbytes = reader.firstBytes(4*sizeof(uint32_t));

		// read first unsigned int from stream
		unsigned int binaryPos = IBK::bytes2value(firstbytes, test, 0);

		// check if this is a binary data file (DELPHIN 6 and later)
		if ( test == magicFirstBinaryMaterialDataFile()) {
			throw IBK::Exception(IBK::FormatString("Cannot open file: '%1'").arg(fname), FUNC_ID);
		}

		// must be an ASCII data file

		bool preV6Format = ( test != magicFirstMaterialDataFile() );
		if (!preV6Format) {
			// if reading in D6 format, continue evaluating magic numbers and file version
			unsigned int test = 0;
			// test magic header
			binaryPos = IBK::bytes2value(firstbytes, test, binaryPos);
			if ( test != magicSecondFile() ){
				throw IBK::Exception("Wrong second magic number in V6 - ASCII material file!", FUNC_ID);
			}

			// read file version and split it
			binaryPos = IBK::bytes2value(firstbytes, test, binaryPos); // 3rd uint
			unsigned int second;
			binaryPos = IBK::bytes2value(firstbytes, second, binaryPos); // 4th uint
			unsigned int version = fromASCIIEncoding( test, second );

			// extract file version
			m_majorFileVersion = majorFromVersionASCII( version );
			m_minorFileVersion = minorFromVersionASCII( version );
		}
		else {
			// D5 format, set file version manually
			m_majorFileVersion = 5;
			m_minorFileVersion = 6;
			binaryPos = 0;
		}

		// the stream had been opened in binary mode, we close the file and
		// reopen the stream again in ASCII mode

//		timeOut << "Version tests: " << stopWatch1.difference() << "\n";
//		stopWatch1.start();

		// in Version 6 format, simply skip the first line, which must contain the magic header
		// for older version test first line for [MATERIAL] keyword
		try {
			reader.readFirst(0, binaryPos);
		}
		catch(IBK::Exception& ex) {
			// didn't find material section
			throw IBK::Exception( ex, IBK::FormatString("Error opening material file (again) in ASCII mode."), FUNC_ID);
		}
		std::vector<std::string> lines = reader.lines();
		if (preV6Format) {
			// skip empty lines or comments
			unsigned int commentsOrEmpty = 0;
			unsigned int firstIndex = 0;
			IBK::trim(lines[firstIndex]);
			while( firstIndex < lines.size() && (lines[firstIndex].empty() || lines[firstIndex][0] == ';')) {
				++firstIndex;
				IBK::trim(lines[firstIndex]);
				++commentsOrEmpty;
			}
			if (firstIndex >= lines.size())
				throw IBK::Exception("Bad header in material file.", FUNC_ID);
			// if [MATERIAL] keyword found skip first line
			if (lines[firstIndex].find("[MATERIAL]") != std::string::npos) {
				++firstIndex;
			}
			lines.erase(lines.begin(), lines.begin()+firstIndex);
		}

//		timeOut << "Read from file: " << stopWatch1.difference() << "\n";
//		stopWatch1.start();

		// ASCII stream is positioned after [MATERIAL] section header om case of V5 format, or in case of V6 format,
		// after the line with the magic header.
		// In either case we can call readASCII.
		readASCII(lines, preV6Format);

//		timeOut << "Read material: " << stopWatch1.difference() << "\n";
//		stopWatch1.start();

	}
	catch (IBK::Exception & ex) {
		throw IBK::Exception(ex, IBK::FormatString("Error reading material from file '%1'.").arg(m_filename), FUNC_ID);
	}
}
// ----------------------------------------------------------------------------


void Material::write(const IBK::Path & fname, bool binary, bool writeEmptyKeywords) const {
	const char * const FUNC_ID = "[Material::write]";

	if (fname.str().empty()) {
		throw IBK::Exception("No filename given.", FUNC_ID);
	}

	// substitute extension if not specified
	IBK::Path fname_wo_ext = fname.withoutExtension();

	// create new file
	if (binary) {
		fname_wo_ext.addExtension("m6b");
	}
	else {
		fname_wo_ext.addExtension("m6");
	}

	if (fname_wo_ext != fname) {
		// Write message to user about changed file extension
		IBK::IBK_Message(IBK::FormatString("Renaming material file name '%1' to '%2'.\n")
			.arg(m_filename).arg(fname_wo_ext), IBK::MSG_PROGRESS, FUNC_ID, 2);
	}
	// store filename
	m_filename = fname_wo_ext;

	// create output file stream
#if defined(_WIN32)
	#if defined(_MSC_VER)
		std::ofstream out(m_filename.wstr().c_str(), std::ios_base::binary);
	#else
		std::string filenameAnsi = IBK::WstringToANSI(m_filename.wstr(), false);
		std::ofstream out(filenameAnsi.c_str(), std::ios_base::binary);
	#endif
#else
	std::ofstream out(m_filename.c_str(), std::ios_base::binary);
#endif

	// check if successful
	if (!out)
		throw IBK::Exception( IBK::FormatString("Couldn't write material file '%1'.").arg(m_filename), FUNC_ID);

	// write magic header

	// different handling based on binary or ASCII settings
	if (binary) {

		IBK::write_uint32_binary( out, magicFirstBinaryMaterialDataFile() );
		IBK::write_uint32_binary( out, magicSecondFile() );
		IBK::write_uint32_binary( out, versionNumberBinary() );
		IBK::write_uint32_binary( out, 0 );

		writeBinary(out);
	}
	else {

		IBK::write_uint32_binary( out, magicFirstMaterialDataFile() );
		IBK::write_uint32_binary( out, magicSecondFile() );
		unsigned int first, second;
		first = 0; second = 0; // this done to prevent windows environment from complaining
		toASCIIEncoding( versionNumberASCII(), first, second );
		IBK::write_uint32_binary( out, first );
		IBK::write_uint32_binary( out, second );
		out << std::endl;

		writeASCII(out, 0, writeEmptyKeywords);
	}
}
// ----------------------------------------------------------------------------


void Material::fromString(const std::string & data, bool preV6) {
	std::stringstream strm(data);
	return readASCII(strm, preV6);
}
// ----------------------------------------------------------------------------


std::string Material::toString(unsigned int indent, bool writeEmptyKeywords) const {
	std::stringstream strm;
	writeASCII(strm, indent, writeEmptyKeywords);
	return strm.str();
}
// ----------------------------------------------------------------------------


void Material::readXML( const IBK::Path & fname ){
	read(fname);
}
// ----------------------------------------------------------------------------


void Material::writeXML(const IBK::Path & fname) const {
	write(fname);
	// set new filename
	m_filename = fname;
}
// ----------------------------------------------------------------------------


void Material::prepareCalculation() {
	const char * const FUNC_ID = "[Material::prepareCalculation]";

	IBK::IBK_Message( IBK::FormatString("Preparing material '%1' ID: %2 for calculation.\n")
					  .arg(m_identification.m_name.string()).arg(m_id), IBK::MSG_PROGRESS, FUNC_ID, 2);
	IBK::MessageIndentor msgIndent; (void)msgIndent;

	m_thermalStorage.prepareCalculation(this);
	m_moistureStorage.prepareCalculation(this);
	// check for O80, calculate if missing, but only if moisture storage capability is given
	if (m_paraStorage[Material::MP_THETA_80].empty() && m_moistureStorage.m_type != MM::MoistureStorage::NUM_MoistureStorageType) {
		double O80 = 0.0;
		if (m_identification.m_flags & MM::MaterialIdentification::AIR) {
			// vapor pressure and vapor density
			double pv = IBK::f_psat(293.15) * 0.8;
			O80 = pv / (IBK::R_VAPOR * 293.15) / IBK::RHO_W;
		}
		else {
			double pl80 = IBK::f_log(0.8) * IBK::RHO_W * IBK::R_VAPOR * 293.15;
			O80 = m_moistureStorage.Thetal_pl(pl80, 293.15);
		}
		std::string errmsg;
		m_paraStorage[Material::MP_THETA_80].set( KeywordList::Keyword("Material::ParaStorage", Material::MP_THETA_80), O80, IBK::Unit("m3/m3"), &errmsg );
	}

	try {

		m_airTransport[U_DIRECTION].prepareCalculation(this, U_DIRECTION);
		m_thermalTransport[U_DIRECTION].prepareCalculation(this, U_DIRECTION);
		m_moistureTransport[U_DIRECTION].prepareCalculation(this, U_DIRECTION);

		if (m_type == MF_2D) {
			m_airTransport[V_DIRECTION].prepareCalculation(this, V_DIRECTION);
			m_thermalTransport[V_DIRECTION].prepareCalculation(this, V_DIRECTION);
			m_moistureTransport[V_DIRECTION].prepareCalculation(this, V_DIRECTION);
		}

		if (m_type == MF_3D) {
			m_airTransport[V_DIRECTION].prepareCalculation(this, V_DIRECTION);
			m_thermalTransport[V_DIRECTION].prepareCalculation(this, V_DIRECTION);
			m_moistureTransport[V_DIRECTION].prepareCalculation(this, V_DIRECTION);
			m_airTransport[W_DIRECTION].prepareCalculation(this, W_DIRECTION);
			m_thermalTransport[W_DIRECTION].prepareCalculation(this, W_DIRECTION);
			m_moistureTransport[W_DIRECTION].prepareCalculation(this, W_DIRECTION);
		}
	}
	catch (IBK::Exception & ex) {
		throw IBK::Exception(ex, IBK::FormatString("Error initializing material '%1'.").arg(m_identification.m_name.string()), FUNC_ID);
	}

	// compute and cache capabilities - only needs to be updated whenever prepare calculation is called
	m_capabilities = getCapabilities();
}
// ----------------------------------------------------------------------------

int Material::capabilities() const {
	return m_capabilities;
}

int Material::getCapabilities() const {
//	const char * FUNC_ID = "[Material::getCapabilities]";


	int capabilities = TT_Thermal;

	// check if transport parameters are given as well, if not, disable flag again
	switch (m_type) {
		case MF_3D :
			if (m_thermalTransport[W_DIRECTION].m_type == ThermalTransport::NUM_ThermalTransportType)
				capabilities &= ~TT_Thermal;
			// fall through
		case MF_2D :
			if (m_thermalTransport[V_DIRECTION].m_type == ThermalTransport::NUM_ThermalTransportType)
				capabilities &= ~TT_Thermal;
			// fall through
		case MF_1D :
			if (m_thermalTransport[U_DIRECTION].m_type == ThermalTransport::NUM_ThermalTransportType)
				capabilities &= ~TT_Thermal;
			break;
		case NUM_MaterialFormat:;
	}



	// check for air transport capability
	if (!(m_identification.m_flags & MaterialIdentification::AIR_TIGHT)) {
		capabilities |= TT_Airflow;
		// check if transport parameters are given as well, if not, disable flag again
		switch (m_type) {
			case MF_3D :
				if (m_airTransport[W_DIRECTION].m_type == AirTransport::NUM_AirTransportType)
					capabilities &= ~TT_Airflow;
				// fall through
			case MF_2D :
				if (m_airTransport[V_DIRECTION].m_type == AirTransport::NUM_AirTransportType)
					capabilities &= ~TT_Airflow;
				// fall through
			case MF_1D :
				if (m_airTransport[U_DIRECTION].m_type == AirTransport::NUM_AirTransportType)
					capabilities &= ~TT_Airflow;
				break;
			case NUM_MaterialFormat:;
		}
	}

	// check for liquid and vapor transport capability
	if (m_moistureStorage.m_type != MoistureStorage::NUM_MoistureStorageType || (m_identification.m_flags & MaterialIdentification::AIR)) {
		// check for liquid transport capability
		if (!(m_identification.m_flags & MaterialIdentification::WATER_TIGHT) && !(m_identification.m_flags & MaterialIdentification::AIR)) {
			capabilities |= TT_Liquid;
			// check if transport parameters are given as well, if not, disable flag again
			switch (m_type) {
				case MF_3D :
					if (m_moistureTransport[W_DIRECTION].m_liquidTransportType == MoistureTransport::NUM_LiquidTransportType)
						capabilities &= ~TT_Liquid;
					// fall through
				case MF_2D :
					if (m_moistureTransport[V_DIRECTION].m_liquidTransportType == MoistureTransport::NUM_LiquidTransportType)
						capabilities &= ~TT_Liquid;
					// fall through
				case MF_1D :
					if (m_moistureTransport[U_DIRECTION].m_liquidTransportType == MoistureTransport::NUM_LiquidTransportType)
						capabilities &= ~TT_Liquid;
					break;
				case NUM_MaterialFormat:;
			}
		}
		// check for vapor transport capability
		if (!(m_identification.m_flags & MaterialIdentification::VAPOR_TIGHT)) {
			capabilities |= TT_Vapour;
			// check if transport parameters are given as well, if not, disable flag again
			switch (m_type) {
				case MF_3D :
					if (m_moistureTransport[W_DIRECTION].m_vaporTransportType == MoistureTransport::NUM_VaporTransportType)
						capabilities &= ~TT_Vapour;
					// fall through
				case MF_2D :
					if (m_moistureTransport[V_DIRECTION].m_vaporTransportType == MoistureTransport::NUM_VaporTransportType)
						capabilities &= ~TT_Vapour;
					// fall through
				case MF_1D :
					if (m_moistureTransport[U_DIRECTION].m_vaporTransportType == MoistureTransport::NUM_VaporTransportType)
						capabilities &= ~TT_Vapour;
					break;
				default:;
			}
		}
	}
	return capabilities;
}
// ----------------------------------------------------------------------------


// *** PRIVATE MEMBER FUNCTION IMPLEMENTATIONS ***

void Material::readASCII(std::istream& in, bool preV6) {
	const char * const FUNC_ID = "[Material::readASCII]";
	// compose vector with section titles
	std::vector<std::string> sectionTitles, sections;

	if (preV6) {
		sectionTitles.resize(NUM_SectionsPreV6);
		for (unsigned int i=0; i<NUM_SectionsPreV6; ++i) {
			sectionTitles[i] = KeywordList::Keyword("Material::SectionsPreV6", i);
		}
	}
	else {
		sectionTitles.resize(NUM_Sections);
		for (unsigned int i=0; i<NUM_Sections; ++i) {
			sectionTitles[i] = KeywordList::Keyword("Material::Sections", i);
		}
	}

	try {
		///  \todo ghorwin: write 100 times I will never ever again call function which throw exceptions outside of try catch blocks!!!
		IBK::explode_sections(in, sectionTitles, sections, ";#");

		// preV6 format
		if (preV6) {
			if( !sections[GENERAL].empty()) {
				sections[PARAMETER] = sections[GENERAL];
			}
			// substitute relevant sections
			std::vector<std::string> v6sections(NUM_Sections);

			v6sections[IDENTIFICATION]				= sections[ID];
			v6sections[STORAGE_BASE_PARAMETERS]		= sections[PARAMETER];
			v6sections[MOISTURE_STORAGE]			= sections[MOISTSTORAGE];
			v6sections[MOISTURE_TRANSPORT]			= sections[MOISTTRANS];
			v6sections[HEAT_TRANSPORT]				= sections[HEATTRANS];
			v6sections[AIR_TRANSPORT]				= sections[AIRTRANS];
			v6sections[THERMAL_STORAGE]				= sections[PCMMATERIAL];

			std::swap(sections, v6sections);
		}

		// start parsing
		if (preV6) {
			m_identification.readPreV6( sections[IDENTIFICATION]);
		}
		else {
			m_identification.read( sections[IDENTIFICATION]);
		}

		// compose common parameter string
		std::string parameters = sections[TRANSPORT_BASE_PARAMETERS] +
								 sections[STORAGE_BASE_PARAMETERS] +
								 sections[MECHANICAL_BASE_PARAMETERS];
		readASCIIParameters	( parameters );

		// read storage parameters
		m_thermalStorage.read( sections[THERMAL_STORAGE] );
		m_moistureStorage.read( sections[MOISTURE_STORAGE]);

		// assume isotropic (1D) material and read corresponding transport sections
		m_type = MF_1D;
		m_moistureTransport[U_DIRECTION].read( sections[MOISTURE_TRANSPORT]);

		m_thermalTransport[U_DIRECTION].read( sections[HEAT_TRANSPORT]);

		m_airTransport[U_DIRECTION].read( sections[AIR_TRANSPORT]);

		// if we have V_DIRECTION data, we have a 2D format
		if (!sections[MOISTURE_TRANSPORT_V].empty() ||
			!sections[HEAT_TRANSPORT_V].empty() ||
			!sections[AIR_TRANSPORT_V].empty())
		{
			m_type = MF_2D;
			m_moistureTransport[V_DIRECTION].read( sections[MOISTURE_TRANSPORT_V]);
			m_thermalTransport[V_DIRECTION].read( sections[HEAT_TRANSPORT_V]);
			m_airTransport[V_DIRECTION].read( sections[AIR_TRANSPORT_V]);
		}


		// if we also have W_DIRECTION data, we have a 3D format
		if (!sections[MOISTURE_TRANSPORT_W].empty() ||
			!sections[HEAT_TRANSPORT_W].empty() ||
			!sections[AIR_TRANSPORT_W].empty())
		{
			// ensure that we also have V direction data, so MF_2D must be set
			if (m_type != MF_2D)
				throw IBK::Exception("Found W-direction parameters but no V-direction parameters "
									 "are given. Invalid anisotropic material format!", FUNC_ID);
			m_type = MF_3D;
			m_moistureTransport[W_DIRECTION].read( sections[MOISTURE_TRANSPORT_W]);
			m_thermalTransport[W_DIRECTION].read( sections[HEAT_TRANSPORT_W]);
			m_airTransport[W_DIRECTION].read( sections[AIR_TRANSPORT_W]);
		}

		// read vapor resistance parameters
		m_vaporContactResistance.read( sections[VAPOR_CONTACT_RESISTANCE] );

		// finally read measured data
		m_measuredData.read( sections[MEASURED_DATA]);

		// read complete, prepare for calculation
		prepareCalculation();
	}
	catch (IBK::Exception & ex) {
		if (m_filename.str().empty())
			throw IBK::Exception(ex, IBK::FormatString("Error reading material '%1'.").arg(m_identification.m_name.string("en", true)), FUNC_ID);
		else
			throw IBK::Exception(ex, IBK::FormatString("Error reading material file '%1'.").arg(m_filename), FUNC_ID);
	}
}
// ----------------------------------------------------------------------------

void Material::readASCII(const std::vector<std::string>& data, bool preV6) {
	const char * const FUNC_ID = "[Material::readASCII]";
	// compose vector with section titles
	std::vector<std::string> sectionTitles;
	std::vector<std::vector<std::string> > sections;

	if (preV6) {
		sectionTitles.resize(NUM_SectionsPreV6);
		for (unsigned int i=0; i<NUM_SectionsPreV6; ++i) {
			sectionTitles[i] = KeywordList::Keyword("Material::SectionsPreV6", i);
		}
	}
	else {
		sectionTitles.resize(NUM_Sections);
		for (unsigned int i=0; i<NUM_Sections; ++i) {
			sectionTitles[i] = KeywordList::Keyword("Material::Sections", i);
		}
	}

	try {
		///  \todo ghorwin: write 100 times I will never ever again call function which throw exceptions outside of try catch blocks!!!
		IBK::explode_sections(data, sectionTitles, sections, ";#");

		// preV6 format
		if (preV6) {
			if( !sections[GENERAL].empty()) {
				sections[PARAMETER] = sections[GENERAL];
			}
			// substitute relevant sections
			std::vector<std::vector<std::string> > v6sections(NUM_Sections);

			v6sections[IDENTIFICATION]				= sections[ID];
			v6sections[STORAGE_BASE_PARAMETERS]		= sections[PARAMETER];
			v6sections[MOISTURE_STORAGE]			= sections[MOISTSTORAGE];
			v6sections[MOISTURE_TRANSPORT]			= sections[MOISTTRANS];
			v6sections[HEAT_TRANSPORT]				= sections[HEATTRANS];
			v6sections[AIR_TRANSPORT]				= sections[AIRTRANS];
			v6sections[THERMAL_STORAGE]				= sections[PCMMATERIAL];

			sections.swap(v6sections);
		}

		// start parsing
		if (preV6) {
			m_identification.readPreV6( sections[IDENTIFICATION]);
		}
		else {
			m_identification.read( sections[IDENTIFICATION]);
		}

		// compose common parameter string
		std::vector<std::string> parameters = sections[TRANSPORT_BASE_PARAMETERS];
		parameters.insert(parameters.end(), sections[STORAGE_BASE_PARAMETERS].begin(),
											sections[STORAGE_BASE_PARAMETERS].end());
		parameters.insert(parameters.end(), sections[MECHANICAL_BASE_PARAMETERS].begin(),
											sections[MECHANICAL_BASE_PARAMETERS].end());
		readASCIIParameters	( parameters );

		// read storage parameters
		m_thermalStorage.read( sections[THERMAL_STORAGE]);
		m_moistureStorage.read( sections[MOISTURE_STORAGE]);

		// assume isotropic (1D) material and read corresponding transport sections
		m_type = MF_1D;
		m_moistureTransport[U_DIRECTION].read( sections[MOISTURE_TRANSPORT]);

		m_thermalTransport[U_DIRECTION].read( sections[HEAT_TRANSPORT]);

		m_airTransport[U_DIRECTION].read( sections[AIR_TRANSPORT]);

		// if we have V_DIRECTION data, we have a 2D format
		if (!sections[MOISTURE_TRANSPORT_V].empty() ||
			!sections[HEAT_TRANSPORT_V].empty() ||
			!sections[AIR_TRANSPORT_V].empty())
		{
			m_type = MF_2D;
			m_moistureTransport[V_DIRECTION].read( sections[MOISTURE_TRANSPORT_V]);
			m_thermalTransport[V_DIRECTION].read( sections[HEAT_TRANSPORT_V]);
			m_airTransport[V_DIRECTION].read( sections[AIR_TRANSPORT_V]);
		}


		// if we also have W_DIRECTION data, we have a 3D format
		if (!sections[MOISTURE_TRANSPORT_W].empty() ||
			!sections[HEAT_TRANSPORT_W].empty() ||
			!sections[AIR_TRANSPORT_W].empty())
		{
			// ensure that we also have V direction data, so MF_2D must be set
			if (m_type != MF_2D)
				throw IBK::Exception("Found W-direction parameters but no V-direction parameters "
									 "are given. Invalid anisotropic material format!", FUNC_ID);
			m_type = MF_3D;
			m_moistureTransport[W_DIRECTION].read( sections[MOISTURE_TRANSPORT_W]);
			m_thermalTransport[W_DIRECTION].read( sections[HEAT_TRANSPORT_W]);
			m_airTransport[W_DIRECTION].read( sections[AIR_TRANSPORT_W]);
		}

		// read vapor resistance parameters
		m_vaporContactResistance.read( sections[VAPOR_CONTACT_RESISTANCE] );

		// finally read measured data
		m_measuredData.read( sections[MEASURED_DATA]);


		// read complete, prepare for calculation
		prepareCalculation();
	}
	catch (IBK::Exception & ex) {
		throw IBK::Exception(ex, IBK::FormatString("Error reading material file '%1'.").arg(m_filename), FUNC_ID);
	}
}
// ----------------------------------------------------------------------------


void Material::writeASCII(std::ostream & out, unsigned int indent, bool writeEmptyKeywords) const {
	m_identification.write(out, indent, writeEmptyKeywords);

	writeASCIIParameters(out, indent, writeEmptyKeywords);

	m_thermalStorage.write(out, indent, NUM_MaterialDirection, writeEmptyKeywords);
	m_moistureStorage.write(out, indent, NUM_MaterialDirection, writeEmptyKeywords);

	m_moistureTransport[U_DIRECTION].write(out, indent, U_DIRECTION, writeEmptyKeywords);
	m_thermalTransport[U_DIRECTION].write(out, indent, U_DIRECTION, writeEmptyKeywords);
	m_airTransport[U_DIRECTION].write(out, indent, U_DIRECTION, writeEmptyKeywords);

	// for 2D or 3D format, write V direction data
	if (m_type == MF_2D || m_type == MF_3D) {
		m_moistureTransport[V_DIRECTION].write(out, indent, V_DIRECTION, writeEmptyKeywords);
		m_thermalTransport[V_DIRECTION].write(out, indent, V_DIRECTION, writeEmptyKeywords);
		m_airTransport[V_DIRECTION].write(out, indent, V_DIRECTION, writeEmptyKeywords);
	}

	// for 3D format, also write W direction data
	if (m_type == MF_3D) {
		m_moistureTransport[W_DIRECTION].write(out, indent, W_DIRECTION, writeEmptyKeywords);
		m_thermalTransport[W_DIRECTION].write(out, indent, W_DIRECTION, writeEmptyKeywords);
		m_airTransport[W_DIRECTION].write(out, indent, W_DIRECTION, writeEmptyKeywords);
	}

	// only write if we have data
	if ( m_vaporContactResistance.m_spline[VaporContactResistanceModel::F_sdPlus].size() > 1 || writeEmptyKeywords)
		m_vaporContactResistance.write( out, indent, U_DIRECTION, writeEmptyKeywords);

	m_measuredData.write(out, indent, NUM_MaterialDirection, writeEmptyKeywords);


}
// ----------------------------------------------------------------------------


void Material::readBinary(std::istream& ) {

	/// \todo implement
//	unsigned int test = 0;
//	// read second unsigned int from stream
//	IBK::read_uint32_binary( in, test);

//	// test magic header
//	if ( test != magicSecondFile()) {
//		errmsg = "Wrong second magic number!";
//		return false;
//	}

//	// we have binary format, new DELPHIN 6 + format
//	m_isBinary = true;
//	m_type = MF_1D;

//	// read file version and split it
//	IBK::read_uint32_binary( in, test);
//	m_majorFileVersion = majorFromVersionBinary( test );
//	m_minorFileVersion = minorFromVersionBinary( test );
//	unsigned int dummy;
//	// read last unsigned integer
//	IBK::read_uint32_binary( in, dummy);
//	IBK_ASSERT(dummy == 0); // safety check

//	// binary 1d read
//	Offsets offsets;
//	IBK::read_uint32_binary( in, offsets.m_parameterSectionOffset );
//	IBK::read_uint32_binary( in, offsets.m_uSectionOffset );
//	IBK::read_uint32_binary( in, offsets.m_vSectionOffset );
//	IBK::read_uint32_binary( in, offsets.m_wSectionOffset );
//	IBK::read_uint32_binary( in, offsets.m_measuredDataSectionOffset );

//	m_identification.readBinary( in );

//	// read parameter
//	for (unsigned int i = 0; i < NUM_ParaStorage; ++i) {
//		m_paraStorage[i].readBinary(in);
//	}
//	for (unsigned int i = 0; i < NUM_ParaTransport;++i){
//		m_paraTransport[i].readBinary(in);
//	}
//	for (unsigned int i = 0; i < NUM_ParaMechanical;++i){
//		m_paraMechanical[i].readBinary(in);
//	}

//	m_moistureStorage.readBinary( in );
//	m_thermalStorage.readBinary( in );

//	m_moistureTransport[U_DIRECTION].readBinary( in );
//	m_thermalTransport[U_DIRECTION].readBinary( in );
//	m_airTransport[U_DIRECTION].readBinary( in );

//	m_measuredData.readBinary(in);
///  \todo prepareCalculation call is missing
}
// ----------------------------------------------------------------------------


void Material::writeBinary(std::ostream& ) const {
	/// \todo implement

	//		// write dummy integer with the offsets
	//		IBK::write_uint32_binary( out, 0 );
	//		IBK::write_uint32_binary( out, 0 );
	//		IBK::write_uint32_binary( out, 0 );
	//		IBK::write_uint32_binary( out, 0 );
	//		IBK::write_uint32_binary( out, 0 );


	//		// write identification file
	//		m_identification.writeBinary( out );

	//		// determine file offset
	//		Offsets offsets;
	//		offsets.m_parameterSectionOffset = (unsigned int)out.tellp();

	//		// write parameter
	//		for (unsigned int i = 0; i < NUM_ParaStorage;++i){
	//			m_paraStorage[i].writeBinary(out);
	//		}
	//		for (unsigned int i = 0; i < NUM_ParaTransport;++i){
	//			m_paraTransport[i].writeBinary(out);
	//		}
	//		for (unsigned int i = 0; i < NUM_ParaMechanical;++i){
	//			m_paraMechanical[i].writeBinary(out);
	//		}

	//		// determine file offset
	//		offsets.m_uSectionOffset = (unsigned int)out.tellp();

	//		m_moistureStorage.writeBinary(out);
	//		m_thermalStorage.writeBinary(out);
	//		m_moistureTransport[U_DIRECTION].writeBinary(out);
	//		m_thermalTransport[U_DIRECTION].writeBinary(out);
	//		m_airTransport[U_DIRECTION].writeBinary(out);


	//		if (m_type == MF_2D || m_type == MF_3D){
	//			offsets.m_vSectionOffset = (unsigned int)out.tellp();
	//			m_moistureTransport[V_DIRECTION].writeBinary(out);
	//			m_thermalTransport[V_DIRECTION].writeBinary(out);
	//			m_airTransport[V_DIRECTION].writeBinary(out);
	//		}

	//		if (m_type == MF_3D){
	//			offsets.m_wSectionOffset = (unsigned int)out.tellp();
	//			m_moistureTransport[W_DIRECTION].writeBinary(out);
	//			m_thermalTransport[W_DIRECTION].writeBinary(out);
	//			m_airTransport[W_DIRECTION].writeBinary(out);
	//		}

	//		// determine file offset
	//		offsets.m_measuredDataSectionOffset = (unsigned int)out.tellp();
	//		m_measuredData.writeBinary(out);

	//		// now go back to file offset with data section offset and write the offset
	//		out.seekp(16, std::ios_base::beg);
	//		IBK::write_uint32_binary( out, offsets.m_parameterSectionOffset );
	//		IBK::write_uint32_binary( out, offsets.m_uSectionOffset );
	//		IBK::write_uint32_binary( out, offsets.m_vSectionOffset );
	//		IBK::write_uint32_binary( out, offsets.m_wSectionOffset );
	//		IBK::write_uint32_binary( out, offsets.m_measuredDataSectionOffset );
}
// ----------------------------------------------------------------------------


void Material::readASCIIParameterFromLine(const std::string & line) {
	const char * const FUNC_ID = "[Material::readASCIIParameterFromLine]";
	std::string keyword, value;
	try {

		// now read one the parameters
		if (!IBK::extract_keyword(line, keyword, value))
			throw IBK::Exception( IBK::FormatString("Expected keyword, got '%1'!").arg(line), FUNC_ID);

		if (value.empty())
			throw IBK::Exception( IBK::FormatString("Missing value to parameter %1.").arg(keyword), FUNC_ID);

		// special handling for NAME and FLAGS keyword (<= D5.6 material files)
		if (keyword == "NAME") {
			// store value in identification section
			// for version 5.6 all names are english
			m_identification.m_name.setString(value, "en");
			return;
		}
		if ( keyword ==  "FLAGS") {
			m_identification.setFlagsPreV6(value);
			return;
		}

		// *** transport parameters ***

		if (KeywordList::KeywordExists("Material::ParaTransport", keyword)) {
			bool deprecated;
			ParaTransport ident = (ParaTransport)KeywordList::Enumeration("Material::ParaTransport", keyword, &deprecated);
			if( deprecated ) {
				std::string new_keyword = KeywordList::Keyword("Material::ParaTransport", ident);
				IBK::IBK_Message( IBK::FormatString("Keyword '%1' is deprecated (new -> '%2').").arg(keyword).arg(new_keyword), IBK::MSG_WARNING, FUNC_ID, 2);
				keyword = new_keyword;
			}

			// keywords must be identifiers
			std::string errmsg;
			switch(ident) {
				case MP_LAMBDA		: m_paraTransport[MP_LAMBDA].set( keyword, value, &errmsg ); break;
				case MP_AW			: m_paraTransport[MP_AW].set( keyword, value, &errmsg ); break;
				case MP_MEW			: m_paraTransport[MP_MEW].set( keyword, value, &errmsg ); break;
				case MP_SD			: m_paraTransport[MP_SD].set( keyword, value, &errmsg ); break;
				case MP_KLEFF		: m_paraTransport[MP_KLEFF].set( keyword, value, &errmsg ); break;
				case MP_DLEFF		: m_paraTransport[MP_DLEFF].set( keyword, value, &errmsg ); break;
				case MP_KG			: m_paraTransport[MP_KG].set( keyword, value, &errmsg ); break;
				case MP_LAMBDA_V	: m_paraTransport[MP_LAMBDA_V].set( keyword, value, &errmsg ); break;
				case MP_AW_V		: m_paraTransport[MP_AW_V].set( keyword, value, &errmsg ); break;
				case MP_MEW_V		: m_paraTransport[MP_MEW_V].set( keyword, value, &errmsg ); break;
				case MP_SD_V		: m_paraTransport[MP_SD_V].set( keyword, value, &errmsg ); break;
				case MP_KLEFF_V		: m_paraTransport[MP_KLEFF_V].set( keyword, value, &errmsg ); break;
				case MP_DLEFF_V		: m_paraTransport[MP_DLEFF_V].set( keyword, value, &errmsg ); break;
				case MP_KG_V		: m_paraTransport[MP_KG_V].set( keyword, value, &errmsg ); break;
				case MP_LAMBDA_W	: m_paraTransport[MP_LAMBDA_W].set( keyword, value, &errmsg ); break;
				case MP_AW_W		: m_paraTransport[MP_AW_W].set( keyword, value, &errmsg ); break;
				case MP_MEW_W		: m_paraTransport[MP_MEW_W].set( keyword, value, &errmsg ); break;
				case MP_SD_W		: m_paraTransport[MP_SD_W].set( keyword, value, &errmsg ); break;
				case MP_KLEFF_W		: m_paraTransport[MP_KLEFF_W].set( keyword, value, &errmsg ); break;
				case MP_DLEFF_W		: m_paraTransport[MP_DLEFF_W].set( keyword, value, &errmsg ); break;
				case MP_KG_W		: m_paraTransport[MP_KG_W].set( keyword, value, &errmsg ); break;
				case MP_LAMBDA_DESIGN		: m_paraTransport[MP_LAMBDA_DESIGN].set( keyword, value, &errmsg ); break;
				default:
					IBK::IBK_Message( IBK::FormatString("Invalid or deprecated keyword '%1'.").arg(keyword), IBK::MSG_WARNING, FUNC_ID, 3);
					return;
			} // switch(ident)

			if (!errmsg.empty())
				throw IBK::Exception( IBK::FormatString("Invalid format of argument '%2' to keyword '%1'.").arg(keyword).arg(value), FUNC_ID);
			return;
		}

		// *** storage parameters ***

		if (KeywordList::KeywordExists("Material::ParaStorage", keyword)) {
			bool deprecated;
			ParaStorage ident = (ParaStorage)KeywordList::Enumeration("Material::ParaStorage", keyword, &deprecated);
			if( deprecated ) {
				std::string new_keyword = KeywordList::Keyword("Material::ParaStorage", ident);
				IBK::IBK_Message( IBK::FormatString("Keyword '%1' is deprecated (new -> '%2').").arg(keyword).arg(new_keyword), IBK::MSG_WARNING, FUNC_ID, 2);
				keyword = new_keyword;
			}

			// all other keywords must be identifiers
			std::string errmsg;
			switch(ident) {
				case MP_RHO				: m_paraStorage[MP_RHO].set( keyword, value, &errmsg ); break;
				case MP_CE				: m_paraStorage[MP_CE].set( keyword, value, &errmsg ); break;
				case MP_THETA_POR		: m_paraStorage[MP_THETA_POR].set( keyword, value, &errmsg ); break;
				case MP_THETA_EFF		: m_paraStorage[MP_THETA_EFF].set( keyword, value, &errmsg ); break;
				case MP_THETA_CAP		: m_paraStorage[MP_THETA_CAP].set( keyword, value, &errmsg ); break;
				case MP_THETA_LIMHYG	: m_paraStorage[MP_THETA_LIMHYG].set( keyword, value, &errmsg ); break;
				case MP_THETA_80		: m_paraStorage[MP_THETA_80].set( keyword, value, &errmsg ); break;
				default:
					IBK::IBK_Message( IBK::FormatString("Invalid or deprecated keyword '%1'.").arg(keyword), IBK::MSG_WARNING, FUNC_ID, 3);
					return;
			} // switch(ident)

			if (!errmsg.empty())
				throw IBK::Exception( IBK::FormatString("Invalid format of argument '%2' to keyword '%1'.").arg(keyword).arg(value), FUNC_ID);
			return;
		}

		// *** mechanical parameters ***

		if (KeywordList::KeywordExists("Material::ParaMechanical", keyword)) {
			bool deprecated;
			ParaMechanical ident = (ParaMechanical)KeywordList::Enumeration("Material::ParaMechanical", keyword, &deprecated);
			if( deprecated ) {
				std::string new_keyword = KeywordList::Keyword("Material::ParaMechanical", ident);
				IBK::IBK_Message( IBK::FormatString("Keyword '%1' is deprecated (new -> '%2').").arg(keyword).arg(new_keyword), IBK::MSG_WARNING, FUNC_ID, 2);
				keyword = new_keyword;
			}

			// all other keywords must be identifiers
			std::string errmsg;
			switch(ident) {
				case MP_BETA_THETA_L		: m_paraMechanical[MP_BETA_THETA_L].set( keyword, value, &errmsg ); break;
				case MP_ALPHA				: m_paraMechanical[MP_ALPHA].set( keyword, value, &errmsg ); break;
				case MP_BETA_THETA_L2		: m_paraMechanical[MP_BETA_THETA_L2].set( keyword, value, &errmsg ); break;
				case MP_ALPHA_V				: m_paraMechanical[MP_ALPHA_V].set( keyword, value, &errmsg ); break;
				case MP_BETA_THETA_L_V		: m_paraMechanical[MP_BETA_THETA_L_V].set( keyword, value, &errmsg ); break;
				case MP_BETA_THETA_L2_V		: m_paraMechanical[MP_BETA_THETA_L2_V].set( keyword, value, &errmsg ); break;
				case MP_ALPHA_W				: m_paraMechanical[MP_ALPHA_W].set( keyword, value, &errmsg ); break;
				case MP_BETA_THETA_L_W		: m_paraMechanical[MP_BETA_THETA_L_W].set( keyword, value, &errmsg ); break;
				case MP_BETA_THETA_L2_W		: m_paraMechanical[MP_BETA_THETA_L2_W].set( keyword, value, &errmsg ); break;
				default:
					IBK::IBK_Message( IBK::FormatString("Invalid or deprecated keyword '%1'.").arg(keyword), IBK::MSG_WARNING, FUNC_ID, 3);
					return;
			} // switch(ident)

			if (!errmsg.empty())
				throw IBK::Exception( IBK::FormatString("Invalid format of argument '%2' to keyword '%1'.").arg(keyword).arg(value), FUNC_ID);
			return;
		}

		IBK::IBK_Message( IBK::FormatString("Invalid or deprecated keyword '%1'.").arg(keyword), IBK::MSG_WARNING, FUNC_ID, 3);

	}
	catch (IBK::Exception& ex) {
		throw IBK::Exception(ex, "Error reading material base parameter section!", FUNC_ID);
	}
}
// ----------------------------------------------------------------------------

void Material::readASCIIParameters(const std::string & data) {
	std::stringstream in(data);
	std::string line;

	// now read all the parameters
	while (std::getline(in, line)) {
		readASCIIParameterFromLine(line);
	}
}

void Material::readASCIIParameters(const std::vector<std::string> & data) {

	// now read all the parameters
	for( unsigned int i=0, size=data.size(); i<size; ++i) {
		readASCIIParameterFromLine(data[i]);
	}
}

// ----------------------------------------------------------------------------


void Material::writeASCIIParameters(std::ostream & out, unsigned int indent, bool writeEmptyKeywords) const {
	const char * const FUNC_ID = "[Material::writeASCIIParameters]";
	std::string istr(indent, ' ');

	try {
		// write storage parameters
		bool empty = true;
		for (int i=0; i<NUM_ParaStorage; ++i)
			empty = empty && m_paraStorage[i].name.empty();
		if ( writeEmptyKeywords || !empty ) {
			out << '\n' << istr << '[' << KeywordList::Keyword("Material::Sections", Material::STORAGE_BASE_PARAMETERS) << "]\n";
			for (int i=0; i<NUM_ParaStorage; ++i) {
				if (m_paraStorage[i].name.empty()) {
					if (writeEmptyKeywords)
						write_keyword(out, istr + "  ", MAX_KW_LEN(), KeywordList::Keyword("Material::ParaStorage", i)) << std::endl;
				}
				else {
					m_paraStorage[i].write(out, indent+2, MAX_KW_LEN(), true);
				}
			}
			out << "\n";
		}

		// write transport parameters
		empty = true;
		for (int i=0; i<NUM_ParaTransport; ++i)
			empty = empty && m_paraTransport[i].name.empty();
		if ( writeEmptyKeywords || !empty ) {
			out << '\n' << istr << '[' << KeywordList::Keyword("Material::Sections", Material::TRANSPORT_BASE_PARAMETERS) << "]\n";
			for (int i=0; i<NUM_ParaTransport; ++i) {
				if (m_paraTransport[i].name.empty()) {
					if (writeEmptyKeywords)
						write_keyword(out, istr + "  ", MAX_KW_LEN(), KeywordList::Keyword("Material::ParaTransport", i)) << std::endl;
				}
				else {
					m_paraTransport[i].write(out, indent+2, MAX_KW_LEN(), true);
				}
			}
			out << "\n";
		}

		// write mechanical parameters
		empty = true;
		for (int i=0; i<NUM_ParaMechanical; ++i)
			empty = empty && m_paraMechanical[i].name.empty();
		if ( writeEmptyKeywords || !empty ) {
			out << '\n' << istr << '[' << KeywordList::Keyword("Material::Sections", Material::MECHANICAL_BASE_PARAMETERS) << "]\n";
			for (int i=0; i<NUM_ParaMechanical; ++i) {
				if (m_paraMechanical[i].name.empty()) {
					if (writeEmptyKeywords)
						write_keyword(out, istr + "  ", MAX_KW_LEN(), KeywordList::Keyword("Material::ParaMechanical", i)) << std::endl;
				}
				else {
					m_paraMechanical[i].write(out, indent+2, MAX_KW_LEN(), true);
				}
			}
			out << "\n";
		}

	}
	catch (std::exception& ex) {
		IBK::IBK_Message( ex.what(), IBK::MSG_ERROR, FUNC_ID, 1);
		// Do not throw any further!
	}
}
// ----------------------------------------------------------------------------




} // namespace MM


