#ifndef MM_ModelDataContainerH
#define MM_ModelDataContainerH

#include <map>
#include <vector>
#include <string>

#include <IBK_Flag.h>
#include <IBK_LinearSpline.h>
#include <IBK_Parameter.h>
#include <IBK_UnitVector.h>

namespace MM {

/*!
\brief This class holds the parametrization of a material function model in a generic way.

Model parameters can be stored in a generic way. Except for linear splines,
all parameters are expected to be in
<pre>
  keyword = value
</pre>
format.

In addition to the parameters a model type identifier is stored. An example data block
will look as follows:
<pre>
[MODEL]
  TYPE = Theta_l(pC) GAUSS
  GAUSS_MODALITY           = 3
  GAUSS_Si                 =              0.7             0.3            0.14            0.25            0.05             0.1             0.6
  GAUSS_Wi                 =        0.0304567        0.375484        0.237761        0.116482        0.121621       0.0969543       0.0212409
  GAUSS_Ri                 =       0.00480666    1.09356e-005    1.45159e-006    7.61805e-007    4.80666e-007    6.05123e-008    3.81807e-009
</pre>
Linear splines are stored in 3 consecutive lines like in the following example:
<pre>
  FUNCTION    = Function Descriptor
  0     1     2     3
  0.01  0.2   5   100
</pre>
First line contains keyword FUNCTION and a descriptor. Second line contains x values and third line contains
y values. Number of x and y values must be equal. x values must be strictly monotonic increasing.

The format of function identifiers and model type identifies is free (may not span several lines).

*/
class ModelDataContainer {
public:

	/*! Default constructor. */
	ModelDataContainer() { clear(); }

	/*! Reads data from the string 'data'. */
	void read(const std::string& data);

	/*! Reads data from a vector  'data'. */
	void read(const std::vector<std::string>& data);

	/*! Reads data from a binary input stream. */
	void readBinary(std::istream& in);

	/*! Writes the model data into the stream 'out' using 'indent' spaces for indentation.
		\param out Output file stream.
		\param indent Indentation characters to use.
	*/
	void write(std::ostream& out, unsigned int indent=0) const;

	/*! Writes the model data to a binary file.
		\param out Output file stream, opened in binary mode.
	*/
	void writeBinary(std::ostream& out) const;

	/*! Returns true if model container is empty. */
	bool empty() const;

	/*! Resets model container data. */
	void clear();

	/*! Searches for a string with given identifier in the map of strings and
		returns it if found, otherwise returns an empty string.
		This function is a convenience function for working with constant
		ModelDataContainer objects.
	*/
	std::string strings(const std::string & keyword) const;



	/*! Type identifier of the current model. */
	std::string  								m_type;

	/*! Holds parameters. */
	std::map<std::string, std::string>			m_strings;

	/*! Holds linear spline data. */
	std::map<std::string, IBK::LinearSpline>	m_linearSplines;
};


/*! Comparison operator for model data containers. */
bool operator==(const ModelDataContainer & lhs, const ModelDataContainer & rhs);


/*! \file MM_ModelDataContainer.h
	\brief Container class for generic model parameters.
*/

} // namespace MM


#endif // MM_ModelDataContainerH
