#ifndef MM_FoilsH
#define MM_FoilsH

#include <IBK_LinearSpline.h>
#include <IBK_MultiLanguageString.h>

namespace MM {

/*! \brief Class for handling special parameters of vapor retarders or barriers (moisture depending or not).
	It is used in special foils database.
*/
class Foils {
public:
	// ***KEYWORDLIST-START***
	/*! Enum with all possible identifications for foils.*/
	enum Ident {
		IF_NAME,				// Keyword: NAME		'Foil name'
		IF_ID_PLUS,				// Keyword: ID_PLUS		'Material ID for sd-value and/or sd+(rh)'
		IF_ID_MINUS,			// Keyword: ID_MINUS	'Material ID for sd-(rh)'
		IF_COMMENT,				// Keyword: COMMENT		'Some comments and explanations.'
		NUM_Ident
	};

	/*! Enum with all possible parameters for foils.*/
	enum Param {
		PF_SD_VALUE,			// Keyword: SD_VALUE			[m]		'sd-value'
		PF_WEIGHT_FACTOR_POS,   // Keyword: WEIGHT_FACTOR_POS	[-]		'Weighting factor for adaptive foil model positive direction'
		PF_WEIGHT_FACTOR_NEG,   // Keyword: WEIGHT_FACTOR_NEG	[-]		'Weighting factor for adaptive foil model negative direction'
		PF_SCALING_A_POS,   	// Keyword: SCALING_A_POS		[-]		'Parameter a for square scaling function for adaptive foil model positive direction'
		PF_SCALING_B_POS,   	// Keyword: SCALING_B_POS		[-]		'Parameter b for square scaling function for adaptive foil model positive direction'
		PF_SCALING_C_POS,   	// Keyword: SCALING_C_POS		[-]		'Parameter c for square scaling function for adaptive foil model positive direction'
		PF_SCALING_A_NEG,   	// Keyword: SCALING_A_NEG		[-]		'Parameter a for square scaling function for adaptive foil model negative direction'
		PF_SCALING_B_NEG,   	// Keyword: SCALING_B_NEG		[-]		'Parameter b for square scaling function for adaptive foil model negative direction'
		PF_SCALING_C_NEG,   	// Keyword: SCALING_C_NEG		[-]		'Parameter c for square scaling function for adaptive foil model negative direction'
		NUM_Param
	};

	/*! Functions provided via spline data. */
	enum Functions {
		F_sdPlus,				// Keyword: sd+(rh)
		F_sdMinus,				// Keyword: sd-(rh)
		NUM_Functions
	};
	// ***KEYWORDLIST-END***

	/*! Standard constructor.*/
	Foils();

	IBK::MultiLanguageString   	m_name;			   ///< The foil name.
	double					   	m_sd;			   ///< sd value in m.
	double					   	m_weightPos;	   ///< Weighting factor for adaptive foil model positive direction.
	double					   	m_weightNeg;	   ///< Weighting factor for adaptive foil model negative direction.
	IBK::LinearSpline		   	m_sdPlusSpline;     ///< linear spline for sd+(rh) function.
	IBK::LinearSpline		  	m_sdMinusSpline;    ///< linear spline for sd-(rh) function.
	int						   	m_idPlus;			///< Material id for sd value and/or sd+ spline.
	int							m_idMinus;			///< Material id for sd- spline.
	IBK::MultiLanguageString   	m_comment;		   ///< A comment.

	double					   	m_scalingAPos;	   ///< Parameter a for square scaling function for adaptive foil model positive direction.
	double					   	m_scalingBPos;	   ///< Parameter b for square scaling function for adaptive foil model positive direction.
	double					   	m_scalingCPos;	   ///< Parameter c for square scaling function for adaptive foil model positive direction.
	double					   	m_scalingANeg;	   ///< Parameter a for square scaling function for adaptive foil model negative direction.
	double					   	m_scalingBNeg;	   ///< Parameter b for square scaling function for adaptive foil model negative direction.
	double					   	m_scalingCNeg;	   ///< Parameter c for square scaling function for adaptive foil model negative direction.

	/*! Returns true if the foil is only usable for constant sd value foils.*/
	bool isConstant() const;

	/*! Returns true if the foil has direction dependent sd values.*/
	bool isDirectionDependent() const;

	/*! Returns true if the sd value is greater than 0.*/
	bool isValid() const;

	/*! Clears the content.*/
	void clear();

	/*! Reads data from the vector of strings (lines) 'data'.
		Throws an IBK::Exception if reading fails.
	*/
	void read(const std::vector<std::string>& data);

	/*! Writes the data into the stream 'out' using 'indent' spaces for indentation. */
	void write(std::ostream& out, unsigned int indent = 0) const;
};

inline bool operator==(const Foils& lhs, const Foils& rhs) {
	if( lhs.m_name != rhs.m_name)
		return false;
	if( lhs.m_sd != rhs.m_sd)
		return false;
	if( lhs.m_sdPlusSpline != rhs.m_sdPlusSpline)
		return false;
	if( lhs.m_sdMinusSpline != rhs.m_sdMinusSpline)
		return false;
	if( lhs.m_idPlus != rhs.m_idPlus)
		return false;
	if( lhs.m_idMinus != rhs.m_idMinus)
		return false;
	if( lhs.m_comment != rhs.m_comment)
		return false;
	return true;
}

}  // namespace MM
#endif
