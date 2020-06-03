#ifndef MM_MoistureStorageH
#define MM_MoistureStorageH

#include <string>
#include <iosfwd>
#include <vector>
#include <map>

#include <IBK_LinearSpline.h>
#include <IBK_physics.h>

#include "MM_Functions.h"
#include "MM_Types.h"
#include "MM_ModelDataContainer.h"
#include "MM_MaterialFunctionBase.h"

namespace MM {

class Material;

/*! \brief Class CubicMRCTransferFunction used as transfer function for MRC from spline to linear extension.
The class calculates the corresponding parameter for a cubic spline based on the given moisture storage parameters
and Theatl_pC spline.
It is used in the range from pl = 0 to pl = -100Pa. Lower than -100Pa the original spline is used.
Higher than 0Pa a linear functuion with the slope m_endSlope is used. This will be done in MoistureStorage::Thetal_pl.
*/
class CubicMRCTransferFunction {
public:
	CubicMRCTransferFunction() ;

	/*! Initialisation function
	 \param mrcSpline Thetal_pC spline from MoistureStorage class.
	 \param endSlope Slope of linear extension (\see MoistureStorage::Thetal_pl_slope).
	 \param thetaEff Theta_eff from Material::storageParameters.
	 All parameters necessary for calculation will be calculated here.
	 If the calculated cubic spline has a extreme value between 0Pa and -100Pa a square polynom will be used instead.
	 The init function should be called in MoistureStorage::prepareCalculation.
	*/
	void init(const IBK::LinearSpline& mrcSpline, double endSlope, double thetaEff);

	/*! Function calculates Thetal for given pl based on cubic or square function calulated in constructor.
		\param pl Capillary pressure.
	*/
	double Thetal_pl(double pl) const;

private:
	double				m_aCubic;
	double				m_bCubic;
	double				m_cCubic;
	double				m_dCubic;
	double				m_aSquare;
	double				m_bSquare;
	double				m_cSquare;
	bool				m_cubic;
};

/*! \brief Class MoistureStorage contains all data for the moisture storage functions.
Data can be stored for all available models. The model that is actually
used is identified by the member variable m_type.

The parametrization in the linear splines is essential for calculation. Parametrization
for analytical functions is optional. There must be either a moisture rentention function
spline, a sorption isotherm spline, or a MRC model parametrization.

Calculation Functionality
---------------------

The class provides calculation capability to compute the moisture storage functionality and
the reverse moisture retention function via the functions Thetal_pl() and pl_Thetal().
The following calculation cases are supported, based on the parametrization that is provided:

1.	if MRC and reversed-MRC spline data is given, both splines are directly used for evaluation
	of Thetal_pl() and pl_Thetal(). In both cases, linear interpolation is done in log10(-pl)
	representation for unsaturated moisture range.
2.	if MRC spline data is given, but rMRC data is missing, the MRC spline is used directly
	for evaluation of both functions. In the case of the reverse moisture retention function
	a numerical algorithm (root finding) is used to determine the pl value from moisture content.
3.	if sorption isotherm data is given, a reversed sorption isotherm is generated and
	both splines are used for calculation. The interpolation is done linearly based on
	relative humidity.

\note There must be only one model block within the MoistureStorage parameter section. This can
	  either describe a MRC or sorption isotherm. If spline data is given additionally, it must
	  be generated from the parametrized function.

Model Support
-------------

The supported model types are uniquely named and identified via keyword (ModelTypes).
- [GAUSS] Gauss-type MRC model, analytical representation for moisture capacity function, MRC function is
  computed numerically and therefore a linear spline is generated during prepare calculation
- [VAN_GENUCHTEN] Power-function series by van Genuchten, analytical expression for MRC function,
  directly evaluated.

In the case of the Gauss-model, the evaluation of the Gauss function itself is costly, so a
moisture retention function is generated automatically.

*/
class MoistureStorage : public MaterialFunctionBase {
public:

	// ***KEYWORDLIST-START***
	/*! Functions provided via spline data. */
	enum Functions {
		/*! The moisture retention function Thetal in [m3/m3] as function of log10(-pc). */
		F_Thetal_pC,			// Keyword: Theta_l(pC)_de  Ol(pC)
		/*! The reverse moisture retention function log10(-pc) as function of Thetal in [m3/m3].
			\deprecated While it is still possible to provide reverse MRC data, it is no longer necessary and does
						not bring any significant benefits (but the drawback of larger material files and
						potential consistency problems). In new materials the function is no longer
						necessary. This must be reviewed, The deprecated flag is V6 solvers only.
		*/
		F_pC_Thetal,			// Keyword: pC(Theta_l)_de  pC(Ol)
		/*! The sorption isotherm function Thetal in [m3/m3] as function of relative humidity in [-]. */
		F_Thetal_rh,			// Keyword: Theta_l(RH)  Theta_l(RH)_de Ol(RH)
		/*! The adsorption moisture retention function as spline. */
		F_Thetal_pC_ad,			// Keyword: Theta_l(pC)_ad
		/*! The adsorption moisture retention function as spline. */
		F_pC_Thetal_ad,			// Keyword: pC(Theta_l)_ad
		/*! The equilibrium unfrozen water mass density [kg/m3] depending on temperature in [K]. */
		F_mwu_T,				// Keyword: mwu(T)
		NUM_Functions
	};

	/*! All known/supported moisture storage models. */
	enum ModelTypes {
		MT_GAUSS,					// Keyword: GAUSS				'Gauss MRC-Model'
		MT_VAN_GENUCHTEN,			// Keyword: VAN_GENUCHTEN		'van Genuchten MRC-Model'
		NUM_ModelTypes
	};
	// ***KEYWORDLIST-END***


	/*! \brief Struct for different models available for the moisture retention characteristics.*/
	enum MoistureStorageType {
		/*! Using moisture retention function given as spline. */
		ST_THETA_L_PC_SPLINE,
		/*! Using moisture retention function given as analytical model. */
		ST_THETA_L_PC_MODEL,
		/*! Using sorption isotherm function. */
		ST_THETA_L_RH_SPLINE,
		/*! Using sorption isotherm function given as analytical model. */
		ST_THETA_L_RH_MODEL,
		NUM_MoistureStorageType
	};

	// *** PUBLIC MEMBER FUNCTIONS ***

	/*! Default constructor.*/
	MoistureStorage() { clear(); }

	/*! Reads data from the string 'data'.
		Throws an IBK::Exception if reading fails.
		\param data Complete block in one string.
	*/
	void read(const std::string& data);

	/*! Reads data from the vector of strings (lines).
		Throws an IBK::Exception if reading fails.
		\param data Vector of lines.
	*/
	void read(const std::vector<std::string>& data);

	/*!	Reads data from input stream.
		Throws an IBK::Exception if reading fails.
		It uses the binary representation.
		\param in	The input stream.
	*/
	void readBinary(std::istream& in);

	/*! Writes the data into the stream 'out' using 'indent' spaces for indentation. */
	void write(std::ostream& out, unsigned int indent=0, MaterialDirection direction=U_DIRECTION, bool writeEmptyKeywords=false ) const;

	/*!	Writes data into the output stream.
		It uses the binary representation.
		\param out	The output stream.
	*/
	void writeBinary(std::ostream& out) const;

	/*! Clears the moisture storage data.*/
	void clear();

	/*! Returns the storage type.
		The storage type is determined in prepareCalculation().
		\sa StorageType.
	*/
	MoistureStorageType storageType() const { return m_type; }

	/*! Computes moisture retention function, Thetal in [m3/m3] as function of liquid pressure in [Pa].
		Depending on the storage data provided, either the sorption isotherm is used (interpolated
		using relative humidities) or the moisture retention function (interpolated in log10(-pc) ).
		\param pl Liquid pressure in [Pa], if negative, equal to capillary pressure
		\param T Temperature in [K], only needed if sorption isotherm is used.
	*/
	double Thetal_pl(double pl, double T = IBK::T_DEFAULT) const;

	/*! Computes reverse moisture retention function, liquid pressure in [Pa] as function of
		moisture content Thetal in [m3/m3].
		\param Thetal Moisture content in [m3/m3].
		\param T Temperature in [K], only needed if sorption isotherm is used.
		\param pl_estimate Estimated liquid pressure in [Pa], needed for iterative evaluation of rMRC
	*/
	double pl_Thetal(double Thetal, double T = IBK::T_DEFAULT, double pl_estimate=-100) const;

	/*! Returns slope of moisture storage function in super-saturated range (positive liquid pressures).
		\return Slope in [m3/m3Pa].
	*/
	double Thetal_pl_slope() const { return m_Thetal_pl_slope; }

	/*! Computes the unfrozen water mass density mwu in kg/m3] as function of temperature in [K].
		It calculates the equilibrium between frozen and unfrozen water for the given temperature based on the storage data provided.
		Function F_mwu_T must be given.
		\param T Temperature in [K], must be < 0 C
	*/
	double mwu_T(double T) const;

	/*! Checks that MRC and rMRC are really inverse functions.
		This function is called from prepareCalculation().
	*/
	void doReverseCheck() const;

	/*! Initializes this data structure for calculation.
		This function is called from Material::prepareCalculation().
		\param mat Pointer to the parent MM::Material class.
		\param direction The direction to initialize material data to.
	*/
	void prepareCalculation(const Material* mat);


	// *** PUBLIC MEMBER VARIABLES ***

	/*! Container for all spline data. */
	IBK::LinearSpline		m_spline[NUM_Functions];

	/*! Counter for total number of iterations within pC_Thetal_iterative() function. */
	static unsigned int GLOBAL_MRC_ITERATIONS;

private:

	/*! Check parametrization for valid ranges.
		Throws an exception if range is exceeded.
		This function is called from prepareCalculation().
		\param	mat Pointer to the parent MM::Material class.
		\return	Returns true if all ranges are correct. In case of an error, messages
				are written using IBK::IBK_Message.
	*/
	void checkLimits(const Material* mat);

	/*! Denormalize a normalized spline data.
		For compatibility reasons to old material files.
		This function is called from prepareCalculation().
		If the function returns with an error code DNR_Error than
		error messages were printed using IBK::IBK_Message.
		\param	mat Pointer to the parent MM::Material class.
	*/
	void denormalize(const Material* mat);

	/*! Computes reverse moisture retention function, log10 of capillary pressure in [Pa] as function of
		moisture content Thetal in [m3/m3] using the MRC function and the secant-method.
		\param Thetal Moisture content in [m3/m3].
		\param pC_estimate Estimated log10 of capillary pressure, needed for iterative evaluation of rMRC.
	*/
	double pC_Thetal_iterative(double Thetal, double pC_estimate=2) const;


	// *** PRIVATE MEMBER VARIABLES ***

	/*! Defines which parametrization for MRC is given (determined in prepareCalculation()). */
	MoistureStorageType		m_type;

	/*! Holds Theta_l_eff in [m3/m3]. */
	double					m_ThetalEff;

	/*! Holds the extrapolation slope in [m3/m3Pa] used for extrapolation of liquid pressures */
	double					m_Thetal_pl_slope;

	/*! Holds Theta_l_limit in [m3/m3].
		This is the maximum moisture content that is computed with the MRC function.
		Above this moisture content a linear shape of the moisture storage function is assumed.
	*/
	double					m_ThetalLimit;

	/*! Holds pl_limit in [Pa], the limiting liquid pressure corresponding to m_ThetalLimit. */
	double					m_plLimit;

	/*! Spline data for auto-generated reverse sorption isotherm. */
	IBK::LinearSpline		m_RH_Thetal_spline;

	/*! Identifies the model to be used for calculation.
		Only meaningful if m_type is ST_THETA_L_PC_MODEL or ST_THETA_L_RH_MODEL.
	*/
	ModelTypes				m_activeModel;

	/*! Calculation class for MRC type "VAN_GENUCHTEN".
		Initialized in prepareCalculation() and used in Theta_l(pc).
	*/
	MrcVG					m_modelVG;

	/*! Class instance for calculating parameters and valuzes of a additional cubic function.*/
	CubicMRCTransferFunction	m_cubicMRCTransfer;

	/*! Is true if the material is air.
		Parameter will be set in prepareCalculation based on identification flag AIR.
	*/
	bool					m_air;

	friend class Material;
};

/*! Comparison operator == between two mrc data sets.*/
bool operator==(const MoistureStorage& lhs, const MoistureStorage& rhs);
/*! Comparison operator != between two mrc data sets.*/
inline bool operator!=(const MoistureStorage& lhs, const MoistureStorage& rhs) { return !(lhs==rhs); }

/*! \file MM_MoistureStorage.h
	\brief Contains the class MoistureStorage which encapsulates the moisture storage functions.
*/

} // namespace MM

#endif // MM_MostStorageH

