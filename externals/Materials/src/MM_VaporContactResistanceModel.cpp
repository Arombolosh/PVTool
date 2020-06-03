
#include "MM_VaporContactResistanceModel.h"

#include <IBK_messages.h>
#include <IBK_StringUtils.h>
#include <IBK_InputOutput.h>

#include "MM_Constants.h"
#include "MM_KeywordList.h"
#include "MM_Utils.h"

namespace MM {

VaporContactResistanceModel::VaporContactResistanceModel() :
	m_weightPos(1.0),
	m_weightNeg(1.0),
	m_scalingAPos(0),
	m_scalingBPos(0),
	m_scalingCPos(0),
	m_scalingANeg(0),
	m_scalingBNeg(0),
	m_scalingCNeg(0)
{}

bool VaporContactResistanceModel::isConstant() const {
	return m_spline[F_sdPlus].empty();
}

bool VaporContactResistanceModel::isDirectionDependent() const {
	return !m_spline[F_sdPlus].empty() && !m_spline[F_sdMinus].empty();
}

void VaporContactResistanceModel::read(const std::string& data){
	const char * const FUNC_ID = "[VaporContactResistanceModel::read]";

	std::string line, keyword, value;
	std::stringstream strm(data);
	try {

		while (std::getline(strm,line)) {
			if( line.empty()) {
				continue;
			}

			if (!IBK::extract_keyword(line, keyword, value)){
				throw IBK::Exception( IBK::FormatString("Expected keyword, got '%1'!").arg(line), FUNC_ID);
			}

			// trim spaces or other crazy stuff from value
			IBK::trim(value);

			if (value.empty())
				throw IBK::Exception( IBK::FormatString("Missing value for keyword '%1'!").arg(keyword), FUNC_ID);


			if (KeywordList::KeywordExists("VaporContactResistanceModel::Param", keyword)) {
				Param param = static_cast<Param>(KeywordList::Enumeration("VaporContactResistanceModel::Param", keyword));
				switch(param) {
					case PF_WEIGHT_FACTOR_POS: m_weightPos = IBK::string2val<double>(value); 	break;
					case PF_WEIGHT_FACTOR_NEG: m_weightNeg = IBK::string2val<double>(value); 	break;
					case PF_SCALING_A_POS: m_scalingAPos = IBK::string2val<double>(value); 		break;
					case PF_SCALING_B_POS: m_scalingBPos = IBK::string2val<double>(value); 		break;
					case PF_SCALING_C_POS: m_scalingCPos = IBK::string2val<double>(value); 		break;
					case PF_SCALING_A_NEG: m_scalingANeg = IBK::string2val<double>(value); 		break;
					case PF_SCALING_B_NEG: m_scalingBNeg = IBK::string2val<double>(value); 		break;
					case PF_SCALING_C_NEG: m_scalingCNeg = IBK::string2val<double>(value); 		break;
					default: 																	break;
				}
			}
			else if (keyword == "FUNCTION") {

				std::string x_data, y_data;
				std::getline(strm,x_data);
				std::getline(strm,y_data);

				// now store the data in the appropriate spline
				if (!KeywordList::KeywordExists("VaporContactResistanceModel::Functions", value)) {
					IBK::IBK_Message("Unknown function type '"+value+"'.", IBK::MSG_WARNING, FUNC_ID, 3);
					continue;
				}
				Functions function = static_cast<Functions>(KeywordList::Enumeration("VaporContactResistanceModel::Functions", value));
				try {
					m_spline[function].read(x_data, y_data);
				}
				catch (IBK::Exception & ex) {
					throw IBK::Exception(ex, IBK::FormatString("Error reading data for function '%1'.").arg(value), FUNC_ID);
				}

			}
			else {
				IBK::IBK_Message(IBK::FormatString("Invalid or deprecated keyword '%1' in this section!").arg(keyword), IBK::MSG_WARNING, FUNC_ID, 3);
			}
		} // while
	}
	catch (IBK::Exception& ex) {
		throw IBK::Exception(ex, "Error reading vapor transport resistance section!", FUNC_ID);
	}
}

void VaporContactResistanceModel::read(const std::vector<std::string>& data) {
	const char * const FUNC_ID = "[VaporContactResistanceModel::read]";

	try {
		unsigned int i=0;
		unsigned int size=data.size();
		while (i<size) {
			std::string keyword;
			std::string value;
			const std::string& line = data[i++];  // put line in an extra variable and counts foreward
			if( line.empty()) {
				continue;
			}

			if (!IBK::extract_keyword(line, keyword, value)){
				throw IBK::Exception( IBK::FormatString("Expected keyword, got '%1'!").arg(line), FUNC_ID);
			}

			// trim spaces or other crazy stuff from value
			IBK::trim(value);

			if (value.empty())
				throw IBK::Exception( IBK::FormatString("Missing value for keyword '%1'!").arg(keyword), FUNC_ID);


			if (KeywordList::KeywordExists("VaporContactResistanceModel::Param", keyword)) {
				Param param = static_cast<Param>(KeywordList::Enumeration("VaporContactResistanceModel::Param", keyword));
				switch(param) {
					case PF_WEIGHT_FACTOR_POS: m_weightPos = IBK::string2val<double>(value); 	break;
					case PF_WEIGHT_FACTOR_NEG: m_weightNeg = IBK::string2val<double>(value); 	break;
					case PF_SCALING_A_POS: m_scalingAPos = IBK::string2val<double>(value); 		break;
					case PF_SCALING_B_POS: m_scalingBPos = IBK::string2val<double>(value); 		break;
					case PF_SCALING_C_POS: m_scalingCPos = IBK::string2val<double>(value); 		break;
					case PF_SCALING_A_NEG: m_scalingANeg = IBK::string2val<double>(value); 		break;
					case PF_SCALING_B_NEG: m_scalingBNeg = IBK::string2val<double>(value); 		break;
					case PF_SCALING_C_NEG: m_scalingCNeg = IBK::string2val<double>(value); 		break;
					default: 																	break;
				}
			}
			else if (keyword == "FUNCTION") {

				// the next 2 lines contain the spline data
				if( size - i < 2) {
					throw IBK::Exception(IBK::FormatString("Error reading data for function '%1'.").arg(value), FUNC_ID);
				}
				std::string x_data = data[i++];
				std::string y_data = data[i++];

				// now store the data in the appropriate spline
				if (!KeywordList::KeywordExists("VaporContactResistanceModel::Functions", value)) {
					IBK::IBK_Message("Unknown function type '"+value+"'.", IBK::MSG_WARNING, FUNC_ID, 3);
					continue;
				}
				Functions function = static_cast<Functions>(KeywordList::Enumeration("VaporContactResistanceModel::Functions", value));
				try {
					m_spline[function].read(x_data, y_data);
				}
				catch (IBK::Exception & ex) {
					throw IBK::Exception(ex, IBK::FormatString("Error reading data for function '%1'.").arg(value), FUNC_ID);
				}
			}
			else {
				IBK::IBK_Message(IBK::FormatString("Invalid or deprecated keyword '%1' in this section!").arg(keyword), IBK::MSG_WARNING, FUNC_ID, 3);
			}
		} // while
	}
	catch (IBK::Exception& ex) {
		throw IBK::Exception(ex, "Error reading material vapor transport resistance section!", FUNC_ID);
	}
}

void VaporContactResistanceModel::readBinary(std::istream& in){

	for (unsigned int i=0; i<NUM_Functions; ++i)
		m_spline[i].readBinary(in);

	in.read( reinterpret_cast< char* >( &m_weightPos ), sizeof(m_weightPos));
	in.read( reinterpret_cast< char* >( &m_weightNeg ), sizeof(m_weightNeg));
	in.read( reinterpret_cast< char* >( &m_scalingAPos ), sizeof(m_scalingAPos));
	in.read( reinterpret_cast< char* >( &m_scalingBPos ), sizeof(m_scalingBPos));
	in.read( reinterpret_cast< char* >( &m_scalingCPos ), sizeof(m_scalingCPos));
	in.read( reinterpret_cast< char* >( &m_scalingANeg ), sizeof(m_scalingANeg));
	in.read( reinterpret_cast< char* >( &m_scalingBNeg ), sizeof(m_scalingBNeg));
	in.read( reinterpret_cast< char* >( &m_scalingCNeg ), sizeof(m_scalingCNeg));

}



void VaporContactResistanceModel::write(std::ostream& out, unsigned int indent, MaterialDirection direction, bool writeEmptyKeywords) const {

	(void) direction;

	const char * const FUNC_ID = "[VaporContactResistanceModel::write]";

	try {
		std::string istr(indent, ' ');
		out << '\n' << istr << '[' << KeywordList::Keyword("Material::Sections", Material::VAPOR_CONTACT_RESISTANCE) << ']' << '\n';
		istr += "  ";

		if ( !m_spline[F_sdPlus].empty() || writeEmptyKeywords ) {
			write_spline(out, istr, MAX_KW_LEN(), m_spline[F_sdPlus], KeywordList::Keyword("VaporContactResistanceModel::Functions", F_sdPlus));
		}

		if ( m_weightPos != 1.00 || writeEmptyKeywords )
			write_keyword(out, istr, MAX_KW_LEN(), KeywordList::Keyword("VaporContactResistanceModel::Param", PF_WEIGHT_FACTOR_POS)) << m_weightPos << '\n';

		if ( m_scalingAPos != 0.00 || m_scalingBPos != 0.00 || m_scalingCPos != 0.00) {
			write_keyword(out, istr, MAX_KW_LEN(), KeywordList::Keyword("VaporContactResistanceModel::Param", PF_SCALING_A_POS)) << m_scalingAPos << '\n';
			write_keyword(out, istr, MAX_KW_LEN(), KeywordList::Keyword("VaporContactResistanceModel::Param", PF_SCALING_B_POS)) << m_scalingBPos << '\n';
			write_keyword(out, istr, MAX_KW_LEN(), KeywordList::Keyword("VaporContactResistanceModel::Param", PF_SCALING_C_POS)) << m_scalingCPos << '\n';
		}

		if ( !m_spline[F_sdMinus].empty() || writeEmptyKeywords ) {
			write_spline(out, istr, MAX_KW_LEN(), m_spline[F_sdMinus], KeywordList::Keyword("VaporContactResistanceModel::Functions", F_sdMinus));
		}

		if ( m_weightNeg != 1.00 || writeEmptyKeywords )
			write_keyword(out, istr, MAX_KW_LEN(), KeywordList::Keyword("VaporContactResistanceModel::Param", PF_WEIGHT_FACTOR_NEG)) << m_weightNeg << '\n';

		if ( m_scalingANeg != 0.00 || m_scalingBNeg != 0.00 || m_scalingCNeg != 0.00 || writeEmptyKeywords ) {
			write_keyword(out, istr, MAX_KW_LEN(), KeywordList::Keyword("VaporContactResistanceModel::Param", PF_SCALING_A_NEG)) << m_scalingANeg << '\n';
			write_keyword(out, istr, MAX_KW_LEN(), KeywordList::Keyword("VaporContactResistanceModel::Param", PF_SCALING_B_NEG)) << m_scalingBNeg << '\n';
			write_keyword(out, istr, MAX_KW_LEN(), KeywordList::Keyword("VaporContactResistanceModel::Param", PF_SCALING_C_NEG)) << m_scalingCNeg << '\n';
		}

		out << std::endl;
	}
	catch (std::exception& ex) {
		IBK::IBK_Message( ex.what(), IBK::MSG_ERROR, FUNC_ID );
		// Don't throw here!
	}
}




void VaporContactResistanceModel::writeBinary(std::ostream& out) const{

	for (unsigned int i=0; i<NUM_Functions; ++i)
		m_spline[i].writeBinary(out);

	out.write( reinterpret_cast<const char*>( &m_weightPos ), sizeof(m_weightPos));
	out.write( reinterpret_cast<const char*>( &m_weightNeg ), sizeof(m_weightNeg));
	out.write( reinterpret_cast<const char*>( &m_scalingAPos ), sizeof(m_scalingAPos));
	out.write( reinterpret_cast<const char*>( &m_scalingBPos ), sizeof(m_scalingBPos));
	out.write( reinterpret_cast<const char*>( &m_scalingCPos ), sizeof(m_scalingCPos));
	out.write( reinterpret_cast<const char*>( &m_scalingANeg ), sizeof(m_scalingANeg));
	out.write( reinterpret_cast<const char*>( &m_scalingBNeg ), sizeof(m_scalingBNeg));
	out.write( reinterpret_cast<const char*>( &m_scalingCNeg ), sizeof(m_scalingCNeg));
}


void VaporContactResistanceModel::clear() {
	MaterialFunctionBase::clear();
	IBK::clear(m_spline, m_spline + NUM_Functions);
	m_weightPos = 1.0;
	m_weightNeg = 1.0;
	m_scalingAPos = 0;
	m_scalingBPos = 0;
	m_scalingCPos = 0;
	m_scalingANeg = 0;
	m_scalingBNeg = 0;
	m_scalingCNeg = 0;
}
//---------------------------------------------------------------------------


}	// namespace MM

