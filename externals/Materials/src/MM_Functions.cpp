#include "MM_Functions.h"

#include <IBK_messages.h>
#include <IBK_FormatString.h>
#include <IBK_math.h>
#include <IBK_Exception.h>
#include <IBK_physics.h>

#include "MM_Material.h"
#include "MM_ModelDataContainer.h"
#include "MM_Functions.h"

namespace MM {

MrcGAUSS_dpC::MrcGAUSS_dpC(const std::string& data) {
	std::stringstream strm(data);
	unsigned int dummy;
	strm >> dummy;
	double d;
	while (strm >> d)
		m_p.push_back(d);
}

void MrcGAUSS_dpC::clear() {
	m_p.clear();
}

void MrcGAUSS_dpC::setData(const std::string& Wi, const std::string& Ri, const std::string& Si) {
	const char * const FUNC_ID = "[MrcGAUSS_dpC::setData]";
	clear();
	std::stringstream strmSi(Si);
	std::stringstream strmWi(Wi);
	std::stringstream strmRi(Ri);
	double dSi, dWi, dRi;
	while ((strmSi >> dSi) &&
		   (strmWi >> dWi) &&
		   (strmRi >> dRi))
	{
		m_p.push_back(dWi);
		m_p.push_back(dRi);
		m_p.push_back(dSi);
	}
	// check that the weight factors sum up to 1
	double sum = 0;
	unsigned int mod = m_p.size()/3;
	for (unsigned int i=0; i<mod; ++i) {
		sum += m_p[i*3];
	}
	if (std::fabs(sum - 1) > 1e-4) {
		throw IBK::Exception(IBK::FormatString("Weight factors for MRC Gauss model sum up to %1, but sum should be 1.")
			.arg(sum), FUNC_ID);
	}
}

void MrcGAUSS_dpC::setData(const ModelDataContainer& modelDataContainer) {
	const char * const FUNC_ID = "[MrcGAUSS_dpC::setData]";
	std::string Wi = modelDataContainer.strings("GAUSS_Wi");
	std::string Si = modelDataContainer.strings("GAUSS_Si");
	std::string Ri = modelDataContainer.strings("GAUSS_Ri");
	if (Wi.empty() || Si.empty() || Ri.empty())
		throw IBK::Exception("Mismatching or missing GAUSS model parameters.", FUNC_ID);
	setData(Wi, Ri, Si);
}


double MrcGAUSS_dpC::operator()(double pC) const {
	double sum = 0.0;
	unsigned int mod = m_p.size()/3;
	for (unsigned int i=0; i<mod; ++i) {
		double wi = m_p[3*i];
		double pCi = IBK::f_log10(2*0.076/m_p[3*i+1]);
		double si = m_p[3*i+2];
		sum += wi/(2.506628*si)*IBK::f_exp(-((pC-pCi)*(pC-pCi))/(2*si*si));
	}
	return sum;
}

// Implementation of the GAUSS type moisture retention curve.
double MrcGAUSS::norm_w(double pC) const {
	// Return the numeric integral from 0 to pC
	return std::max(1.0 - MM::romberg_integral(m_dpC_mrc, 0, pC, m_tolerance),0.0);
}

void MrcGAUSS::setData(double thetaEff, const std::string& Wi, const std::string& Ri, const std::string& Si) {
	m_thetaEff = thetaEff;
	m_dpC_mrc.setData(Wi, Ri, Si);
}

void MrcGAUSS::setData(double thetaEff, const ModelDataContainer& modelDataContainer) {
	const char * const FUNC_ID = "[MrcGAUSS::setData]";
	std::string Wi = modelDataContainer.strings("GAUSS_Wi");
	std::string Si = modelDataContainer.strings("GAUSS_Si");
	std::string Ri = modelDataContainer.strings("GAUSS_Ri");
	if (Wi.empty() || Si.empty() || Ri.empty())
		throw IBK::Exception("Mismatching or missing GAUSS model parameters.", FUNC_ID);
	setData(thetaEff, Wi, Ri, Si);
}

void MrcGAUSS::setDefault() {
	m_dpC_mrc.m_p.resize(6);
	// set some example parameters to show how it works
	m_dpC_mrc.m_p[0] = 0.7; 	// W_1
	m_dpC_mrc.m_p[1] = 5e-5; // r_1
	m_dpC_mrc.m_p[2] = 0.4; 	// std_1
	m_dpC_mrc.m_p[3] = 0.3; 	// W_2
	m_dpC_mrc.m_p[4] = 1e-7; // r_2
	m_dpC_mrc.m_p[5] = 0.6; 	// std_2
}

/// Converts MRC Gauss parameters given as triples of W_i, r_i, std_i into
/// parameters given in Ol_i, pC_i, std_i.
void MrcGAUSS::convertTo_pC(std::vector<double>& param_mrc_pC, double thetaEff) const {

	if (m_dpC_mrc.m_p.empty() || m_dpC_mrc.m_p.size() % 3 != 0) {
		param_mrc_pC.clear();
		return;
	}
	unsigned int modality = m_dpC_mrc.m_p.size()/3;
	param_mrc_pC.resize(modality*3);

	// linear case
	if (modality == 1) {
		param_mrc_pC[0] = thetaEff;
		param_mrc_pC[1] = IBK::f_pC_r(m_dpC_mrc.m_p[1]);
		param_mrc_pC[2] = m_dpC_mrc.m_p[2];
		return;
	}

	// special treatment for last modality
	unsigned int i = modality-1;
	param_mrc_pC[3*i] 		= m_dpC_mrc.m_p[i*3] * thetaEff;
	param_mrc_pC[3*i+1] 	= IBK::f_pC_r(m_dpC_mrc.m_p[3*i+1]);
	param_mrc_pC[3*i+2] 	= m_dpC_mrc.m_p[3*i+2];

	// loop until first element
	do {
		--i;
		param_mrc_pC[3*i] 	= param_mrc_pC[3*(i+1)] + m_dpC_mrc.m_p[i*3] * thetaEff;
		param_mrc_pC[3*i+1] = IBK::f_pC_r(m_dpC_mrc.m_p[3*i+1]);
		param_mrc_pC[3*i+2] = m_dpC_mrc.m_p[3*i+2];
	} while (i != 0);
}
//---------------------------------------------------------------------------

/// Converts MRC Gauss parameters given as triples of Ol_i, pC_i, std_i into
/// parameters given in W_i, r_i, std_i.
void MrcGAUSS::setFrom_pC(const std::vector<double>& param_mrc_pC, double thetaEff) {

	m_thetaEff = thetaEff;

	if (param_mrc_pC.empty() || param_mrc_pC.size() % 3 != 0) {
		return;
	}
	unsigned int modality = param_mrc_pC.size()/3;
	m_dpC_mrc.m_p.resize(modality*3);

	// special treatment if we only have one modality
	if (modality == 1) {
		m_dpC_mrc.m_p[0] 	= 1;		// the weight
		m_dpC_mrc.m_p[1] 	= IBK::f_r_pC(param_mrc_pC[1]);
		m_dpC_mrc.m_p[2]	= param_mrc_pC[2];
		return;
	}
	// calculate pC -> r, Ol -> w
	unsigned int i;
	for (i=0; i<modality-1; ++i) {
		double weight = (param_mrc_pC[i*3] - param_mrc_pC[(i+1)*3])/param_mrc_pC[0];
		m_dpC_mrc.m_p[i*3] 		= weight;
		m_dpC_mrc.m_p[i*3+1] 	= IBK::f_r_pC(param_mrc_pC[i*3+1]);
		m_dpC_mrc.m_p[i*3+2] 	= param_mrc_pC[i*3+2];
	}
	// copy last modality
	m_dpC_mrc.m_p[i*3] 		= param_mrc_pC[i*3]/param_mrc_pC[0];
	m_dpC_mrc.m_p[i*3+1] 	= IBK::f_r_pC(param_mrc_pC[i*3+1]);
	m_dpC_mrc.m_p[i*3+2] 	= param_mrc_pC[i*3+2];
}
//---------------------------------------------------------------------------

MrcVG::MrcVG(const std::string& data) {
	std::stringstream strm(data);
	strm >> m_n;
	double d;
	while (strm >> d)
		m_p.push_back(d);
}

void MrcVG::clear() {
	m_thetaEff = 1;
	m_p.clear();
}

void MrcVG::setData(double thetaEff, const std::string& Li, const std::string& Ci, const std::string& Ni) {
	clear();
	m_thetaEff = thetaEff;
	std::stringstream strmLi(Li);
	std::stringstream strmCi(Ci);
	std::stringstream strmNi(Ni);
	double dLi, dCi, dNi;
	while ((strmLi >> dLi) &&
		   (strmCi >> dCi) &&
		   (strmNi >> dNi))
	{
		m_p.push_back(dLi);
		m_p.push_back(dCi);
		m_p.push_back(dNi);
	}
}

void MrcVG::setData(double thetaEff, const ModelDataContainer& modelDataContainer) {
	const char * const FUNC_ID = "[MrcVG::setData]";
	std::string Li = modelDataContainer.strings("VG_Li");
	std::string Ci = modelDataContainer.strings("VG_Ci");
	std::string Ni = modelDataContainer.strings("VG_Ni");
	if (Li.empty() || Ci.empty() || Ni.empty())
		throw IBK::Exception("Mismatching or missing VG model parameters.", FUNC_ID);
	setData(thetaEff, Li, Ci, Ni);
}

// Implementation of the VG type moisture retention curve.
double MrcVG::norm_w(double pc) const {
	double sum=0.0;
	for (unsigned int i=0; i<m_n; ++i) {
		double l = m_p[3*i];
		double c = m_p[3*i+1];
		double n = m_p[3*i+2];
		sum += l*(IBK::f_pow(1 + IBK::f_pow(-pc*c, n), (1-n)/n) );
	}
	return sum;
}

// Implementation of the VG type moisture capacity curve.
double MrcVG::norm_cap(double pc) const {
	double sum=0.0;
	for (unsigned int i=0; i<m_n; ++i) {
		double l = m_p[3*i];
		double c = m_p[3*i+1];
		double n = m_p[3*i+2];
		sum -= l*c*(1-n)*IBK::f_pow(-pc*c,n-1)*IBK::f_pow(1 + IBK::f_pow(-pc*c, n), (1-2*n)/n );
	}
	return sum;
}

// material functors

MaterialFunctorBase::MaterialFunctorBase(const MM::Material* material) :
	m_material(material)
{}

bool MaterialFunctorBase::valid() {
	if( m_material == 0)
		return false;
	return m_material->capabilities() > 0;
}

// RHFromThetal

RHFromThetal::RHFromThetal(const MM::Material* material) :
			MaterialFunctorBase(material)
{
	if(m_material->m_identification.m_flags & MM::MaterialIdentification::AIR) {
		m_thetalMax = IBK::f_psat(IBK::T_DEFAULT) / (IBK::RHO_W * IBK::R_VAPOR * IBK::T_DEFAULT);
	} else {
		m_thetalMax = m_material->m_paraStorage[MM::Material::MP_THETA_EFF].value;
	}
}

double RHFromThetal::operator()(double Thetal) {
	if(m_material->m_identification.m_flags & MM::MaterialIdentification::VAPOR_TIGHT &&
	   m_material->m_identification.m_flags & MM::MaterialIdentification::WATER_TIGHT)
		return 0.0;

	if(m_material->m_identification.m_flags & MM::MaterialIdentification::AIR) {
		if( Thetal < m_thetalMax)
			return Thetal / m_thetalMax;
		return 1.0;
	}

	double pc = m_material->m_moistureStorage.pl_Thetal(Thetal, IBK::T_DEFAULT);
	return IBK::f_relhum(IBK::T_DEFAULT, pc);
}

std::pair<double,double> RHFromThetal::xRange() const {
	double xmin = 0.0;
	double xmax = m_thetalMax;
	return std::make_pair(xmin,xmax);
}

// ThetalFromRH

ThetalFromRH::ThetalFromRH(const MM::Material* material) :
		MaterialFunctorBase(material)
{
	if(m_material->m_identification.m_flags & MM::MaterialIdentification::AIR) {
		m_thetalMax = IBK::f_psat(IBK::T_DEFAULT) / (IBK::RHO_W * IBK::R_VAPOR * IBK::T_DEFAULT);
	} else {
		m_thetalMax = m_material->m_paraStorage[MM::Material::MP_THETA_EFF].value;
	}
}

double ThetalFromRH::operator()(double RH) {
	if(m_material->m_identification.m_flags & MM::MaterialIdentification::VAPOR_TIGHT &&
	   m_material->m_identification.m_flags & MM::MaterialIdentification::WATER_TIGHT)
		return 0.0;

	if( RH < 1.0 ) {
		if(m_material->m_identification.m_flags & MM::MaterialIdentification::AIR)
			return RH * m_thetalMax;

		double pc = IBK::f_pc_rh(RH, IBK::T_DEFAULT);
		return m_material->m_moistureStorage.Thetal_pl(pc);
	}

	return m_thetalMax;
}

std::pair<double,double> ThetalFromRH::xRange() const {
	double xmin = 0.0;
	double xmax = 1.0;
	return std::make_pair(xmin,xmax);
}

// LamdaFromThetal

LamdaFromThetal::LamdaFromThetal(const MM::Material* material) :
		MaterialFunctorBase(material)
{}

double LamdaFromThetal::operator()(double Thetal) {
	return m_material->m_thermalTransport[MM::U_DIRECTION].lambda(Thetal, IBK::T_DEFAULT);
}

std::pair<double,double> LamdaFromThetal::xRange() const {
	double xmin = 0.0;
	double xmax;
	if(m_material->m_identification.m_flags & MM::MaterialIdentification::AIR) {
		xmax = IBK::f_psat(IBK::T_DEFAULT) / (IBK::RHO_W * IBK::R_VAPOR * IBK::T_DEFAULT);
	} else {
		xmax = m_material->m_paraStorage[MM::Material::MP_THETA_EFF].value;
	}
	return std::make_pair(xmin,xmax);
}

// LamdaFromT

LamdaFromT::LamdaFromT(const MM::Material* material) :
		MaterialFunctorBase(material)
{}

double LamdaFromT::operator()(double T) {
	return m_material->m_thermalTransport[MM::U_DIRECTION].lambda(0, T - 273.15);
}

std::pair<double,double> LamdaFromT::xRange() const {
	double xmin = -40.0;
	double xmax = 100.0;
	return std::make_pair(xmin,xmax);
}

// KvFromThetal

KvFromThetal::KvFromThetal(const MM::Material* material) :
	MaterialFunctorBase(material),
	m_RHFromThetal(material)
{}

double KvFromThetal::operator()(double Thetal) {
	if(m_material->m_identification.m_flags & MM::MaterialIdentification::VAPOR_TIGHT)
		return 0.0;

	try {
		double RH = m_RHFromThetal(Thetal);
		return m_material->m_moistureTransport[MM::U_DIRECTION].Kv_Thetal(Thetal, RH);
	}
	catch(...) {
		return -1.0;
	}
}

std::pair<double,double> KvFromThetal::xRange() const {
	double xmin = 0.0;
	double xmax;
	if(m_material->m_identification.m_flags & MM::MaterialIdentification::AIR) {
		xmax = IBK::f_psat(IBK::T_DEFAULT) / (IBK::RHO_W * IBK::R_VAPOR * IBK::T_DEFAULT);
	} else {
		xmax = m_material->m_paraStorage[MM::Material::MP_THETA_EFF].value;
	}
	return std::make_pair(xmin,xmax);
}

// MewFromRH

MewFromRH::MewFromRH(const MM::Material* material) :
	MaterialFunctorBase(material),
	m_thetalFromRH(material)
{}

double MewFromRH::operator()(double RH) {
	if(m_material->m_identification.m_flags & MM::MaterialIdentification::VAPOR_TIGHT)
		return 0.0;

	double Thetal = m_thetalFromRH(RH);
	double Kv = m_material->m_moistureTransport[MM::U_DIRECTION].Kv_Thetal(Thetal, RH);
	return IBK::DV_AIR / (Kv * IBK::R_VAPOR * IBK::T_DEFAULT);
}

std::pair<double,double> MewFromRH::xRange() const {
	double xmin = 0.0;
	double xmax = 1.0;
	return std::make_pair(xmin,xmax);
}

// DvFromThetal

DvFromThetal::DvFromThetal(const MM::Material* material) :
	MaterialFunctorBase(material),
	m_RHFromThetal(material)
{}

double DvFromThetal::operator()(double Thetal) {
	if(m_material->m_identification.m_flags & MM::MaterialIdentification::VAPOR_TIGHT)
		return 0.0;

	try {
		double RH = m_RHFromThetal(Thetal);
		double Kv = m_material->m_moistureTransport[MM::U_DIRECTION].Kv_Thetal(Thetal, RH);
		return Kv * IBK::R_VAPOR * IBK::T_DEFAULT;
	}
	catch(...) {
		return -1.0;
	}
}

std::pair<double,double> DvFromThetal::xRange() const {
	double xmin = 0.0;
	double xmax;
	if(m_material->m_identification.m_flags & MM::MaterialIdentification::AIR) {
		xmax = IBK::f_psat(IBK::T_DEFAULT) / (IBK::RHO_W * IBK::R_VAPOR * IBK::T_DEFAULT);
	} else {
		xmax = m_material->m_paraStorage[MM::Material::MP_THETA_EFF].value;
	}
	return std::make_pair(xmin,xmax);
}

// KlFromThetal

KlFromThetal::KlFromThetal(const MM::Material* material) :
		MaterialFunctorBase(material)
{
	isDiffusivity = m_material->m_moistureTransport[MM::U_DIRECTION].liquidTransportType() == MM::MoistureTransport::DL_THETAL_SPLINE;
}

double KlFromThetal::operator()(double Thetal) {
	if(m_material->m_identification.m_flags & MM::MaterialIdentification::AIR ||
	   m_material->m_identification.m_flags & MM::MaterialIdentification::WATER_TIGHT)
		return 0.0;

	if( isDiffusivity) {
//		double Dl = m_material->m_moistureTransport[MM::U_DIRECTION].Dl_Thetal(Thetal);
//
//		const IBK::LinearSpline& pCThetal = m_material->m_moistureStorage.m_spline[MM::MoistureStorage::F_pC_Thetal];
//		if( pCThetal.empty() )
//			return 0.0;
//
//		double dpCdThetal = pCThetal.slope(Thetal) * -1.0;
//		return Dl * IBK::RHO_W / dpCdThetal;

		double pl = m_material->m_moistureStorage.pl_Thetal(Thetal);
		if(m_material->m_moistureTransport[MM::U_DIRECTION].m_Psi_Thetal_spline.empty()) {
			return 0.0;
//			m_material->m_moistureTransport[MM::U_DIRECTION].prepareCalculation(m_material, MM::U_DIRECTION);
		}
		double psipl = m_material->m_moistureTransport[MM::U_DIRECTION].Psi_Thetal(Thetal);
		return m_material->m_moistureTransport[MM::U_DIRECTION].Kl_Thetal(Thetal, pl, psipl, m_material);
	} else {
		return m_material->m_moistureTransport[MM::U_DIRECTION].Kl_Thetal(Thetal);
	}
}

// PsiFromThetal

PsiFromThetal::PsiFromThetal(const MM::Material* material) :
		MaterialFunctorBase(material)
{
	isDiffusivity = m_material->m_moistureTransport[MM::U_DIRECTION].liquidTransportType() == MM::MoistureTransport::DL_THETAL_SPLINE;
}

double PsiFromThetal::operator()(double Thetal) {
	if(m_material->m_identification.m_flags & MM::MaterialIdentification::AIR ||
	   m_material->m_identification.m_flags & MM::MaterialIdentification::WATER_TIGHT)
		return 0.0;

	if(m_material->m_moistureTransport[MM::U_DIRECTION].m_Psi_Thetal_spline.empty()) {
		return 0.0;
	}
	double psipl = m_material->m_moistureTransport[MM::U_DIRECTION].Psi_Thetal(Thetal);
	return psipl;
}

// KlFrompc

KlFrompC::KlFrompC(const MM::Material* material) :
		MaterialFunctorBase(material),
		m_thetalFrompC(material),
		m_KlFromThetal(material)
{
}

double KlFrompC::operator()(double pC) {
	if(m_material->m_identification.m_flags & MM::MaterialIdentification::AIR ||
	   m_material->m_identification.m_flags & MM::MaterialIdentification::WATER_TIGHT)
		return 0.0;

	double Thetal = m_thetalFrompC(pC);
	return m_KlFromThetal(Thetal);
}

std::pair<double,double> KlFrompC::xRange() const {
	double xmin = 0;
	double xmax = 12;
	return std::make_pair(xmin,xmax);
}

// DlFromThetal

DlFromThetal::DlFromThetal(const MM::Material* material) :
		MaterialFunctorBase(material)
{
	isDiffusivity = m_material->m_moistureTransport[MM::U_DIRECTION].liquidTransportType() == MM::MoistureTransport::DL_THETAL_SPLINE;
	if(!isDiffusivity) {

		bool noWaterTransport = m_material->m_identification.m_flags & MM::MaterialIdentification::AIR ||
								m_material->m_identification.m_flags & MM::MaterialIdentification::WATER_TIGHT;

		if( !noWaterTransport) {

			const IBK::LinearSpline& pCThetal = m_material->m_moistureStorage.m_spline[MM::MoistureStorage::F_pC_Thetal];

			if( !pCThetal.empty()) {
				std::vector<double> thetal = pCThetal.x();
				std::vector<double> pc = pCThetal.y();

				size_t count = pc.size();
				for( size_t i=0; i<count; ++i) {
					pc[i] = IBK::f_pow10(pc[i]) * -1;
				}

				pcFromThetal.setValues(thetal, pc);
			}
		}
	}
}

double DlFromThetal::operator()(double Thetal) {
	if(m_material->m_identification.m_flags & MM::MaterialIdentification::AIR ||
	   m_material->m_identification.m_flags & MM::MaterialIdentification::WATER_TIGHT)
		return 0.0;

	if( isDiffusivity) {
		return m_material->m_moistureTransport[MM::U_DIRECTION].Dl_Thetal(Thetal);
	}


	if( pcFromThetal.empty() )
		return 0.0;

	double Kl = m_material->m_moistureTransport[MM::U_DIRECTION].Kl_Thetal(Thetal);

//		const IBK::LinearSpline& pCThetal = m_material->m_moistureStorage.m_spline[MM::MoistureStorage::F_pC_Thetal];

	double dpCdThetal = pcFromThetal.slope(Thetal);
	return Kl / IBK::RHO_W * dpCdThetal;
}

std::pair<double,double> DlFromThetal::xRange() const {
	double xmin = 0.0;
	double xmax = m_material->m_paraStorage[MM::Material::MP_THETA_EFF].value;
	return std::make_pair(xmin,xmax);
}

// KgFromThetal

KgFromThetal::KgFromThetal(const MM::Material* material) :
		MaterialFunctorBase(material)
{}

double KgFromThetal::operator()(double Thetal) {
	if(m_material->m_identification.m_flags & MM::MaterialIdentification::AIR_TIGHT)
		return 0.0;

	return m_material->m_airTransport[MM::U_DIRECTION].Kg_Thetal(Thetal);
}

std::pair<double,double> KgFromThetal::xRange() const {
	double xmin = 0.0;
	double xmax;
	if(m_material->m_identification.m_flags & MM::MaterialIdentification::AIR) {
		xmax = IBK::f_psat(IBK::T_DEFAULT) / (IBK::RHO_W * IBK::R_VAPOR * IBK::T_DEFAULT);
	} else {
		xmax = m_material->m_paraStorage[MM::Material::MP_THETA_EFF].value;
	}
	return std::make_pair(xmin,xmax);
}

// ThetalFrompC

ThetalFrompC::ThetalFrompC(const MM::Material* material) :
		MaterialFunctorBase(material),
		m_thetalMax(0.0)
{
	if(m_material->m_identification.m_flags & MM::MaterialIdentification::AIR) {
		m_thetalMax = IBK::f_psat(IBK::T_DEFAULT) / (IBK::RHO_W*IBK::R_VAPOR*IBK::T_DEFAULT);
	}
	else {
		m_thetalMax = m_material->m_paraStorage[MM::Material::MP_THETA_EFF].value;
	}
}

ThetalFrompC::ThetalFrompC(const ThetalFrompC& src) :
	MaterialFunctorBase(src.m_material),
	m_thetalMax(src.m_thetalMax)
{}

double ThetalFrompC::operator()(double pC) {
	if(m_material->m_identification.m_flags & MM::MaterialIdentification::VAPOR_TIGHT &&
	   m_material->m_identification.m_flags & MM::MaterialIdentification::WATER_TIGHT)
		return 0.0;

	if(m_material->m_identification.m_flags & MM::MaterialIdentification::AIR) {
		double pc = -1.0 * std::pow(10, pC);
		double rh = IBK::f_relhum(IBK::T_DEFAULT, pc);
		return rh * m_thetalMax;
	}
	else {
		double pc = -1.0 * std::pow(10, pC);
		return m_material->m_moistureStorage.Thetal_pl(pc);
	}
}

std::pair<double,double> ThetalFrompC::xRange() const {
	double xmin = 0.0;
	double xmax = 12.0;
	return std::make_pair(xmin,xmax);
}

// pcFromThetal

pcFromThetal::pcFromThetal(const MM::Material* material, bool positivePressure) :
		MaterialFunctorBase(material),
		m_positvePressure(positivePressure)
{
	if(m_material->m_identification.m_flags & MM::MaterialIdentification::AIR) {
		m_thetalMax = IBK::f_psat(IBK::T_DEFAULT) / (IBK::RHO_W*IBK::R_VAPOR*IBK::T_DEFAULT);
	} else {
		m_thetalMax = m_material->m_paraStorage[MM::Material::MP_THETA_EFF].value;
	}
}

double pcFromThetal::operator()(double Thetal) {
	if(m_material->m_identification.m_flags & MM::MaterialIdentification::VAPOR_TIGHT &&
	   m_material->m_identification.m_flags & MM::MaterialIdentification::WATER_TIGHT)
		return 0.0;

	double factor = m_positvePressure ? -1.0 : 1.0;

	if(m_material->m_identification.m_flags & MM::MaterialIdentification::AIR) {
		double rh = Thetal >= m_thetalMax ? 1.0 : Thetal / m_thetalMax;
		return IBK::f_pc_rh(rh, IBK::T_DEFAULT) * factor;
	} else {
		return m_material->m_moistureStorage.pl_Thetal(Thetal, IBK::T_DEFAULT) * factor;
	}
}

std::pair<double,double> pcFromThetal::xRange() const {
	double xmin = 0.0;
	return std::make_pair(xmin,m_thetalMax);
}

// DpcFromDThetal

DpcFromDThetal::DpcFromDThetal(const MM::Material* material) :
		MaterialFunctorBase(material)
{
	const IBK::LinearSpline& pCThetal = m_material->m_moistureStorage.m_spline[MM::MoistureStorage::F_pC_Thetal];
	const IBK::LinearSpline& ThetalRH = m_material->m_moistureStorage.m_spline[MM::MoistureStorage::F_Thetal_rh];
	std::vector<double> thetal;
	if( !pCThetal.empty())
		thetal = pCThetal.x();
	else if( !ThetalRH.empty()) {
		thetal = ThetalRH.y();
	}
	else if( m_material->m_identification.m_flags & MM::MaterialIdentification::AIR) {
		for(unsigned int i=0; i<100; ++i) {
			thetal.push_back(i/100.0);
        }
	}

	if( !thetal.empty()) {
		std::vector<double> pc(thetal.size(), 0);
		MM::pcFromThetal pcCreator(material);

		size_t count = pc.size();
		for( size_t i=0; i<count; ++i) {
			pc[i] = pcCreator(thetal[i]);
		}

		pcFromThetal.setValues(thetal, pc);
	}
}

double DpcFromDThetal::operator()(double Thetal) {
	if(m_material->m_identification.m_flags & MM::MaterialIdentification::VAPOR_TIGHT &&
	   m_material->m_identification.m_flags & MM::MaterialIdentification::WATER_TIGHT)
		return 0.0;

	if( pcFromThetal.empty() )
		return 0.0;

	return pcFromThetal.slope(Thetal);
}

std::pair<double,double> DpcFromDThetal::xRange() const {
	double xmin = 0.0;
	return std::make_pair(xmin,m_thetalMax);
}

// pCFromThetal

pCFromThetal::pCFromThetal(const MM::Material* material) :
		MaterialFunctorBase(material)
{
	if(m_material->m_identification.m_flags & MM::MaterialIdentification::AIR) {
		m_thetalMax = IBK::f_psat(IBK::T_DEFAULT) / (IBK::RHO_W*IBK::R_VAPOR*IBK::T_DEFAULT);
	} else {
		m_thetalMax = m_material->m_paraStorage[MM::Material::MP_THETA_EFF].value;
	}
}

double pCFromThetal::operator()(double Thetal) {
	if(m_material->m_identification.m_flags & MM::MaterialIdentification::VAPOR_TIGHT &&
	   m_material->m_identification.m_flags & MM::MaterialIdentification::WATER_TIGHT)
		return 0.0;

	if(m_material->m_identification.m_flags & MM::MaterialIdentification::AIR) {
		double rh = Thetal >= m_thetalMax ? 1.0 : Thetal / m_thetalMax;
		return std::log10(IBK::f_pc_rh(rh, IBK::T_DEFAULT) * -1.0);
	} else {
		return std::log10(m_material->m_moistureStorage.pl_Thetal(Thetal, IBK::T_DEFAULT) * -1.0);
	}
}

std::pair<double,double> pCFromThetal::xRange() const {
	double xmin = 0.0;
	return std::make_pair(xmin,m_thetalMax);
}

/// Comparison operator == between two VG data sets.
bool operator==(const MrcVG& lhs, const MrcVG& rhs) {
	if (lhs.m_n != rhs.m_n) return false;
	return (lhs.m_p == rhs.m_p);
}


// ROMBERG-Integration
// (algorithm found on http://www.voidware.com/rombint.htm)
//
// Approximates the numeric integral of `f' between `a' and `b' subject
// to a given `eps'ilon of error.
//
// Use Romberg method with refinement substitution, x = (3u-u^3)/2 which
// prevents endpoint evaluation and causes non-uniform sampling.
//
// max samples 2^(ROMB_MAX)+1
//
double romberg_integral(const IBK::ScalarFunction &f, double a, double b, double eps) {
	const int ROMB_MAX = 20;

	int n, i=0;
	double s;

	double c[ROMB_MAX+1];
	double p;
	double t, u;

	double h = 2;
	c[0] = 0;
	double l = 0;
	int lim = 1;
	b -= a;
	for (n = 0; n < ROMB_MAX; ++n) {
		p = h/2 - 1;
		s = 0.0;
		for (i = 0; i < lim; i++) {
			t = 1-p*p;
			u = p + t*p/2;
			u = (u*b + b)/2 + a;
			s = s + t*f(u);
			p += h;
		}

		p = 4;
		t = c[0];
		c[0] = (c[0] + h*s)/2.0;
		for(i = 0; i <= n; i++) {
			u = c[i+1];
			c[i+1] = (p*c[i] - t)/(p-1);
			t = u;
			p *= 4;
		}

		if (std::fabs(c[i] - l) < eps * std::fabs(c[i]) * 16) break;
		l = c[i];
		lim <<= 1;
		h /= 2.0;
	}
	return c[i]*b*3/4;
}
// ----------------------------------------------------------------------------


} // namespace MM




