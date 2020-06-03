#ifndef MM_ThermalStorageH
#define MM_ThermalStorageH

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

/*! \brief Class ThermalStorage contains all data for the non-standard thermal storage functions.
*/
class ThermalStorage : public MaterialFunctionBase {
public:

	// ***KEYWORDLIST-START***
	/*! Functions provided via spline data. */
	enum Functions {
		/*! The thermal storage u in [J/m3] as function of T [K]. */
		F_u_T,			// Keyword: u(T)
		/*! The inverse thermal storage T [K] as function of u in [J/m3], will be automatically generated in prepareCalculation() */
		F_T_u,
		NUM_Functions
	};
	// ***KEYWORDLIST-END***

	// *** PUBLIC MEMBER FUNCTIONS ***

	/*! Default constructor.*/
	ThermalStorage() { clear(); }

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

	/*! Initializes this data structure for calculation.
		This function is called from Material::prepareCalculation().
		\param mat Pointer to the parent MM::Material class.
		\param direction The direction to initialize material data to.
	*/
	void prepareCalculation(const Material* mat);


	// *** PUBLIC MEMBER VARIABLES ***

	/*! Container for all spline data. */
	IBK::LinearSpline		m_spline[NUM_Functions];


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

	friend class Material;
};

/*! Comparison operator == between two mrc data sets.*/
bool operator==(const ThermalStorage& lhs, const ThermalStorage& rhs);
/*! Comparison operator != between two mrc data sets.*/
inline bool operator!=(const ThermalStorage& lhs, const ThermalStorage& rhs) { return !(lhs==rhs); }

/*! \file MM_ThermalStorage.h
	\brief Contains the class ThermalStorage which encapsulates the moisture storage functions.
*/

} // namespace MM

#endif // MM_MostStorageH

