#include "MM_ThermalStorage.h"

#include <iomanip>
#include <iterator>
#include <fstream>

#include <IBK_messages.h>
#include <IBK_StringUtils.h>
#include <IBK_FormatString.h>
#include <IBK_InputOutput.h>
#include <IBK_Exception.h>
#include <IBK_math.h>
#include <IBK_Constants.h>
#include <IBK_physics.h>
#include <IBK_algorithm.h>
#include <IBK_StopWatch.h>

#include "MM_Constants.h"
#include "MM_Material.h"
#include "MM_KeywordList.h"
#include "MM_Types.h"
#include "MM_Utils.h"

namespace MM {


void ThermalStorage::read(const std::string& data) {
	const char * const FUNC_ID = "[ThermalStorage::read]";

	std::string line, keyword, value;
	std::string dataWithoutModels = extractModelData(data);
	std::stringstream strm(dataWithoutModels);

	try {

		while (std::getline(strm,line)) {

			if (!IBK::extract_keyword(line, keyword, value))
				throw IBK::Exception( IBK::FormatString("Expected keyword, got '%1'!").arg(line), FUNC_ID);

			// trim spaces or other crazy stuff from value
			IBK::trim(value);

			if (value.empty())
				throw IBK::Exception( IBK::FormatString("Missing value for keyword '%1'!").arg(keyword), FUNC_ID);

			if (keyword == "FUNCTION") {
				// the next 2 lines contain the spline data
				std::string x_data, y_data;
				std::getline(strm,x_data);
				std::getline(strm,y_data);

				// now store the data in the appropriate splines
				if (!KeywordList::KeywordExists("ThermalStorage::Functions", value)) {
					IBK::IBK_Message("Unknown function type '"+value+"'.", IBK::MSG_WARNING, FUNC_ID, 3);
					continue;
				}
				Functions fid = (Functions)KeywordList::Enumeration("ThermalStorage::Functions", value);
				try {
					m_spline[fid].read(x_data, y_data);
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
		throw IBK::Exception(ex, "Error reading moisture storage material data!", FUNC_ID);
	}
}
//---------------------------------------------------------------------------


void ThermalStorage::read(const std::vector<std::string>& data) {
	const char * const FUNC_ID = "[ThermalStorage::read]";

	std::string keyword, value;
	std::vector<std::string>  dataWithoutModels = extractModelData(data);

	try {

		unsigned int i=0;
		unsigned int size=dataWithoutModels.size();
		while (i<size) {

			if (!IBK::extract_keyword(dataWithoutModels[i], keyword, value))
				throw IBK::Exception( IBK::FormatString("Expected keyword, got '%1'!").arg(dataWithoutModels[i]), FUNC_ID);

			// trim spaces or other crazy stuff from value
			IBK::trim(value);

			if (value.empty())
				throw IBK::Exception( IBK::FormatString("Missing value for keyword '%1'!").arg(keyword), FUNC_ID);

			if (keyword == "FUNCTION") {
				// the next 2 lines contain the spline data
				if( i >= size-2) {
					throw IBK::Exception(IBK::FormatString("Error reading data for function '%1'.").arg(value), FUNC_ID);
				}
				++i;
				std::string x_data = dataWithoutModels[i];
				++i;
				std::string y_data = dataWithoutModels[i];

				// now store the data in the appropriate spline
				if (!KeywordList::KeywordExists("ThermalStorage::Functions", value)) {
					IBK::IBK_Message("Unknown function type '"+value+"'.", IBK::MSG_WARNING, FUNC_ID, 3);
					continue;
				}
				Functions fid = static_cast<Functions>(KeywordList::Enumeration("ThermalStorage::Functions", value));
				try {
					m_spline[fid].read(x_data, y_data);
				}
				catch (IBK::Exception & ex) {
					throw IBK::Exception(ex, IBK::FormatString("Error reading data for function '%1'.").arg(value), FUNC_ID);
				}

			}
			else {
				IBK::IBK_Message(IBK::FormatString("Invalid or deprecated keyword '%1' in this section!").arg(keyword), IBK::MSG_WARNING, FUNC_ID, 3);
			}
			++i;
		} // while
	}
	catch (IBK::Exception& ex) {
		throw IBK::Exception(ex, "Error reading moisture storage material data!", FUNC_ID);
	}
}
//---------------------------------------------------------------------------


void ThermalStorage::readBinary( std::istream& in ){

	for (unsigned int i=0; i<NUM_Functions; ++i)
		m_spline[i].readBinary(in);

}
//---------------------------------------------------------------------------


void ThermalStorage::write(std::ostream& out, unsigned int indent, MaterialDirection /* direction */, bool writeEmptyKeywords) const {
	const char * const FUNC_ID = "[ThermalStorage::write]";

	// only write if we have data
	if (m_models.empty() && IBK::all_empty(m_spline, m_spline + NUM_Functions) && !writeEmptyKeywords)
		return;

	try {
		std::string istr(indent, ' ');
		out << '\n' << istr << '[' << KeywordList::Keyword("Material::Sections", Material::THERMAL_STORAGE) << ']' << '\n';
		istr += "  ";

		// we need to write moisture storage functions with increased accuracy
		unsigned int currentPrecision = (unsigned int)out.precision();
		out.precision(12);
		// print all defined spline functions
		for (unsigned int i=0; i<NUM_Functions; ++i) {
			if (m_spline[i].empty()) {
				if (writeEmptyKeywords) {
					write_keyword(out, istr, MAX_KW_LEN(), "FUNCTION") << KeywordList::Keyword("ThermalStorage::Functions", i) << std::endl;
					out << istr << "0" << std::endl;
					out << istr << "0" << std::endl;
				}
			}
			else
				write_spline(out, istr, MAX_KW_LEN(), m_spline[i], KeywordList::Keyword("ThermalStorage::Functions", i));
		}
		out << std::endl;
		out.precision(currentPrecision);

	}
	catch (std::exception& ex) {
		IBK::IBK_Message( ex.what(), IBK::MSG_ERROR, FUNC_ID, 1);
		// Don't throw here!
	}
}
//---------------------------------------------------------------------------


void ThermalStorage::writeBinary(std::ostream& out) const {

	for (unsigned int i=0; i<NUM_Functions; ++i)
		m_spline[i].writeBinary(out);

}
//---------------------------------------------------------------------------


void ThermalStorage::clear() {
	MaterialFunctionBase::clear();

	IBK::clear(m_spline, m_spline + NUM_Functions);
}
//---------------------------------------------------------------------------


void ThermalStorage::prepareCalculation(const Material* mat) {
	const char * const FUNC_ID = "[ThermalStorage::prepareCalculation]";
	IBK_ASSERT(mat != NULL);

	try {
		// if we have a u_T spline, generate reverse function
		m_spline[F_T_u].clear();
		if (!m_spline[F_u_T].empty()) {
			m_spline[F_T_u].setValues( m_spline[F_u_T].y(), m_spline[F_u_T].x() );
			std::string errmsg;
			bool res = m_spline[F_T_u].makeSpline(errmsg);
			if (!res)
				throw IBK::Exception("Error generating inverse thermal storage function T(u):\n" + errmsg, FUNC_ID);
		}
	}
	catch (IBK::Exception & ex) {
		throw IBK::Exception(ex, "Error initializing moisture storage functionality for calculation.", FUNC_ID);
	}
}
//---------------------------------------------------------------------------


} // namespace MM


