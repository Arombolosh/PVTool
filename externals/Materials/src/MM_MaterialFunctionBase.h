#ifndef MM_MaterialFunctionBaseH
#define MM_MaterialFunctionBaseH

#include <string>

#include <IBK_InputOutput.h>

#include "MM_ModelDataContainer.h"
#include "MM_Types.h"

namespace MM {

/*! Base class for all material function classes.
	Declares interface for all material function/data storage classes.
	Provides generic read/write functionality for model parameters.
*/
class MaterialFunctionBase {
public:
	/*! Virtual destructor, so that destructor of derived classes is called. */
	virtual ~MaterialFunctionBase() {}

	/*! Reads data from the string 'data'. */
	virtual void read(const std::string& data) = 0;

	/*! Reads data from the string 'data'. */
	virtual void read(const std::vector<std::string>& data) = 0;

	/*! Reads data from binary stream (V6 format and newer). */
	virtual void readBinary(std::istream& in) = 0;

	/*! Writes the data into the stream 'out' using 'indent' spaces for indentation (ASCII mode).
		\param out					The output stream.
		\param indent				Number of indentation characters to be used.
		\param direction			The transport direction that this data set corresponds to. Only
									needed for transport models.
		\param writeEmptyKeywords	If true, also all empty keywords/function names are written (can be used to create templates).
	*/
	virtual void write(std::ostream& out, unsigned int indent=0, MaterialDirection direction=U_DIRECTION, bool writeEmptyKeywords=false) const = 0;

	/*!	Writes data into the output stream in binary mode.
		\param out	The output stream, must have been opened in binary mode.
	*/
	virtual void writeBinary(std::ostream& out) const = 0;

	/*! Clears the data.
		This function should reset the objects content to newly constructed state.
		Default implementation resets the m_models container.
	*/
	virtual void clear();

	/*! Generic storage member for modeling data. */
	std::map< std::string, ModelDataContainer >	m_models;

protected:

	/*! Reads trailing model sections from string.

		This function splits the input data string at the first occurancy of the
		model section keyword [MODEL]. Then it parses all model sections and
		adds the models to the map with models.
		\return Returns a data string without the model sections.
	*/
	std::string extractModelData(const std::string & data);

	/*! Reads trailing model sections from vector of strings.

		This function splits the input data string at the first occurancy of the
		model section keyword [MODEL]. Then it parses all model sections and
		adds the models to the map with models.
		\return Returns a data vector without the model sections.
	*/
	std::vector<std::string> extractModelData(const std::vector<std::string> & data);

	/*! Reads model from the current position of the given stream.
		\param in Input stream
		\param currentLine Last line read from stream. Normally contents the MODEL keyword
	*/
	void readModelsASCII( std::istream& in, const std::string& currentLine );

	/*! Reads model from the given line.
		\param next_line Next line after Model keyword.
		\param value Last line read from stream. Normally contents the MODEL keyword
	*/
	void readModelsASCII( const std::string& next_line, const std::string& value );

	/*! Read models from binary stream. */
	void readModelsBinary( std::istream& in );

	/*! Writes models to ASCII stream. */

	void writeModels( std::ostream& out, unsigned int indent ) const;

	/*! Writes models to binary stream. */
	void writeModelsBinary( std::ostream& out ) const;

}; // class MaterialFunctionBase

/*! \file MM_MaterialFunctionBase.h
	\brief Contains the class interface for material functions access.
*/


} // namespace MM

#endif // MM_MaterialFunctionBaseH
