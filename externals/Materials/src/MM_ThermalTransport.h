#ifndef MM_ThermalTransportH
#define MM_ThermalTransportH

#include <string>
#include <iosfwd>

#include <IBK_LinearSpline.h>

#include "MM_Functions.h"
#include "MM_Types.h"
#include "MM_ModelDataContainer.h"
#include "MM_MaterialFunctionBase.h"

namespace MM {

class Material;

/*! \brief Class ThermalTransport contains all data for the thermal conductivity models.*/
class ThermalTransport : public MaterialFunctionBase {
public:

	// ***KEYWORDLIST-START***
	/*! Functions provided via spline data. */
	enum Functions {
		/*! Lambda values as function of Thetal (m3/m3). */
		F_lbd_Thetal,				// Keyword: lambda(Theta_l) lambda(Ol)
		/*! The tabulated lambda values as function of T (T in [C]).
			\warning Mind that the spline holds temperatures in degree C as x-values!
		*/
		F_lbd_T,					// Keyword: lambda(T)
		NUM_Functions
	};
	// ***KEYWORDLIST-END***

	/*! Enum with all possible thermal transport model parametrizations.
		The actual function type of the thermal conductivity function is determined during
		runtime and stored in m_type.
	*/
	enum ThermalTransportType {
		/*! Lambda as function of moisture content (linear spline data) */
		ST_LBD_THETA_L_SPLINE,
		/*! Lambda as function of temparatur (linear spline data) */
		ST_LBD_T_SPLINE,
		/*! Lambda as function of temparatur and moisture content (linear spline data for temperature) */
		ST_LBD_T_THETA_L_SPLINE,
		/*! Lambda as function of moisture content (trivial linear model) */
		ST_LBD_THETA_L_LINEAR,
		NUM_ThermalTransportType
	};


	// *** PUBLIC MEMBER FUNCTIONS ***

	/*! Default constructor.*/
	ThermalTransport() { clear(); }

	/*! Reads data from the string 'data'.
		Throws an IBK::Exception if reading fails.
	*/
	void read(const std::string& data);

	/*! Reads data from the vector of strings (lines) 'data'.
		Throws an IBK::Exception if reading fails.
	*/
	void read(const std::vector<std::string>& data);

	/*!	Reads data from input stream.
		Throws an IBK::Exception if reading fails.
		It uses the binary representation.
		\param in	The input stream.
	*/
	void readBinary(std::istream& in);

	/*! Writes the data into the stream 'out' using 'indent' spaces for indentation. */
	void write(std::ostream& out, unsigned int indent=0, MaterialDirection direction=U_DIRECTION, bool writeEmptyKeywords=false) const;

	/*!	Writes data into the output stream.
		It uses the binary representation.
		\param out	The output stream.
	*/
	void writeBinary(std::ostream& out) const;

	/*! Clears the data. */
	void clear();

	/*! Return the type of parametrization used with this thermal transport model. */
	ThermalTransportType thermalTransportType() const { return m_type; }

	/*! Calculates the thermal conductivity for a given moisture and/or ice content and temperature.
		The liquid water content must be pure liquid water without ice. Vapor content can be neglected.
		If the model doesn't support a dependency on either parameter, it is treated as a dummy
		parameter.
		\param Thetal Liquid moisture content in [m3/m3]
		\param T Temperature in [K]
		\param ThetaIce Ice content in [m3/m3]. Not used in case of no ice calculation.
		\return Returns thermal conductivity in [W/mK]
	*/
	double lambda(double Thetal, double T, double ThetaIce = 0.0) const;


	// *** PUBLIC MEMBER VARIABLES ***

	/*! Container for all spline data. */
	IBK::LinearSpline		m_spline[NUM_Functions];

private:

	/*! Initializes this data structure for calculation.
		This function is called from Material::prepareCalculation().
		\param mat Pointer to the parent MM::Material class.
		\param direction The direction to initialize material data to.
	*/
	void prepareCalculation(const Material* mat, MaterialDirection direction);

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
		This function is called just after material data was read in
		MM::Material. If the function returns with an error code DNR_Error
		than error messages were printed using IBK::IBK_Message.
		\param	mat Pointer to the parent MM::Material class.
	*/
	void denormalize(const Material* mat);


	// *** PRIVATE MEMBER VARIABLES ***

	ThermalTransportType	m_type;					///< The choosen model.

	/*! The lambda dry value in [W/mK].
		The value is transfered in initializeBasicParameter() from the Material basic
		parameter list and cached for use during solver runtime.
	*/
	double					m_lambdaDry;


	friend class Material;
};

/*! Comparison operator == between two thermal conductivity data sets.*/
bool operator==(const ThermalTransport& lhs, const ThermalTransport& rhs);
/*! Comparison operator != between two thermal conductivity data sets.*/
inline bool operator!=(const ThermalTransport& lhs, const ThermalTransport& rhs) { return !(lhs==rhs); }

/*! \file MM_ThermalTransport.h
	\brief Contains the class ThermalTransport which encapsulates the thermal conductivity functions.
*/

} // namespace MM

#endif // MM_ThermalTransportH



