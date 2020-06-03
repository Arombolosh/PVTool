#ifndef MM_MoistureTransportH
#define MM_MoistureTransportH

#include <string>
#include <iosfwd>
#include <vector>
#include <map>

#include <IBK_LinearSpline.h>

#include "MM_Functions.h"
#include "MM_Types.h"
#include "MM_ModelDataContainer.h"
#include "MM_MaterialFunctionBase.h"

namespace MM {

class Material;

/*! \brief Contains all data for the moisture transport functions.
	Moisture transport can be either stored in a combined way or separately for
	liquid and vapour transport.
	Linear splines are used to store the data.
*/
class MoistureTransport : public MaterialFunctionBase {
public:

	// ***KEYWORDLIST-START***
	/*! Functions provided via spline data. */
	enum Functions {
		/*! Liquid conductivity as function of Thetal (m3/m3).
			Liquid conductivity is stored as log10 values.
		*/
		F_Kl_Thetal,				// Keyword: lgKl(Theta_l) lgKl(Ol)
		/*! Liquid diffusivitiy as function of Thetal (m3/m3).
			Liquid diffusivitiy is stored as log10 values.
		*/
		F_Dl_Thetal,				// Keyword: lgDl(Theta_l) lgDl(Ol)
		/*! Vapor permeability as function of Thetal (m3/m3).
			Vapor permeability is stored as log10 values.
		*/
		F_Kv_Thetal,				// Keyword: lgKv(Theta_l) lgKv(Ol)
		/*! Vapor diffusion resistance factors as function of Thetal (m3/m3). */
		F_mew_rh,					// Keyword: mew(RH) My(Phi)
		NUM_Functions
	};

	/*! All known/supported moisture storage models. */
	enum ModelTypes {
		MT_SP_MODEL,				// Keyword: SERIAL_PARALLEL SP_MODEL			'Serial Parallel vapor transport model'
		NUM_ModelTypes
	};
	// ***KEYWORDLIST-END***

	/*! Enum for all possible liquid water transport types. */
	enum LiquidTransportType {
		/*! Liquid conductivity as function of moisture content. */
		KL_THETAL_SPLINE,
		/*! Liquid diffusivity as function of moisture content. */
		DL_THETAL_SPLINE,
		NUM_LiquidTransportType
	};

	/*! Enum for all possible water vapour transport types. */
	enum VaporTransportType {
		/*! Vapor conductivity as function of moisture content. */
		KV_THETAL_SPLINE,
		/*! Vapor diffusion resistance factor as function of moisture content. */
		MEW_RH_SPLINE,
		/*! Vapor conductivity based on constant mew value (standard model). */
		KV_MEW_CONST,
		NUM_VaporTransportType
	};


	// *** PUBLIC MEMBER FUNCTIONS ***

	/*! Default contructor.*/
	MoistureTransport() { clear(); }

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
	void write(std::ostream& out, unsigned int indent=0, MaterialDirection direction=U_DIRECTION, bool writeEmptyKeywords=false ) const;

	/*!	Writes data into the output stream.
		It uses the binary representation.
		\param out	The output stream.
	*/
	void writeBinary(std::ostream& out) const;

	/*! Clears the data.*/
	void clear();


	/*! Returns the parametrization type used for the liquid moisture transport model. */
	LiquidTransportType liquidTransportType() const { return m_liquidTransportType; }

	/*! Returns the parametrization type used for the vapor moisture transport model. */
	VaporTransportType vaporTransportType() const { return m_vaporTransportType; }


	/*! Calculates the liquid conductivity [s] for a given moisture content (Thetal in [m3/m3]).
		\param Thetal Moisture content, if larger than effective moisture content, function return
					  liquid conductivity at moisture saturation.
		\warning This function is only available when KL_THETAL_SPLINE is used.
	*/
	double Kl_Thetal(double Thetal) const;

	/*! Computes liquid conductivity from Kirchhoff-potential.
		\param Thetal Moisture content to compute Kl for.
		\param pl Corresponding liquid pressure.
		\param Psipl Corresponding Kirchhoff potential
		\param mat Material object (needed for MRC and parameters).
		\return Returns a liquid conductivity in [s].
	*/
	double Kl_Thetal(double Thetal, double pl, double Psipl, const MM::Material * mat) const;

	/*! Calculates the liquid water diffusivity [m2/s] for a given
		moisture content (Thetal in [m3/m3]).
		\warning This function is only available when DL_THETAL_SPLINE is used.
	*/
	double Dl_Thetal(double Thetal) const;

	/*! Calculates the vapour permeability [s] for a given moisture content (Thetal in [m3/m3]).
		\param Thetal Moisture content in [m3/m3]
		\param rh Corresponding rh in [-], only needed for MEW_RH_SPLINE type.
		\note In the case of F_Kv_Thetal spline data, we interpolate linearly logKv over Thetal.
				In the case of F_mew_rh spline data, we interpolate linearly mew over rh. We
				do not use the MRC function to convert rh into Thetal. The conversion of mew to
				Kv is done using Tref = 293.15 K.
	*/
	double Kv_Thetal(double Thetal, double rh) const;

	/*! Initializes this data structure for calculation.
		This function is called from Material::prepareCalculation().
		\param mat Pointer to the parent MM::Material class.
		\param direction The direction to initialize material data to.
	*/
	void prepareCalculation(const Material* mat, MaterialDirection direction);

	/*! Calculates the Kirchhoff potential as function of moisture content (Thetal in [m3/m3]).
		Kirchoff potential function is generated from Kl and Dl spline data using the moisture
		storage function implementation.
	*/
	double Psi_Thetal(double Thetal) const;


	// *** PUBLIC MEMBER VARIABLES ***

	/*! Container for all spline data. */
	IBK::LinearSpline		m_spline[NUM_Functions];

	/*! Spline data for liquid conductivity as Kirchhoff potential.
		The Kirchhoff potential is a function Psi of Thetal (Thetal in [m3/m3]).
		\note This function is generated from Kl_Thetal or Dl_Thetal.
	*/
	IBK::LinearSpline		m_Psi_Thetal_spline;

private:
	/*! Check parametrization for valid ranges.
		Throws an exception if range is exceeded.
		This function is called from prepareCalculation().
		\param	mat Pointer to the parent MM::Material class.
		\return	Returns true if all ranges are correct. In case of an error, messages
				are written using IBK::IBK_Message.
	*/
	bool checkLimits(const Material* mat);

	/*! Checks if the given liquid conductivity function is monoton increasing.
		For liquid diffusivity only a warning will appear.
		Should only be called if a liquid transport function is determined.
		\param	mat Pointer to the parent MM::Material class.
	  */
	void checkLiquidFunctionMonotony(const Material *mat);

	/*! Denormalize a normalized spline data.
		For compatibility reasons to old material files.
		This function is called just after material data was read in
		MM::Material. If the function returns with an error code DNR_Error
		than error messages were printed using IBK::IBK_Message.
		\param	mat Pointer to the parent MM::Material class.
	*/
	void denormalize(const Material* mat);

	/*! Generates Kirchhoff potential spline from given Kl or Dl spline data.
		Requires moisture retention data to be available and already
		initialized for calculation.
		Throws and IBK::Exception when generation fails
	*/
	void generateKichhoffPotential(const Material* mat);


	// *** PRIVATE MEMBER VARIABLES ***

	/*! Defines the type of liquid transfer model used. */
	LiquidTransportType		m_liquidTransportType;
	/*! Defines the type of vapor transfer model used. */
	VaporTransportType		m_vaporTransportType;

	/*! Vapor diffusion resistance of the dry material. */
	double					m_mew;

	/*! Porosity for down scaling of permeability. */
	double					m_ThetaPor;


	friend class Material;
};

/*! \file MM_MoistureTransport.h
	\brief Contains the class MoistureTransport.
*/

} // namespace MM

#endif // MM_MoistureTransportH



