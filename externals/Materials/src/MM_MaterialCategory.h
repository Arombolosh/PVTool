#ifndef MM_MaterialCategoryH
#define MM_MaterialCategoryH

#include <vector>
#include <map>
#include <string>

namespace MM {

/*! This class encapsulates up to three categories for a material.
	This class also manages internationalized naming of the individual
	categories.
	\todo Split this into two classes, one that represents a single
			category and handles translation, and one that holds and
			manages a set of 3 categories.
*/
class MaterialCategory {
public:
	// ***KEYWORDLIST-START***
	/*! Enumeration values for the individual categories. */
	enum CategoryID {
		Invalid,            // Keyword: INVALID            'Invalid Category'
		Coating,            // Keyword: COATING            'Coating'
		Plaster,            // Keyword: PLASTER            'Plaster and mortar'
		Bricks,             // Keyword: BRICK              'Building brick'
		NaturalStones,      // Keyword: NATURAL_STONES     'Natural stones'
		Cementitious,       // Keyword: CONCRETE           'Concrete containing building materials'
		Insulations,        // Keyword: INSULATION         'Insulation materials'
		BuildingBoards,     // Keyword: BUILDING_BOARDS    'Building boards'
		Woodbased,          // Keyword: TIMBER             'Timber'
		NaturalMaterials,   // Keyword: NATURAL_MATERIALS  'Natural materials'
		Soils,              // Keyword: SOIL               'Soil'
		CladdingSystems,    // Keyword: CLADDING           'Cladding panels and ceramic tiles'
		Foils,              // Keyword: FOILS              'Foils and waterproofing products'
		Miscellaneous,      // Keyword: MISC               'Miscellaneous'
		NumCategories
	};
	// ***KEYWORDLIST-END***

	/*! Default constructor.
		Three parameters for categories are possible.
	*/
	MaterialCategory(CategoryID first = Invalid, CategoryID second = Invalid, CategoryID third = Invalid);

	/*! Constructor with a string parameter by using MaterialCategory::set().*/
	MaterialCategory(const std::string& src);

	/*!
		Add a category ID to the listed IDs.
		\returns True if successfull or allready added, false otherwise.
	*/
	bool addCategory( CategoryID );

	/*! Sets a new default language.
		This language will be used if no additional language is given.
	*/
	void setDefaultLanguage(const std::string& lang);

	/*! Initialize the class with the given string.
		\param src Represents a list of up to three comma-separated category indizes.
		The string is stored as "old version"-category if not valid (see MaterialCategory::oldStyle()).
	*/
	void set(const std::string& src);

	/*! Returns true if no category id is set.*/
	bool isOldStyle() const { return m_first == 0; }

	/*! Returns first category id or 0 if no category is set.*/
	CategoryID first() const { return m_first; }

	/*! Returns second category id or 0 if no category is set.*/
	CategoryID second() const { return m_second; }

	/*! Returns third category id or 0 if no category is set.*/
	CategoryID third() const { return m_third; }

	/*! Returns the string for the first category in the given language (UTF8).
		If no category index is set the "old version"-category will be returned.
	*/
	std::string firstString(const std::string& languageId) const;

	/*! Returns the string for the first category in the given language (UTF8).
		If no category index is set the "old version"-category will be returned.
	*/
	std::string secondString(const std::string& languageId) const;

	/*! Returns the string for the first category in the given language (UTF8).
		If no category index is set the "old version"-category will be returned.
	*/
	std::string thirdString(const std::string& languageId) const;

	/*! Clears categories (sets all category IDs to zero). */
	void clear();

	/*! returns true if now category is set. */
	bool isEmpty() const { return ((m_first|m_second|m_third) == Invalid); }


	// *** STATIC FUNCTIONS ***

	/*! Returns the description string of a given category UTF-8 encoded.
		\param id Id of the category (0 means no/invalid category).
		\param language Language to be used when returning the description.
		\return Returns a descriptive name of the category, or an empty string if
				no such category exists.
	*/
	static std::string toString(CategoryID id, const std::string& language = "en");

	/*! Tries to evaluate a categoryID from a given string.
		This function can be used for old style category strings.
		It tries to find characteristic words in the given string like 'brick' or 'mortar'.
		Mistakes are possible.
	*/
	static CategoryID fromString( const std::string& str );

	/*! Encodes given categories into a single unsigned int. */
	unsigned int toUInt() const;

	/*! Sets categories from int. Used in binary write mode. */
	void fromUInt( unsigned int myInt );



	/*! Returns the number of categories. */
	static unsigned int size();

	/*! Gets a vector of defined languages.
		Returns the size of this vector.
		\param langs Vector which returns the existing languages.
	*/
	static void languages(std::vector<std::string>& langs);

private:
	CategoryID	m_first;				///< Id for the first (primary) category.
	CategoryID	m_second;				///< Id for the second category.
	CategoryID	m_third;				///< Id for the third category.

	std::string m_old;					///< Holds a string for old category class version.
	std::string m_defaultLanguage;		///< String with the default language ID. Default is "en" for english.


	/*! Initialization function, called when any of the conversion functions are called for the first time. */
	static void initCategoryNames();

	// friend declarations

	/*! 'Less than' operator. Necessary for sorting. */
	friend bool operator<(const MaterialCategory& lhs, const MaterialCategory& rhs);
	/*! Operator equal. Necessary for find. */
	friend bool operator==(const MaterialCategory& lhs, const MaterialCategory& rhs);
	/*! Friend declaration for global operator<<. */
	friend std::ostream& operator<<(std::ostream& out, const MaterialCategory& val);

	/*! The category map in all defined languages. */
	static std::vector<std::map<std::string,std::string> > m_categories;

};

/*! Less operator. Necessary for sorting. */
inline bool operator<(const MaterialCategory& lhs, const MaterialCategory& rhs) {
	std::string lang(lhs.m_defaultLanguage);
	if( lhs.m_first == 0 &&  rhs.m_first == 0)
		return lhs.m_old < rhs.m_old;
	if( lhs.firstString(lang) == rhs.firstString(lang)) {
		if( lhs.secondString(lang) == rhs.secondString(lang) )
			return lhs.thirdString(lang) < rhs.thirdString(lang);
		else return lhs.secondString(lang) < rhs.secondString(lang);
	}
	else return lhs.firstString(lang) < rhs.firstString(lang);
}

/*! Operator equal. Necessary for find. */
inline bool operator==(const MaterialCategory& lhs, const MaterialCategory& rhs) {
	if( lhs.m_first != rhs.m_first) return false;
	if( lhs.m_second != rhs.m_second ) return false;
	if( lhs.m_third != rhs.m_third ) return false;
	return true;
}

/*! Operator 'not equal'. Necessary for find. */
inline bool operator!=(const MaterialCategory& lhs, const MaterialCategory& rhs) {
	return !(lhs == rhs);
}


/*! Writes the category ids to stream.
	If all categories are 0 than the old string (UTF8 encoded) will be written.
*/
std::ostream& operator<<(std::ostream& out, const MaterialCategory& val);

/*! \file MM_MaterialCategory.h
	\brief Contains the declaration of class MaterialCategory.
*/

} // namespace MM

#endif


