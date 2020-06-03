#include "MM_ThermalTransport.h"

#include <iomanip>
#include <iterator>
#include <algorithm>

#include <IBK_Constants.h>
#include <IBK_messages.h>
#include <IBK_StringUtils.h>
#include <IBK_FormatString.h>
#include <IBK_InputOutput.h>
#include <IBK_Exception.h>
#include <IBK_assert.h>
#include <IBK_algorithm.h>

#include "MM_Constants.h"
#include "MM_Material.h"
#include "MM_Types.h"
#include "MM_KeywordList.h"
#include "MM_Utils.h"

namespace MM {


void ThermalTransport::read(const std::string& data) {
	const char * const FUNC_ID = "[ThermalTransport::read]";

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
				if (!KeywordList::KeywordExists("ThermalTransport::Functions", value)) {
					IBK::IBK_Message("Unknown function type '"+value+"'.", IBK::MSG_WARNING, FUNC_ID, 3);
					continue;
				}
				Functions fid = (Functions)KeywordList::Enumeration("ThermalTransport::Functions", value);
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
		throw IBK::Exception(ex, "Error reading thermal transport data!", FUNC_ID);
	}
}
//---------------------------------------------------------------------------

void ThermalTransport::read(const std::vector<std::string>& data) {
	const char * const FUNC_ID = "[ThermalTransport::read]";

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
				if (!KeywordList::KeywordExists("ThermalTransport::Functions", value)) {
					IBK::IBK_Message("Unknown function type '"+value+"'.", IBK::MSG_WARNING, FUNC_ID, 3);
					continue;
				}
				Functions fid = (Functions)KeywordList::Enumeration("ThermalTransport::Functions", value);
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
		throw IBK::Exception(ex, "Error reading thermal transport data!", FUNC_ID);
	}
}
//---------------------------------------------------------------------------


void ThermalTransport::readBinary(std::istream& in) {

	for (unsigned int i=0; i<NUM_Functions; ++i)
		m_spline[i].readBinary(in);

	// read models
	readModelsBinary(in);

}
//---------------------------------------------------------------------------


void ThermalTransport::write(std::ostream& out, unsigned int indent, MaterialDirection direction, bool writeEmptyKeywords ) const {
	const char * const FUNC_ID = "[ThermalTransport::write]";

	// only write if we have data
	if (m_models.empty() && IBK::all_empty(m_spline, m_spline + NUM_Functions) && !writeEmptyKeywords)
		return;

	try {
		std::string istr(indent, ' ');
		out << '\n' << istr << '[' << KeywordList::Keyword("Material::Sections", Material::HEAT_TRANSPORT );
		switch ( direction ) {
			case U_DIRECTION : out << "]\n";  break;
			case V_DIRECTION : out << "_V]\n";  break;
			case W_DIRECTION : out << "_W]\n";  break;
			default:
				throw IBK::Exception("Invalid material transport direction type!", FUNC_ID);
		}

		istr += "  ";
		for (unsigned int i=0; i<NUM_Functions; ++i) {
			if (m_spline[i].empty()) {
				if (writeEmptyKeywords) {
					write_keyword(out, istr, MAX_KW_LEN(), "FUNCTION") << KeywordList::Keyword("ThermalTransport::Functions", i) << std::endl;
					out << istr << "0" << std::endl;
					out << istr << "0" << std::endl;
				}
			}
			else
				write_spline(out, istr, MAX_KW_LEN(), m_spline[i], KeywordList::Keyword("ThermalTransport::Functions", i));
		}
		out << std::endl;

		// write model stuff
		writeModels(out, indent+2);
	}
	catch (std::exception& ex) {
		IBK::IBK_Message( ex.what(), IBK::MSG_ERROR, FUNC_ID );
		// Don't throw here!
	}
}
//---------------------------------------------------------------------------


void ThermalTransport::writeBinary(std::ostream& out) const{

	for (unsigned int i=0; i<NUM_Functions; ++i)
		m_spline[i].writeBinary(out);

	// write model
	writeModelsBinary(out);

}
//---------------------------------------------------------------------------


void ThermalTransport::clear() {
	MaterialFunctionBase::clear();

	m_type = NUM_ThermalTransportType;
	IBK::clear(m_spline, m_spline + NUM_Functions);
}
//---------------------------------------------------------------------------


void ThermalTransport::prepareCalculation(const Material* mat, MaterialDirection direction) {
	IBK_ASSERT(mat != NULL);
	const char * const FUNC_ID = "[ThermalTransport::prepareCalculation]";

	IBK::IBK_Message("Initializing thermal transport functions.\n", IBK::MSG_PROGRESS, FUNC_ID, 3);
	IBK::MessageIndentor indent; (void)indent;

	m_type = NUM_ThermalTransportType;

	try {

		// in case we have normalized spline data, denormalize all functions first
		denormalize(mat);

		Material::ParaTransport p;
		switch (direction) {
			case U_DIRECTION : p = Material::MP_LAMBDA; break;
			case V_DIRECTION : p = Material::MP_LAMBDA_V; break;
			case W_DIRECTION : p = Material::MP_LAMBDA_W; break;
			default: throw IBK::Exception("Invalid value for material direction.", FUNC_ID);
		}
		if (!checkTransportParameter(mat, p, "thermal transport model", FUNC_ID, 1e-20))
			return;
		m_lambdaDry = mat->m_paraTransport[p].value;

		// check for available parametrization
		if (!m_spline[F_lbd_Thetal].empty() && !m_spline[F_lbd_T].empty()) {

			m_type = ST_LBD_T_THETA_L_SPLINE;

		}
		else if (!m_spline[F_lbd_Thetal].empty()) {

			m_type = ST_LBD_THETA_L_SPLINE;

		}
		else if (!m_spline[F_lbd_T].empty()) {

			m_type = ST_LBD_T_SPLINE;

		}
		else {
			// check for parameters needed for the linear model

			m_type = ST_LBD_THETA_L_LINEAR;
		}

		// finally check the resulting functions for validity
		checkLimits(mat);

	}
	catch (IBK::Exception & ex) {
		throw IBK::Exception(ex, "Error initializing thermal transport functionality for calculation.", FUNC_ID);
	}
}
//---------------------------------------------------------------------------



void ThermalTransport::checkLimits(const Material* mat) {
	IBK_ASSERT(mat != NULL);
	const char * const FUNC_ID = "[ThermalTransport::checkLimits]";

	double val, EPS = 1e-3;
	double ThetaEffective = mat->m_paraStorage[Material::MP_THETA_EFF].value;

	switch (m_type) {
		case ST_LBD_THETA_L_SPLINE : {
			val = m_spline[F_lbd_Thetal].x().front();
			if (std::fabs(val) > EPS)
				IBK::IBK_Message(IBK::FormatString("Material '%1' has an invalid lambda(Theta_l) spline:\n"
								 "The minimum Theta_l value (%2 m3/m3) should be zero!")
								 .arg(mat->m_identification.m_name.string("en")).arg(val),
								 IBK::MSG_WARNING, FUNC_ID, 2);
			val = m_spline[F_lbd_Thetal].x().back();
			if (std::fabs(val - ThetaEffective) > EPS)
				IBK::IBK_Message(IBK::FormatString("Material '%1' has an invalid lambda(Theta_l) spline:\n"
								 "The maximum Thetal value (%2 m3/m3) should be equal to 'THETA_EFF' (%3 m3/m3)!")
								 .arg(mat->m_identification.m_name.string("en")).arg(val).arg(ThetaEffective),
								 IBK::MSG_WARNING, FUNC_ID, 2);
			// check the values
			val = m_spline[F_lbd_Thetal].y().front();
			if (std::fabs(val - m_lambdaDry) > EPS)
				IBK::IBK_Message(IBK::FormatString("Material '%1' has an invalid lambda(Theta_l) spline:\n"
								 "The lambda-value (%2 W/mK) at zero must match LAMBDA (%3 W/mK)!")
								 .arg(mat->m_identification.m_name.string("en")).arg(val).arg(m_lambdaDry),
								 IBK::MSG_WARNING, FUNC_ID, 2);
		} break;

		case ST_LBD_T_SPLINE : {
			// check the values
			val = m_spline[F_lbd_T].value(23);
			if (std::fabs(val - m_lambdaDry) > EPS)
				IBK::IBK_Message(IBK::FormatString("Material '%1' has an invalid lambda(T) spline:\n"
									"The lambda-value (%2 W/mK) at 23 C must match LAMBDA (%3 W/mK)!")
								 .arg(mat->m_identification.m_name.string("en")).arg(val).arg(m_lambdaDry),
								 IBK::MSG_WARNING, FUNC_ID, 2);
		} break;

		case ST_LBD_T_THETA_L_SPLINE : {
			if (m_lambdaDry <= 0) {
				throw IBK::Exception( IBK::FormatString("Material '%1' has an invalid lambda dry-value (negativ or zero).\n")
							.arg(mat->m_identification.m_name.string("en")), FUNC_ID);
			}
			// check the values
			// T_23C = 1, Ol_0 = 1
			val = m_spline[F_lbd_T].value(23);
			if (val > 1 - EPS && val < 1 + EPS)
				IBK::IBK_Message(IBK::FormatString("Material '%1' has an invalid lambda(T) spline:\n"
								 "The lambda(T) scaling value (%2) at 23 C must be 1!")
								 .arg(mat->m_identification.m_name.string("en")).arg(val),
								 IBK::MSG_WARNING, FUNC_ID, 2);
			val = m_spline[F_lbd_Thetal].value(0);
			if (val > 1 - EPS && val < 1 + EPS)
				IBK::IBK_Message(IBK::FormatString("Material '%1' has an invalid lambda(Theta_l) spline:\n"
								 "The lambda(Theta_l) scaling value (%2) at 0 m3/m3 must be 1!")
								 .arg(mat->m_identification.m_name.string("en")).arg(val),
								 IBK::MSG_WARNING, FUNC_ID, 2);
			break;
		}

		// no test for the default model needed
		case ST_LBD_THETA_L_LINEAR :
			// check lambdadry
			if (m_lambdaDry <= 0) {
				throw IBK::Exception( IBK::FormatString("Material '%1' has an invalid lambda dry-value (negativ or zero).\n")
							.arg(mat->m_identification.m_name.string("en")), FUNC_ID);
			}

		default					: break;
	}
}
//---------------------------------------------------------------------------


double ThermalTransport::lambda(double Thetal, double T, double ThetaIce) const {

	IBK_ASSERT_X( (m_type==ST_LBD_THETA_L_SPLINE || m_type==ST_LBD_T_SPLINE || m_type==ST_LBD_T_THETA_L_SPLINE || m_type==ST_LBD_THETA_L_LINEAR ),
				  "Unsupported type of thermal transport model not implemented or missing initialisation found.");

	double val;
	switch (m_type) {
		// moisture dependent lambda
		case ST_LBD_THETA_L_SPLINE :
			val = m_spline[F_lbd_Thetal].value(Thetal);
			val += IBK::LAMBDA_ICE * ThetaIce;
			break;

		// temperature dependent lambda
		case ST_LBD_T_SPLINE  :
			val = m_spline[F_lbd_T].value(T-273.15);
			break;

		// temperature and moisture dependent lambda
		case ST_LBD_T_THETA_L_SPLINE  :
			val = m_lambdaDry*m_spline[F_lbd_T].value(T-273.15)*m_spline[F_lbd_Thetal].value(Thetal);
			val += IBK::LAMBDA_ICE * ThetaIce;
			break;

		// standard linear model (moisture dependent)
		case ST_LBD_THETA_L_LINEAR :
			val = m_lambdaDry + 0.56 * Thetal;
			val += IBK::LAMBDA_ICE * ThetaIce;
			break;

		default:
			val = 0.0;
	}
	return val;
}
//---------------------------------------------------------------------------


// Comparison operator == between two mrc data sets.
bool operator==(const ThermalTransport& lhs, const ThermalTransport& rhs) {
	if (lhs.thermalTransportType() != rhs.thermalTransportType()) return false;
	for (unsigned int i=0; i < ThermalTransport::NUM_Functions; ++i)
		if (lhs.m_spline[i] != rhs.m_spline[i]) return false;

	return lhs.m_models == rhs.m_models;
}
//---------------------------------------------------------------------------


void ThermalTransport::denormalize(const Material* mat) {
	const char * const FUNC_ID = "[ThermalTransport::denormalize]";
	IBK_ASSERT(mat != NULL);
	double EPS=1e-3;

	// denormalize if only Lambda(Ol) is given
	if (!m_spline[F_lbd_Thetal].empty() && m_spline[F_lbd_T].empty()) {
		bool skip = false;
		// x must be given in the range from [0..1]
		skip |= (std::fabs(m_spline[F_lbd_Thetal].x().front()   ) > EPS);
		skip |= (std::fabs(m_spline[F_lbd_Thetal].x().back() - 1) > EPS);
		// y must be given in the range from [1..max]
		skip |= (std::fabs(m_spline[F_lbd_Thetal].y().front() - 1) > EPS);
		if (!skip) {
			// x is given in the range from [0..1], multiply by THETA_POR
			std::vector<double> vx(m_spline[F_lbd_Thetal].x());
			std::vector<double> vy(m_spline[F_lbd_Thetal].y());
			for( std::size_t i=0, size = vx.size(); i<size; ++i) {
				vx[i] *= mat->m_paraStorage[Material::MP_THETA_POR].value;
				vy[i] *= mat->m_paraTransport[Material::MP_LAMBDA].value;
			}
			IBK::IBK_Message("Denormalizing lambda(Theta_l) spline.\n", IBK::MSG_PROGRESS, FUNC_ID, 2);
			try {
				m_spline[F_lbd_Thetal].setValues(vx, vy);
			}
			catch (IBK::Exception & ex) {
				throw IBK::Exception(ex, "Error creating denormalized lambda(Theta_l) spline.", FUNC_ID);
			}
		}
	}

	// denormalize if only Lambda(T) is given
	if (m_spline[F_lbd_Thetal].empty() && !m_spline[F_lbd_T].empty()) {
		bool skip = false;
		// we have to make the spline here since we use it for calculation
		// of the reference point at 23 C
		// y must be given as [..1..], reference point is 23 C
		skip |= (std::fabs(m_spline[F_lbd_T].value(IBK::T_REF_23) - 1) > EPS);
		if (!skip) {
			// y is given as [..1..], reference point is 23 C, multiply by LAMBDA
			std::vector<double> vx(m_spline[F_lbd_Thetal].x());
			std::vector<double> vy(m_spline[F_lbd_Thetal].y());
			for( size_t i=0, size = vy.size(); i<size; ++i) {
				vy[i] *= mat->m_paraTransport[Material::MP_LAMBDA].value;
			}
			IBK::IBK_Message("Denormalizing lambda(T) spline.\n", IBK::MSG_PROGRESS, FUNC_ID, 2);
			try {
				m_spline[F_lbd_Thetal].setValues(vx, vy);
			}
			catch (IBK::Exception & ex) {
				throw IBK::Exception(ex, "Error creating denormalized lambda(T) spline.", FUNC_ID);
			}
		}
	}

	// if both functions are given, we have the mixed model and do not normalize the values
}


} // namespace MM

