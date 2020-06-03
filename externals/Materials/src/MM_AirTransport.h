#ifndef MM_AirTransportH
#define MM_AirTransportH

#include <string>
#include <iosfwd>
#include <vector>

#include <IBK_LinearSpline.h>
#include <IBK_Parameter.h>

#include "MM_Types.h"
#include "MM_ModelDataContainer.h"
#include "MM_MaterialFunctionBase.h"


namespace MM {

class Material;

/*! \brief Contains all data for the air transport functions.
	This class encapsulates data and functionality of the different air
	transport models.
*/
class AirTransport : public MaterialFunctionBase {
public:

	// ***KEYWORDLIST-START***
	/*! Functions provided via spline data. */
	enum Functions {
		/*! Gas permeability given as Kg (in s) of Thetal (m3/m3). */
		F_Kg_Thetal,				// Keyword: Kg(Theta_l) Kg(Ol)
		NUM_Functions
	};
	// ***KEYWORDLIST-END***

	/*!	Different types of air transport parametrizations. */
	enum AirTransportType {
		/*! Air permeability as function of moisture content using tabulated spline data. */
		KG_THETA_L_SPLINE,
		/*! Air permeability as function of moisture content (inverse linear). */
		KG_THETA_L_LINEAR,
		NUM_AirTransportType
	};



	// *** PUBLIC MEMBER FUNCTIONS ***

	/*! Default constructor. */
	AirTransport() { clear(); }

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
	void readBinary(std::istream& in) ;

	/*! Writes the data into the stream 'out' using 'indent' spaces for indentation.*/
	void write(std::ostream& out, unsigned int indent=0, MaterialDirection direction=U_DIRECTION, bool writeEmptyKeywords=false) const;

	/*!	Writes data into the output stream.
		It uses the binary representation.
		\param out	The output stream.
	*/
	void writeBinary(std::ostream& out) const;

	/*! Clears the data. */
	void clear();

	/*! Returns the parametrization type used for this model. */
	AirTransportType airTransportType() const { return m_type; }

	/*! Calculates the air permeability in [s] for a given moisture content.
		\param Thetal Moisture content in [m3/m3]
		\return Returns air permeability in [S].
	*/
	double Kg_Thetal(double Thetal) const;


	// *** PUBLIC MEMBER VARIABLES ***

	/*! Container for all spline data. */
	IBK::LinearSpline		m_spline[NUM_Functions];

private:
	/*! Initializes this data structure for calculation.
		This function is called from Material::prepareCalculation().
		\param mat Pointer to the parent MM::Material class.
		\param direction The direction to initialize material data to.
	*/
	void prepareCalculation(Material* mat, MaterialDirection direction);

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

	AirTransportType		m_type;						///< Transport model that is parametrized.
	double					m_ThetaPor;					///< Porosity for down scaling of permeability.
	double					m_KgDry;					///< Permeability of the dry material.


	friend class Material;
};

/*! \file MM_AirTransport.h
	\brief Contains the class AirTransport.
*/

} // namespace MM

#endif // MM_AirTransportH




