
#include "MM_Identification.h"

#include <iomanip>
#include <fstream>
#include <iostream>
#include <sstream>
#include <ostream>
#include <algorithm>
#include <string>
#include <list>
#include <ctime>

#include <IBK_messages.h>
#include <IBK_StringUtils.h>
#include <IBK_FormatString.h>
#include <IBK_Exception.h>
#include <IBK_InputOutput.h>
#include <IBK_Time.h>

#include "MM_Material.h"
#include "MM_Constants.h"
#include "MM_KeywordList.h"
#include "MM_Version.h"
#include "MM_Utils.h"

namespace MM {


unsigned int MaterialIdentification::m_newColorIndex = 0;

/*! Set of default colors used for newly created materials.
	\todo add more default colors
*/
unsigned int materialColors[5] = {
	0x604040,
	0x209040,
	0xa02050,
	0xf0f010,
	0x1010ff };

MaterialIdentification::MaterialIdentification() {
	clear();
	m_color = IBK::Color::fromTColor(
								materialColors[ ++m_newColorIndex % (sizeof(materialColors)/sizeof(unsigned int))]
							);
}
//---------------------------------------------------------------------------

void MaterialIdentification::readLine( const std::string& line) {
	const char * const FUNC_ID = "[MaterialIdentification::readLine]";
	std::string keyword, value;

	if (!IBK::extract_keyword(line, keyword, value)){
		throw IBK::Exception( IBK::FormatString("Expected keyword, got '%1'!").arg(line), FUNC_ID);
	}

	// trim spaces or other crazy stuff from value
	IBK::trim(value);

	if (value.empty())
		throw IBK::Exception( IBK::FormatString("Missing value for keyword '%1'!").arg(keyword), FUNC_ID);

	if (!KeywordList::KeywordExists("MaterialIdentification::Ident", keyword)) {
		IBK::IBK_Message( IBK::FormatString("Invalid keyword found in identification section '%1'.").arg(keyword), IBK::MSG_WARNING, FUNC_ID, 2 );
		return;
	}

	Ident ident = (Ident)KeywordList::Enumeration("MaterialIdentification::Ident", keyword);

	// all other keywords must be identifiers
	switch(ident) {
		case ID_NAME				: m_name.setEncodedString(value); 	break;
		case ID_PRODUCER			: m_producer.setEncodedString(value); break;
		case ID_PRODUCT_ID			: m_productId.setEncodedString(value); break;
		case ID_INVESTIGATOR		: m_investigator = value; break;
		case ID_LABORATORY			: m_laboratory.setEncodedString(value); break;
		case ID_DATE				: m_date = value; break;
		case ID_PRODUCTION_DATE		: m_productionDate = value; break;
		case ID_SAMPLING_DATE		: m_samplingDate = value; break;
		case ID_SAMPLE_ID			: m_sampleID = value; break;
		case ID_SAMPLING_METHOD		:
			m_samplingMethod = (SamplingMethods) KeywordList::Enumeration("MaterialIdentification::SamplingMethods", value);
			break;
		case ID_COUNTRY				: m_country = value; break;
		case ID_COLOUR				: {
			try {
				m_color.read(line);
			}
			catch(IBK::Exception& ex) {
				throw IBK::Exception(ex, IBK::FormatString("Invalid parameter value '%1' for keyword '%1'!")
									  .arg(value).arg(keyword), FUNC_ID);
			}
			break;
		}
		case ID_FLAGS				: setFlags(value);	break;
		case ID_USE_INSTEAD			: m_deprecatedState = IBK::string2val<int>(value); 	break;
		case ID_AQUISITION_ID		: m_aquisitionID = IBK::string2val<int>(value); 	break;
		case ID_COPYRIGHT			: m_copyright = value; break;
		case ID_CATEGORY			: m_cat.set(value); break;
		case ID_COMMENTS			: m_comments.setEncodedString(value); break;
		case ID_U_DIRECTION			: m_uDirection = value; break;
		case ID_V_DIRECTION			: m_vDirection = value; break;
		case ID_W_DIRECTION			: m_wDirection = value; break;
		case ID_DBTYPE				:
		{
			m_DBtypes.clear();
			std::list< std::string > tmpStringList;
			IBK::explode(value, tmpStringList, ',');
			std::transform(tmpStringList.begin(), tmpStringList.end(), tmpStringList.begin(), IBK::trimmer());
			unsigned int number;
			for ( std::list< std::string >::const_iterator it = tmpStringList.begin(); it != tmpStringList.end(); ++it){
				number = IBK::string2val<unsigned int>(*it);
				m_DBtypes.push_back( static_cast<MaterialDBType>(number) );
			}
			break;
		}
		case ID_SIGNATURE_SOURCE	: m_signatureSource = value; break;
		case ID_QUALITY_FLAGS		: m_flags = IBK::string2val<unsigned int>(value); break;
		case ID_DATA_SHEET_SOURCE	: m_dataSheetSource = value; break;
		case ID_HATCHING			: {
			int itemp = IBK::string2val<int>(value);
			if( itemp < HT_Ground || itemp >= HT_None )
				itemp = HT_None;
			m_hatchingType = static_cast<HatchingType>(itemp);
			break;
		}
		default: 				break;
	} // switch(ident)
}

void MaterialIdentification::read( const std::string& data) {
	const char * const FUNC_ID = "[MaterialIdentification::read]";
	std::stringstream in(data);
	std::string line;
	try {

		while (std::getline(in, line)) {
			readLine(line);
		} // while
	}
	catch (IBK::Exception& ex) {
		throw IBK::Exception(ex, "Error reading material identification section!", FUNC_ID);
	}
}
//---------------------------------------------------------------------------

void MaterialIdentification::read( const std::vector<std::string>& data) {
	const char * const FUNC_ID = "[MaterialIdentification::read]";

	try {
		for(unsigned int i=0, size=data.size(); i<size; ++i) {
			readLine(data[i]);
		} // while
	}
	catch (IBK::Exception& ex) {
		throw IBK::Exception(ex, "Error reading material identification section!", FUNC_ID);
	}
}
//---------------------------------------------------------------------------


void MaterialIdentification::readBinary( std::istream&  ){

/*
	m_name.readBinary(in);
	m_producer.readBinary(in);
	m_productId.readBinary(in);
	IBK::read_string_binary(in, m_investigator, 10000);
	m_laboratory.readBinary(in);
	IBK::read_string_binary(in, m_date, 10000);
	IBK::read_string_binary(in, m_samplingDate, 10000);
	IBK::read_string_binary(in, m_productionDate, 10000);
	IBK::read_string_binary(in, m_sampleID, 10000);
	in.read(reinterpret_cast<char *>(&m_samplingMethod), sizeof(int));
	IBK::read_string_binary(in, m_country, 10000);
	unsigned int tmpUInt;
	in.read(reinterpret_cast<char *>(&tmpUInt), sizeof(unsigned int));
	m_color.fromQRgb(tmpUInt);
	in.read(reinterpret_cast<char *>(&m_flags), sizeof(int));
	in.read(reinterpret_cast<char *>(&m_deprecatedState), sizeof(int));
	IBK::read_string_binary(in, m_copyright, 10000);
	in.read(reinterpret_cast<char *>(&tmpUInt), sizeof(unsigned int));
	m_cat.fromUInt(tmpUInt);
	m_comments.readBinary(in);

	size_t tmpSizeT;
	in.read(reinterpret_cast<char *>(&tmpSizeT), sizeof(size_t));
	m_DBtypes.resize(tmpSizeT);

	for ( size_t i = 0; i < tmpSizeT; ++i){
		in.read(reinterpret_cast<char *>(&tmpUInt), sizeof(unsigned int));
		m_DBtypes[i] = (MaterialDBType)tmpUInt;
	}

	IBK::read_string_binary(in, m_uDirection, 10000);
	IBK::read_string_binary(in, m_vDirection, 10000);
	IBK::read_string_binary(in, m_wDirection, 10000);
	IBK::read_string_binary(in, m_signatureSource, 10000);
	in.read(reinterpret_cast<char *>(&m_qualityFlags), sizeof(int));
	IBK::read_string_binary(in, m_dataSheetSource, 10000);
*/

}
//---------------------------------------------------------------------------


void MaterialIdentification::write(std::ostream& out, unsigned int indent, bool writeEmptyKeywords) const {
	std::string istr(indent, ' ');

	try {

		out << '\n' << istr << '[' << KeywordList::Keyword("Material::Sections", Material::IDENTIFICATION) << "]\n";
		istr += "  ";
		if ( writeEmptyKeywords || !(m_name.empty()) )
			write_keyword(out, istr, MAX_KW_LEN(), KeywordList::Keyword("MaterialIdentification::Ident", ID_NAME)) << m_name << '\n';
		if (writeEmptyKeywords || m_aquisitionID != 0)
			write_keyword(out, istr, MAX_KW_LEN(), KeywordList::Keyword("MaterialIdentification::Ident", ID_AQUISITION_ID)) << m_aquisitionID << '\n';
		if (writeEmptyKeywords || !m_producer.empty())
			write_keyword(out, istr, MAX_KW_LEN(), KeywordList::Keyword("MaterialIdentification::Ident", ID_PRODUCER)) << m_producer << '\n';
		if (writeEmptyKeywords || !m_productId.empty())
			write_keyword(out, istr, MAX_KW_LEN(), KeywordList::Keyword("MaterialIdentification::Ident", ID_PRODUCT_ID)) << m_productId << '\n';
		if (writeEmptyKeywords || !m_investigator.empty())
			write_keyword(out, istr, MAX_KW_LEN(), KeywordList::Keyword("MaterialIdentification::Ident", ID_INVESTIGATOR)) << m_investigator << '\n';
		if (writeEmptyKeywords || !m_laboratory.empty())
			write_keyword(out, istr, MAX_KW_LEN(), KeywordList::Keyword("MaterialIdentification::Ident", ID_LABORATORY)) << m_laboratory << '\n';
		if (m_date.empty()){
			m_date = IBK::Time::dateRFC2822(MM::dateFormatStrftime());
		}
		write_keyword(out, istr, MAX_KW_LEN(), KeywordList::Keyword("MaterialIdentification::Ident", ID_DATE)) << m_date << '\n';
		if (writeEmptyKeywords || !m_samplingDate.empty())
			write_keyword(out, istr, MAX_KW_LEN(), KeywordList::Keyword("MaterialIdentification::Ident", ID_SAMPLING_DATE)) << m_samplingDate << '\n';
		if (writeEmptyKeywords || !m_productionDate.empty())
			write_keyword(out, istr, MAX_KW_LEN(), KeywordList::Keyword("MaterialIdentification::Ident", ID_PRODUCTION_DATE)) << m_productionDate << '\n';
		if (writeEmptyKeywords || !m_sampleID.empty())
			write_keyword(out, istr, MAX_KW_LEN(), KeywordList::Keyword("MaterialIdentification::Ident", ID_SAMPLE_ID)) << m_sampleID << '\n';
		if (m_samplingMethod != 0)
			write_keyword(out, istr, MAX_KW_LEN(), KeywordList::Keyword("MaterialIdentification::Ident", ID_SAMPLING_METHOD)) << m_samplingMethod << '\n';
		if (writeEmptyKeywords || !m_country.empty())
			write_keyword(out, istr, MAX_KW_LEN(), KeywordList::Keyword("MaterialIdentification::Ident", ID_COUNTRY)) << m_country << '\n';
		// there must be a color
		m_color.write(out, indent + 2);

		if (m_flags != 0){
			write_keyword(out, istr, MAX_KW_LEN(), KeywordList::Keyword("MaterialIdentification::Ident", ID_FLAGS));
			if ( m_flags & AIR )
				out << KeywordList::Keyword("MaterialIdentification::Flags", AIR) << " ";
			if ( m_flags & AIR_TIGHT )
				out << KeywordList::Keyword("MaterialIdentification::Flags", AIR_TIGHT) << " ";
			if ( m_flags & VAPOR_TIGHT )
				out << KeywordList::Keyword("MaterialIdentification::Flags", VAPOR_TIGHT) << " ";
			if ( m_flags & WATER_TIGHT )
				out << KeywordList::Keyword("MaterialIdentification::Flags", WATER_TIGHT) << " ";
			if ( m_flags & WOOD )
				out << KeywordList::Keyword("MaterialIdentification::Flags", WOOD) << " ";
			if ( m_flags & WOOD_BASED )
				out << KeywordList::Keyword("MaterialIdentification::Flags", WOOD_BASED) << " ";
			if ( m_flags & FOILS )
				out << KeywordList::Keyword("MaterialIdentification::Flags", FOILS) << " ";
			out << "\n";
		}
		if (writeEmptyKeywords || m_deprecatedState != 0)
			write_keyword(out, istr, MAX_KW_LEN(), KeywordList::Keyword("MaterialIdentification::Ident", ID_USE_INSTEAD)) << m_deprecatedState << '\n';
		if (writeEmptyKeywords || !m_copyright.empty())
			write_keyword(out, istr, MAX_KW_LEN(), KeywordList::Keyword("MaterialIdentification::Ident", ID_COPYRIGHT)) << m_copyright << '\n';
		if (writeEmptyKeywords || !m_cat.isEmpty())
			write_keyword(out, istr, MAX_KW_LEN(), KeywordList::Keyword("MaterialIdentification::Ident", ID_CATEGORY)) << m_cat << '\n';
		if (writeEmptyKeywords || !m_comments.empty())
			write_keyword(out, istr, MAX_KW_LEN(), KeywordList::Keyword("MaterialIdentification::Ident", ID_COMMENTS)) << m_comments << '\n';
		if( writeEmptyKeywords || !m_DBtypes.empty() ) {
			write_keyword(out, istr, MAX_KW_LEN(), KeywordList::Keyword("MaterialIdentification::Ident", ID_DBTYPE));
			size_t size = m_DBtypes.size();
			if (size > 0) {
				for( size_t i=0; i < size - 1; ++i) {
					out << m_DBtypes[i];
					if( i< size-1)
						out << ",";
				}
				out << m_DBtypes.back();
			}
			out << "\n";
		}
		if (writeEmptyKeywords || !m_uDirection.empty())
			write_keyword(out, istr, MAX_KW_LEN(), KeywordList::Keyword("MaterialIdentification::Ident", ID_U_DIRECTION)) << m_uDirection << '\n';
		if (writeEmptyKeywords || !m_vDirection.empty())
			write_keyword(out, istr, MAX_KW_LEN(), KeywordList::Keyword("MaterialIdentification::Ident", ID_V_DIRECTION)) << m_vDirection << '\n';
		if (writeEmptyKeywords || !m_wDirection.empty())
			write_keyword(out, istr, MAX_KW_LEN(), KeywordList::Keyword("MaterialIdentification::Ident", ID_W_DIRECTION)) << m_wDirection << '\n';
		if (writeEmptyKeywords || !m_signatureSource.empty())
			write_keyword(out, istr, MAX_KW_LEN(), KeywordList::Keyword("MaterialIdentification::Ident", ID_SIGNATURE_SOURCE)) << m_signatureSource << '\n';
		if (m_qualityFlags != 0)
			out << istr << std::setw(24) << std::left << "QUALITY_FLAGS" << " = " << std::hex << m_qualityFlags << '\n';
		if (writeEmptyKeywords || !m_dataSheetSource.empty())
			write_keyword(out, istr, MAX_KW_LEN(), KeywordList::Keyword("MaterialIdentification::Ident", ID_DATA_SHEET_SOURCE)) << m_dataSheetSource << '\n';

		write_keyword(out, istr, MAX_KW_LEN(), KeywordList::Keyword("MaterialIdentification::Ident", ID_HATCHING)) << m_hatchingType << '\n';
		out << '\n';
	}
	catch (std::exception& ex) {
		IBK::IBK_Message( ex.what() );
		// Do not throw any further!
	}
}
//---------------------------------------------------------------------------


void MaterialIdentification::writeBinary(std::ostream& out) const {

	/// \todo implement
	// write
	m_name.writeBinary(out);
	m_producer.writeBinary(out);
	m_productId.writeBinary(out);
	IBK::write_string_binary(out, m_investigator);
	m_laboratory.writeBinary(out);
	IBK::write_string_binary(out, m_date);
	IBK::write_string_binary(out, m_samplingDate);
	IBK::write_string_binary(out, m_productionDate);
	IBK::write_string_binary(out, m_sampleID);
	out.write(reinterpret_cast<const char *>(&m_samplingMethod), sizeof(int));
	IBK::write_string_binary(out, m_country);
	unsigned int tmpUInt = m_color.toQRgb();
	out.write(reinterpret_cast<const char *>(&tmpUInt), sizeof(unsigned int));
	out.write(reinterpret_cast<const char *>(&m_flags), sizeof(int));
	out.write(reinterpret_cast<const char *>(&m_deprecatedState), sizeof(int));
	IBK::write_string_binary(out, m_copyright);
	tmpUInt = m_cat.toUInt();
	out.write(reinterpret_cast<const char *>(&tmpUInt), sizeof(unsigned int));
	m_comments.writeBinary(out);
	size_t tmpSizeT = m_DBtypes.size();
	out.write(reinterpret_cast<const char *>(&tmpSizeT), sizeof(size_t));
	for ( std::vector< MaterialDBType >::const_iterator it = m_DBtypes.begin(), end = m_DBtypes.end(); it != end; ++it ){
		tmpUInt = *it;
		out.write(reinterpret_cast<const char *>(&tmpUInt), sizeof(unsigned int));
	}
	IBK::write_string_binary(out, m_uDirection);
	IBK::write_string_binary(out, m_vDirection);
	IBK::write_string_binary(out, m_wDirection);
	IBK::write_string_binary(out, m_signatureSource);
	out.write(reinterpret_cast<const char *>(&m_qualityFlags), sizeof(int));
	IBK::write_string_binary(out, m_dataSheetSource);
	int intHatch = m_hatchingType;
	out.write(reinterpret_cast<const char *>(&intHatch), sizeof(int));

}
//---------------------------------------------------------------------------


void MaterialIdentification::clear() {

	m_name.clear();
	m_producer.clear();
	m_productId.clear();
	m_investigator.clear();
	m_laboratory.clear();
	m_date.clear();
	m_samplingDate.clear();
	m_productionDate.clear();
	m_sampleID.clear();
	m_samplingMethod = 0;
	m_country.clear();
	m_color.fromQRgb( 0xFF000000 );
	m_flags = 0;
	m_deprecatedState=0;
	m_copyright.clear();
	m_cat.clear();
	m_comments.clear();
	m_DBtypes.clear();
	m_uDirection.clear();
	m_vDirection.clear();
	m_wDirection.clear();
	m_signatureSource.clear();
	m_qualityFlags = 0;
	m_dataSheetSource.clear();
	m_hatchingType = HT_None;
	m_aquisitionID = -1;

}
//---------------------------------------------------------------------------


void MaterialIdentification::setFlags(const std::string& src) {
	const char * const FUNC_ID = "[MaterialIdentification::setFlags]  ";
	std::stringstream fstrm(src);
	std::string flag;
	while (fstrm >> flag) {
		try {
			int fnum = KeywordList::Enumeration("MaterialIdentification::Flags", flag);
			m_flags |= fnum;
		}
		catch (std::exception& ex) {
			IBK::IBK_Message( ex.what(), IBK::MSG_ERROR, FUNC_ID, 1);
			throw IBK::Exception( IBK::FormatString("Invalid parameter value '%1' for keyword '%1'!").arg(flag).arg("FLAGS"), FUNC_ID);
		}
	}
}
//---------------------------------------------------------------------------


bool MaterialIdentification::obsoleteKeys(int kw) const {

	// all undefined keywords are not in the list
	switch(kw){

		case ID_NAME:
		case ID_PRODUCER:
		case ID_PRODUCT_ID:
		case ID_INVESTIGATOR:
		case ID_LABORATORY:
		case ID_DATE:
		case ID_SAMPLING_DATE:
		case ID_SAMPLE_ID:
		case ID_SAMPLING_METHOD:
		case ID_COUNTRY:
		case ID_COLOUR:
		case ID_FLAGS:
		case ID_USE_INSTEAD:
		case ID_COPYRIGHT:
		case ID_CATEGORY:
		case ID_COMMENTS:
		case ID_U_DIRECTION:
		case ID_V_DIRECTION:
		case ID_W_DIRECTION:
		case ID_DBTYPE:
		case ID_SIGNATURE_SOURCE:
		case ID_QUALITY_FLAGS:
		case ID_DATA_SHEET_SOURCE:
		case ID_HATCHING:
			return false;
//		break;

		default:
			return true;
	}
}
//---------------------------------------------------------------------------


size_t MaterialIdentification::languages(std::set<std::string>& langs) const {
	langs.clear();
	m_name.languages(langs);
	m_producer.languages(langs);
	m_productId.languages(langs);
	m_laboratory.languages(langs);
	m_comments.languages(langs);
	return langs.size();
}
//---------------------------------------------------------------------------


bool operator==(const MaterialIdentification& lhs, const MaterialIdentification& rhs) {

	/// \todo think about which values can be ignored

	bool res = lhs.m_color == rhs.m_color;
	res &= lhs.m_name == rhs.m_name;
	res &= lhs.m_producer == rhs.m_producer;
	res &= lhs.m_productId == rhs.m_productId;
	res &= lhs.m_investigator == rhs.m_investigator;
	res &= lhs.m_laboratory == rhs.m_laboratory;
	res &= lhs.m_date == rhs.m_date;
	res &= lhs.m_samplingDate == rhs.m_samplingDate;
	res &= lhs.m_productionDate == rhs.m_productionDate;
	res &= lhs.m_sampleID == rhs.m_sampleID;
	res &= lhs.m_samplingMethod == rhs.m_samplingMethod;
	res &= lhs.m_country == rhs.m_country;
	res &= lhs.m_flags == rhs.m_flags;
	res &= lhs.m_deprecatedState == rhs.m_deprecatedState;
	res &= lhs.m_copyright == rhs.m_copyright;
	res &= lhs.m_cat == rhs.m_cat;
	res &= lhs.m_comments == rhs.m_comments;
	res &= lhs.m_DBtypes == rhs.m_DBtypes;
	res &= lhs.m_uDirection == rhs.m_uDirection;
	res &= lhs.m_vDirection == rhs.m_vDirection;
	res &= lhs.m_wDirection == rhs.m_wDirection;
	res &= lhs.m_signatureSource == rhs.m_signatureSource;
	res &= lhs.m_qualityFlags == rhs.m_qualityFlags;
	res &= lhs.m_dataSheetSource == rhs.m_dataSheetSource;
	res &= lhs.m_hatchingType == rhs.m_hatchingType;
	return res;
}
//---------------------------------------------------------------------------


// ----------------------------------------------------------------------------
// CAUTION YOU ARE ENTERING PRE V6 AREA
// ----------------------------------------------------------------------------

MaterialIdentification::HatchingType hatchingFromCategory(const MaterialCategory& cat) {
	switch(cat.first()) {
		case MaterialCategory::Coating: return MaterialIdentification::HT_SealingFoil;
		case MaterialCategory::Plaster: return MaterialIdentification::HT_MortarPlaster;
		case MaterialCategory::Bricks: return MaterialIdentification::HT_Masonry;
		case MaterialCategory::NaturalStones: return MaterialIdentification::HT_ConcreteNotReinforced;
		case MaterialCategory::Cementitious: return MaterialIdentification::HT_ConcreteNotReinforced;
		case MaterialCategory::Insulations: return MaterialIdentification::HT_Insulation;
		case MaterialCategory::BuildingBoards: return MaterialIdentification::HT_MortarPlaster;
		case MaterialCategory::Woodbased: return MaterialIdentification::HT_WoodTransverseToFibre;
		case MaterialCategory::NaturalMaterials: return MaterialIdentification::HT_None;
		case MaterialCategory::Soils: return MaterialIdentification::HT_Ground;
		case MaterialCategory::CladdingSystems: return MaterialIdentification::HT_Sealant;
		case MaterialCategory::Foils: return MaterialIdentification::HT_SealingFoil;
		case MaterialCategory::Miscellaneous: return MaterialIdentification::HT_Metal;
		default: return MaterialIdentification::HT_None;
	}
}

void MaterialIdentification::readLinePreV6( const std::string& line) {
	const char * const FUNC_ID = "[MaterialIdentification::readLine]";
	std::string keyword, value;

	if (!IBK::extract_keyword(line, keyword, value))
		throw IBK::Exception( IBK::FormatString("Expected keyword, got '%1'!").arg(line), FUNC_ID);

	// trim spaces or other crasy stuff from value
	IBK::trim(value);

	if (value.empty() && keyword.find("FLAGS") == std::string::npos){
		// should we really do an abort while importing old file formats
		//				throw IBK::Exception( IBK::FormatString(MSG_MISSING_VALUE).arg(keyword), FUNC_ID);
//				IBK::IBK_Message( IBK::FormatString("No value for Keyword %1, this is not acceptable! User please fix me!").arg(keyword), IBK::MSG_WARNING, FUNC_ID, 2 );
		return;
	}

	// v5 paras
	if ( keyword.find("UNIQUE_ID") != std::string::npos ) {
		// this must be adapted manually by the user!
//				IBK::IBK_Message( "A new ID must be set manually!", IBK::MSG_WARNING, FUNC_ID, 2 );
		return;
	}
	if ( keyword.find("NAME") != std::string::npos ) {
		m_name.setEncodedString(value);
		return;
	}
	if ( keyword.find("COLOR") != std::string::npos || keyword.find("COLOUR") != std::string::npos) {
		try {
			m_color.read(line);
		}
		catch(IBK::Exception& ) {
			IBK::IBK_Message(IBK::FormatString("Invalid parameter value '%1' for keyword '%2'!")
								  .arg(value).arg("COLOR"), IBK::MSG_WARNING, FUNC_ID, 2);
		}
		return;
	}
	if ( keyword.find("PRODUCER") != std::string::npos ) {
		m_producer.setEncodedString(value);
		return;
	}
	if ( keyword.find("PRODUCT_ID") != std::string::npos ) {
		m_productId.setEncodedString(value);
		return;
	}
	if ( keyword.find("FLAGS") != std::string::npos ) {
		setFlagsPreV6(value);
		return;
	}
	if ( keyword.find("SAMPLINGDATE") != std::string::npos ) {
		m_samplingDate = value;
		return;
	}
	if ( keyword.find("SAMPLINGMETHOD") != std::string::npos ) {
		IBK::IBK_Message( "An unsigned int must be selected for sampling method! Please check Delphin 6 material file specification!", IBK::MSG_WARNING, FUNC_ID, 2 );
		return;
	}
	if ( keyword.find("SOURCE") != std::string::npos ) {
		m_laboratory.setEncodedString(value);
		return;
	}
	if ( keyword.find("INVESTIGATOR") != std::string::npos ) {
		m_investigator = value;
		return;
	}
	if ( keyword.find("MM_ID") != std::string::npos ) {
		// we don't convert this
//				IBK::IBK_Message( "A new ID must be set manually!", IBK::MSG_WARNING, FUNC_ID, 2 );
		return;
	}
	if ( keyword.find("CLASSIFICATION") != std::string::npos ) {
		IBK::IBK_Message( "CLASSIFICATION is obsolete!", IBK::MSG_WARNING, FUNC_ID, 2 );
		int cat;
		std::stringstream str(value);
		if(str >> cat)
			m_cat = MM::MaterialCategory(static_cast<MM::MaterialCategory::CategoryID>(cat));
		return;
	}
	if ( keyword.find("CATEGORY") != std::string::npos ) {
		m_cat.set(value);
		return;
	}
	if ( keyword.find("MASTERFORMAT1") != std::string::npos ) {
		// we don't convert this
		IBK::IBK_Message( "MASTERFORMAT1 is obsolete!", IBK::MSG_WARNING, FUNC_ID, 2 );
		return;
	}
	if ( keyword.find("MASTERFORMAT2") != std::string::npos ) {
		// we don't convert this
		IBK::IBK_Message( "MASTERFORMAT2N is obsolete!", IBK::MSG_WARNING, FUNC_ID, 2 );
		return;
	}
	if ( keyword.find("MASTERFORMAT3") != std::string::npos ) {
		// we don't convert this
		IBK::IBK_Message( "MASTERFORMAT3 is obsolete!", IBK::MSG_WARNING, FUNC_ID, 2 );
		return;
	}
	if ( keyword.find("COMMENTS") != std::string::npos ) {
		m_comments.setEncodedString(value);
		return;
	}
	if ( keyword.find("DBTYPE") != std::string::npos ) {
		m_DBtypes.clear();
		std::vector<std::string> tmpvect;
		unsigned int size = IBK::explode(value, tmpvect, ',');
		for( unsigned int i=0; i<size; ++i) {
			if(tmpvect[i] == "Standard")
				m_DBtypes.push_back(MM::Standard);
			else if(tmpvect[i] == "Demo")
				m_DBtypes.push_back(MM::Demo);
			else if(tmpvect[i] == "Feuchteatlas")
				m_DBtypes.push_back(MM::Feuchteatlas);
			else if(tmpvect[i] == "Haacke")
				m_DBtypes.push_back(MM::Haacke);
			else if(tmpvect[i] == "Knauf")
				m_DBtypes.push_back(MM::Knauf);
			else if(tmpvect[i] == "iQLator-Demo")
				m_DBtypes.push_back(MM::IQLatorDemo);
			else if(tmpvect[i] == "iQLator-Pro")
				m_DBtypes.push_back(MM::IQLatorPro);
			else if(tmpvect[i] == "iQLator-Standard")
				m_DBtypes.push_back(MM::IQLatorStandardBasic);
			else if(tmpvect[i] == "iQLator-Standard::Bauplatten")
				m_DBtypes.push_back(MM::IQLatorStandardBuildingBoards);
			else if(tmpvect[i] == "iQLator-Standard::Betone")
				m_DBtypes.push_back(MM::IQLatorStandardConcrete);
			else if(tmpvect[i] == "iQLator-Standard::Daemmstoffe")
				m_DBtypes.push_back(MM::IQLatorStandardInsulation);
			else if(tmpvect[i] == "iQLator-Standard::Folien")
				m_DBtypes.push_back(MM::IQLatorStandardFoils);
			else if(tmpvect[i] == "iQLator-Standard::Haacke_Cellco")
				m_DBtypes.push_back(MM::IQLatorStandardHaacke);
			else if(tmpvect[i] == "iQLator-Standard::HolzDaemmPlatten")
				m_DBtypes.push_back(MM::IQLatorStandardWoodenInsulationPanels);
			else if(tmpvect[i] == "iQLator-Standard::Mauersteine")
				m_DBtypes.push_back(MM::IQLatorStandardMasonry);
			else if(tmpvect[i] == "iQLator-Standard::MineralDaemmPlatten")
				m_DBtypes.push_back(MM::IQLatorStandardMineralInsulationPanels);
			else if(tmpvect[i] == "iQLator-Standard::Natursteine")
				m_DBtypes.push_back(MM::IQLatorStandardNaturalStone);
			else if(tmpvect[i] == "iQLator-Standard::Naturstoffe")
				m_DBtypes.push_back(MM::IQLatorStandardNaturalMaterials);
			else if(tmpvect[i] == "iQLator-Standard::Putze_1")
				m_DBtypes.push_back(MM::IQLatorStandardPlasterLight);
			else if(tmpvect[i] == "iQLator-Standard::Putze_2")
				m_DBtypes.push_back(MM::IQLatorStandardPlasterHeavy);
			else if(tmpvect[i] == "iQLator-Standard::Sonstiges")
				m_DBtypes.push_back(MM::IQLatorStandardMiscellaneous);
			else if(tmpvect[i] == "iQLator-Standard::Ziegel")
				m_DBtypes.push_back(MM::IQLatorStandardBrick);
		}
		return;
	}
	if ( keyword.find("DATE") != std::string::npos ) {
		// we set the current conversion date
		m_date = IBK::Time::dateRFC2822();
		return;
	}
	if ( keyword.find("COUNTRY") != std::string::npos ) {
		m_country = value;
		return;
	}
	if ( keyword.find("DEPRICATED_STATE") != std::string::npos ) {
		m_deprecatedState = IBK::string2val<int>(value);
		return;
	}
}

void MaterialIdentification::readPreV6( const std::string& data ){

	const char * const FUNC_ID = "[Material::readASCIIPreV6Parameters]";
	std::stringstream in(data);
	std::string line;

	try {
		// now read all the parameters
		while (std::getline(in, line)) {
			readLinePreV6(line);
		}
		m_hatchingType = hatchingFromCategory(m_cat);

	}
	catch (IBK::Exception& ex) {
		throw IBK::Exception(ex, IBK::FormatString("Error reading basic parameters from string:\n-----\n%2\n-----\n").arg(data), FUNC_ID);
	}

}
//---------------------------------------------------------------------------

void MaterialIdentification::readPreV6( const std::vector<std::string>& data ){

	const char * const FUNC_ID = "[Material::readASCIIPreV6Parameters]";

	try {
		// now read all the parameters
		for (unsigned int i=0, size=data.size(); i<size; ++i) {
			readLinePreV6(data[i]);
		}
		m_hatchingType = hatchingFromCategory(m_cat);

	}
	catch (IBK::Exception& ex) {
		std::string message;
		for (unsigned int i=0, size=data.size(); i<size; ++i) {
			message += data[i];
		}
		throw IBK::Exception(ex, IBK::FormatString("Error reading basic parameters from string:\n-----\n%1\n-----\n").arg(message), FUNC_ID);
	}

}
//---------------------------------------------------------------------------


void MaterialIdentification::setFlagsPreV6(const std::string& src) {
	const char * const FUNC_ID = "[MaterialIdentification::setFlagsPreV6] ";
	std::stringstream fstrm(src);
	std::string flag;
	m_flags = 0;
	while (fstrm >> flag) {
		try {
			int fnum = KeywordList::Enumeration("MaterialIdentification::FlagsPreV6", flag);
			switch(fnum){
				case PREV6_AIR			: m_flags |= AIR;			break;
				case PREV6_AIRTIGHT		: m_flags |= AIR_TIGHT;		break;
				case PREV6_VAPORTIGHT	: m_flags |= VAPOR_TIGHT;	break;
				case PREV6_WATERTIGHT	: m_flags |= WATER_TIGHT;	break;
				case PREV6_WOOD			: m_flags |= WOOD;			break;
				case PREV6_WOODBASED	: m_flags |= WOOD_BASED;	break;
				case PREV6_ONLYHYG		: IBK::IBK_Message( "Flag ONLY_HYGROSCOPIC is no longer supported! Skipping it now.", IBK::MSG_WARNING, FUNC_ID ); break;
				default:
					IBK::IBK_Message( "Unknown flag found! Skipping it for now...", IBK::MSG_WARNING, FUNC_ID, 3);
			}
		}
		catch (std::exception& ex) {
//			converters do not throw exceptions ! since user must master all issues and converters shouldn't stall!
			IBK::IBK_Message( ex.what(), IBK::MSG_ERROR, FUNC_ID, 1);
		}
	}
}
//---------------------------------------------------------------------------


} // namespace MM
