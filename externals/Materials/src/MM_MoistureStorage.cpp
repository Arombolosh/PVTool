#include "MM_MoistureStorage.h"

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

unsigned int MoistureStorage::GLOBAL_MRC_ITERATIONS = 0;

CubicMRCTransferFunction::CubicMRCTransferFunction() :
	m_aCubic(0.0),
	m_bCubic(0.0),
	m_cCubic(0.0),
	m_dCubic(0.0),
	m_aSquare(0.0),
	m_bSquare(0.0),
	m_cSquare(0.0),
	m_cubic(true)
{}

void CubicMRCTransferFunction::init(const IBK::LinearSpline& mrcSpline, double endSlope, double thetaEff) {
	double O1 = mrcSpline.value(2.0);
	double O2 = thetaEff;
	double dO2 = endSlope;

	// calculate slope at point -100Pa
	const double pc2M = -105.9;
	const double pc2P = -94.406;
	const double O2M = mrcSpline.value(IBK::f_log10(-pc2M));
	const double O2P = mrcSpline.value(IBK::f_log10(-pc2P));
	double dO1 = (O2M - O2P) / (pc2M - pc2P);

	// calculate cubic parameter
	m_aCubic = (151.0 * O1 + 500000 * O2 + 5050.0 * (101.0 * dO2 + dO1)) / 500151.0;
	m_bCubic = dO2;
	m_cCubic = ((333333.0 * dO1 + 656667.0 * dO2) / 200.0 + 50.0 * (O1 - O2)) / 166717;
	m_dCubic = (O1 - O2 + (9799.0 * dO2 + 10001.0 * dO1) / 200.0) / 500151.0;
	// check for extrema in value area
	double p_2 = m_bCubic / 3.0 / m_dCubic;
	double q = m_cCubic / 3.0 / m_dCubic;
	double inner = p_2 * p_2 - q;
	// exist a extrema?
	if( inner > 0.0) {
		double squareRoot = std::sqrt(p_2 * p_2 - q);
		double x1 = -p_2 + squareRoot;
		double x2 = -p_2 - squareRoot;
		// do we have extrema inside value area?
		if( (x1 < 0 && x1 > -100) || (x2 < 0 && x2 > -100)) {
			m_cubic = false;
			m_aSquare = ((10000.0 * O2 - 199.0 * O1) / 99 - 100.0 * dO1) / 99.0;
			m_bSquare = ((O2 -  O1) * 200.0 / 99.0 - dO1 * 101.0) / 99.0;
			m_cSquare = ((O2 -  O1) / 99.0 - dO1) / 99.0;
		}
	}
}
//---------------------------------------------------------------------------


double CubicMRCTransferFunction::Thetal_pl(double pl) const {
	if(m_cubic )
		return  m_aCubic + pl * (m_bCubic + pl * (m_cCubic + pl * m_dCubic));
	return m_aSquare + pl * (m_bSquare + pl * m_cSquare);
}
//---------------------------------------------------------------------------


void MoistureStorage::read(const std::string& data) {
	const char * const FUNC_ID = "[MoistureStorage::read]";

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
				if (!KeywordList::KeywordExists("MoistureStorage::Functions", value)) {
					IBK::IBK_Message("Unknown function type '"+value+"'.", IBK::MSG_WARNING, FUNC_ID, 3);
					continue;
				}
				Functions fid = (Functions)KeywordList::Enumeration("MoistureStorage::Functions", value);
				try {
					m_spline[fid].read(x_data, y_data);
				}
				catch (IBK::Exception & ex) {
					throw IBK::Exception(ex, IBK::FormatString("Error reading data for function '%1'.").arg(value), FUNC_ID);
				}

			}
			else if (keyword == "MODEL") {
				// preV6 model found
				readModelsASCII(strm, value);

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


void MoistureStorage::read(const std::vector<std::string>& data) {
	const char * const FUNC_ID = "[MoistureStorage::read]";

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
				if (!KeywordList::KeywordExists("MoistureStorage::Functions", value)) {
					IBK::IBK_Message("Unknown function type '"+value+"'.", IBK::MSG_WARNING, FUNC_ID, 3);
					continue;
				}
				Functions fid = static_cast<Functions>(KeywordList::Enumeration("MoistureStorage::Functions", value));
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
		} // while
	}
	catch (IBK::Exception& ex) {
		throw IBK::Exception(ex, "Error reading moisture storage material data!", FUNC_ID);
	}
}
//---------------------------------------------------------------------------


void MoistureStorage::readBinary( std::istream& in ){

	for (unsigned int i=0; i<NUM_Functions; ++i)
		m_spline[i].readBinary(in);

	// read models
	readModelsBinary(in);

}
//---------------------------------------------------------------------------


void MoistureStorage::write(std::ostream& out, unsigned int indent, MaterialDirection /* direction */, bool writeEmptyKeywords) const {
	const char * const FUNC_ID = "[MoistureStorage::write]";

	// only write if we have data
	if (m_models.empty() && IBK::all_empty(m_spline, m_spline + NUM_Functions) && !writeEmptyKeywords)
		return;

	try {
		std::string istr(indent, ' ');
		out << '\n' << istr << '[' << KeywordList::Keyword("Material::Sections", Material::MOISTURE_STORAGE) << ']' << '\n';
		istr += "  ";

		// we need to write moisture storage functions with increased accuracy
		unsigned int currentPrecision = (unsigned int)out.precision();
		out.precision(12);
		// print all defined spline functions
		for (unsigned int i=0; i<NUM_Functions; ++i) {
			if (m_spline[i].empty()) {
				if (writeEmptyKeywords) {
					write_keyword(out, istr, MAX_KW_LEN(), "FUNCTION") << KeywordList::Keyword("MoistureStorage::Functions", i) << std::endl;
					out << istr << "0" << std::endl;
					out << istr << "0" << std::endl;
				}
			}
			else
				write_spline(out, istr, MAX_KW_LEN(), m_spline[i], KeywordList::Keyword("MoistureStorage::Functions", i));
		}
		out << std::endl;
		out.precision(currentPrecision);

		writeModels(out, indent+2);
	}
	catch (std::exception& ex) {
		IBK::IBK_Message( ex.what(), IBK::MSG_ERROR, FUNC_ID, 1);
		// Don't throw here!
	}
}
//---------------------------------------------------------------------------


void MoistureStorage::writeBinary(std::ostream& out) const {

	for (unsigned int i=0; i<NUM_Functions; ++i)
		m_spline[i].writeBinary(out);

	// write model
	writeModelsBinary(out);

}
//---------------------------------------------------------------------------


void MoistureStorage::clear() {
	MaterialFunctionBase::clear();

	m_type = NUM_MoistureStorageType;
	m_Thetal_pl_slope = 1e-7;
	m_air = false;
	IBK::clear(m_spline, m_spline + NUM_Functions);
}
//---------------------------------------------------------------------------

double MoistureStorage::Thetal_pl(double pl, double T) const {

	const char * const FUNC_ID = "[MoistureStorage::Thetal_pl]";

	// safeguard against call of this function with missing parameterization
	// can occur in case of thermal balance equation alone when computing initial
	// moisture-dependent lambda
	if (!(m_type==ST_THETA_L_PC_SPLINE || m_type==ST_THETA_L_PC_MODEL || m_type==ST_THETA_L_RH_SPLINE || m_type==ST_THETA_L_RH_MODEL))
		throw IBK::Exception("Unsupported type of moisture storage function or missing initialisation.", FUNC_ID);

	// trivial case
	if (pl == 0)
		return m_ThetalEff;

	// case dependent evaluation
	switch (m_type) {
		case ST_THETA_L_PC_SPLINE : {
#define MM_USE_CUBIC_MRC_TRANSFER_SPLINE
#ifdef MM_USE_CUBIC_MRC_TRANSFER_SPLINE
			if (pl < -100) {
				// ok, we use the moisture retention spline directly
				double pC = IBK::f_log10(-pl);
				return m_spline[F_Thetal_pC].value(pC);
			}
			if (pl < 0) {
				// ok, we use a cubic spline as adoption function from spline to linear interpolation
				return m_cubicMRCTransfer.Thetal_pl(pl);
			}
#else // MM_USE_CUBIC_MRC_TRANSFER_SPLINE
			if (pl < 0) {
				// ok, we use the moisture retention spline directly
				double pC = IBK::f_log10(-pl);
				return m_spline[F_Thetal_pC].value(pC);
			}
#endif // MM_USE_CUBIC_MRC_TRANSFER_SPLINE
			else {
				// use linear extrapolation into positive range
				// with m_ThetalLimit == m_ThetalEff and m_plLimit == 0, the
				// interpolation function simplifies to:
				return m_ThetalEff + pl*m_Thetal_pl_slope;
			}
		}

		case ST_THETA_L_PC_MODEL : {
			if (pl < m_plLimit) {
				IBK_ASSERT_X( (m_activeModel==MT_VAN_GENUCHTEN),
							  "Unsupported type of moisture storage model or missing initialisation found.");
				switch (m_activeModel) {
					case MT_VAN_GENUCHTEN :
						return m_ThetalEff*m_modelVG.norm_w(pl);
					default :
						return 0.0;
				}
			}
			else {
				// use linear extrapolation into positive range
				// extrapolation starts off m_ThetalLimit
				return m_ThetalLimit + (pl-m_plLimit)*m_Thetal_pl_slope;
			}
		}

		case ST_THETA_L_RH_SPLINE : {
			// ok, we use the moisture retention spline directly
			if (pl < 0) {
				// unsaturated range
				double rh = IBK::f_relhum(T, pl);
				return m_spline[F_Thetal_rh].value(rh);
			}
			else {
				// high-moisture content range, rh ~ 100%
				// use linear extrapolation into positive range
				return m_ThetalEff + pl*m_Thetal_pl_slope;
			}
		}

		case ST_THETA_L_RH_MODEL : {
			if (pl < m_plLimit) {

				/// \todo implement
//				IBK_ASSERT_X( false, "ST_THETA_L_RH_MODEL - is not implemented, yet!")
				return m_ThetalLimit;

			}
			else {
				// use linear extrapolation into positive range
				// extrapolation starts off m_ThetalLimit
				return m_ThetalLimit + (pl - m_plLimit) * m_Thetal_pl_slope;
			}
		}

		default :
			return 0.0;
	}
}
//---------------------------------------------------------------------------


double MoistureStorage::pl_Thetal(double Thetal, double T, double pl_estimate) const {

	const char * const FUNC_ID = "[MoistureStorage::pl_Thetal]";

	if (!(m_type==ST_THETA_L_PC_SPLINE || m_type==ST_THETA_L_PC_MODEL || m_type==ST_THETA_L_RH_SPLINE || m_type==ST_THETA_L_RH_MODEL))
		throw IBK::Exception( "Unsupported type of moisture storage function or missing initialization.", FUNC_ID);

	switch (m_type) {
		case ST_THETA_L_PC_MODEL :
		case ST_THETA_L_PC_SPLINE : {
			if (Thetal < m_ThetalLimit) {
				// unsaturated moisture range, use either rMRC data or numerical algorithm
				if (m_spline[F_pC_Thetal].empty()) {
					// evaluate pc(Thetal) numerically using Thetal(pc) function
					double plEstimate = std::min(-1.0, pl_estimate);
					double pC = pC_Thetal_iterative(Thetal, IBK::f_log10(-plEstimate));
					return -IBK::f_pow10(pC);
				}
				else {
					// use spline interpolation
					double pC = m_spline[F_pC_Thetal].value(Thetal);
					return -IBK::f_pow10(pC);
				}
			}
			else {
				// saturated range, use linear function, inverse function of ThetaL(pl)
				// interpolation function is:
				// Thetal = m_ThetalLimit + (pl-m_plLimit)*m_Thetal_pl_slope;
				// inverse function is:
				return (Thetal - m_ThetalLimit)/m_Thetal_pl_slope + m_plLimit;
			}
		}

		case ST_THETA_L_RH_MODEL :
		case ST_THETA_L_RH_SPLINE : {
			if (Thetal < m_ThetalLimit) {
				// unsaturated moisture range, must have either rMRC (sorption isotherm) data or numerical algorithm
				if (m_RH_Thetal_spline.empty()) {
					// evaluate RH(Thetal) numerically using Thetal(RH) model
					bool falseVar = false;
					IBK_ASSERT_X( falseVar, "evaluate RH(Thetal) numerically using Thetal(RH) model - is not implemented, yet!");
					return 0.0; // just to make compiler happy, this could should never be executed anyway!
				}
				else {
					// use spline interpolation
					double rh = m_RH_Thetal_spline.value(Thetal);
					return IBK::f_pc_rh(rh, T);
				}
			}
			else {
				// saturated range, use linear function, inverse function of ThetaL(pl)
				// interpolation function is:
				// Thetal = m_ThetalLimit + (pl-m_plLimit)*m_Thetal_pl_slope;
				// inverse function is:
				return (Thetal - m_ThetalLimit)/m_Thetal_pl_slope + m_plLimit;
			}
		}

		default :
			return 0.0;
	}
}
//---------------------------------------------------------------------------


double MoistureStorage::mwu_T(double T) const {
	IBK_ASSERT(!m_spline[F_mwu_T].empty());
	if (T > 273.15) {
		IBK_ASSERT(T <= 273.15);
	}
	double mwu = m_spline[F_mwu_T].value(T);
	return mwu;
}
//---------------------------------------------------------------------------


void MoistureStorage::prepareCalculation(const Material* mat) {
	const char * const FUNC_ID = "[MoistureStorage::prepareCalculation]";
	IBK_ASSERT(mat != NULL);

	m_type = NUM_MoistureStorageType;

	// set slope of linear function (may need to tweak this if errors in numerical algorithm appear)
	m_Thetal_pl_slope = 1e-7; // in m3/m3 / Pa

	IBK::IBK_Message("Initializing moisture storage functions.\n", IBK::MSG_PROGRESS, FUNC_ID, 3);
	IBK::MessageIndentor indent; (void)indent;

	try {
		m_air = mat->m_identification.m_flags & MaterialIdentification::AIR;

		// skip initialization if this material is an air material
		if (m_air)
			return;

		// in case we have normalized spline data, denormalize all functions first
		denormalize(mat);

		// currently, all MRC parametrizations need ThetalEff (actually, the Thetal_pl() and pl_Thetal()
		// functions require it
		if (!checkStorageParameter(mat, Material::MP_THETA_EFF, "moisture storage models", FUNC_ID, 1e-20))
			return;

		m_ThetalEff = mat->m_paraStorage[Material::MP_THETA_EFF].value;

		// check for available parametrization
		if (!m_spline[F_Thetal_pC].empty()) {

			m_type = ST_THETA_L_PC_SPLINE;

			// check for highest pC value
			double xe = m_spline[F_Thetal_pC].x().back();
			double ye = m_spline[F_Thetal_pC].y().back();

			if( ye == 0 && xe < 12.0) {
				std::vector<double> xn = m_spline[F_Thetal_pC].x();
				xn.back() = 12.0;
				std::vector<double> yn = m_spline[F_Thetal_pC].y();
				m_spline[F_Thetal_pC].setValues(xn, yn);

				if( !m_spline[F_pC_Thetal].empty()) {
					std::vector<double> xn = m_spline[F_pC_Thetal].x();
					std::vector<double> yn = m_spline[F_pC_Thetal].y();
//					double y0 = yn.front();
					yn.front() = 12.0;
					m_spline[F_pC_Thetal].setValues(xn, yn);
				}
			}

			m_cubicMRCTransfer.init(m_spline[F_Thetal_pC], m_Thetal_pl_slope, m_ThetalEff);
		}
		else if (!m_spline[F_Thetal_rh].empty())  {

			m_type = ST_THETA_L_RH_SPLINE;

			// auto-generate reverse MRC function
			if (m_type == ST_THETA_L_RH_SPLINE) {
				std::vector<double> rh = m_spline[F_Thetal_rh].x();
				std::vector<double> Ol = m_spline[F_Thetal_rh].y();
				// create the inverse spline
				m_RH_Thetal_spline.setValues(Ol, rh);
			}

		}
		else {

			// check for available MRC model parametrization
			if (m_models.empty()) {
				return; // no MRC parameters present, we don't have MRC calculation capability
			}

			// we require that only one model block is provided per material file
			if (m_models.size() != 1) {
				throw IBK::Exception("There must not be more than one moisture storage model parameter section.", FUNC_ID);
			}

			// check if any of the parametrization is known
			std::string modelID = m_models.begin()->first;
			if (!KeywordList::KeywordExists("MoistureStorage::ModelTypes", modelID)) {
				IBK::IBK_Message(IBK::FormatString("Unknown MRC model type '%1'.").arg(modelID),
								 IBK::MSG_WARNING, FUNC_ID, 3);
				return; // we don't have MRC calculation capability
			}

			m_activeModel = static_cast<ModelTypes>(KeywordList::Enumeration("MoistureStorage::ModelTypes", modelID));
			switch (m_activeModel) {
				case MT_GAUSS : {
					// Note: the GAUSS Theta_l(pc) is actually of type ST_THETA_L_PC_MODEL, but for
					//       performance reasons we will generate a spline -> therefore we set the
					//       ST_THETA_L_PC_SPLINE type
					m_type = ST_THETA_L_PC_SPLINE;
					// we need to parse the parametrization, create a GaussMRC model and
					// generate a spline.
					MrcGAUSS mrc;
					mrc.setData(m_ThetalEff, m_models.begin()->second); // can throw in case of errors
					mrc.setTolerance(1e-7);
					// now generate a linear spline
					IBK::IBK_Message("Generating MRC spline data from GAUSS model.\n",
									 IBK::MSG_PROGRESS, FUNC_ID, 2);
					IBK::MessageIndentor indent; (void)indent;

					IBK_ASSERT(m_spline[F_Thetal_pC].empty()); // spline must be empty, but that was checked before

					m_spline[F_Thetal_pC].generate(
								mrc,	// the function
								0, 10,	// range for pC values
								1e-5,	// absolute generation tolerance, must be less than tolerance used in MRC
								1e-4,	// relative tolerance
								10000,	// remove redundant points
								false);	// start from scratch

					// add point with 0 moisture content at pC=12
					std::vector< double > x = m_spline[F_Thetal_pC].x();
					x.push_back(12);
					std::vector< double > y = m_spline[F_Thetal_pC].y();
					y.push_back(0);
					m_spline[F_Thetal_pC].setValues( x, y );
					IBK::IBK_Message( IBK::FormatString("MRC spline generated with %1 points.\n").arg((unsigned int)m_spline[F_Thetal_pC].size()),
									 IBK::MSG_PROGRESS, FUNC_ID, 2);
				} break;

				case MT_VAN_GENUCHTEN : {
//					m_type = ST_THETA_L_PC_MODEL; // Theta_l(pc) MODEL type
//					m_modelVG.clear();
//					// parse from data strings
//					// check that the model data block provides needed data
//					const ModelDataContainer & data = m_models.begin()->second;
//					// check that all strings are present
//					std::string Li = data.strings("VG_l");
//					std::string Ci = data.strings("VG_c");
//					std::string Ni = data.strings("VG_n");
//					if (Li.empty() || Ci.empty() || Ni.empty())
//						throw IBK::Exception("Mismatching or missing VAN GENUCHTEN model parameters.", FUNC_ID);
//					m_modelVG.setData(m_ThetalEff, Li, Ci, Ni);
					IBK::IBK_Message(IBK::FormatString("VAN GENUCHTEN Model is recognized, but not supported yet."), IBK::MSG_ERROR, FUNC_ID );
				} break;

				default :
					IBK::IBK_Message(IBK::FormatString("MRC Model type '%1' is recognized, but not supported yet.").arg(modelID),
									 IBK::MSG_WARNING, FUNC_ID, 3);
			}

		} // else -> model parameters


		// in case that we have both MRC and rMRC spline data, we do a consistency check
		if (m_type == ST_THETA_L_PC_SPLINE) {
			doReverseCheck(); // check automatically if both splines are given
		}

		// finally, check the parametrization
		checkLimits(mat);

		// if we have a MRC spline, we switch to the linear function of pl > 0
		if (!m_spline[F_Thetal_pC].empty() ||
			!m_spline[F_Thetal_rh].empty())
		{
			m_plLimit = 0;
			m_ThetalLimit = m_ThetalEff;
		}
		else {
			// for analytical models we need to compute m_ThetalEff at a limiting liquid pressure,
			// because analytical models may asymptotically approach 0 slope at saturation
//			m_plLimit = 0; // set plLimit to 0 for now
			// compute MRC value at limiting value
			m_ThetalLimit = Thetal_pl(-100, 293.15);
			m_plLimit = -100;
		}

	}
	catch (IBK::Exception & ex) {
		throw IBK::Exception(ex, "Error initializing moisture storage functionality for calculation.", FUNC_ID);
	}
}
//---------------------------------------------------------------------------


void MoistureStorage::checkLimits(const Material* mat) {

	(void)mat;

	const char * const FUNC_ID = "[MoistureStorage::checkLimits]";
	IBK_ASSERT(mat!=NULL);

	IBK::IBK_Message("Checking parameter and function limits.\n", IBK::MSG_PROGRESS, FUNC_ID, 3);
	IBK::MessageIndentor indent; (void)indent;

	double val, EPS=1e-3;
	switch (m_type) {
		// if we have splines, do consistency checks
		case ST_THETA_L_PC_SPLINE : {
			// check that all slopes are negative
			for (unsigned int i=0, end = m_spline[F_Thetal_pC].size()-1; i<end; ++i) {
				if (m_spline[F_Thetal_pC].slopes()[i] > 0)
					IBK::IBK_Message(IBK::FormatString("Invalid spline data for Theta_l(pC), found positive slope in interval #%1.").arg(i),
									 IBK::MSG_WARNING, FUNC_ID, 3);
			}
			val = m_spline[F_Thetal_pC].y().front();
			if (std::fabs(val - m_ThetalEff) > EPS)
				IBK::IBK_Message(IBK::FormatString("Material has an invalid Theta_l(pC) spline: "
								"the maximum Theta_l value (%1 m3/m3) must be equal to 'THETA_EFF' (%1 m3/m3)!").arg(val).arg(m_ThetalEff),
								 IBK::MSG_WARNING, FUNC_ID, 3);
			val = m_spline[F_Thetal_pC].y().back();
			if (std::fabs(val) > 0)
				IBK::IBK_Message(IBK::FormatString("Material has an invalid Theta_l(pC) spline: "
								 "the minimum Thetal value (%1 m3/m3) must be zero!").arg(val),
								 IBK::MSG_WARNING, FUNC_ID, 3);
			val = m_spline[F_Thetal_pC].x().front();
			if (std::fabs(val) > EPS)
				IBK::IBK_Message(IBK::FormatString("Material has an invalid Theta_l(pC) spline: "
								 "the minimum pC value (%1 logPa) must be zero!").arg(val),
								 IBK::MSG_WARNING, FUNC_ID, 3);

			// also check reverse MRC spline
			if (!m_spline[F_pC_Thetal].empty()) {
				// check that all slopes are negative
				for (unsigned int i=0, end = m_spline[F_pC_Thetal].size()-1; i<end; ++i) {
					if (m_spline[F_pC_Thetal].slopes()[i] >= 0)
						IBK::IBK_Message(IBK::FormatString("Invalid spline data for pC(Theta_l), found positive slope in interval #%1.").arg(i),
										 IBK::MSG_WARNING, FUNC_ID, 3);
				}
				val = m_spline[F_pC_Thetal].x().back();
				if (std::fabs(val - m_ThetalEff) > EPS)
					IBK::IBK_Message(IBK::FormatString("Material has an invalid pC(Theta_l) spline: "
									 "the maximum Thetal value (%1 m3/m3) must be equal to 'THETA_EFF' (%2 m3/m3)!").arg(val).arg(m_ThetalEff),
									 IBK::MSG_WARNING, FUNC_ID, 3);
				val = m_spline[F_pC_Thetal].x().front();
				if (std::fabs(val) > EPS)
					IBK::IBK_Message(IBK::FormatString("Material has an invalid pC(Theta_l) spline: "
									 "the minimum Thetal value (%1 m3/m3) must be zero!").arg(val),
									 IBK::MSG_WARNING, FUNC_ID, 3);
				val = m_spline[F_pC_Thetal].y().back();
				if (std::fabs(val) > EPS)
					IBK::IBK_Message(IBK::FormatString("Material has an invalid pC(Theta_l) spline: "
									 "the minimum pC value (%1 logPa) must be zero!").arg(val),
									 IBK::MSG_WARNING, FUNC_ID, 3);

			}
		} break;

		case ST_THETA_L_RH_SPLINE : {
			// check that all slopes are negative
			for (unsigned int i=0, end = m_spline[F_Thetal_rh].size()-1; i<end; ++i) {
				if (m_spline[F_Thetal_rh].slopes()[i] < 0)
					IBK::IBK_Message(IBK::FormatString("Invalid spline data for Theta_l(rh), found negative slope in interval #%1.").arg(i),
									 IBK::MSG_WARNING, FUNC_ID, 3);
			}
			// also check reverse MRC spline
			if (!m_RH_Thetal_spline.empty()) {
				// check that all slopes are negative
				for (unsigned int i=0, end = m_RH_Thetal_spline.size()-1; i<end; ++i) {
					if (m_RH_Thetal_spline.slopes()[i] < 0)
						IBK::IBK_Message(IBK::FormatString("Invalid spline data for rh(Theta_l), found negative slope in interval #%1.").arg(i),
										 IBK::MSG_WARNING, FUNC_ID, 3);
				}
			}
		} break;

		case ST_THETA_L_PC_MODEL :
		case ST_THETA_L_RH_MODEL :
		default : ; // nothing to do !?
	}
}
//---------------------------------------------------------------------------


void MoistureStorage::denormalize(const Material * mat) {
	const char * const FUNC_ID = "[MoistureStorage::denormalize]";
	double thetaEff = mat->m_paraStorage[Material::MP_THETA_EFF].value;
	double eps=1e-3;

	// Theta_l(pC) spline
	if (!m_spline[F_Thetal_pC].empty()) {
		bool skip = false;
		// x must be given in the range from [0..]
		skip |= (std::fabs(m_spline[F_Thetal_pC].x().front()    ) > eps);
		// y must be given in the range from [1..0]
		skip |= (std::fabs(m_spline[F_Thetal_pC].y().front() - 1) > eps);
		skip |= (std::fabs(m_spline[F_Thetal_pC].y().back()     ) > eps);

		if (!skip) {
			// Theta_l(pC) spline
			std::vector<double> vx(m_spline[F_Thetal_pC].x());
			std::vector<double> vy(m_spline[F_Thetal_pC].y());
			for( size_t i=0, size = vy.size(); i<size; ++i)
				vy[i] *= thetaEff;
			IBK::IBK_Message("Denormalizing Theta_l(pC) spline.\n", IBK::MSG_PROGRESS, FUNC_ID, 2);
			m_spline[F_Thetal_pC].setValues(vx, vy);

		}
	}

	// pC(Theta_l) spline
	if (!m_spline[F_pC_Thetal].empty()) {
		bool skip = false;

		// x must be given in the range from [0..1]
		skip |= (std::fabs(m_spline[F_pC_Thetal].x().front())    > eps);
		skip |= (std::fabs(m_spline[F_pC_Thetal].x().back() - 1) > eps);
		// y must be given in the range from [..0]
		skip |= (std::fabs(m_spline[F_pC_Thetal].y().back()    ) > eps);

		if (!skip) {
			// x is given in the range from [0..1], multiply by OEFF
			std::vector<double> vx = m_spline[F_pC_Thetal].x();
			std::vector<double> vy = m_spline[F_pC_Thetal].y();
			for( size_t i=0, size = vx.size(); i<size; ++i) {
				vx[i] *= thetaEff;
			}
			IBK::IBK_Message("Denormalizing pC(Theta_l) spline.\n", IBK::MSG_PROGRESS, FUNC_ID, 2);
			m_spline[F_pC_Thetal].setValues(vx, vy);
		}
	}

	// Theta_l(rh) spline
	if (!m_spline[F_Thetal_rh].empty()) {
		bool skip = false;
		// y must be given in the range from [1..0]
		skip |= (std::fabs(m_spline[F_Thetal_rh].y().front() - 1) > eps);
		skip |= (std::fabs(m_spline[F_Thetal_rh].y().back()     ) > eps);
		if (!skip) {
			// Theta_l(pC) spline
			std::vector<double> vx(m_spline[F_Thetal_rh].x());
			std::vector<double> vy(m_spline[F_Thetal_rh].y());
			for( size_t i=0, size = vy.size(); i<size; ++i)
				vy[i] *= thetaEff;
			IBK::IBK_Message("Denormalizing Theta_l(rh) spline.\n", IBK::MSG_PROGRESS, FUNC_ID, 2);
			m_spline[F_Thetal_pC].setValues(vx, vy);
		}
	}
}
//---------------------------------------------------------------------------


void MoistureStorage::doReverseCheck() const {
	const char * const FUNC_ID = "[MoistureStorage::doReverseCheck]";
	// requires m_ThetaEff to be set already

	IBK_ASSERT(m_type == ST_THETA_L_PC_SPLINE);

	// nothing to do if reverse MRC spline data is missing
	if (m_spline[F_pC_Thetal].empty())
		return;

	IBK::IBK_Message("Performing consistency check between storage functions.\n", IBK::MSG_PROGRESS, FUNC_ID, 3);
	IBK::MessageIndentor indent; (void)indent;

	// check that all MRC values have a valid representation in rMRC
	unsigned int count = 0;
	for (unsigned int i=0, size = m_spline[F_Thetal_pC].size(); i < size; ++i) {
		double Thetal = m_spline[F_Thetal_pC].y()[i];
		double pCOriginal = m_spline[F_Thetal_pC].x()[i];
		double pC = m_spline[F_pC_Thetal].value(Thetal);
		if (std::fabs(pC - pCOriginal) > 1e-3) {
			IBK::IBK_Message(IBK::FormatString("pC = %1 -> Theta_l = %2 m3/m3 --(rMRC)-> pC = %3.\n")
							 .arg(pCOriginal).arg(Thetal).arg(pC), IBK::MSG_PROGRESS, FUNC_ID, 3);
			++count;
		}
	}
	if (count > 0) {
		IBK::IBK_Message(IBK::FormatString("Material may have inconsistant Theta_l(pC) and pC(Theta_l) "
										   "functions (%1 errors in rMRC check).").arg(count), IBK::MSG_WARNING, FUNC_ID, 3);
	}

	// check that all rMRC values have a valid representation in MRC
	count = 0;
	for (unsigned int i=0, size = m_spline[F_pC_Thetal].size(); i < size; ++i) {
		double pC = m_spline[F_pC_Thetal].y()[i];
		double Thetal_Original = m_spline[F_pC_Thetal].x()[i];
		double Thetal = m_spline[F_Thetal_pC].value(pC);
		if (std::fabs(Thetal - Thetal_Original) > 1e-3) {
			IBK::IBK_Message(IBK::FormatString("Theta_l = %1 m3/m3 -> pC = %2 --(MRC)-> Theta_l = %3 m3/m3\n")
							 .arg(Thetal_Original).arg(pC).arg(Thetal), IBK::MSG_PROGRESS, FUNC_ID, 3);
			++count;
		}
	}
	if (count > 0) {
		IBK::IBK_Message(IBK::FormatString("Material may have inconsistant Theta_l(pC) and pC(Theta_l) "
										   "functions (%1 errors in MRC check).").arg(count), IBK::MSG_WARNING, FUNC_ID, 3);
	}
}
//---------------------------------------------------------------------------


double MoistureStorage::pC_Thetal_iterative(double Thetal, double pC_estimate) const {
	const char * const FUNC_ID = "[MoistureStorage::pC_Thetal_iterative]";

	// minimization function: F(pC) = Thetal(pc(pC)) - Thetal = 0

	// range limits:  Thetal <= m_ThetalLimit
	//				  pc	 <= m_plLimit
	//				  pC     > 0 !!!

	// performance considerations:
	//   most functions will be provided via MRC spline (for example, MRC splines are
	//   auto-generated from all GAUSS model parameter sets).
	//   MRC spline is Thetal(pC), therefore we define in this case the root function
	//     F(pC) = Thetal(pC) - Thetal
	//   and avoid a call to log10() and pow10() functions per iteration
	//
	// The calling function should check if Thetal(pC_estimate) == Thetal and
	// avoid calling this function in this case (saves one evaluation of Thetal).

	IBK_ASSERT(Thetal < m_ThetalLimit);
	if( Thetal == 0.0) {
		return 14.0;
	}
	IBK_ASSERT(Thetal > 0);

	//#define ITER_DEBUG
	#ifdef ITER_DEBUG
	const unsigned int FIELD_WIDTH = 15;
	std::cout << std::setw(6) << "iter"
				  << std::setw(FIELD_WIDTH) << "pC"
				  << std::setw(FIELD_WIDTH) << "F(pC)"
				  << std::setw(FIELD_WIDTH) << "lg(F)"
				  << std::setw(FIELD_WIDTH) << "dpC"
				  << std::setw(FIELD_WIDTH) << "new_pC"
				  << std::endl;
	#endif // ITER_DEBUG

	// constants
	const unsigned int MaxIters = 50;
//	const double AbsTolF = 1e-12;
	const double AbsTol = 1e-12;
	const double RelTol = 1e-10;
	double minpC = 0;
	if (m_plLimit < 0)
		minpC = IBK::f_log10(-m_plLimit); // pC must not become < minpC
	const double limpC = 1.1; // maximum jump size of pC

	unsigned int iters = 0;
	double pCm=0;	// holds pC_{m}, initial value is just there to make compiler happy
	double Fm=0;	// holds F_{m}, initial value is just there to make compiler happy
	double F;	// holds F_{m+1}
	double pC = std::max(pC_estimate, minpC+AbsTol); // holds pC_{m+1}, initialize with initial guess

	// iterate as long as we have iterations left
	while (++iters < MaxIters) {
		++GLOBAL_MRC_ITERATIONS;
		// evaluate function at estimated solution
		switch (m_type) {
			case ST_THETA_L_PC_SPLINE : F = m_spline[F_Thetal_pC].value(pC) - Thetal; break;
			case ST_THETA_L_PC_MODEL  : F = Thetal_pl(-IBK::f_pow10(pC)) - Thetal; break;
			default: throw IBK::Exception("Invalid MRC function type.", FUNC_ID);
		}
		// evaluate convergence criterion 1
		if (F==0) {
			return pC;
		}
		// absTolF is a value that is 'small' compared to Fm
/*
		double F_normalized = F/AbsTolF;
		if (F_normalized*F_normalized < 1) {
#ifdef ITER_DEBUG
			std::cout
				<< std::setw(6) << std::right << iters
				<< std::setw(FIELD_WIDTH) << std::right << pC
				<< std::setw(FIELD_WIDTH) << std::right << F
				<< std::setw(FIELD_WIDTH) << std::right << std::log10(std::fabs(F+AbsTolF))
				<< std::setw(FIELD_WIDTH) << std::right << "-"
				<< std::setw(FIELD_WIDTH) << std::right << "-"
				<< std::endl;
#endif // ITER_DEBUG
			return pC;
		}
*/
		// if this is the first iteration, estimate last solution
		if (iters==1) {
			pCm = pC + pC*RelTol + AbsTol;
			switch (m_type) {
				case ST_THETA_L_PC_SPLINE : Fm = m_spline[F_Thetal_pC].value(pCm) - Thetal; break;
				case ST_THETA_L_PC_MODEL  : Fm = Thetal_pl(-IBK::f_pow10(pCm)) - Thetal; break;
				default: throw IBK::Exception("Invalid MRC function type.", FUNC_ID);
			}
		}

		// compute new x, we know that F != 0
		double dpC = - F/(F-Fm+1e-20)*(pC-pCm);

		// limit dpC to avoid excessive oscillations, for most cases we only
		// expect small dpC anyway, so limiting dpC may be meaningful
		if (dpC > limpC)		dpC = limpC;
		else if (dpC < -limpC)	dpC = -limpC;
		double newpC = pC + dpC;

		// apply constraints
		if (newpC < minpC)
			newpC = minpC;
		/// \todo add constraints for functions/splines with fixed end point

		// evaluate convergence criterion 2
		// epsilon is a value that is 'small' compared to x
		double epsilon = RelTol*std::fabs(pC) + AbsTol;
		// compute iterative step size as difference of current iterate versus last iterate
		dpC = newpC - pC;
		double dpC_normalized = dpC/epsilon;

		// print statistics
#ifdef ITER_DEBUG
		std::cout
			<< std::setw(6) << std::right << iters
			<< std::setw(FIELD_WIDTH) << std::right << pC
			<< std::setw(FIELD_WIDTH) << std::right << F // still of old iteration
			<< std::setw(FIELD_WIDTH) << std::right << std::log10(std::fabs(F+AbsTolF))
			<< std::setw(FIELD_WIDTH) << std::right << dpC
			<< std::setw(FIELD_WIDTH) << std::right << newpC
			<< std::endl;
#endif // ITER_DEBUG

		// store current iterative solution
		pCm = pC;
		Fm = F;

		pC = newpC;

		// compare dpC_normalized with 1, dpC_normalized might be negative,
		// therefore we square both sides and 1*1 is still 1.
		if (dpC_normalized*dpC_normalized < 1) {
			return pC; // success
		}
	} // while

	IBK::IBK_Message("Error converging on rMRC calculation with given number of iteration. "
					 "Returning best bet so far (may lead to Newton convergence errors).", IBK::MSG_WARNING, FUNC_ID, IBK::VL_STANDARD);
	return pC;
}
//---------------------------------------------------------------------------

} // namespace MM


