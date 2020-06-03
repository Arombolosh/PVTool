#ifndef MM_IdentificationH
#define MM_IdentificationH

#include "MM_MaterialCategory.h"

#include <bitset>
#include <set>

#include <IBK_Color.h>
#include <IBK_MultiLanguageString.h>

#include "MM_Types.h"

namespace MM {

/*! \brief This class encapsulates all identification information of the material.
	The data herein is not necessary for the simulation.
*/
class MaterialIdentification {
public:

	// ***KEYWORDLIST-START***
	/*! Enum with all possible identification parameters for a material.*/
	enum Ident {
		ID_NAME,				// Keyword: NAME
		ID_PRODUCER,			// Keyword: PRODUCER
		ID_PRODUCT_ID,			// Keyword: PRODUCT_ID
		ID_INVESTIGATOR,		// Keyword: INVESTIGATOR
		ID_LABORATORY,			// Keyword: LABORATORY SOURCE
		ID_DATE,				// Keyword: DATE
		ID_SAMPLING_DATE,		// Keyword: SAMPLING_DATE
		ID_PRODUCTION_DATE,		// Keyword: PRODUCTION_DATE
		ID_SAMPLE_ID,			// Keyword: SAMPLE_ID
		ID_SAMPLING_METHOD,		// Keyword: SAMPLING_METHOD
		ID_COUNTRY,				// Keyword: COUNTRY
		ID_COLOUR,				// Keyword: COLOUR COLOR
		ID_FLAGS,				// Keyword: FLAGS
		ID_USE_INSTEAD,			// Keyword: USE_INSTEAD
		ID_COPYRIGHT,			// Keyword: COPYRIGHT
		ID_CATEGORY,			// Keyword: CATEGORY
		ID_COMMENTS,			// Keyword: COMMENTS
		ID_U_DIRECTION,			// Keyword: U_DIRECTION
		ID_V_DIRECTION,			// Keyword: V_DIRECTION
		ID_W_DIRECTION,			// Keyword: W_DIRECTION
		ID_DBTYPE,				// Keyword: DBTYPE
		ID_SIGNATURE_SOURCE,	// Keyword: SIGNATURE_SOURCE
		ID_QUALITY_FLAGS,		// Keyword: QUALITY_FLAGS
		ID_DATA_SHEET_SOURCE,	// Keyword: DATA_SHEET_SOURCE
		ID_HATCHING,			// Keyword: HATCHING
		ID_AQUISITION_ID,		// Keyword: AQUISITION_ID
		NUM_Ident
	};

	/*! Enum with all possible material flags.*/
	enum Flags {
		AIR					= 0x01,		// Keyword: AIR			'Material is air.'
		AIR_TIGHT			= 0x02,		// Keyword: AIR_TIGHT   'No gas transport through the material possible.'
		VAPOR_TIGHT			= 0x04,		// Keyword: VAPOR_TIGHT	'No water vapor transport through the material.'
		WATER_TIGHT			= 0x08,		// Keyword: WATER_TIGHT 'No liquid water transport through the materials.'
		WOOD				= 0x10,		// Keyword: WOOD		'Material is wood (timber)'
		WOOD_BASED			= 0x20,		// Keyword: WOOD_BASED	'Material is based on wood (e.g. wood fiber board)'
		FOILS				= 0x40,		// Keyword: FOILS		'Material is a foil or vapor retarder. See in comments for additional informations.'
		NUM_Flags
	};

	/*!		The enumeration describes the 4 different versions a sample was created.
	*/
	enum SamplingMethods {
//		S_INVALID,
		S_CASTED_CUTTED_PRODUCER = 1,	// Keyword: S_CAST_CUT_PRODUCER		'casted and cutted by producer'
		S_CASTED_PRODUCER,				// Keyword: S_CAST_PRODUCER			'casted by producer'
		S_CASTED_CUTTED_IBK,			// Keyword: S_CAST_CUT_IBK			'casted and cutted by IBK'
		S_CASTED_IBK					// Keyword: S_CAST_IBK				'casted by IBK'
	};

	/*! Flags used in pre Version 6 material files. */
	enum FlagsPreV6 {
		PREV6_AIR,				// Keyword: AIR
		PREV6_AIRTIGHT,			// Keyword: AIRTIGHT
		PREV6_VAPORTIGHT,		// Keyword: VAPORTIGHT
		PREV6_WATERTIGHT,		// Keyword: WATERTIGHT
		PREV6_WOOD,				// Keyword: WOOD
		PREV6_WOODBASED,		// Keyword: WOODBASED
		PREV6_ONLYHYG,			// Keyword: ONLY_HYGROSCOPIC
		NUM_FlagsPreV6
	};

	enum HatchingType {
		HT_Ground,					// Keyword: Ground					'code according DIN ISO 128-50 no, DIN 1356-1 8.1'
		HT_Gravel,					// Keyword: Gravel					'code according DIN ISO 128-50 SN4'
		HT_Sand,					// Keyword: Sand					'code according DIN ISO 128-50 SN5'
		HT_ConcreteNotReinforced,	// Keyword: ConcreteNotReinforced	'code according DIN ISO 128-50 SN47'
		HT_ConcreteReinforced,		// Keyword: ConcreteReinforced		'code according DIN ISO 128-50 SN46'
		HT_Masonry,					// Keyword: Masonry					'code according DIN ISO 128-50 SN41'
		HT_WoodTransverseToFibre,	// Keyword: WoodTransverseToFibre	'code according DIN ISO 128-50 SN21'
		HT_WoodLongitudinal,		// Keyword: WoodLongitudinal		'code according DIN ISO 128-50 SN22'
		HT_Metal,					// Keyword: Metal					'code according DIN ISO 128-50 SM'
		HT_MortarPlaster,			// Keyword: MortarPlaster			'code according DIN ISO 128-50 no, DIN 1356-1 8.10'
		HT_Insulation,				// Keyword: Insulation				'code according DIN ISO 128-50 SN55'
		HT_SealingFoil,				// Keyword: SealingFoil				'code according DIN ISO 128-50 SN59'
		HT_Sealant,					// Keyword: Sealant					'code according DIN ISO 128-50 SN24'
		HT_None						// Keyword: None
	};
	// ***KEYWORDLIST-END***

	/*! Default constructor, assigns default values and a unique color for the material.*/
	MaterialIdentification();

	/*! Reads the identification data from a string. */
	void read( const std::string& data);

	/*! Reads the identification data from a vector of strings (lines). */
	void read( const std::vector<std::string>& data);

	/*! Read the identification from binary file. */
	void readBinary( std::istream& in );

	/*! Reads the identification data from a pre V6 string. */
	void readPreV6( const std::string& data );

	/*! Reads the identification data from a pre V6  vector of strings (lines). */
	void readPreV6( const std::vector<std::string>& data );

	/*! Writes the material identification data into the stream 'out'
		using 'indent' spaces for indentation.
		The function may throws a std::runtime_error exception if an error occurs.
		Writes in UTF8 encoding.
	*/
	void write( std::ostream& out, unsigned int indent=0, bool writeEmptyKeywords=false ) const;

	/*!	Writes identification into the output stream.
		It uses the binary representation.
		\param out	The output stream.
	*/
	void writeBinary(std::ostream& out) const;


	/*! Clears the material identification data.*/
	void clear();

	/*! Set the flags from a string.*/
	void setFlags(const std::string& src);

	/*! Converter function for pre V6 material file formats. */
	void setFlagsPreV6(const std::string& src);

	/*! Get languages from the strings.*/
	size_t languages(std::set<std::string>& langs) const;

	/*! Returns true if the material data set is deprecated.*/
	bool isDeprecated() const { return m_deprecatedState != 0; }

	IBK::MultiLanguageString	m_name;				///< The material name.
	IBK::MultiLanguageString	m_producer;			///< The material producer.
	IBK::MultiLanguageString	m_productId;		///< The producer's product identification.
	std::string					m_investigator;		///< Contact to the investigating laboratory or contact person.
	IBK::MultiLanguageString	m_laboratory;		///< The source of the material data in case of literature review.
	mutable std::string			m_date;				///< Date of material file production.
	std::string					m_samplingDate;		///< Sampling date. Date when the values where measured.
	std::string					m_productionDate;	///< Production date. Date when the material is produced (year is normally enough).
	std::string					m_sampleID;			///< The sampling id used by the IBK laboratory.
	unsigned int				m_samplingMethod;	///< \todo get data from the laboratory ???
	std::string					m_country;			///< Country where the the material is mainly produced or selled.
	IBK::Color					m_color;			///< The color code for display in the discretisation grid.
	unsigned int				m_flags;			///< Special Flags for use and/or properties of the material.
	/*! Set current state for material if deprecated or not or replaced by other data set.
		\li \c -1 is deprecated and removed, don't use anymore
		\li \c 0  is not deprecated
		\li \c >0 is deprecated and replaced by the material with the given ID
		\todo Why not rename the variable to "m_replacedByMaterialWithID" or similar?
	*/
	int							m_deprecatedState;
	std::string					m_copyright;		///< copyright text or link to copyright text
	MaterialCategory			m_cat;				///< Contains the material classification/categories.
	IBK::MultiLanguageString	m_comments;			///< Additional comments.
	std::vector< MaterialDBType >	m_DBtypes;			///< Database type
	std::string					m_uDirection;		///< Name of the materials u direction
	std::string					m_vDirection;		///< Name of the materials v direction
	std::string					m_wDirection;		///< Name of the materials w direction
	std::string					m_signatureSource;	///< url link to the signature to this file
	unsigned int				m_qualityFlags;		///< Flag field for material properties (\todo we may calculate these while reading the file automatically)
	std::string					m_dataSheetSource;	///< url link to the data sheet location at the material provider side
	HatchingType				m_hatchingType;		///< Type of hatching for material.
	int							m_aquisitionID;		///< ID of material aquisition. 0 means no aquisition procedure known.


private:
	static unsigned int			m_newColorIndex;    ///< Index of the material (used for colour assignments).

	/*! Returns true if the given key is obsolete.*/
	bool obsoleteKeys(int kw) const;

	/*! Interprets the given string as one line from a identification section.*/
	void readLine( const std::string& line);

	/*! Interprets the given string as one line from a identification section.
		It uses the pre version 6 format.
	*/
	void readLinePreV6( const std::string& line);
};

/*! Comparison operator for two MaterialIdentification objects.
	Compares all internal variables.
*/
bool operator==(const MaterialIdentification& lhs, const MaterialIdentification& rhs);

} // namespace MM

/*! \file MM_Identification.h
	\brief Contains the class MaterialIdentification and two helper classes.
*/

#endif // MM_IdentificationH




