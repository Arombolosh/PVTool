#ifndef MM_VaporContactResistanceModelH
#define MM_VaporContactResistanceModelH

#include <string>

#include <IBK_LinearSpline.h>
#include <IBK_MultiLanguageString.h>

#include "MM_MaterialFunctionBase.h"

namespace MM {

/*! \brief Class for handling special parameters of vapor retarders or barriers (moisture depending or not).
*/
class VaporContactResistanceModel : public MaterialFunctionBase  {
public:
	// ***KEYWORDLIST-START***
	/*! Enum with all possible parameters for foils.*/
	enum Param {
		PF_WEIGHT_FACTOR_POS,		// Keyword: WEIGHT_FACTOR_POS	[-]		'Weighting factor for adaptive foil model positive direction'
		PF_WEIGHT_FACTOR_NEG,		// Keyword: WEIGHT_FACTOR_NEG	[-]		'Weighting factor for adaptive foil model negative direction'
		PF_SCALING_A_POS,			// Keyword: SCALING_A_POS		[-]		'Parameter a for square scaling function for adaptive foil model positive direction'
		PF_SCALING_B_POS,			// Keyword: SCALING_B_POS		[-]		'Parameter b for square scaling function for adaptive foil model positive direction'
		PF_SCALING_C_POS,			// Keyword: SCALING_C_POS		[-]		'Parameter c for square scaling function for adaptive foil model positive direction'
		PF_SCALING_A_NEG,			// Keyword: SCALING_A_NEG		[-]		'Parameter a for square scaling function for adaptive foil model negative direction'
		PF_SCALING_B_NEG,			// Keyword: SCALING_B_NEG		[-]		'Parameter b for square scaling function for adaptive foil model negative direction'
		PF_SCALING_C_NEG,			// Keyword: SCALING_C_NEG		[-]		'Parameter c for square scaling function for adaptive foil model negative direction'
		NUM_Param
	};

	/*! Functions provided via spline data. */
	enum Functions {
		F_sdPlus,					// Keyword: sd+(rh)
		F_sdMinus,					// Keyword: sd-(rh)
		NUM_Functions
	};
	// ***KEYWORDLIST-END***

	/*! Standard constructor.*/
	VaporContactResistanceModel();

	/*! Container for all spline data. */
	IBK::LinearSpline		m_spline[NUM_Functions];

	double						m_weightPos;		///< Weighting factor for adaptive foil model positive direction.
	double						m_weightNeg;		///< Weighting factor for adaptive foil model negative direction.
	double						m_scalingAPos;		///< Parameter a for square scaling function for adaptive foil model positive direction.
	double						m_scalingBPos;		///< Parameter b for square scaling function for adaptive foil model positive direction.
	double						m_scalingCPos;		///< Parameter c for square scaling function for adaptive foil model positive direction.
	double						m_scalingANeg;		///< Parameter a for square scaling function for adaptive foil model negative direction.
	double						m_scalingBNeg;		///< Parameter b for square scaling function for adaptive foil model negative direction.
	double						m_scalingCNeg;		///< Parameter c for square scaling function for adaptive foil model negative direction.

	/*! Returns true if the foil is only usable for constant sd value foils.*/
	bool isConstant() const;

	/*! Returns true if the foil has direction dependent sd values.*/
	bool isDirectionDependent() const;

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



};


}	// namespace MM
#endif
