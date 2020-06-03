#ifndef MM_FunctionsH
#define MM_FunctionsH

#include <vector>
#include <cmath>
#include <IBK_math.h>
#include <IBK_ScalarFunction.h>
#include <IBK_LinearSpline.h>

namespace MM {

class Material;
class ModelDataContainer;

/*! \brief Function object encapsulating the derivation after pC of the GAUSS type moisture retention curve.
	This function shares the parameter data with the actual MRC function object.
*/
class MrcGAUSS_dpC : public IBK::ScalarFunction {
public:
	/*! Default constructor. Creates an empty Gauss function object.*/
	MrcGAUSS_dpC() {}

	/*! Constructs a instance for a Gauss function of order n with the parameters given in p.*/
	MrcGAUSS_dpC(const unsigned int /*n*/, const std::vector<double>& p) : m_p(p) {}

	/*! Creats a Gauss function from the given string.*/
	MrcGAUSS_dpC(const std::string& data);

	/*! Creats a Gauss function from three string with parameters.*/
	void setData(const std::string& Wi, const std::string& Ri, const std::string& Si);

	/*! Creats a Gauss function from a model data container.*/
	void setData(const ModelDataContainer& modelDataContainer);

	/*! Clears the data.*/
	void clear();

	/*! Returns the moisture content for the given capillary pressure pC.
		\param pC Capillary pressure given in log(Pa).
	*/
	double operator()(double pC) const;

	friend bool operator==(const MrcGAUSS_dpC& lhs, const MrcGAUSS_dpC& rhs) {
		return (lhs.m_p == rhs.m_p);
	}
private:
	/*! Contains the Gauss-model parameters in the order of w_i, r_i, s_i.*/
	std::vector<double>	m_p;

	friend class MrcGAUSS;
};


/*! \brief Function object encapsulating the GAUSS type moisture retention curve.
	\note The operator() takes a pC as argument.
*/
class MrcGAUSS : public IBK::ScalarFunction {
public:
	/*! Default constructor.*/
	MrcGAUSS() : m_thetaEff(1), m_tolerance(1e-10) {}

	/*! Initialisation constructor.*/
	MrcGAUSS(double thetaEff, const MrcGAUSS_dpC& dpC_mrc) : m_thetaEff(thetaEff), m_dpC_mrc(dpC_mrc) {}

	/*! Allows to specify the data of the GAUSS model through strings holding the individual
		parameters as white-space separated doubles. The number of doubles must be the same for
		all strings.
	*/
	void setData(double thetaEff, const std::string& Wi, const std::string& Ri, const std::string& Si);

	/*! Allows to specify the data of the GAUSS model through the model data container.
	*/
	void setData(double thetaEff, const ModelDataContainer& modelDataContainer);

	/*! Clears the data.*/
	void clear() {	m_dpC_mrc.clear(); m_thetaEff=1; }

	/*! Implementation of MRC curve, takes the logarithm of the capillary pressures as
		argument! Returns a moisture content in [m3/m3] in the range of 0 .. m_thetaEff.
	*/
	double operator()(double pC) const { return m_thetaEff * norm_w(pC); }

	/*! Returns the integral under the given GAUSS derivative function between 0 and pC,
		which is a normalized moisture content in the range 0 .. 1.
		\param pC Capillary pressure given in log(Pa).
	*/
	double norm_w(double pC) const;

	double w(unsigned int index) const { return m_dpC_mrc.m_p[3*index]; }
	double r(unsigned int index) const { return m_dpC_mrc.m_p[3*index+1]; }
	double s(unsigned int index) const { return m_dpC_mrc.m_p[3*index+2]; }

	void setTolerance(double tolerance) {
		m_tolerance = tolerance;
	}

	unsigned int modality() const { return (unsigned int)m_dpC_mrc.m_p.size() / 3; }

	void setDefault();

	/*! Converts MRC Gauss parameters given as triples of Ol_i, pC_i, std_i into
		parameters given in W_i, r_i, std_i.*/
	void setFrom_pC(const std::vector<double>& param_mrc_pC, double thetaEff);

	/*! Converts MRC Gauss parameters given as triples of W_i, r_i, std_i into
		parameters given in Ol_i, pC_i, std_i.*/
	void convertTo_pC(std::vector<double>& param_mrc_pC, double thetaEff) const;

	double dpC_mrc(double pC) const { return m_dpC_mrc(pC); }

	/*! return current value of theta effective in this class. */
	double thetaEff() const { return m_thetaEff; }

	friend bool operator==(const MrcGAUSS& lhs, const MrcGAUSS& rhs) {
		if (lhs.m_thetaEff != rhs.m_thetaEff) return false;
		return (lhs.m_dpC_mrc == rhs.m_dpC_mrc);
	}
private:
	double			m_thetaEff;		///< Effective moisture content in [m3/m3], scales the function.
	MrcGAUSS_dpC	m_dpC_mrc;		///< The function object to calculate the MRC derivative.
	double			m_tolerance;	///< Tolerance criterion for numerical integration.
};

/*! Comparison operator == between two GAUSS data sets.*/
bool operator==(const MrcGAUSS& lhs, const MrcGAUSS& rhs);
/*! Comparison operator != between two mrc data sets.*/
inline bool operator!=(const MrcGAUSS& lhs, const MrcGAUSS& rhs) { return !(lhs==rhs); }


/*! \brief Function object encapsulating the VG2 type moisture retention curve.

	\note The operator() takes a pC as argument wheras thetaEff() takes a pc as argument.
*/
class MrcVG : public IBK::ScalarFunction {
public:
	/*! Default constructor.*/
	MrcVG() : m_thetaEff(1), m_n(0) {}

	/*! Initialisation constructor based on map string.*/
	MrcVG(const std::string& data);

	/*! Initialisation constructor with modality and parameter vector.*/
	MrcVG(unsigned int n, const std::vector<double>& p) : m_n(n), m_p(p) {}

	/*! Clears the data.*/
	void clear();

	/*! Sets the data for the VG calculation. */
	void setData(double thetaEff, const std::string& Li, const std::string& Ci, const std::string& Ni);

	/*! Allows to specify the data of the VG model through the model data container.*/
	void setData(double thetaEff, const ModelDataContainer& modelDataContainer);

	/*! Implementation of MRC curve, takes the logarithm of the capillary pressures as
		argument and returns the moisture content in [m3/m3] in the range of 0 .. m_thetaEff.
	*/
	double operator()(double pC) const { return m_thetaEff*norm_w(-IBK::f_pow10(pC)); }

	/*! Implementation of MRC curve, takes the capillary pressures in [Pa] as
		argument and returns the moisture content in [m3/m3] in the range of 0 .. m_thetaEff.
	*/
	double thetaEff(double pc) const { return m_thetaEff*norm_w(pc); }

	/*! Implementation of MRC curve, takes the capillary pressure in [Pa] as
		argument and returns the normalized moisture content 0 <= norm_w <= 1.
	*/
	double norm_w(double pc) const;

	/*! Implementation of moisture capacity function, takes the capillary pressure
		in [Pa] as argument!
	*/
	double norm_cap(double pc) const;

	double				m_thetaEff;		///< Effective moisture content in [m3/m3], scales the function.
	unsigned int 		m_n;			///< Modality of the VG function.
	/*! order of the parameters: l_1 c_1 n_1   l_2 c_2 n_2  ...  l_i c_i n_i.*/
	std::vector<double>	m_p;
};

// material functors

struct MaterialFunctorBase {
public:
	/*! Standard constructor.
		\param material Pointer to material.
	*/
	MaterialFunctorBase(const MM::Material* material);

	/*! Returns true if the internal material is valid*/
	bool valid();

protected:
	const MM::Material* m_material;	///< Pointer to used material.
};

/*! \brief Functor for reverse sorption isotherm.*/
struct RHFromThetal : public MaterialFunctorBase {
public:
	/*! Standard constructor.
		\param material Pointer to material.
	*/
	RHFromThetal(const MM::Material* material);

	/*! Functional operator.
		\param Thetal volumetric moisture content (m3/m3).
		\return Relative humidity (---).
	*/
	double operator()(double Thetal);

	/*! Returns the range for x values <min,max>.*/
	std::pair<double,double> xRange() const;

private:
	double m_thetalMax;				///< Maximum moisture content (normally ThetaEff);
};

/*! \brief Functor for sorption isotherm.*/
struct ThetalFromRH : public MaterialFunctorBase {
public:
	/*! Standard constructor.
		\param material Pointer to material.
	*/
	ThetalFromRH(const MM::Material* material);

	/*! Functional operator.
		\param RH Relative humidity (---).
		\return volumetric moisture content (m3/m3).
	*/
	double operator()(double RH);

	/*! Returns the range for x values <min,max>.*/
	std::pair<double,double> xRange() const;

private:
	double m_thetalMax;				///< Maximum moisture content (normally ThetaEff);
};

/*! \brief Functor for moisture depending thermal conductivity.*/
struct LamdaFromThetal : public MaterialFunctorBase {
public:
	/*! Standard constructor.
		\param material Pointer to material.
	*/
	LamdaFromThetal(const MM::Material* material);

	/*! Functional operator.
		\param Thetal volumetric moisture content (m3/m3).
		\return Thermal conductivity (W/mK).
	*/
	double operator()(double Thetal);

	/*! Returns the range for x values <min,max>.*/
	std::pair<double,double> xRange() const;
};

/*! \brief Functor for temperature depending thermal conductivity.*/
struct LamdaFromT : public MaterialFunctorBase {
public:
	/*! Standard constructor.
		\param material Pointer to material.
	*/
	LamdaFromT(const MM::Material* material);

	/*! Functional operator.
		\param T Temperature (C).
		\return Thermal conductivity (W/mK).
	*/
	double operator()(double T);

	/*! Returns the range for x values <min,max>.*/
	std::pair<double,double> xRange() const;
};

/*! \brief Functor for moisture depending vapour conductivity.*/
struct KvFromThetal : public MaterialFunctorBase {
public:
	/*! Standard constructor.
		\param material Pointer to material.
	*/
	KvFromThetal(const MM::Material* material);

	/*! Functional operator.
		\param Thetal volumetric moisture content (m3/m3).
		\return Vapour conductivity (s).
	*/
	double operator()(double Thetal);

	/*! Returns the range for x values <min,max>.*/
	std::pair<double,double> xRange() const;

private:
	/*! Functor for calculating relative humidity from water content.
		Will be used in case of given relative humidity depending vapour resistance factor function.
	*/
	RHFromThetal m_RHFromThetal;
};

/*! \brief Functor for vapour diffusion resistance factor depending on relative humidity.
	Is normally used for moisture depending vapour retarders.
*/
struct MewFromRH : public MaterialFunctorBase {
public:
	/*! Standard constructor.
		\param material Pointer to material.
	*/
	MewFromRH(const MM::Material* material);

	/*! Functional operator.
		\param RH Relative humidity (---).
		\return Water vapour resistance factor (---).
	*/
	double operator()(double RH);

	/*! Returns the range for x values <min,max>.*/
	std::pair<double,double> xRange() const;

private:
	/*! Functor for calculating water content from relative humidity.
		Will be used in case of not existing relative humidity depending vapour resistance factor function.
		In this case other vapour transport functions will be used (e.g. vapour conductivity).
	*/
	ThetalFromRH m_thetalFromRH;
};

/*! \brief Functor for moisture depending vapour diffusivity.*/
struct DvFromThetal : public MaterialFunctorBase {
public:
	/*! Standard constructor.
		\param material Pointer to material.
	*/
	DvFromThetal(const MM::Material* material);

	/*! Functional operator.
		\param Thetal volumetric moisture content (m3/m3).
		\return vapour diffusivity (m2/s).
	*/
	double operator()(double Thetal);

	/*! Returns the range for x values <min,max>.*/
	std::pair<double,double> xRange() const;

private:
	/*! Functor for calculating relative humidity from water content.
		Will be used in case of given relative humidity depending vapour resistance factor function.
	*/
	RHFromThetal m_RHFromThetal;
};

/*! \brief Functor for moisture depending liquid water conductivity.*/
struct KlFromThetal : public MaterialFunctorBase {
public:
	/*! Standard constructor.
		\param material Pointer to material.
	*/
	KlFromThetal(const MM::Material* material);

	/*! Functional operator.
		\param Thetal volumetric moisture content (m3/m3).
		\return Liquid water conductivity (s).
	*/
	double operator()(double Thetal);

	/*! Returns the range for x values <min,max>.*/
	std::pair<double,double> xRange() const;

private:
	bool isDiffusivity;   			///< Is true in case of existing diffusivity instead of conductivity.
};

/*! \brief Functor for moisture depending Kirchhoff potential.*/
struct PsiFromThetal : public MaterialFunctorBase {
public:
	/*! Standard constructor.
		\param material Pointer to material.
	*/
	PsiFromThetal(const MM::Material* material);

	/*! Functional operator.
		\param Thetal volumetric moisture content (m3/m3).
		\return Liquid water conductivity (s).
	*/
	double operator()(double Thetal);

	/*! Returns the range for x values <min,max>.*/
	std::pair<double,double> xRange() const;

private:
	bool isDiffusivity;   			///< Is true in case of existing diffusivity instead of conductivity.
};

/*! \brief Functor for moisture depending liquid water diffusivity.*/
struct DlFromThetal : public MaterialFunctorBase {
public:
	/*! Standard constructor.
		\param material Pointer to material.
	*/
	DlFromThetal(const MM::Material* material);

	/*! Functional operator.
		\param Thetal volumetric moisture content (m3/m3).
		\return Liquid water diffusivity (m2/s).
	*/
	double operator()(double Thetal);

	/*! Returns the range for x values <min,max>.*/
	std::pair<double,double> xRange() const;


private:
	bool isDiffusivity;   			///< Is true in case of existing diffusivity instead of conductivity.
	IBK::LinearSpline pcFromThetal;
};

/*! \brief Functor for moisture depending air permeability.*/
struct KgFromThetal : public MaterialFunctorBase {
public:
	/*! Standard constructor.
		\param material Pointer to material.
	*/
	KgFromThetal(const MM::Material* material);

	/*! Functional operator.
		\param Thetal volumetric moisture content (m3/m3).
		\return Air permeability (s).
	*/
	double operator()(double Thetal);

	/*! Returns the range for x values <min,max>.*/
	std::pair<double,double> xRange() const;
};

/*! \brief Functor for calculating capillary pressure from negative logarithmic capillary pressure.
	This negative logarithmic capillary pressure is mainly used for moisture retention characteristic.
*/
struct pcFrompC {
public:
	/*! Functional operator.
		\param pC Logarithmic negative capillary pressure (logPa).
		\return Capillary pressure (Pa).
	*/
	double operator()(double pC) {
		return std::pow(10, pC);
	}
};

/*! \brief Functor for moisture retention characteristic.
	In this case volumetric moisture content depending on negative logarithmic capillary pressure.
*/
struct ThetalFrompC : public MaterialFunctorBase {
public:
	/*! Standard constructor.
		\param material Pointer to material.
	*/
	ThetalFrompC(const MM::Material* material);

	/*! Copy constructor.*/
	ThetalFrompC(const ThetalFrompC& src);

	/*! Functional operator.
		\param pC Logarithmic negative capillary pressure (logPa).
		\return Volumetric water content (m3/m3).
	*/
	double operator()(double pC);

	/*! Returns the range for x values <min,max>.*/
	std::pair<double,double> xRange() const;

private:
	double m_thetalMax;				///< Maximum moisture content (normally ThetaEff);
};

/*! \brief Functor for reverse moisture retention characteristic.
	In this case capillary pressure depending on volumetric moisture content.
*/
struct pcFromThetal : public MaterialFunctorBase {
public:
	/*! Standard constructor.
		\param material Pointer to material.
	*/
	pcFromThetal(const MM::Material* material, bool positivePressure = false);

	/*! Functional operator.
		\param Thetal Volumetric water content (m3/m3).
		\return Capillary pressure (Pa).
	*/
	double operator()(double Thetal);

	/*! Returns the range for x values <min,max>.*/
	std::pair<double,double> xRange() const;

private:
	double 	m_thetalMax;				///< Maximum moisture content (normally ThetaEff);
	bool	m_positvePressure;          ///< If true the pressure will be set to a positive value by multiplying with -1.
};

/*! \brief Functor for derivation of reverse moisture retention characteristic.
	In this case capillary pressure depending on volumetric moisture content.
*/
struct DpcFromDThetal : public MaterialFunctorBase {
public:
	/*! Standard constructor.
		\param material Pointer to material.
	*/
	DpcFromDThetal(const MM::Material* material);

	/*! Functional operator.
		\param Thetal Volumetric water content (m3/m3).
		\return Capillary pressure (Pa).
	*/
	double operator()(double Thetal);

	/*! Returns the range for x values <min,max>.*/
	std::pair<double,double> xRange() const;

private:
	double 	m_thetalMax;				///< Maximum moisture content (normally ThetaEff);
	IBK::LinearSpline pcFromThetal;
};

/*! \brief Functor for reverse moisture retention characteristic.
	In this case logarithm of negative capillary pressure depending on volumetric moisture content.
*/
struct pCFromThetal : public MaterialFunctorBase {
public:
	/*! Standard constructor.
		\param material Pointer to material.
	*/
	pCFromThetal(const MM::Material* material);

	/*! Functional operator.
		\param Thetal Volumetric water content (m3/m3).
		\return Logarithm of negative capillary pressure (lgPa).
	*/
	double operator()(double Thetal);

	/*! Returns the range for x values <min,max>.*/
	std::pair<double,double> xRange() const;

private:
	double m_thetalMax;				///< Maximum moisture content (normally ThetaEff);
};

/*! \brief Functor for liquid water conductivity depending on liquid pressure.
*/
struct KlFrompC : public MaterialFunctorBase {
public:
	/*! Standard constructor.
		\param material Pointer to material.
	*/
	KlFrompC(const MM::Material* material);

	/*! Functional operator.
		\param pc capillaray (liquid) pressure (Pa).
		\return Liquid water conductivity (s).
	*/
	double operator()(double pc);

	/*! Returns the range for x values <min,max>.*/
	std::pair<double,double> xRange() const;

private:
	/*! Functor for calculating water content from capillary pressure.*/
	ThetalFrompC m_thetalFrompC;
	/*! Functor for calculating liquid water conductivity from water content.*/
	KlFromThetal m_KlFromThetal;
};

/*! Comparison operator == between two VG data sets.*/
bool operator==(const MrcVG& lhs, const MrcVG& rhs);
/*! Comparison operator != between two mrc data sets.*/
inline bool operator!=(const MrcVG& lhs, const MrcVG& rhs) { return !(lhs==rhs); }


/*! Performs a numeric integration using the Romberg algorithm.
	\param f 	The function object encapsulating the function.
	\param a	The lower bound of the integration range.
	\param b	The upper bound of the integration range.
	\param eps  (optional) The allowed relative tolerance.
*/
double romberg_integral(const IBK::ScalarFunction &f, double a, double b, double eps);


/*! \file MM_Functions.h
	\brief Contains function objects encapsulating material functions.
*/

} // namespace MM

#endif // MM_FunctionsH



