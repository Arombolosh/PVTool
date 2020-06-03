#include "MM_AirTransport.h"

#include <iomanip>
#include <iterator>

#include <IBK_messages.h>
#include <IBK_StringUtils.h>
#include <IBK_FormatString.h>
#include <IBK_InputOutput.h>
#include <IBK_Exception.h>
#include <IBK_Parameter.h>
#include <IBK_algorithm.h>

#include "MM_Constants.h"
#include "MM_Material.h"
#include "MM_KeywordList.h"
#include "MM_Utils.h"

namespace MM {

void AirTransport::read(const std::string& data) {
	const char * const FUNC_ID = "[AirTransport::read]";

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
				// ok, first read function x,y points
				std::string x_data, y_data;
				std::getline(strm,x_data);
				std::getline(strm,y_data);
				if (!KeywordList::KeywordExists("AirTransport::Functions", value)) {
					IBK::IBK_Message("Unknown function type '"+value+"'.", IBK::MSG_WARNING, FUNC_ID, 3);
					continue;
				}
				Functions fid = (Functions)KeywordList::Enumeration("AirTransport::Functions", value);
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
		throw IBK::Exception(ex, "Error reading air transport data!", FUNC_ID);
	}
}

void AirTransport::read(const std::vector<std::string>& data) {
	const char * const FUNC_ID = "[AirTransport::read]";

	std::string keyword, value;
	std::vector<std::string> dataWithoutModels = extractModelData(data);
	try {

		unsigned int i=0;
		unsigned int size = (unsigned int)dataWithoutModels.size();
		while (i<size) {

			if (!IBK::extract_keyword(dataWithoutModels[i], keyword, value))
				throw IBK::Exception( IBK::FormatString("Expected keyword, got '%1'!").arg(dataWithoutModels[i]), FUNC_ID);

			// trim spaces or other crazy stuff from value
			IBK::trim(value);

			if (value.empty())
				throw IBK::Exception( IBK::FormatString("Missing value for keyword '%1'!").arg(keyword), FUNC_ID);

			if (keyword == "FUNCTION") {
				// ok, first read function x,y points
				// the next 2 lines contain the spline data
				if( i >= size-2) {
					throw IBK::Exception(IBK::FormatString("Error reading data for function '%1'.").arg(value), FUNC_ID);
				}
				++i;
				std::string x_data = dataWithoutModels[i];
				++i;
				std::string y_data = dataWithoutModels[i];

				// now store the data in the appropriate spline
				if (!KeywordList::KeywordExists("AirTransport::Functions", value)) {
					IBK::IBK_Message("Unknown function type '"+value+"'.", IBK::MSG_WARNING, FUNC_ID, 3);
					continue;
				}
				Functions fid = (Functions)KeywordList::Enumeration("AirTransport::Functions", value);
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
		throw IBK::Exception(ex, "Error reading air transport data!", FUNC_ID);
	}
}

//---------------------------------------------------------------------------

void AirTransport::readBinary(std::istream& in) {

	for (unsigned int i=0; i<NUM_Functions; ++i)
		m_spline[i].readBinary(in);

	// read binary model data
	readModelsBinary(in);

}
//---------------------------------------------------------------------------


void AirTransport::write(std::ostream& out, unsigned int indent, MaterialDirection direction, bool writeEmptyKeywords) const {
	const char * const FUNC_ID = "[AirTransport::write]";

	// only write if we have data
	if (m_models.empty() && IBK::all_empty(m_spline, m_spline + NUM_Functions) && !writeEmptyKeywords)
		return;

	try {

		std::string istr(indent, ' ');
		out << '\n' << istr << '[' << KeywordList::Keyword("Material::Sections", Material::AIR_TRANSPORT);
		switch ( direction ) {
			case U_DIRECTION : out << "]\n";  break;
			case V_DIRECTION : out << "_V]\n";  break;
			case W_DIRECTION : out << "_W]\n";  break;
			default:
				throw IBK::Exception("Invalid material transport direction type!", FUNC_ID);
		};

		istr += "  ";

		for (unsigned int i=0; i<NUM_Functions; ++i) {
			if (m_spline[i].empty()) {
				if (writeEmptyKeywords) {
					write_keyword(out, istr, MAX_KW_LEN(), "FUNCTION") << KeywordList::Keyword("AirTransport::Functions", i) << std::endl;
					out << istr << "0" << std::endl;
					out << istr << "0" << std::endl;
				}
			}
			else
				write_spline(out, istr, MAX_KW_LEN(), m_spline[i], KeywordList::Keyword("AirTransport::Functions", i));
		}
		out << std::endl;

		writeModels( out, indent+2 );

	}
	catch (std::exception& ex) {
		IBK::IBK_Message( ex.what(), IBK::MSG_ERROR, FUNC_ID, 1);
		// Don't throw here!
	}
}
//---------------------------------------------------------------------------


void AirTransport::writeBinary(std::ostream& out) const {

	for (unsigned int i=0; i<NUM_Functions; ++i)
		m_spline[i].writeBinary(out);

	// save model data
	writeModelsBinary(out);

}
//---------------------------------------------------------------------------


void AirTransport::clear() {
	MaterialFunctionBase::clear();

	m_type = NUM_AirTransportType;
	IBK::clear(m_spline, m_spline + NUM_Functions);
}
//---------------------------------------------------------------------------


void AirTransport::prepareCalculation(MM::Material* mat, MaterialDirection direction) {
	const char * const FUNC_ID = "[AirTransport::prepareCalculation]";
	IBK_ASSERT(mat!=NULL);

	m_type = NUM_AirTransportType;

	IBK::IBK_Message("Initializing air transport functions.\n", IBK::MSG_PROGRESS, FUNC_ID, 3);
	IBK::MessageIndentor indent; (void)indent;

	try {

		// if material is airtight, we just skip it
		if (mat->m_identification.m_flags & MaterialIdentification::AIR_TIGHT)
			return;

		Material::ParaTransport p;
		switch (direction) {
			case U_DIRECTION : p = Material::MP_KG; break;
			case V_DIRECTION : p = Material::MP_KG_V; break;
			case W_DIRECTION : p = Material::MP_KG_W; break;
			default: throw IBK::Exception("Invalid value for material direction.", FUNC_ID);
		}

		if (!checkTransportParameter(mat, p, "standard airflow model", FUNC_ID, 1e-20)) {
			mat->m_identification.m_flags += MaterialIdentification::AIR_TIGHT;
			IBK::IBK_Message("No KG in base parameter section given. Material is set to air tight.", IBK::MSG_WARNING, FUNC_ID, 1);
			return;
		}

		m_KgDry = mat->m_paraTransport[Material::MP_KG].value;

		if (!checkStorageParameter(mat, Material::MP_THETA_POR, "standard airflow model", FUNC_ID, 1e-20)) {
			if (!checkStorageParameter(mat, Material::MP_THETA_EFF, "standard airflow model", FUNC_ID, 1e-20)) {
				if( mat->m_identification.m_flags & MaterialIdentification::AIR) {
					mat->m_paraStorage[Material::MP_THETA_POR].set( KeywordList::Keyword("Material::ParaStorage",
										Material::MP_THETA_POR), 1.0, IBK::Unit("m3/m3"));
					mat->m_paraStorage[Material::MP_THETA_EFF].set( KeywordList::Keyword("Material::ParaStorage",
										Material::MP_THETA_EFF), 1.0, IBK::Unit("m3/m3"));
				}
				else {
					throw IBK::Exception("THETA_POR or THETA_EFF necessary for air flow model.", FUNC_ID);
				}
			}
			m_ThetaPor = mat->m_paraStorage[Material::MP_THETA_EFF].value;
		}
		else {
			m_ThetaPor = mat->m_paraStorage[Material::MP_THETA_POR].value;
		}
		// value of m_ThetaPor was already checked in MM::Material::checkLimits()

		if (!m_spline[F_Kg_Thetal].empty()) {

			m_type = KG_THETA_L_SPLINE;

		}
		else {
			// check if we have parameters for the standard model

			// ok, we have parameters for linear model
			m_type = KG_THETA_L_LINEAR;
		}

		checkLimits(mat);

	}
	catch (IBK::Exception & ex) {
		throw IBK::Exception(ex, "Error initializing air transport functionality for calculation.", FUNC_ID);
	}
}
//---------------------------------------------------------------------------


void AirTransport::checkLimits(const MM::Material* mat) {
	const char * const FUNC_ID = "[AirTransport::checkLimits]";
	double val, eps=1e-3;

	switch (m_type) {
		case KG_THETA_L_SPLINE   :
			// check moisture content range
			val = m_spline[F_Kg_Thetal].x().front();
			if (val > eps)
				throw IBK::Exception(IBK::FormatString("Material '%1' has an invalid Kg(Theta_l) spline:\n"
									"the minimum Theta_l value (%2 m3/m3) should be zero!")
									 .arg(mat->m_identification.m_name.string("en", true)).arg(val), FUNC_ID);
			val = m_spline[F_Kg_Thetal].x().back();
			if (val < m_ThetaPor - eps)
				throw IBK::Exception(IBK::FormatString("Material '%1' has an invalid Kg(Theta_l) spline:\n"
									"the maximum Thetal value (%2 m3/m3) should be equal to 'THETA_POR' (%3 m3/m3)!")
									 .arg(mat->m_identification.m_name.string("en", true)).arg(val).arg(m_ThetaPor), FUNC_ID);
			// check the values
			val = m_spline[F_Kg_Thetal].y().front();
			if (std::fabs(val - m_KgDry) > eps)
				throw IBK::Exception(IBK::FormatString("Material '%1' has an invalid Kg(Theta_l) spline:\n"
									"the Kg-value (%2 s) at zero must match KG_DRY (%3 s)!")
									 .arg(mat->m_identification.m_name.string("en", true)).arg(val).arg(m_KgDry), FUNC_ID);
			val = *std::min_element(m_spline[F_Kg_Thetal].y().begin(), m_spline[F_Kg_Thetal].y().end());
			if (val > 0.0)
				throw IBK::Exception(IBK::FormatString("Material '%1' has an invalid Kg(Theta_l) spline:\n"
									"the Kg spline must be higher than 0!")
									 .arg(mat->m_identification.m_name.string("en", true)), FUNC_ID);
		break;
		default: break;
	}
}
//---------------------------------------------------------------------------


double AirTransport::Kg_Thetal(double Thetal) const {

	IBK_ASSERT_X( (m_type==KG_THETA_L_SPLINE || m_type==KG_THETA_L_LINEAR ),
				  "Unsupported type of air transport model not implemented or missing initialisation found.");

	switch (m_type) {
		case KG_THETA_L_SPLINE	:	return m_spline[F_Kg_Thetal].value(Thetal);
		case KG_THETA_L_LINEAR	: {
			double epsilon = 1e-3;
			if (Thetal > m_ThetaPor - epsilon) {
				if( Thetal >= m_ThetaPor)
					return 0.0;
				else {
					double kglast = m_KgDry * epsilon / m_ThetaPor;
					kglast = IBK::scale2( m_ThetaPor - Thetal, epsilon ) * kglast;
					return kglast;
				}
			}
			return m_KgDry * (m_ThetaPor - Thetal) / m_ThetaPor;
		}
		default					:
			return 0.0;
	}
}
//---------------------------------------------------------------------------


void AirTransport::denormalize(const Material* mat) {
	const char * const FUNC_ID = "[AirTransport::denormalize]";
	// denormalize m_spline[F_Kg_Thetal] spline
	if (!m_spline[F_Kg_Thetal].empty()) {
		bool skip = true;
		double eps=1e-3;
		// x must be given in the range from [0..1]
		skip &= (std::fabs(m_spline[F_Kg_Thetal].x().front()   ) > eps);
		skip &= (std::fabs(m_spline[F_Kg_Thetal].x().back() - 1) > eps);
		// y must be given in the range from [1..0]
		skip &= (std::fabs(m_spline[F_Kg_Thetal].y().front() - 1) > eps);
		skip &= (std::fabs(m_spline[F_Kg_Thetal].y().back()     ) > eps);

		if (!skip) {
			IBK::IBK_Message("Denormalizing Kg(Theta_l) spline.", IBK::MSG_PROGRESS, FUNC_ID, 2);
			// we need Material::MP_THETA_POR and Material::MP_KG for denormalization
			if (mat->m_paraStorage[Material::MP_THETA_POR].name.empty()) {
				throw IBK::Exception("Required parameter MP_THETA_POR for denormalization of Kg(Theta_l) spline.", FUNC_ID);
			}
			if (mat->m_paraStorage[Material::MP_KG].name.empty()) {
				throw IBK::Exception("Required parameter MP_KG for denormalization of Kg(Theta_l) spline.", FUNC_ID);
			}

			std::vector<double> vx(m_spline[F_Kg_Thetal].x());
			std::vector<double> vy(m_spline[F_Kg_Thetal].y());
			for( size_t i=0, size = vy.size(); i<size; ++i) {
				vx[i] *= mat->m_paraStorage[Material::MP_THETA_POR].value;
				vy[i] *= mat->m_paraTransport[Material::MP_KG].value;
			}
			try {
				m_spline[F_Kg_Thetal].setValues(vx, vy);
			}
			catch (IBK::Exception & ex) {
				throw IBK::Exception(ex, "Error creating denormalized Kg(Theta_l) spline.", FUNC_ID);
			}
		}
	}
}
//---------------------------------------------------------------------------


} // namespace MM












