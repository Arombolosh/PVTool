#include "MM_ModelDataContainer.h"

#include <IBK_InputOutput.h>

#include <string>
#include <map>
#include <ostream>


#include "MM_Constants.h"
#include "MM_KeywordList.h"
#include "MM_Utils.h"

namespace MM {


void ModelDataContainer::read(const std::string& data) {
	const char * const FUNC_ID = "[ModelDataContainer::read]";

	std::string line, keyword, value;
	std::stringstream strm(data);
	try {

		while (std::getline(strm,line)) {

			if (!IBK::extract_keyword(line, keyword, value))
				throw IBK::Exception( IBK::FormatString("Expected keyword, got '%1'!").arg(line), FUNC_ID);

			// trim spaces or other crazy stuff from value
			IBK::trim(value);

			if (value.empty())
				throw IBK::Exception( IBK::FormatString("Missing value for keyword '%1'!").arg(keyword), FUNC_ID);

			if (keyword == "TYPE") {
				m_type = value;
			}
			else if (keyword == "FUNCTION") {
				// the next 2 lines contain the spline data
				std::string x_data, y_data;
				std::getline(strm,x_data);
				std::getline(strm,y_data);
				// check if function name is already used
				if (m_linearSplines.find(value) != m_linearSplines.end()) {
					IBK::IBK_Message(IBK::FormatString("Duplicate definition of function '%1' "
													   "within model data block. Overwriting previous definition.\n").arg(value),
									 IBK::MSG_PROGRESS, FUNC_ID, 2);
				}
				try {
					m_linearSplines[value].read(x_data, y_data);
				}
				catch (IBK::Exception & ex) {
					throw IBK::Exception(ex, IBK::FormatString("Error reading data for function '%1'.").arg(value), FUNC_ID);
				}
			}
			else {
				if (m_strings.find(keyword) != m_strings.end()) {
					IBK::IBK_Message(IBK::FormatString("Duplicate definition of parameter '%1' "
													   "within model data block. Overwriting previous definition.\n").arg(keyword),
									 IBK::MSG_PROGRESS, FUNC_ID, 2);
				}
				m_strings[keyword] = value;
			}
		} // while
	}
	catch (IBK::Exception& ex) {
		throw IBK::Exception(ex, "Error reading model data!", FUNC_ID);
	}
}


void ModelDataContainer::read(const std::vector<std::string>& data) {
	const char * const FUNC_ID = "[ModelDataContainer::read]";

	std::string keyword, value;
	try {

		unsigned int i=0;
		unsigned int size=data.size();
		while(i<size) {

			if (data[i].find("[MODEL]") != std::string::npos) {
				++i;
				continue;
			}

			if (!IBK::extract_keyword(data[i], keyword, value))
				throw IBK::Exception( IBK::FormatString("Expected keyword, got '%1'!").arg(data[i]), FUNC_ID);

			// trim spaces or other crazy stuff from value
			IBK::trim(value);

			if (value.empty())
				throw IBK::Exception( IBK::FormatString("Missing value for keyword '%1'!").arg(keyword), FUNC_ID);

			if (keyword == "TYPE") {
				m_type = value;
			}
			else if (keyword == "FUNCTION") {
				// the next 2 lines contain the spline data
				if( i >= size-2) {
					throw IBK::Exception(IBK::FormatString("Error reading data for function '%1'.").arg(value), FUNC_ID);
				}
				++i;
				std::string x_data = data[i];
				++i;
				std::string y_data = data[i];
				// check if function name is already used
				if (m_linearSplines.find(value) != m_linearSplines.end()) {
					IBK::IBK_Message(IBK::FormatString("Duplicate definition of function '%1' "
													   "within model data block. Overwriting previous definition.\n").arg(value),
									 IBK::MSG_PROGRESS, FUNC_ID, 2);
				}
				try {
					m_linearSplines[value].read(x_data, y_data);
				}
				catch (IBK::Exception & ex) {
					throw IBK::Exception(ex, IBK::FormatString("Error reading data for function '%1'.").arg(value), FUNC_ID);
				}
			}
			else {
				if (m_strings.find(keyword) != m_strings.end()) {
					IBK::IBK_Message(IBK::FormatString("Duplicate definition of parameter '%1' "
													   "within model data block. Overwriting previous definition.\n").arg(keyword),
									 IBK::MSG_PROGRESS, FUNC_ID, 2);
				}
				m_strings[keyword] = value;
			}
			++i;
		} // while
	}
	catch (IBK::Exception& ex) {
		throw IBK::Exception(ex, "Error reading model data!", FUNC_ID);
	}
}

void ModelDataContainer::readBinary(std::istream& in) {
	(void)in;
}


void ModelDataContainer::write(std::ostream& out, unsigned int indent) const {
	//const char * const FUNC_ID = "[ModelDataContainer::write]";
	if (empty()) return;

	std::string istr(indent, ' ');
	// write model identifier
	out << istr << "[MODEL]\n";
	istr += "  ";

	// write type string
	if (!m_type.empty()) {
		write_keyword(out, istr, MAX_KW_LEN(), "TYPE") << m_type << std::endl;
	}

	// write all string parameters
	for (std::map<std::string, std::string>::const_iterator it = m_strings.begin();
		 it != m_strings.end(); ++it)
	{
		if (!it->first.empty()) {
			write_keyword(out, istr, MAX_KW_LEN(), it->first.c_str()) << it->second << std::endl;
		}
	}

	// write all linear spline parameters
	for (std::map<std::string, IBK::LinearSpline>::const_iterator it = m_linearSplines.begin();
		 it != m_linearSplines.end(); ++it)
	{
		if (!it->first.empty()) {
			write_spline(out, istr, MAX_KW_LEN(), it->second, it->first);
		}
	}
}


void ModelDataContainer::writeBinary(std::ostream &out) const {
	(void)out;
}


void ModelDataContainer::clear() {
	m_type.clear();
	m_strings.clear();
	m_linearSplines.clear();
}


bool ModelDataContainer::empty() const {

	// no model set
	if (m_type.empty())
		return true;

	if (m_strings.empty() && m_linearSplines.empty())
		return true;

	return false;
}

std::string ModelDataContainer::strings(const std::string & keyword) const {
	std::map<std::string, std::string>::const_iterator it = m_strings.find(keyword);
	if (it == m_strings.end())
		return "";
	else
		return it->second;
}

bool operator==(const ModelDataContainer & lhs, const ModelDataContainer & rhs) {
	/// \todo implement this
	(void)lhs;
	(void)rhs;
	return false;
}

} // namespace MM

