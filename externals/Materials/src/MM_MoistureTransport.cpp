#include "MM_MoistureTransport.h"

#include <iomanip>
#include <iterator>
#include <cmath>
#include <sstream>

#include <IBK_messages.h>
#include <IBK_StringUtils.h>
#include <IBK_FormatString.h>
#include <IBK_InputOutput.h>
#include <IBK_assert.h>
#include <IBK_Constants.h>
#include <IBK_Exception.h>
#include <IBK_physics.h>
#include <IBK_algorithm.h>

#include "MM_Constants.h"
#include "MM_Material.h"
#include "MM_KeywordList.h"
#include "MM_Types.h"
#include "MM_Utils.h"

namespace MM {


void MoistureTransport::read(const std::string& data) {
	const char * const FUNC_ID = "[MoistureTransport::read]";

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
				if (!KeywordList::KeywordExists("MoistureTransport::Functions", value)) {
					/// \todo handle old D5 files
					// if (value == "Dv(Ol)") { }
					IBK::IBK_Message( IBK::FormatString("Unknown function type '%1'.").arg(value), IBK::MSG_WARNING, FUNC_ID, 3);
					continue;
				}
				Functions fid = (Functions)KeywordList::Enumeration("MoistureTransport::Functions", value);
				try {
					m_spline[fid].read(x_data, y_data);
				}
				catch (IBK::Exception & ex) {
					throw IBK::Exception(ex, IBK::FormatString("Error reading data for function '%1' while %2.").arg(value).arg(ex.what()), FUNC_ID);
				}
			}
			else if (keyword == "MODEL") {
				// preV6 model found
				readModelsASCII(strm, line);
			}
			else {
				IBK::IBK_Message(IBK::FormatString("Invalid or deprecated keyword '%1' in this section!").arg(keyword), IBK::MSG_WARNING, FUNC_ID, 3);
			}
		}
	}
	catch (IBK::Exception& ex) {
		throw IBK::Exception(ex, "Error reading moisture transport material data!", FUNC_ID);
	}
}
//---------------------------------------------------------------------------

void MoistureTransport::read(const std::vector<std::string>& data) {
	const char * const FUNC_ID = "[MoistureTransport::read]";

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
					throw IBK::Exception(IBK::FormatString("Error reading data for function '%1' because too less lines left.").arg(value), FUNC_ID);
				}
				++i;
				std::string x_data = dataWithoutModels[i];
				++i;
				std::string y_data = dataWithoutModels[i];

				if (!KeywordList::KeywordExists("MoistureTransport::Functions", value)) {
					/// \todo handle old D5 files
					// if (value == "Dv(Ol)") { }
					IBK::IBK_Message( IBK::FormatString("Unknown function type '%1'.").arg(value), IBK::MSG_WARNING, FUNC_ID, 3);
					continue;
				}
				Functions fid = (Functions)KeywordList::Enumeration("MoistureTransport::Functions", value);
				try {
					m_spline[fid].read(x_data, y_data);
				}
				catch (IBK::Exception & ex) {
					throw IBK::Exception(ex, IBK::FormatString("Error reading data for function '%1'.").arg(value), FUNC_ID);
				}
			}
			else if (keyword == "MODEL") {
				// preV6 model found
				if( i >= size-1) {
					throw IBK::Exception(IBK::FormatString("Error reading data for function '%1'.").arg(value), FUNC_ID);
				}
				++i;
				readModelsASCII(dataWithoutModels[i], value);
			}
			else {
				IBK::IBK_Message(IBK::FormatString("Invalid or deprecated keyword '%1' in this section!").arg(keyword), IBK::MSG_WARNING, FUNC_ID, 3);
			}
			++i;
		} // while(i<size)
	}
	catch (IBK::Exception& ex) {
		throw IBK::Exception(ex, "Error reading moisture transport material data!", FUNC_ID);
	}
}
//---------------------------------------------------------------------------


void MoistureTransport::readBinary(std::istream& in) {

	for (unsigned int i=0; i<NUM_Functions; ++i)
		m_spline[i].readBinary(in);

	// read models
	readModelsBinary(in);

}
//---------------------------------------------------------------------------


void MoistureTransport::write(std::ostream& out, unsigned int indent, MaterialDirection direction, bool writeEmptyKeywords) const {
	const char * const FUNC_ID = "[MoistureTransport::write]";

	// only write if we have data
	if (m_models.empty() && IBK::all_empty(m_spline, m_spline + NUM_Functions) && !writeEmptyKeywords)
		return;

	try {

		std::string istr(indent, ' ');
		out << '\n' << istr << '[' << KeywordList::Keyword("Material::Sections", Material::MOISTURE_TRANSPORT);
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
					write_keyword(out, istr, MAX_KW_LEN(), "FUNCTION") << KeywordList::Keyword("MoistureTransport::Functions", i) << std::endl;
					out << istr << "0" << std::endl;
					out << istr << "0" << std::endl;
				}
			}
			else
				write_spline(out, istr, MAX_KW_LEN(), m_spline[i], KeywordList::Keyword("MoistureTransport::Functions", i));
		}
		out << std::endl;

		writeModels( out, indent+2 );
	}
	catch (std::exception& ex) {
		IBK::IBK_Message( ex.what(), IBK::MSG_ERROR, FUNC_ID );
		// Don't throw here!
	}
}
//---------------------------------------------------------------------------


void MoistureTransport::writeBinary(std::ostream& out) const {

	for (unsigned int i=0; i<NUM_Functions; ++i)
		m_spline[i].writeBinary(out);

	// write model
	writeModelsBinary(out);

}
//---------------------------------------------------------------------------


void MoistureTransport::clear() {
	MaterialFunctionBase::clear();

	m_liquidTransportType = NUM_LiquidTransportType;
	m_vaporTransportType = NUM_VaporTransportType;
	IBK::clear(m_spline, m_spline + NUM_Functions);
	m_Psi_Thetal_spline.clear();
}
//---------------------------------------------------------------------------


void MoistureTransport::prepareCalculation(const Material* mat, MaterialDirection direction) {
	IBK_ASSERT(mat != NULL);
	const char * const FUNC_ID = "[MoistureTransport::prepareCalculation]";

	m_liquidTransportType = NUM_LiquidTransportType;
	m_vaporTransportType = NUM_VaporTransportType;

	IBK::IBK_Message("Initializing moisture transport functions.\n", IBK::MSG_PROGRESS, FUNC_ID, 3);
	IBK::MessageIndentor indent; (void)indent;

	try {

		// in case we have normalized spline data, denormalize all functions first
		denormalize(mat);

		// *** LIQUID TRANSPORT ***

		// initialize liquid transport unless this is an air material or it is watertight
		if (!(mat->m_identification.m_flags & MaterialIdentification::AIR) &&
			!(mat->m_identification.m_flags & MaterialIdentification::WATER_TIGHT) )
		{
			// determine which potential and function to use

			// assume pressure as default
			if (!m_spline[F_Kl_Thetal].empty()) {

				m_liquidTransportType = KL_THETAL_SPLINE;

			}
			// if we don't have Kl data, use Dl
			else if (!m_spline[F_Dl_Thetal].empty()) {

				m_liquidTransportType = DL_THETAL_SPLINE;

			}
			else {

				/// \todo implement support for analytical models

				return;
			}

			if (m_liquidTransportType != NUM_LiquidTransportType) {
				checkLiquidFunctionMonotony(mat);
				generateKichhoffPotential(mat);
			}
		}
		else {
			/// \todo handle air materials and liquid transport due to gravity
		}

		// *** VAPOR TRANSPORT ***

		// initialize vapor transport unless this material is vaportight
		if (!(mat->m_identification.m_flags & MaterialIdentification::VAPOR_TIGHT) ) {

			// Theta_por is needed for all vapor transport models
			if( (mat->m_identification.m_flags & MaterialIdentification::AIR)) {
				m_ThetaPor = 1.0;
			}
			else {
				if (!checkStorageParameter(mat, Material::MP_THETA_POR, "standard vapor diffusion model", FUNC_ID, 1e-20))
					return;
				m_ThetaPor = mat->m_paraStorage[Material::MP_THETA_POR].value;
			}

			// determine which potential and function to use

			// assume pressure as default
			if (!m_spline[F_Kv_Thetal].empty()) {

				m_vaporTransportType = KV_THETAL_SPLINE;

			}
			// if we don't have Kv data, use mew
			else if (!m_spline[F_mew_rh].empty()) {

				m_vaporTransportType = MEW_RH_SPLINE;

			}
			else {
				/// \todo implement support for analytical models

				// use standard model, check for required parameters
				Material::ParaTransport p;
				switch (direction) {
					case U_DIRECTION : p = Material::MP_MEW; break;
					case V_DIRECTION : p = Material::MP_MEW_V; break;
					case W_DIRECTION : p = Material::MP_MEW_W; break;
					default: throw IBK::Exception("Invalid value for material direction.", FUNC_ID);
				}
				if (!checkTransportParameter(mat, p, "standard vapor diffusion model", FUNC_ID, 1e-20)) return;
				m_mew = mat->m_paraTransport[p].value;

				m_vaporTransportType = KV_MEW_CONST;
			}
		}
		checkLimits(mat);
	}
	catch (IBK::Exception & ex) {
		throw IBK::Exception(ex, "Error initializing moisture transport functionality for calculation.", FUNC_ID);
	}
}
//---------------------------------------------------------------------------

void MoistureTransport::checkLiquidFunctionMonotony(const Material *mat) {
	IBK_ASSERT(mat != NULL);
	const char * const FUNC_ID = "[MoistureTransport::checkLiquidFunctionMonotony]";

	if( m_liquidTransportType == NUM_LiquidTransportType) {
		IBK::IBK_Message(IBK::FormatString("Liquid transport function for material '%1' missing. Either one of lgKl(Ol) or lgDl(Ol) is required!")
							 .arg(mat->m_identification.m_name),
						 IBK::MSG_WARNING, FUNC_ID, 3);
	}

	// create splines according to selected potential
	switch (m_liquidTransportType) {
		case KL_THETAL_SPLINE : {
			// check that all slopes are positive or 0
			const std::vector<double>& slopes = m_spline[F_Kl_Thetal].slopes();
			for (unsigned int i=0, end = slopes.size(); i<end; ++i) {
				if (slopes[i] < 0) {
					if (MM::Material::m_useStrictConsistencyChecking)
						throw IBK::Exception(IBK::FormatString("Check for monotonic liquid conductivity function failed "
															   "for material '%1'. Invalid spline data for lgKl(Thetal), "
															   "found negative slope in interval #%2.")
											 .arg(mat->m_identification.m_name).arg(i), FUNC_ID);
					else {
						IBK::IBK_Message(IBK::FormatString("Check for monotonic liquid conductivity function failed "
														   "for material '%1'. Invalid spline data for lgKl(Thetal), "
														   "found negative slope in interval #%2.")
										 .arg(mat->m_identification.m_name).arg(i),
										 IBK::MSG_WARNING, FUNC_ID, IBK::VL_STANDARD);
						break;
					}
				}
			}
			break;
		}
		case DL_THETAL_SPLINE : {
			// check that all slopes are positive or 0
			const std::vector<double>& slopes = m_spline[F_Dl_Thetal].slopes();
			for (unsigned int i=0, end = slopes.size(); i<end; ++i) {
				if (slopes[i] < 0) {
					if (MM::Material::m_useStrictConsistencyChecking)
						throw IBK::Exception(IBK::FormatString("Check for monotonic liquid conductivity function failed "
															   "for material '%1'. Invalid spline data for lgDl(Thetal), "
															   "found negative slope in interval #%2.")
											 .arg(mat->m_filename.filename()).arg(i), FUNC_ID);
					else {
						IBK::IBK_Message(IBK::FormatString("Check for monotonic liquid conductivity function failed "
														   "for material '%1'. Invalid spline data for lgDl(Thetal), "
														   "found negative slope in interval #%2.")
										 .arg(mat->m_filename.filename()).arg(i),
										 IBK::MSG_WARNING, FUNC_ID, IBK::VL_STANDARD);
						break;
					}
				}
			}
			break;
		}
		default: break;
	}
}

bool MoistureTransport::checkLimits(const Material* mat) {

	(void)mat;

//	double val;
//	const double eps=1e-3;
//	double ThetaEff = mat->m_paraStorage[Material::MP_THETA_EFF].value;
//	IBK::FormatString err;
//	MoistureTransport::LiquidTransportType type = determineLiqType(mat);
//	if( type == NUM_LiquidTransportType) {
//		errmsg = "Liquid transport function missing. Either one of lgKl(Ol) or lgDl(Ol) is required!";
//		return false;
//	}
//	// create splines according to selected potential
//	switch (type) {
//		case KL_THETAL_SPLINE :
//		{
//			// check moisture content range
//			if (strictrangetest) {
//				val = m_spline[F_Kl_Thetal].x().front();
//				if (std::fabs(val) > eps)
//					err = IBK::FormatString("Material '%1' has an invalid Kl(Ol) spline:\n"
//										"the minimum Ol value (%2 m3/m3) must be zero!").arg(mat->m_identification.m_name.string("en")).arg(val);
//				val = m_spline[F_Kl_Thetal].x().back();
//				if (std::fabs(val - ThetaEff) > eps)
//					err = IBK::FormatString("Material '%1' has an invalid Kl(Ol) spline:\n"
//										"the maximum Ol value (%2 m3/m3) must be equal to 'OEFF' (%3 m3/m3)!").arg(mat->m_identification.m_name.string("en")).arg(val).arg(ThetaEff);
//			}
//			else {
//				val = m_spline[F_Kl_Thetal].x().front();
//				if (val > eps)
//					err = IBK::FormatString("Material '%1' has an invalid Kl(Ol) spline:\n"
//										"the minimum Ol value (%2 m3/m3) should be zero!").arg(mat->m_identification.m_name.string("en")).arg(val);
//				val = m_spline[F_Kl_Thetal].x().back();
//				if (val < ThetaEff-eps)
//					err = IBK::FormatString("Material '%1' has an invalid Kl(Ol) spline:\n"
//										"the maximum Ol value (%2 m3/m3) should be equal to 'OEFF' (%3 m3/m3)!").arg(mat->m_identification.m_name.string("en")).arg(val).arg(ThetaEff);
//			}
//			break;
//		}

//		case DL_THETAL_SPLINE :
//		{
//			// check moisture content range
//			if (strictrangetest) {
//				val = m_spline[F_Dl_Thetal].x().front();
//				if (std::fabs(val) > eps)
//					err = IBK::FormatString("Material '%1' has an invalid Dl(Thetal) spline:\n"
//										"the minimum Thetal value (%2 m3/m3) must be zero!").arg(mat->m_identification.m_name.string("en")).arg(val);
//				val = m_spline[F_Dl_Thetal].x().back();
//				if (std::fabs(val - ThetaEff) > eps)
//					err = IBK::FormatString("Material '%1' has an invalid Dl(Thetal) spline:\n"
//										"the maximum Thetal value (%2 m3/m3) must be equal to 'THETA_EFF' (%3 m3/m3)!").arg(mat->m_identification.m_name.string("en")).arg(val).arg(ThetaEff);
//			}
//			else {
//				val = m_spline[F_Dl_Thetal].x().front();
//				if (val > eps)
//					err = IBK::FormatString("Material '%1' has an invalid Dl(Thetal) spline:\n"
//										"the minimum Thetal value (%2 m3/m3) should be zero!").arg(mat->m_identification.m_name.string("en")).arg(val);
//				val = m_spline[F_Dl_Thetal].x().back();
//				if (val < ThetaEff-eps)
//					err = IBK::FormatString("Material '%1' has an invalid Dl(Thetal) spline:\n"
//										"the maximum Thetal value (%2 m3/m3) should be equal to 'THETA_EFF' (%3 m3/m3)!").arg(mat->m_identification.m_name.string("en")).arg(val).arg(ThetaEff);
//			}
//			break;
//		}
//		default: break;
//	}

// // vapor diffusion

//	switch (determineVapType(mat)) {
//		case KV_THETAL_SPLINE :
//		{
//			// check moisture content range
//			if (strictrangetest) {
//				val = m_spline[F_Kv_Thetal].x().front();
//				if (std::fabs(val) > eps)
//					err = IBK::FormatString("Material '%1' has an invalid Kv(Thetal) spline:\n"
//										"the first Thetal value (%2 m3/m3) must = 0!").arg(mat->m_identification.m_name.string("en")).arg(val);
//				val = m_spline[F_Kv_Thetal].x().back();
//				if (std::fabs(val - m_thetaPor) > eps)
//					err = IBK::FormatString("Material '%1' has an invalid Kv(Thetal) spline:\n"
//										"the last Thetal value (%2 m3/m3) must be = 'THETA_POR' (%3 m3/m3)!").arg(mat->m_identification.m_name.string("en")).arg(val).arg(m_thetaPor);
//			}
//			else {
//				val = m_spline[F_Kv_Thetal].x().front();
//				if (val > eps)
//					err = IBK::FormatString("Material '%1' has an invalid Kv(Thetal) spline:\n"
//										"the first Thetal value (%2 m3/m3) should = 0!").arg(mat->m_identification.m_name.string("en")).arg(val);
//				val = m_spline[F_Kv_Thetal].x().back();
//				if (val < m_thetaPor-eps)
//					err = IBK::FormatString("Material '%1' has an invalid Kv(Thetal) spline:\n"
//										"the last Thetal value (%2 m3/m3) should be = 'THETA_POR' (%3 m3/m3)!").arg(mat->m_identification.m_name.string("en")).arg(val).arg(m_thetaPor);
//			}
//			// check the values only if MEW value is given
//			if (!mat->m_paraTransport[Material::MP_MEW].name.empty()) {
//				val = m_spline[F_Kv_Thetal].y().front();
//				double Kv0 = IBK::f_log10(IBK::DV_AIR() / (m_mew * IBK::R_VAPOR() * IBK::T_REF_23()));
//				if (std::fabs(val - Kv0)/Kv0 > eps)
//					err = IBK::FormatString("Material '%1' has an invalid Kv(Thetal) spline:\n"
//										"the first Kv-value (%2 s) must match MEW!").arg(mat->m_identification.m_name.string("en")).arg(val);
//			}
//			break;
//		}
//		default: break;
//	}
//	errMsg = err.str();
//	return errMsg.empty();

	return true;
}


void MoistureTransport::denormalize(const Material* mat) {
	const char * const FUNC_ID = "[MoistureTransport::denormalize]";
	double thetaEff = mat->m_paraStorage[Material::MP_THETA_EFF].value;
	double eps=1e-3;

	// lgKl(Thetal) spline
	if (!m_spline[F_Kl_Thetal].empty()) {
		bool skip = false;
		// x must be given in the range from [0..1]
		skip |= std::fabs(m_spline[F_Kl_Thetal].x().front()   ) > eps;
		skip |= std::fabs(m_spline[F_Kl_Thetal].x().back() - 1) > eps;
		// y must be given in the range from [0..1], log10 values are given, y0 must be <= -5
		skip |= m_spline[F_Kl_Thetal].y().front()  > -5;
		skip |= std::fabs(m_spline[F_Kl_Thetal].y().back())  > eps;

		if (!skip) {
			// y is given in the range from [0..1], multiply by KLEFF
			// log10 values are given, do log scaling, i.e. just add
			if (mat->m_paraTransport[Material::MP_KLEFF].name.empty())
				throw IBK::Exception( IBK::FormatString(
					"Require KLEFF parameter for denormalization of Kl_Thetal spline."), FUNC_ID);
			if (mat->m_paraTransport[Material::MP_KLEFF].value <= 0)
				throw IBK::Exception( IBK::FormatString(
					"Invalid KLEFF value (%1 s) for denormalization of Kl_Thetal spline, must be > 0 s.")
									  .arg(mat->m_paraTransport[Material::MP_KLEFF].value), FUNC_ID);
			const double logKleff = IBK::f_log10(mat->m_paraTransport[Material::MP_KLEFF].value);
			std::vector<double> vx(m_spline[F_Kl_Thetal].x());
			std::vector<double> vy(m_spline[F_Kl_Thetal].y());
			for( size_t i=0, size = vy.size(); i<size; ++i) {
				vx[i] *= thetaEff;
				vy[i] += logKleff;
			}
			IBK::IBK_Message("Denormalizing Kl(Theta_l) spline.\n", IBK::MSG_PROGRESS, FUNC_ID, 3);
			m_spline[F_Kl_Thetal].setValues(vx, vy);
		}
	}

	// lgDl(Thetal) spline
	if (!m_spline[F_Dl_Thetal].empty()) {
		bool skip = false;
		// x must be given in the range from [0..1]
		skip |= std::fabs(m_spline[F_Dl_Thetal].x().front()   ) > eps;
		skip |= std::fabs(m_spline[F_Dl_Thetal].x().back() - 1) > eps;
		// y must be given in the range from [0..1], log10 values are given, y0 must be <= -5
		skip |= m_spline[F_Dl_Thetal].y().front()  > -5;
		skip |= std::fabs(m_spline[F_Dl_Thetal].y().back())  > eps;

		if (!skip) {
			// y is given in the range from [0..1], multiply by DLEFF
			// log10 values are given, do log scaling, i.e. just add
			if (mat->m_paraTransport[Material::MP_DLEFF].name.empty())
				throw IBK::Exception( IBK::FormatString(
					"Require DLEFF parameter for denormalization of Dl_Thetal spline."), FUNC_ID);
			if (mat->m_paraTransport[Material::MP_DLEFF].value <= 0)
				throw IBK::Exception( IBK::FormatString(
					"Invalid DLEFF value (%1 m2/s) for denormalization of Dl_Thetal spline, must be > 0 m2/s.")
									  .arg(mat->m_paraTransport[Material::MP_DLEFF].value), FUNC_ID);

			const double logDleff = IBK::f_log10(mat->m_paraTransport[Material::MP_DLEFF].value);
			std::vector<double> vx(m_spline[F_Dl_Thetal].x());
			std::vector<double> vy(m_spline[F_Dl_Thetal].y());
			for( size_t i=0, size = vy.size(); i<size; ++i) {
				vx[i] *= thetaEff;
				vy[i] += logDleff;
			}
			IBK::IBK_Message("Denormalizing Dl(Theta_l) spline.\n", IBK::MSG_PROGRESS, FUNC_ID, 3);
			m_spline[F_Dl_Thetal].setValues(vx, vy);
		}
	}

	// lgKv(Thetal) spline
	if (!m_spline[F_Kv_Thetal].empty()) {
		bool skip = false;
		// x must be given in the range from [0..1]
		skip |= std::fabs(m_spline[F_Kv_Thetal].x().front()   ) > eps;
		skip |= std::fabs(m_spline[F_Kv_Thetal].x().back() - 1) > eps;
		// y must be given in the range from [1..0],
		// log10 values are given, y0 must be 0, y1 must <= -5
		skip |= std::fabs(m_spline[F_Kv_Thetal].y().front()) > eps;
		skip |= m_spline[F_Kv_Thetal].y().back()        > -5;
		if (!skip) {
			// x is given in the range from [0..1], multiply by OPOR
			std::vector<double> vx(m_spline[F_Kv_Thetal].x());
			std::vector<double> vy(m_spline[F_Kv_Thetal].y());
			if (mat->m_paraTransport[Material::MP_MEW].name.empty())
				throw IBK::Exception( IBK::FormatString(
					"Require MEW parameter for denormalization of Kv_Thetal spline."), FUNC_ID);
			if (mat->m_paraTransport[Material::MP_MEW].value <= 0)
				throw IBK::Exception( IBK::FormatString(
					"Invalid MEW value (%1 -) for denormalization of Kv_Thetal spline, must be > 0.")
									  .arg(mat->m_paraTransport[Material::MP_MEW].value), FUNC_ID);
			// y is given in the range from [1..0], multiply by D_air/(MEW*Rv*T)
			// log10 values are given, do log scaling, i.e. just add
			const double logKv0 = IBK::f_log10(IBK::DV_AIR / (mat->m_paraTransport[Material::MP_MEW].value * IBK::R_VAPOR * IBK::T_REF_23));
			double thetaPor = mat->m_paraStorage[Material::MP_THETA_POR].value;
			for( size_t i=0, size = vy.size(); i<size; ++i) {
				vx[i] *= thetaPor;
				vy[i] += logKv0;
			}
			IBK::IBK_Message("Denormalizing Kv(Theta_l) spline.\n", IBK::MSG_PROGRESS, FUNC_ID, 3);
			m_spline[F_Kv_Thetal].setValues(vx, vy);
		}
	}
}
//---------------------------------------------------------------------------


double MoistureTransport::Kl_Thetal(double Thetal) const {
	double logKl = m_spline[F_Kl_Thetal].value(Thetal);
	return IBK::f_pow10(logKl);
}
//---------------------------------------------------------------------------


double MoistureTransport::Kl_Thetal(double Thetal, double pl, double Psipl, const MM::Material * mat) const {
	// if we have liquid conductivity spline data, directly use this
	if (m_liquidTransportType == KL_THETAL_SPLINE)
		return Kl_Thetal(Thetal);

	// compute Kl numerically, using dPsi/dpl = Kl = (Psi(pl+eps) - Psi(pl))/eps
	const double EPS = 1e-8;
	double deltaPl = std::fabs(pl*EPS) + EPS; // use relative and absolute tolerance to compute deltaPl
	double pl_EPS = pl - deltaPl; // to avoid overshoot subtract
	double Thetal_EPS = mat->m_moistureStorage.Thetal_pl(pl_EPS);
	double Psi_EPS = m_Psi_Thetal_spline.value(Thetal_EPS);
	double Kl = (Psi_EPS - Psipl)/(-deltaPl);
	return Kl;
}
//---------------------------------------------------------------------------


double MoistureTransport::Dl_Thetal(double Thetal) const {
	double logDl = m_spline[F_Dl_Thetal].value(Thetal);
	return IBK::f_pow10(logDl);
}
//---------------------------------------------------------------------------


double MoistureTransport::Psi_Thetal(double Thetal) const {
	return m_Psi_Thetal_spline.value(Thetal);
}
//---------------------------------------------------------------------------


double MoistureTransport::Kv_Thetal(double Thetal, double rh) const {

	IBK_ASSERT_X( (m_vaporTransportType==KV_THETAL_SPLINE || m_vaporTransportType==MEW_RH_SPLINE || m_vaporTransportType==KV_MEW_CONST ),
				  "Unsupported type of Moisture transport model not implemented or missing initialisation found.");


	// The function will return Kv as logarithmic value
	switch (m_vaporTransportType) {
		case KV_THETAL_SPLINE : {
			double logKv = m_spline[F_Kv_Thetal].value(Thetal);
			return IBK::f_pow10(logKv);
		}

		case MEW_RH_SPLINE : {
			double mew = m_spline[F_mew_rh].value(rh);
			double Kv = IBK::DV_AIR / (mew * IBK::R_VAPOR * IBK::T_REF_23) * (m_ThetaPor - Thetal) / m_ThetaPor;
			if (Kv < 1e-20)
				return 1e-20;
			else
				return Kv;
		}

		case KV_MEW_CONST : {
			double Kv = IBK::DV_AIR / (m_mew * IBK::R_VAPOR * IBK::T_REF_23) * (m_ThetaPor - Thetal) / m_ThetaPor;
			if (Kv < 1e-20)
				return 1e-20;
			else
				return Kv;
		}

		default:
			return 0.0;
	}
}
//---------------------------------------------------------------------------


void MoistureTransport::generateKichhoffPotential(const Material* mat) {
	const char * const FUNC_ID = "[MoistureTransport::prepareKichhoffPotential]";

	IBK::IBK_Message("Generating Kirchhoff potential functions.\n", IBK::MSG_PROGRESS, FUNC_ID, 3);
	IBK::MessageIndentor indent; (void)indent;

	switch (m_liquidTransportType) {
		case KL_THETAL_SPLINE :
		{
			// create a Kirchhoff Potential function from Kl(Thetal)
			IBK::IBK_Message("Converting: Kl -> Psi\n", IBK::MSG_PROGRESS, FUNC_ID, 3);
			try {
				// we generate a Psi_l(Theta_l) function by integrating
				// Psi = Psi_0 + \int Kl(pl) dpl
				// with Psi_0 = 0
				std::vector<double> psi_vec;
				std::vector<double> pc_vec;
				std::vector<double> Thetal_vec;
				psi_vec.push_back(0);
				Thetal_vec.push_back(0);
				pc_vec.push_back( -IBK::f_pow10(10) );

				// we integrate from pl = pc = -1e10
				double last_pc = -IBK::f_pow10(10);
				double last_Thetal = mat->m_moistureStorage.Thetal_pl(last_pc);
				double last_Kl = IBK::f_pow10( m_spline[F_Kl_Thetal].value(last_Thetal) );

				// lgKl interpolation, dense spacing in pC space

				// we want to have 1000 values from pC = 10 to pC = 0
				unsigned int n = 1000;
				for (unsigned int j=0; j<n; ++j) {
					double pC = 10 - 10.0/n*(j+1);
					double pc = -IBK::f_pow10(pC);
					double Thetal = mat->m_moistureStorage.Thetal_pl(pc);

					double Kl = IBK::f_pow10(m_spline[F_Kl_Thetal].value(Thetal));
					// integrate with trapozoid rule
					double Psi_diff = 0.5*(Kl + last_Kl)*(pc - last_pc);
					double Psi = psi_vec.back() + Psi_diff;

					// only add point if > than last
					if (Thetal > Thetal_vec.back()) {
						Thetal_vec.push_back(Thetal);
						psi_vec.push_back(Psi);
						pc_vec.push_back(pc);
						last_Kl = Kl;
						last_pc = pc;
						last_Thetal = Thetal;
					}
				}
				// manually add point at saturation
				double ThetalEff = mat->m_paraStorage[Material::MP_THETA_EFF].value;
				if (ThetalEff - last_Thetal > 1e-6) {
					double Psi_diff = last_Kl*(0 - last_pc);
					double Psi = psi_vec.back() + Psi_diff;
					Thetal_vec.push_back(ThetalEff);
					psi_vec.push_back(Psi);
					pc_vec.push_back(0);
				}
				// add point at positive liquid range
				double pl = 200000;
				double ThetalPos = mat->m_moistureStorage.Thetal_pl(pl);
				double Psi_diff = last_Kl*(pl-0);
				double Psi = psi_vec.back() + Psi_diff;
				Thetal_vec.push_back(ThetalPos);
				psi_vec.push_back(Psi);
				pc_vec.push_back(pl);

				m_Psi_Thetal_spline.setValues(Thetal_vec, psi_vec);
//#define DUMP_PSI_THETAL
#ifdef DUMP_PSI_THETAL
				std::ofstream out("Psil_Thetal.txt");
				for (unsigned int i=0; i<Thetal_vec.size(); ++i) {
					out << Thetal_vec[i] << "\t" << pc_vec[i] << "\t" << psi_vec[i] << std::endl;
				}
#endif // DUMP_PSI_THETAL
			}
			catch (IBK::Exception& ex) {
				throw IBK::Exception(ex, "Error while generating Psi spline!", FUNC_ID);
			}
			if(!m_Psi_Thetal_spline.valid()) {
				throw IBK::Exception("Error while generating Psi spline!", FUNC_ID);
			}
			break;
		}

		case DL_THETAL_SPLINE :
		{
			// create a Kirchhoff Potential function from Dl(Ol)
			IBK::IBK_Message("Converting: Dl -> Psi\n", IBK::MSG_PROGRESS, FUNC_ID, 3);
			try {
				// get the original data
				std::vector<double> x_vec = m_spline[F_Dl_Thetal].x();
				std::vector<double> y_vec = m_spline[F_Dl_Thetal].y();
				// now integrate the data
				std::vector<double> psi_vec;
				std::vector<double> Thetal_vec;
				psi_vec.push_back(0);
				Thetal_vec.push_back(0);
				double Thetal_last = 0;
				double Dl_last = 0;
				for (unsigned int i=1; i<x_vec.size(); ++i) {
					double Thetal_current = x_vec[i];
					Thetal_vec.push_back(Thetal_current);
					double Dl_current = IBK::f_pow10( y_vec[i]+3);
					psi_vec.push_back(psi_vec.back() + 0.5*(Dl_last + Dl_current)*(Thetal_current - Thetal_last));
					Thetal_last = Thetal_current;
					Dl_last = Dl_current;
				}

				// add point at positive liquid range
				double pl = 1000;
				double Thetal = mat->m_moistureStorage.Thetal_pl(pl);
				double Psi_diff = (Thetal-Thetal_last)*Dl_last;
				double Psi = psi_vec.back() + Psi_diff;
				Thetal_vec.push_back(Thetal);
				psi_vec.push_back(Psi);

				m_Psi_Thetal_spline.setValues(Thetal_vec, psi_vec);
//#define DUMP_PSI_THETAL
#ifdef DUMP_PSI_THETAL
				std::ofstream out("Psil_Thetal.txt");
				for (unsigned int i=0; i<Thetal_vec.size(); ++i) {
					out << Thetal_vec[i] << "\t" << psi_vec[i] << std::endl;
				}
#endif // DUMP_PSI_THETAL
			}
			catch (IBK::Exception& ex) {
				throw IBK::Exception(ex, "Error while generating Psi spline!", FUNC_ID);
			}
			if(!m_Psi_Thetal_spline.valid()) {
				throw IBK::Exception("Error while generating Psi spline!", FUNC_ID);
			}
			break;
		}
		default: break;
	}
}
//---------------------------------------------------------------------------


} // namespace MM

