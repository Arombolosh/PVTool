#ifndef MM_UtilsH
#define MM_UtilsH

#include <set>
#include <string>
#include <vector>
#include <map>
#include <limits>

#include <IBK_NotificationHandler.h>
#include <IBK_Path.h>

#include "MM_Types.h"
#include "MM_Material.h"



namespace MM {

/*! Type which holds the filename and a material. */
typedef std::pair<IBK::Path, MM::Material> MaterialFile;

/*! \brief Functor for sorting arrays of MM::MaterialFile.
	The sorting criteria can be set in the constructor.
*/
class MaterialSorter : public std::binary_function<MaterialFile, MaterialFile, bool> {
public:
	/*! Enumeration for sorting criterion. */
	enum SortCriterion {
		Filename,
		Name,
		Category,
		Source,
		Producer,
		Rho,
		Lambda,
		Mew
	};

	/*! Constructor of MaterialSorter
		\param k Sorting criterion
		\param lang Language that is used for string comparisons
	*/
	MaterialSorter(SortCriterion k, const std::string& lang = "en") : m_kind(k), m_language(lang) {}
	/*! Functor that gives true if the two materials are equal.
		\param lhs Left hand side material.
		\param rhs Right hand side material.
	*/
	bool operator()(const MaterialFile& lhs, const MaterialFile& rhs);

private:
	SortCriterion	m_kind;
	std::string		m_language;
};
//---------------------------------------------------------------------------


/*! \brief Helper class for using find_if to find a material by ID name.
	This function is primarily used for resolving named referenced
	in old Delphin 5 files.
*/
class MaterialFinder : public std::unary_function<MM::Material, bool> {
public:
	/*! Constructor with string paramter.
		Search for a material by ID name.
		\param name Name of the material given as std::string.
	*/
	MaterialFinder(const std::string& name) : m_name(name), m_useIdent(false) {}

	/*! Constructor with IBK::MultiLanguageString paramter.
		Search for a material name using the english name to identify the material.
		\param name Name of the material given as MultiLanguageString.
	*/
	MaterialFinder(const IBK::MultiLanguageString& name) :  m_ident(name), m_useIdent(true) {}

	/*! Functor that gives true if the given name is equal to the identification name in the material.
		\param mat The Material data.
	*/
	bool operator()(const MM::Material& mat) const {
		if (m_useIdent)
			return mat.m_identification.m_name == m_name;
		else
			return mat.m_identification.m_name.string("en", true) == m_name;
	}

private:
	std::string					m_name;
	IBK::MultiLanguageString	m_ident;
	bool						m_useIdent;
};
//---------------------------------------------------------------------------


/*! \brief Helper functor for use with std::find_if.
	Can be used to find a material that includes a certain DB type.
	\sa MaterialDBType
	\sa MaterialIdentification::m_DBtypes
*/
class MaterialFinderByDBType : public std::unary_function<MaterialFile, bool> {
public:
	/*! Constructor.
		\param dbtype String for database type for searching.
	*/
	MaterialFinderByDBType(MaterialDBType dbtype) : m_dbtype(dbtype) {}
	/*! Functor that gives true if the given database type is included in the material.
		\param val MaterialFile which includes the material.
	*/
	bool operator()(const MaterialFile& val) const;

private:
	MaterialDBType m_dbtype;
};
//---------------------------------------------------------------------------

/*! Tests if material has a required base storage parameter.
	The function emits a warning message (verbosity level 3) if parameter is missing
	or is outside the allowed value range.
	A parameter is given when its name is not empty.
	The range change includes min and max values. If you need a parameter to
	be larger than 0, use 1e-20 or something like that as minimum value.
	\param mat Pointer to the material object.
	\param p Parameter type to be searched for.
	\param modelContext Model/functionality that the parameter is used for, used in error message.
	\param funcID Context to be printed in IBK::IBK_Message() calls
	\param minVal Minimum value, value is valid if it is larger or equal to minVal.
	\param maxVal Maximum value, value is valid if it is less or equal to maxVal.
	\return Returns true if parameter is given and in the allowed value range.
*/
bool checkStorageParameter(const Material * mat,
							 Material::ParaStorage p,
							 const std::string & modelContext,
							 const char * const & funcID,
#ifdef _MSC_VER
							 double minVal = DBL_MIN,
							 double maxVal = DBL_MAX);
#else
							 double minVal = std::numeric_limits<double>::min(),
							 double maxVal = std::numeric_limits<double>::max());
#endif

/*! Tests if material has a required base parameter.
	The function emits a warning message (verbosity level 3) if parameter is missing
	or is outside the allowed value range.
	A parameter is given when its name is not empty.
	The range change includes min and max values. If you need a parameter to
	be larger than 0, use 1e-20 or something like that as minimum value.
	\param mat Pointer to the material object.
	\param p Parameter type to be searched for.
	\param modelContext Model/functionality that the parameter is used for, used in error message.
	\param funcID Context to be printed in IBK::IBK_Message() calls
	\param minVal Minimum value, value is valid if it is larger or equal to minVal.
	\param maxVal Maximum value, value is valid if it is less or equal to maxVal.
	\return Returns true if parameter is given and in the allowed value range.
*/
bool checkTransportParameter(const Material * mat,
							 Material::ParaTransport p,
							 const std::string & modelContext,
							 const char * const & funcID,
#ifdef _MSC_VER
							 double minVal = DBL_MIN,
							 double maxVal = DBL_MAX);
#else
							 double minVal = std::numeric_limits<double>::min(),
							 double maxVal = std::numeric_limits<double>::max());
#endif


/*!	Returns true if the given material contains all parameters necessary for the software COND.
	Material is handled as const, theta_80 is ignored.
	\param material A material object, prepareCalculation() must have been called already.
	\param testCategory If true, an additional check on valid categorization is done.
	\return Returns true, if material has all parameters needed for use in COND.
*/
bool isCONDReady(const MM::Material& material, bool testCategory);

/*! Returns true if the given material contains all parameters necessary for the software DELPHIN.
	\param material A material object, prepareCalculation() must have been called already.
	\return Returns true, if material has all parameters needed for a hygrothermal
			DELPHIN simulation.
*/
bool isDELPHINReady(const MM::Material & material);

/*! Returns true if the given material contains all parameters necessary for the THERAKLES Room Model.
	\param material A material object, prepareCalculation() must have been called already.
	\return Returns true, if material has all parameters needed for a thermal THERAKLES simulation.
*/
bool isRoomModelReady(const MM::Material& material);

/*! Returns true if the given material has a valid Kl(Thetal) function.
	\param material A material object, prepareCalculation() must have been called already.
	\param monotonyCheck If true an additional check for monotony will be performed
	\return Returns true, if material has all parameters needed for a thermal THERAKLES simulation.
*/
bool hasKl_Thetal(const MM::Material& material, bool monotonyCheck);

/*! Returns true if the given material has a valid Dl(Thetal) function.
	\param material A material object, prepareCalculation() must have been called already.
	\param monotonyCheck If true an additional check for monotony will be performed
	\return Returns true, if material has all parameters needed for a thermal THERAKLES simulation.
*/
bool hasDl_Thetal(const MM::Material& material, bool monotonyCheck);

/*! Returns true if the given material has a valid Thetal(RH) function.
	\param material A material object, prepareCalculation() must have been called already.
	\return Returns true, if material has all parameters needed for a thermal THERAKLES simulation.
*/
bool hasThetal_RH(const MM::Material& material);

/*! Returns the distance between two materials.
	Distance is a weighted sum of differences of all basic material parameters.
*/
double materialDistance(const MM::Material& lhs, const MM::Material& rhs, UsedParameterForDistance usedPara);


/*! Returns the distance between a material and a list of base parameters.
	Distance is a weighted sum of differences of all basic material parameters.
*/
double materialDistance(const MM::Material& lhs, const MM::BaseParameter& rhs, int usedPara);

//  Creates a mapping of material object with their IDs as keys.
//	All material objecst in mlist are copied into the map with their indices as keys. For all referenced
//	materials (where a filename is set), the function resolves path placeholders using the
//	pathPlaceHolders map and attempts to load the material files.
//	If one of the material files cannot be loaded, the function adds this file to the matFileErrorList.
//	\param mlist List of materials to be inserted into the map.
//	\param pathPlaceHolders Mapping with place holder - directory mappings.
//	\param matMap ID -> Material map (may already contain elements).
//void appendToMaterialIndexMap(const MaterialList & mlist,
//							const std::map<std::string, std::string> & pathPlaceHolders,
//							std::map<unsigned int, MM::Material> & matMap,
//							std::vector<std::string> & matFileErrorList,
//							std::map<unsigned int, unsigned int> & newOldIdMap,
//							const std::string& bracket = "{}");

/*! Writes a formatted keyword (which is equivalent to 'out << istr << setw(len) << left << keyword << " = "'). */
std::ostream& write_keyword(std::ostream& out, const std::string& istr, unsigned int len, const char * const kw);

/*! Returns the filename for a actual material file which matches the given DELPHIN5.6 material file.
	It returns an empty string if no current file can be found.
	\param oldName String with old filename (must have the .mat extension).
	\return Returns new filename, or an empty string if no match was found.
*/
std::string currentNameFromD5_6Name(const std::string& oldName);

/*! Returns the filename for a actual material file which matches the given DELPHIN5.7 material file.
	It returns an empty string if no current file can be found.
	\param oldName String with old filename (must have the .mat extension).
	\return Returns new filename, or an empty string if no match was found.
*/
std::string currentNameFromD5_7Name(const std::string& oldName);


/*! Writes the formatted keyword 'FUNCTION' with the lengths len and in the next two lines
	the x and y values of the spline sp.
*/
std::ostream& write_spline(std::ostream& out, const std::string& istr, unsigned int len,
	const IBK::LinearSpline & sp, const std::string& fn_name);



/*! \file MM_Utils.h
	\brief Contains helper functions.
*/

} // namespace MM

#endif // MM_UtilsH
