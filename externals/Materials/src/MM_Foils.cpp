
#include "MM_Foils.h"

#include <IBK_messages.h>
#include <IBK_StringUtils.h>
#include <IBK_InputOutput.h>

#include "MM_Constants.h"
#include "MM_KeywordList.h"
#include "MM_Utils.h"

namespace MM {

Foils::Foils() :
	m_sd(0),
	m_weightPos(1.0),
	m_weightNeg(1.0),
	m_idPlus(-1),
	m_idMinus(-1),
	m_scalingAPos(0),
	m_scalingBPos(0),
	m_scalingCPos(0),
	m_scalingANeg(0),
	m_scalingBNeg(0),
	m_scalingCNeg(0)
{}

bool Foils::isConstant() const {
	return m_sdPlusSpline.empty();
}

bool Foils::isDirectionDependent() const {
	return !m_sdPlusSpline.empty() && !m_sdMinusSpline.empty();
}

bool Foils::isValid() const {
	return m_sd > 0.0;
}

void Foils::clear() {
	m_name.clear();
	m_sd = 0.0;
	m_sdPlusSpline.clear();
	m_sdMinusSpline.clear();
	m_idPlus = -1;
	m_idMinus = -1;
	m_comment.clear();
	m_weightPos = 1.0;
	m_weightNeg = 1.0;
	m_scalingAPos = 0.0;
	m_scalingBPos = 0.0;
	m_scalingCPos = 0.0;
	m_scalingANeg = 0.0;
	m_scalingBNeg = 0.0;
	m_scalingCNeg = 0.0;
}

void Foils::read(const std::vector<std::string>& data) {
	const char * const FUNC_ID = "[Foils::read]";

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


			if (KeywordList::KeywordExists("Foils::Ident", keyword)) {
				Ident ident = static_cast<Ident>(KeywordList::Enumeration("Foils::Ident", keyword));
				switch(ident) {
					case IF_NAME: m_name.setEncodedString(value); 				break;
					case IF_ID_PLUS: m_idPlus = IBK::string2val<int>(value); 	break;
					case IF_ID_MINUS: m_idMinus = IBK::string2val<int>(value); 	break;
					case IF_COMMENT: m_comment.setEncodedString(value); 		break;
					default: 													break;
				}
			}
			else if (KeywordList::KeywordExists("Foils::Param", keyword)) {
				Param param = static_cast<Param>(KeywordList::Enumeration("Foils::Param", keyword));
				switch(param) {
					case PF_SD_VALUE: m_sd = IBK::string2val<double>(value); 					break;
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
				if( i >= size - 2) {
					throw IBK::Exception(IBK::FormatString("Error reading data for function '%1'.").arg(value), FUNC_ID);
				}
				std::string x_data = data[i++];
				std::string y_data = data[i++];

				// now store the data in the appropriate spline
				if (!KeywordList::KeywordExists("Foils::Functions", value)) {
					IBK::IBK_Message("Unknown function type '"+value+"'.", IBK::MSG_WARNING, FUNC_ID, 3);
					continue;
				}
				Functions function = static_cast<Functions>(KeywordList::Enumeration("Foils::Functions", value));

				switch(function) {
					case F_sdPlus: {
						try {
							m_sdPlusSpline.read(x_data, y_data);
						}
						catch (IBK::Exception & ex) {
							throw IBK::Exception(ex, IBK::FormatString("Error reading data for function '%1'.").arg(value), FUNC_ID);
						}
						break;
					}
					case F_sdMinus: {
						try {
							m_sdMinusSpline.read(x_data, y_data);
						}
						catch (IBK::Exception & ex) {
							throw IBK::Exception(ex, IBK::FormatString("Error reading data for function '%1'.").arg(value), FUNC_ID);
						}
						break;
					}
					default: break;
				}
			}
			else {
				// unknown keyword
			}
		} // while
	}
	catch (IBK::Exception& ex) {
		throw IBK::Exception(ex, "Error reading material identification section!", FUNC_ID);
	}
}

void Foils::write(std::ostream& out, unsigned int indent) const {
	const char * const FUNC_ID = "[Foils::write]";

	// only write if we have data
	if (m_sd == 0.0 && m_sdPlusSpline.size() < 2 )
		return;

	try {
		std::string istr(indent, ' ');
		istr += "  ";

		if ( !m_name.empty() )
			write_keyword(out, istr, MAX_KW_LEN(), KeywordList::Keyword("Foils::Ident", IF_NAME)) << m_name << '\n';

		if ( m_sd > 0 )
			write_keyword(out, istr, MAX_KW_LEN(), KeywordList::Keyword("Foils::Param", PF_SD_VALUE)) << m_sd << '\n';

		write_keyword(out, istr, MAX_KW_LEN(), KeywordList::Keyword("Foils::Ident", IF_ID_PLUS)) << m_idPlus << '\n';

		if (!m_sdPlusSpline.empty()) {
			write_spline(out, istr, MAX_KW_LEN(), m_sdPlusSpline, KeywordList::Keyword("Foils::Functions", F_sdPlus));
		}

		if ( m_weightPos != 1.00 )
			write_keyword(out, istr, MAX_KW_LEN(), KeywordList::Keyword("Foils::Param", PF_WEIGHT_FACTOR_POS)) << m_weightPos << '\n';

		if ( m_scalingAPos != 0.00 || m_scalingBPos != 0.00 || m_scalingCPos != 0.00) {
			write_keyword(out, istr, MAX_KW_LEN(), KeywordList::Keyword("Foils::Param", PF_SCALING_A_POS)) << m_scalingAPos << '\n';
			write_keyword(out, istr, MAX_KW_LEN(), KeywordList::Keyword("Foils::Param", PF_SCALING_B_POS)) << m_scalingBPos << '\n';
			write_keyword(out, istr, MAX_KW_LEN(), KeywordList::Keyword("Foils::Param", PF_SCALING_C_POS)) << m_scalingCPos << '\n';
		}

		if (!m_sdMinusSpline.empty()) {
			write_spline(out, istr, MAX_KW_LEN(), m_sdMinusSpline, KeywordList::Keyword("Foils::Functions", F_sdMinus));
			write_keyword(out, istr, MAX_KW_LEN(), KeywordList::Keyword("Foils::Ident", IF_ID_MINUS)) << m_idMinus << '\n';
		}

		if ( m_weightNeg != 1.00 )
			write_keyword(out, istr, MAX_KW_LEN(), KeywordList::Keyword("Foils::Param", PF_WEIGHT_FACTOR_NEG)) << m_weightNeg << '\n';

		if ( m_scalingANeg != 0.00 || m_scalingBNeg != 0.00 || m_scalingCNeg != 0.00) {
			write_keyword(out, istr, MAX_KW_LEN(), KeywordList::Keyword("Foils::Param", PF_SCALING_A_NEG)) << m_scalingANeg << '\n';
			write_keyword(out, istr, MAX_KW_LEN(), KeywordList::Keyword("Foils::Param", PF_SCALING_B_NEG)) << m_scalingBNeg << '\n';
			write_keyword(out, istr, MAX_KW_LEN(), KeywordList::Keyword("Foils::Param", PF_SCALING_C_NEG)) << m_scalingCNeg << '\n';
		}

		if ( !m_comment.empty() )
			write_keyword(out, istr, MAX_KW_LEN(), KeywordList::Keyword("Foils::Ident", IF_COMMENT)) << m_comment << '\n';
		out << std::endl;
	}
	catch (std::exception& ex) {
		IBK::IBK_Message( ex.what(), IBK::MSG_ERROR, FUNC_ID );
		// Don't throw here!
	}
}

}  // namespace MM

