#include "MM_MeasuredData.h"

#include <IBK_InputOutput.h>
#include <IBK_messages.h>
#include <IBK_StringUtils.h>

#include "MM_Constants.h"
#include "MM_KeywordList.h"

namespace MM {

MeasuredData::MeasuredData()
{
}

void MeasuredData::read(const std::string &data) {

	(void) data;

	/// \todo : we need a better exception handling in the whole library

	//// read geometry part
	//try {
	//	readGeometry( GeometryData );
	//} catch ( IBK::Exception& ex ) {
	//	IBK::IBK_Message( ex.what() );
	//}


	///// \todo  implement
	//const char * const FUNC_ID = "[MeasuredData::read]  ";

	//std::stringstream in(measuredData);
	//std::string line, keyword, value, currentSpecimen;
	//std::vector < DataSample >	currentSampleVector;
	//DataSample					currentSample;
	//IBK::UnitVector				currentUnitVector;
	//unsigned int				currentExperimentType;

	//try {
	//	// now read all the parameters
	//	while (std::getline(in, line)) {

	//		if (!IBK::extract_keyword(line, keyword, value)){
	//			throw IBK::Exception( IBK::FormatString("Expected keyword, got '%1'!").arg(line), FUNC_ID);
	//		} else {

	//			// if keyword is obsolete ignore it
	//			ExperimentKeys ident;
	//			try {

	//				ident = (ExperimentKeys)KeywordList::Enumeration("MeasuredData::ExperimentKeys", keyword);
	//			}
	//			catch(IBK::Exception&) {
	//				IBK::IBK_Message( IBK::FormatString("Invalid Keyword found in measured data section : %1").arg(keyword), IBK::MSG_WARNING, FUNC_ID );
	//				continue;
	//			}

	//			// trim spaces or other crasy stuff from value
	//			IBK::trim(value);

	//			switch(ident){
	//				case EK_SPECIMEN :
	//					// save last sample
	//					// save previous specimen
	//					if (!currentSpecimen.empty()){
	//						currentSampleVector.push_back( currentSample );
	//						m_data[currentSpecimen] = currentSampleVector;
	//					}

	//					// remember currentSpecimen name
	//					currentSpecimen = value;

	//					break;
	//				case EK_EXPERIMENT :

	//					// the experiment also encodes the ad or desorption flag
	//					// and which values are to be stored in the y value vector
	//					currentSampleVector.push_back( currentSample );
	//					currentSample.clear();

	//					currentSample.m_assignedExperiment = IBK::string2val<unsigned int>(value);

	//					currentExperimentType = currentSample.m_assignedExperiment & ExperimentsMask;

	//					switch(currentExperimentType){
	//						case WET_CUP:
	//						case DRY_CUP:
	//						case MED_CUP:
	//							currentSample.m_yValue.resize( NUM_WaterVapourPermeabilityValues );
	//							break;
	//						case WATER_RETENTION:
	//						case SORPTION_ISOTHERM:
	//							currentSample.m_yValue.resize( NUM_WaterRetentionValues );
	//							break;
	//						default:
	//							IBK::IBK_Message( IBK::FormatString("Unknown experiment type found in measured data section : %1! Please update material library!").arg(value), IBK::MSG_WARNING, FUNC_ID );
	//					}

	//					break;

	//				case EK_X_VALUES :
	//				case EK_DATE_SIZE :
	//				case EK_BC_DATE :
	//					// read value vector
	//					currentSample.m_xValue.read( value );
	//					break;
	//				case EK_NON_LOG_PC :
	//					currentSample.m_yValue[WR_NON_LOG_PC].read( value );
	//					break;
	//				case EK_TEMPERATURE :
	//					currentSample.m_yValue[WR_TEMPERATURE].read( value );
	//					break;
	//				case EK_THETA_L :
	//					currentSample.m_yValue[WR_THETA_L].read( value );
	//					break;
	//				case EK_PHI_INSIDE :
	//					currentSample.m_yValue[VP_PHI_INSIDE].read( value );
	//					break;
	//				case EK_PHI_OUTSIDE :
	//					currentSample.m_yValue[VP_PHI_OUTSIDE].read( value );
	//					break;
	//				case EK_MEW :
	//					currentSample.m_yValue[VP_MEW].read( value );
	//					break;
	//				case EK_STD_DEVIATION :
	//					switch(currentExperimentType){
	//						case WET_CUP:
	//						case DRY_CUP:
	//						case MED_CUP:
	//							currentSample.m_yValue[VP_STD_DEVIATION].read( value );
	//							break;
	//						case WATER_RETENTION:
	//						case SORPTION_ISOTHERM:
	//							currentSample.m_yValue[WR_STD_DEVIATION].read( value );
	//							break;
	//						default:
	//							IBK::IBK_Message( IBK::FormatString("Unknown experiment type found in measured data section : %1! Please update material library!").arg(value), IBK::MSG_WARNING, FUNC_ID );
	//					}
	//					break;
	//				case EK_MINIMUM :
	//					switch(currentExperimentType){
	//						case WET_CUP:
	//						case DRY_CUP:
	//						case MED_CUP:
	//							currentSample.m_yValue[VP_MINIMUM].read( value );
	//							break;
	//						case WATER_RETENTION:
	//						case SORPTION_ISOTHERM:
	//							currentSample.m_yValue[WR_MINIMUM].read( value );
	//							break;
	//						default:
	//							IBK::IBK_Message( IBK::FormatString("Unknown experiment type found in measured data section : %1! Please update material library!").arg(value), IBK::MSG_WARNING, FUNC_ID );
	//					}
	//					break;
	//				case EK_MAXIMUM :
	//					switch(currentExperimentType){
	//						case WET_CUP:
	//						case DRY_CUP:
	//						case MED_CUP:
	//							currentSample.m_yValue[VP_MAXIMUM].read( value );
	//							break;
	//						case WATER_RETENTION:
	//						case SORPTION_ISOTHERM:
	//							currentSample.m_yValue[WR_MAXIMUM].read( value );
	//							break;
	//						default:
	//							IBK::IBK_Message( IBK::FormatString("Unknown experiment type found in measured data section : %1! Please update material library!").arg(value), IBK::MSG_WARNING, FUNC_ID );
	//					}
	//					break;

	//				case EK_WEIGHT:
	//					currentSample.m_yValue[D_WEIGHT].read( value );
	//					break;
	//				case EK_WIDTH_0:
	//					currentSample.m_yValue[D_WIDTH_0].read( value );
	//					break;
	//				case EK_LENGTH_0:
	//					currentSample.m_yValue[D_LENGTH_0].read( value );
	//					break;
	//				case EK_HEIGHT_0:
	//					currentSample.m_yValue[D_HEIGHT_0].read( value );
	//					break;
	//				case EK_WIDTH_1:
	//					currentSample.m_yValue[D_WIDTH_0].read( value );
	//					break;
	//				case EK_LENGTH_1:
	//					currentSample.m_yValue[D_LENGTH_1].read( value );
	//					break;
	//				case EK_HEIGHT_1:
	//					currentSample.m_yValue[D_HEIGHT_1].read( value );
	//					break;
	//				case EK_BC_TEMPERATURE_0:
	//					currentSample.m_yValue[D_BC_TEMPERATURE_0].read( value );
	//					break;
	//				case EK_BC_RH_0:
	//					currentSample.m_yValue[D_BC_RH_0].read( value );
	//					break;
	//				case EK_BC_TEMPERATURE_1:
	//					currentSample.m_yValue[D_BC_TEMPERATURE_1].read( value );
	//					break;
	//				case EK_BC_RH_1:
	//					currentSample.m_yValue[D_BC_RH_1].read( value );
	//					break;
	//				case EK_SAMPLE_TEMP_0:
	//					currentSample.m_yValue[D_SAMPLE_TEMP_0].read( value );
	//					break;
	//				case EK_SAMPLE_TEMP_1:
	//					currentSample.m_yValue[D_SAMPLE_TEMP_1].read( value );
	//					break;
	//				case EK_SAMPLE_TEMP_2:
	//					currentSample.m_yValue[D_SAMPLE_TEMP_2].read( value );
	//					break;
	//				case EK_SAMPLE_TEMP_3:
	//					currentSample.m_yValue[D_SAMPLE_TEMP_3].read( value );
	//					break;
	//				case EK_SAMPLE_TEMP_4:
	//					currentSample.m_yValue[D_SAMPLE_TEMP_4].read( value );
	//					break;
	//				case EK_SAMPLE_TEMP_5:
	//					currentSample.m_yValue[D_SAMPLE_TEMP_5].read( value );
	//					break;
	//				default:
	//					IBK::IBK_Message( IBK::FormatString("Unknown keyword found in measured data section : %1! Please update material library.").arg(keyword), IBK::MSG_WARNING, FUNC_ID );
	//					break;
	//			}; // switch (ident)

	//		} // if (!IBK::extract_keyword(line, keyword, value)){
	//	} // while (std::getline(in, line)) {
	//}
	//catch (IBK::Exception& ex) {
	//	IBK::IBK_Message( ex.what() );
	//	throw IBK::Exception(ex, "Error reading material measured data section!", FUNC_ID);
	//}
	//catch (std::exception& ex) {
	//	IBK::IBK_Message( ex.what() );
	//	throw IBK::Exception("Error reading material measured data section!", FUNC_ID);
	//}
}

void MeasuredData::read(const std::vector<std::string>& data) {

	(void) data;
}

void MeasuredData::write(std::ostream& out, unsigned int indent, MaterialDirection direction, bool writeEmptyKeywords) const {

	(void) out;
	(void) indent;
	(void) direction;
	(void) writeEmptyKeywords;
	/// \todo  implement
//	const char * const FUNC_ID = "[MeasuredData::write]  ";
//	std::string istr(indent, ' ');

	//try {

	//	if ( writeEmptyKeywords || !m_geometry.empty() )
	//		out << '\n' << istr << '[' << KeywordList::Keyword("Material::Sections", Material::GEOMETRY) << "]\n";

	//	istr += "  ";
	//	std::string istrL2 = istr;
	//	istrL2 += "  ";


	//	if ( writeEmptyKeywords || !(m_geometry.empty()) ){

	//		// do write out for keyword template
	//		if (m_geometry.empty()){
	//			out << istr << "SPECIMEN = " << "\n";
	//			out << istrL2 << "CYLINDRIC = " << "\n";
	//			out << istrL2 << "WIDTH_0 = " << "\n";
	//			out << istrL2 << "WIDTH_1 = " << "\n";
	//			out << istrL2 << "WIDTH_2 = " << "\n";
	//			out << istrL2 << "WIDTH_3 = " << "\n";
	//			out << istrL2 << "LENGTH_0 = " << "\n";
	//			out << istrL2 << "LENGTH_1 = " << "\n";
	//			out << istrL2 << "LENGTH_2 = " << "\n";
	//			out << istrL2 << "LENGTH_3 = " << "\n";
	//			out << istrL2 << "HEIGHT_0 = " << "\n";
	//			out << istrL2 << "HEIGHT_1 = " << "\n";
	//			out << istrL2 << "HEIGHT_2 = " << "\n";
	//			out << istrL2 << "HEIGHT_3 = " << "\n";
	//		}

	//		for (	std::map< std::string, GeometryData >::const_iterator it = m_geometry.begin(), end = m_geometry.end();
	//				it != end;
	//				++it)
	//		{
	//			out << istr << "SPECIMEN = " << (*it).first << "\n";
	//			if (((*it).second).isCylindric) {
	//				out << istrL2 << "CYLINDRIC = TRUE" << "\n";
	//			} else {
	//				out << istrL2 << "CYLINDRIC = FALSE" << "\n";
	//			}
	//			if ( writeEmptyKeywords || ((*it).second.width[0] != 0) )
	//				out << istrL2 << "WIDTH_0 = " << ((*it).second).width[0] << "\n";
	//			if ( writeEmptyKeywords || ((*it).second.width[1] != 0) )
	//				out << istrL2 << "WIDTH_1 = " << ((*it).second).width[1] << "\n";
	//			if ( writeEmptyKeywords || ((*it).second.width[2] != 0) )
	//				out << istrL2 << "WIDTH_2 = " << ((*it).second).width[2] << "\n";
	//			if ( writeEmptyKeywords || ((*it).second.width[3] != 0) )
	//				out << istrL2 << "WIDTH_3 = " << ((*it).second).width[3] << "\n";
	//			if ( writeEmptyKeywords || ((*it).second.length[0] != 0) )
	//				out << istrL2 << "LENGTH_0 = " << ((*it).second).length[0] << "\n";
	//			if ( writeEmptyKeywords || ((*it).second.length[1] != 0) )
	//				out << istrL2 << "LENGTH_1 = " << ((*it).second).length[1] << "\n";
	//			if ( writeEmptyKeywords || ((*it).second.length[2] != 0) )
	//				out << istrL2 << "LENGTH_2 = " << ((*it).second).length[2] << "\n";
	//			if ( writeEmptyKeywords || ((*it).second.length[3] != 0) )
	//				out << istrL2 << "LENGTH_3 = " << ((*it).second).length[3] << "\n";
	//			if ( writeEmptyKeywords || ((*it).second.height[0] != 0) )
	//				out << istrL2 << "HEIGHT_0 = " << ((*it).second).height[0] << "\n";
	//			if ( writeEmptyKeywords || ((*it).second.height[1] != 0) )
	//				out << istrL2 << "HEIGHT_1 = " << ((*it).second).height[1] << "\n";
	//			if ( writeEmptyKeywords || ((*it).second.height[2] != 0) )
	//				out << istrL2 << "HEIGHT_2 = " << ((*it).second).height[2] << "\n";
	//			if ( writeEmptyKeywords || ((*it).second.height[3] != 0) )
	//				out << istrL2 << "HEIGHT_3 = " << ((*it).second).height[3] << "\n";
	//		}

	//	} // if ( writeEmptyKeywords || !(m_geometry.empty()) ){

	//	std::string istr3(indent, ' ');
	//	if ( writeEmptyKeywords || !m_data.empty() )
	//		out << "\n\n" << istr3 << '[' << KeywordList::Keyword("Material::Sections", Material::MEASURED_DATA) << "]\n\n";


	//	if ( writeEmptyKeywords || !m_data.empty() ){

	//		if ( m_data.empty() ) {
	//			out << istr << "SPECIMEN = \n";
	//			out << istrL2 << std::setw(MAX_KW_LEN()) << std::left << "EXPERIMENT" << " = for WET_CUP,DRY_CUP,and MEAN_CUP experiments use following keywords \n";
	//			out << istrL2 << std::setw(MAX_KW_LEN()) << std::left << "X_VALUES" << " = \n";
	//			for (unsigned int i = 0; i < NUM_WaterVapourPermeabilityValues; ++i) {
	//				write_keyword(out, istrL2, MAX_KW_LEN(), KeywordList::Keyword("MeasuredData::WaterVapourPermeabilityValues", i)) << "\n";
	//			}
	//			out << istr << "SPECIMEN = \n";
	//			out << istrL2 << std::setw(MAX_KW_LEN()) << std::left << "EXPERIMENT" << " = for WATER_RETENTION and SORPTION_ISOTHERM experiments use following keywords \n";
	//			out << istrL2 << std::setw(MAX_KW_LEN()) << std::left << "X_VALUES" << " = \n";
	//			for (unsigned int j = 0; j < NUM_WaterRetentionValues; ++j){
	//				write_keyword(out, istrL2, MAX_KW_LEN(), KeywordList::Keyword("MeasuredData::WaterRetentionValues", j)) << "\n";
	//			}
	//			out << istr << "SPECIMEN = \n";
	//			out << istrL2 << std::setw(MAX_KW_LEN()) << std::left << "EXPERIMENT" << " = for DRYING_0 experiment use following keywords \n";
	//			out << istrL2 << std::setw(MAX_KW_LEN()) << std::left << "DATE_SIZE" << " = \n";
	//			for (unsigned int j = 0; j < NUM_DryingValues0; ++j){
	//				write_keyword(out, istrL2, MAX_KW_LEN(), KeywordList::Keyword("MeasuredData::DryingValues0", j)) << "\n";
	//			}
	//			out << istr << "SPECIMEN = \n";
	//			out << istrL2 << std::setw(MAX_KW_LEN()) << std::left << "EXPERIMENT" << " = for DRYING_1 experiments use following keywords \n";
	//			out << istrL2 << std::setw(MAX_KW_LEN()) << std::left << "DATE_BC" << " = \n";
	//			for (unsigned int j = 0; j < NUM_DryingValues1; ++j){
	//				write_keyword(out, istrL2, MAX_KW_LEN(), KeywordList::Keyword("MeasuredData::DryingValues1", j)) << "\n";
	//			}
	//		}

	//		for (	std::map< std::string, std::vector< DataSample > >::const_iterator it = m_data.begin(), end = m_data.end();
	//				it != end;
	//				++it)
	//		{

	//			out << istr << "SPECIMEN = " << (*it).first << "\n";

	//			for (	std::vector< DataSample >::const_iterator itSample = (*it).second.begin(), lastSample = (*it).second.end();
	//					itSample != lastSample;
	//					++itSample)
	//			{

	//				write_keyword(out, istrL2, MAX_KW_LEN(), "EXPERIMENT = ") << (*itSample).m_assignedExperiment << "\n";
	//				write_keyword(out, istrL2, MAX_KW_LEN(), "X_VALUES = ");
	//				(*itSample).m_xValue.write( out, indent+2, true );

	//				unsigned int i = 0;
	//				for (	std::vector< IBK::UnitVector >::const_iterator itYValues = (*itSample).m_yValue.begin(), endYValue = (*itSample).m_yValue.end();
	//						itYValues != endYValue;
	//						++itYValues, ++i
	//				){

	//					// search experiment
	//					switch( (*itSample).m_assignedExperiment ){
	//						case WET_CUP:
	//						case DRY_CUP:
	//						case MED_CUP:
	//							write_keyword(out, istrL2, MAX_KW_LEN(), KeywordList::Keyword("MeasuredData::WaterVapourPermeabilityValues", i));
	//							break;
	//						case WATER_RETENTION:
	//						case SORPTION_ISOTHERM:
	//							write_keyword(out, istrL2, MAX_KW_LEN(), KeywordList::Keyword("MeasuredData::WaterRetentionValues", i));
	//							break;
	//						case WATER_UPTAKE:
	//							write_keyword(out, istrL2, MAX_KW_LEN(), KeywordList::Keyword("MeasuredData::WaterRetentionValues", i));
	//							break;
	//						case DRYING_0:
	//							write_keyword(out, istrL2, MAX_KW_LEN(), KeywordList::Keyword("MeasuredData::DryingValues0", i));
	//							break;
	//						case DRYING_1:
	//							write_keyword(out, istrL2, MAX_KW_LEN(), KeywordList::Keyword("MeasuredData::DryingValues1", i));
	//							break;
	//						default:
	//							IBK::IBK_Message(  IBK::FormatString("Trying to write unknown experiment! Please update material lib."), IBK::MSG_WARNING, FUNC_ID );
	//							break;
	//					}
	//					(*itYValues).write( out, indent, true );


	//				} // loop all dependent values

	//			} // loop all data samples

	//		}// loop all specimens

	//	} // if ( writeEmptyKeywords || m_data.empty() ){


	//}
	//catch (std::exception& ex) {
	//	IBK::IBK_Message( ex.what() );
	//	// Do not throw any further!
	//}

}

void MeasuredData::writeBinary(std::ostream &out) const {

	/// \todo implement
	size_t sizeGeometry = m_geometry.size();
	out.write(reinterpret_cast<const char *>(&sizeGeometry), sizeof(size_t));
	for (	std::map< std::string, GeometryData >::const_iterator it = m_geometry.begin(), end = m_geometry.end();
			it != end;
			++it)
	{
		// write specimen identifier string
		IBK::write_string_binary( out, (*it).first );

		// write accoring geometry data
		out.write(reinterpret_cast<const char *>(&((it->second).isCylindric)), sizeof(bool));
		out.write(reinterpret_cast<const char *>(&((it->second).width[0])), sizeof(double));
		out.write(reinterpret_cast<const char *>(&((it->second).width[1])), sizeof(double));
		out.write(reinterpret_cast<const char *>(&((it->second).width[2])), sizeof(double));
		out.write(reinterpret_cast<const char *>(&((it->second).width[3])), sizeof(double));
		out.write(reinterpret_cast<const char *>(&((it->second).height[0])), sizeof(double));
		out.write(reinterpret_cast<const char *>(&((it->second).height[1])), sizeof(double));
		out.write(reinterpret_cast<const char *>(&((it->second).height[2])), sizeof(double));
		out.write(reinterpret_cast<const char *>(&((it->second).height[3])), sizeof(double));
		out.write(reinterpret_cast<const char *>(&((it->second).length[0])), sizeof(double));
		out.write(reinterpret_cast<const char *>(&((it->second).length[1])), sizeof(double));
		out.write(reinterpret_cast<const char *>(&((it->second).length[2])), sizeof(double));
		out.write(reinterpret_cast<const char *>(&((it->second).length[3])), sizeof(double));

	}


	size_t sizeData = m_data.size();
	out.write(reinterpret_cast<const char *>(&sizeData), sizeof(size_t));
	for (	std::map< std::string, std::vector< DataSample > >::const_iterator it = m_data.begin(), end = m_data.end();
			it != end;
			++it)
	{
		// write specimen identifier string
		IBK::write_string_binary( out, (*it).first );

		// write experiments saved for this speciment
		size_t experimentCount = (*it).second.size();
		out.write(reinterpret_cast<const char *>(&experimentCount), sizeof(size_t));
		for (	std::vector< DataSample >::const_iterator itSample = (*it).second.begin(), lastSample = (*it).second.end();
				itSample != lastSample;
				++itSample)
		{

			out.write(reinterpret_cast<const char *>(&((*itSample).m_assignedExperiment)), sizeof(unsigned int));
			(*itSample).m_xValue.writeBinary(out);

			size_t dependentValuesCount = (*itSample).m_yValue.size();
			out.write(reinterpret_cast<const char *>(&dependentValuesCount), sizeof(size_t));
			for (	std::vector< IBK::UnitVector >::const_iterator itYValues = (*itSample).m_yValue.begin(), endYValue = (*itSample).m_yValue.end();
					itYValues != endYValue;
					++itYValues
			){
				(*itYValues).writeBinary(out);
			} // loop all dependent values

		} // loop all data samples

	} // loop all specimen

}


void MeasuredData::readGeometry( const std::string & data ){

	/// \todo  implement
	const char * const FUNC_ID = "[MeasuredData::readGeometry]  ";

	std::stringstream in(data);
	std::string line, keyword, value, currentSpecimen;
	GeometryData currentGeometryData;
	try {
		// now read all the parameters
		while (std::getline(in, line)) {

			if (!IBK::extract_keyword(line, keyword, value)){
				throw IBK::Exception( IBK::FormatString("Expected keyword, got '%1'!").arg(line), FUNC_ID);
			} else {

				// if keyword is obsolete ignore it
				GeometryKeys ident;
				try {

					ident = (GeometryKeys)KeywordList::Enumeration("MeasuredData::GeometryKeys", keyword);
				}
				catch (IBK::Exception& ) {
					IBK::IBK_Message(IBK::FormatString("Invalid Keyword found in geometry section : %1").arg(keyword), IBK::MSG_WARNING, FUNC_ID, 2 );
					continue;
				}

				// trim spaces or other crasy stuff from value
				IBK::trim(value);

				switch (ident){
					case SPECIMEN :
						// if we had data bevor we save to the data base
						if (!currentSpecimen.empty()){
							m_geometry[currentSpecimen] = currentGeometryData;
						}
						// remember new specimen indentifier
						currentSpecimen = value;
						break;
					case CYLINDRIC :
						currentGeometryData.isCylindric =  IBK::string2val<bool>(value);
						break;
					case WIDTH_0 :
					case WIDTH_1 :
					case WIDTH_2 :
					case WIDTH_3 :
						currentGeometryData.width[ident - WIDTH_0] = IBK::string2val<double>(value);
						break;
					case LENGTH_0 :
					case LENGTH_1 :
					case LENGTH_2 :
					case LENGTH_3 :
						currentGeometryData.length[ident - LENGTH_0] = IBK::string2val<double>(value);
						break;
					case HEIGHT_0 :
					case HEIGHT_1 :
					case HEIGHT_2 :
					case HEIGHT_3 :
						currentGeometryData.height[ident - HEIGHT_0] = IBK::string2val<double>(value);
						break;
					default:
						IBK::IBK_Message( IBK::FormatString("Unknown keyword found in geometry section : %1! Please update material library.").arg(keyword), IBK::MSG_WARNING, FUNC_ID );
						break;
				}

			} // if (!IBK::extract_keyword(line, keyword, value)){

			// attach last geometry object to data structure
			if ( !currentSpecimen.empty() ){
				m_geometry[currentSpecimen] = currentGeometryData;
			}


		} // while (std::getline(in, line)) {
	}
	catch (IBK::Exception& ex) {
		throw IBK::Exception(ex, IBK::FormatString("Error reading material geometry section!\n%1").arg(ex.what()), FUNC_ID);
	}
	catch (std::exception& ex) {
		throw IBK::Exception(IBK::FormatString("Error reading material geometry section!\n%1").arg(ex.what()), FUNC_ID);
	}

}

void MeasuredData::readBinary(std::istream& in){

	/// \todo implement
	size_t sizeGeometry;
	in.read(reinterpret_cast<char *>(&sizeGeometry), sizeof(size_t));
	for (size_t i = 0; i < sizeGeometry; ++i){

		// read specimen identifier string
		std::string tmpIdentifier;
		IBK::read_string_binary( in, tmpIdentifier, 10000 );

		// read accoring geometry data
		GeometryData tmpGeoData;
		in.read(reinterpret_cast<char *>(&tmpGeoData.isCylindric), sizeof(bool));
		in.read(reinterpret_cast<char *>(&tmpGeoData.width[0]), sizeof(double));
		in.read(reinterpret_cast<char *>(&tmpGeoData.width[1]), sizeof(double));
		in.read(reinterpret_cast<char *>(&tmpGeoData.width[2]), sizeof(double));
		in.read(reinterpret_cast<char *>(&tmpGeoData.width[3]), sizeof(double));
		in.read(reinterpret_cast<char *>(&tmpGeoData.height[0]), sizeof(double));
		in.read(reinterpret_cast<char *>(&tmpGeoData.height[1]), sizeof(double));
		in.read(reinterpret_cast<char *>(&tmpGeoData.height[2]), sizeof(double));
		in.read(reinterpret_cast<char *>(&tmpGeoData.height[3]), sizeof(double));
		in.read(reinterpret_cast<char *>(&tmpGeoData.length[0]), sizeof(double));
		in.read(reinterpret_cast<char *>(&tmpGeoData.length[1]), sizeof(double));
		in.read(reinterpret_cast<char *>(&tmpGeoData.length[2]), sizeof(double));
		in.read(reinterpret_cast<char *>(&tmpGeoData.length[3]), sizeof(double));

		m_geometry[tmpIdentifier] = tmpGeoData;

	}


	size_t sizeData;
	in.read(reinterpret_cast<char *>(&sizeData), sizeof(size_t));

	for ( size_t k = 0; k < sizeData; ++k )
	{
		// read specimen identifier string
		std::string tmpIdentifierData;
		IBK::read_string_binary( in, tmpIdentifierData, 10000 );

		// read experiments saved for this speciment
		std::vector< DataSample > tmpSampleVector;
		size_t experimentCount;
		in.read(reinterpret_cast<char *>(&experimentCount), sizeof(size_t));
		tmpSampleVector.resize(experimentCount);
		for (	size_t l =0; l < experimentCount; ++l )
		{

			DataSample tmpDataSample;
			in.read(reinterpret_cast<char *>(&tmpDataSample.m_assignedExperiment), sizeof(unsigned int));
			tmpDataSample.m_xValue.readBinary(in);

			size_t dependentValuesCount;
			in.read(reinterpret_cast<char *>(&dependentValuesCount), sizeof(size_t));
			std::vector< IBK::UnitVector > yValue;
			yValue.resize(dependentValuesCount);
			for (	size_t j = 0; j < dependentValuesCount; ++j ){
				yValue[j].readBinary(in);
			} // loop all dependent values

			tmpDataSample.m_yValue = yValue;
			tmpSampleVector[l] = tmpDataSample;

		} // loop all data samples

		m_data[tmpIdentifierData] = tmpSampleVector;

	} // loop all specimen


}

void MeasuredData::clear(){
	m_data.clear();
	m_geometry.clear();
}


} // namespace MM
