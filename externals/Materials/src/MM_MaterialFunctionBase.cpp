#include "MM_MaterialFunctionBase.h"

#include <IBK_messages.h>

namespace MM {


void MaterialFunctionBase::clear() {
	m_models.clear();
}

std::string MaterialFunctionBase::extractModelData(const std::string & data) {
	const char * const FUNC_ID = "[MaterialFunctionBase::extractModelData]";
	std::string::size_type pos = data.find("[MODEL]");
	if (pos == std::string::npos)
		return data; // no model data

	// extract model section
	std::string modelSections = data.substr(pos);
	std::vector<std::string> models;
	IBK::explode_section(modelSections, "MODEL", models);
	for (unsigned int i=0; i<models.size(); ++i) {
		ModelDataContainer mdata;
		try {
			mdata.read(models[i]);
			// check if type keyword is given
			if (mdata.m_type.empty())
				throw IBK::Exception(IBK::FormatString("Missing TYPE keyword in model data block:\n%1").arg(models[i]), FUNC_ID);
			// check if already a model with this type was specified
			if (m_models.find(mdata.m_type) != m_models.end()) {
				IBK::IBK_Message(IBK::FormatString("Duplicate model parametrization block with type '%1' found. "
												   "Overwriting first parameter block.\n").arg(mdata.m_type),
								 IBK::MSG_PROGRESS, FUNC_ID, 2);
			}
			m_models[mdata.m_type] = mdata;
		}
		catch (IBK::Exception & ex) {
			throw IBK::Exception(ex, IBK::FormatString("Error reading model parameter #%1.").arg(i+1), FUNC_ID);
		}
	}
	if (pos == 0)
		return "";
	std::string::size_type pos2 = data.find_last_not_of(" \t", pos-1);
	// special case, section only contains models -> pos2 = std::string::npos
	if (pos2 == std::string::npos)
		return "";
	return data.substr(0, pos2);
}

std::vector<std::string> MaterialFunctionBase::extractModelData(const std::vector<std::string> & data) {
	const char * const FUNC_ID = "[MaterialFunctionBase::extractModelData]";

	std::vector<unsigned int> modelSectionIndex;

	for( unsigned int i=0, size=data.size(); i<size; ++i) {
		if (data[i].find("[MODEL]") != std::string::npos) {
			modelSectionIndex.push_back(i);
			break;
		}
	}
	// no model section
	if( modelSectionIndex.empty())
		return data;

	// data contains only models
	if (modelSectionIndex[0] == 0)
		return std::vector<std::string>();

	std::vector<std::vector<std::string> > models(modelSectionIndex.size());
	for( unsigned int i=0; i<modelSectionIndex.size(); ++i) {
		if( i==modelSectionIndex.size()-1) {
			models[i] = std::vector<std::string>(data.begin()+modelSectionIndex[i], data.end());
		}
		else {
			models[i] = std::vector<std::string>(data.begin()+modelSectionIndex[i], data.begin()+modelSectionIndex[i+1]);
		}
	}

	// extract model section
	for (unsigned int i=0; i<models.size(); ++i) {
		ModelDataContainer mdata;
		try {
			mdata.read(models[i]);
			// check if type keyword is given
			if (mdata.m_type.empty()) {
				std::string message;
				for( unsigned int j=0; j<models[i].size(); ++j) {
					message += models[i][j];
				}
				throw IBK::Exception(IBK::FormatString("Missing TYPE keyword in model data block:\n%1").arg(message), FUNC_ID);
			}
			// check if already a model with this type was specified
			if (m_models.find(mdata.m_type) != m_models.end()) {
				IBK::IBK_Message(IBK::FormatString("Duplicate model parametrization block with type '%1' found. "
												   "Overwriting first parameter block.\n").arg(mdata.m_type),
								 IBK::MSG_PROGRESS, FUNC_ID, 2);
			}
			m_models[mdata.m_type] = mdata;
		}
		catch (IBK::Exception & ex) {
			throw IBK::Exception(ex, IBK::FormatString("Error reading model parameter #%1.").arg(i+1), FUNC_ID);
		}
	}
	// returns the section before models as data section
	// models must be at end of data section
	return std::vector<std::string>(data.begin(), data.begin()+modelSectionIndex[0]);
}


void MaterialFunctionBase::writeModels( std::ostream& out, unsigned int indent ) const {

	for ( std::map< std::string, ModelDataContainer >::const_iterator it = m_models.begin(); it != m_models.end(); ++it)
		it->second.write( out, indent );

}

void MaterialFunctionBase::writeModelsBinary( std::ostream& out ) const {

	size_t tmpSize = m_models.size();
	out.write(reinterpret_cast<const char *>(&tmpSize), sizeof(size_t));
	for ( std::map< std::string, ModelDataContainer >::const_iterator it = m_models.begin(), end = m_models.end() ;it != end ;++it  ){
		IBK::write_string_binary(out, it->first);
		it->second.writeBinary(out);
	}

}

std::string transferPreV6GAUSSModel(const std::string& line) {
	std::string result;
	result += "TYPE = GAUSS\n";
	std::stringstream strm(line);
	double modality;

	if( !(strm >> modality))
		return "";
	result += "GAUSS_MODALITY           = " + IBK::val2string(modality) + "\n";
	double gsi, gwi, gri;
	std::vector<double> gsiVect;
	std::vector<double> gwiVect;
	std::vector<double> griVect;
	while( strm >> gwi >> gri >> gsi) {
		gsiVect.push_back(gsi);
		gwiVect.push_back(gwi);
		griVect.push_back(gri);
	}
	if( gsiVect.size() != modality)
		return "";
	result += "GAUSS_Si                 = ";
	for( unsigned int i=0, count=gsiVect.size(); i<count; ++i) {
		result += IBK::val2string(gsiVect[i]);
		if( i<count-1)
			result += "\t";
	}
	result += "\n";

	result += "GAUSS_Wi                 = ";
	for( unsigned int i=0, count=gwiVect.size(); i<count; ++i) {
		result += IBK::val2string(gwiVect[i]);
		if( i<count-1)
			result += "\t";
	}
	result += "\n";

	result += "GAUSS_Ri                 = ";
	for( unsigned int i=0, count=griVect.size(); i<count; ++i) {
		result += IBK::val2string(griVect[i]);
		if( i<count-1)
			result += "\t";
	}
	result += "\n";
	return result;
}

std::string transferPreV6SPModel(const std::string& line) {
	std::string result;
	result += "TYPE = SERIAL_PARALLEL\n";
	std::stringstream strm(line);

	double SP_KScale;		// scaling parameter for liquid conductivity function
	if( !(strm >> SP_KScale))
		return "";
	result += "SP_KScale           = " + IBK::val2string(SP_KScale) + "\n";

	double SP_SP;		// SP - model specific parameter, influences vapour transport
	if( !(strm >> SP_SP))
		return "";
	result += "SP_SP               = " + IBK::val2string(SP_SP) + "\n";

	bool SP_unsatSet;	// flag whether unsaturated conductivity values shall be included
	if( !(strm >> SP_unsatSet))
		return "";
	result += "SP_UNSAT         = " + IBK::val2string(SP_unsatSet) + "\n";

	bool SP_useAdsorption;	// flag whether adsorption moisture retention courve is  used to calculate Theta_l values via RH(mew)
	if( !(strm >> SP_useAdsorption))
		return "";
	result += "SP_UseAdsorption    = " + IBK::val2string(SP_useAdsorption) + "\n";

	double SP_pCmax;		// supremum of logarithmic capillary pressure, integration limit for pore-model, influences the liquid conductivity function
	if( !(strm >> SP_pCmax))
		return "";
	result += "SP_pCmax            = " + IBK::val2string(SP_pCmax) + "\n";

	unsigned int additionalSize;
	if( !(strm >> additionalSize))
		return "";
	if(additionalSize > 0) {
		std::vector<double>			SP_ThetaLAdditional(additionalSize);	// vector containing moisture content as indepedent variable
		std::vector<double>			SP_Kl(additionalSize);					// vector containing the conductivity values used to create the conductivity function (dependent variable)
		for( unsigned int i=0; i<additionalSize; ++i) {
			if( !(strm >> SP_ThetaLAdditional[i] >> SP_Kl[i]))
				return "";
		}
		result += "SP_ThetaLAdditional    = ";
		for( unsigned int i=0, count=SP_ThetaLAdditional.size(); i<count; ++i) {
			result += IBK::val2string(SP_ThetaLAdditional[i]);
			if( i<count-1)
				result += "\t";
		}
		result += "\n";
		result += "SP_Kl                  = ";
		for( unsigned int i=0, count=SP_Kl.size(); i<count; ++i) {
			result += IBK::val2string(SP_Kl[i]);
			if( i<count-1)
				result += "\t";
		}
		result += "\n";
	}

	return result;
}

void MaterialFunctionBase::readModelsASCII( std::istream& in, const std::string& value ) {
	const char * const FUNC_ID = "[MaterialFunctionBase::readModelsASCII]";

	// value contains name of function

	// get next line with data
	std::string next_line;
	std::getline(in, next_line);

	// check for allready defined models
	if (m_models.find(value) != m_models.end())
		IBK::IBK_Message(IBK::FormatString("Multiple definitions of model identifier '%1'!").arg(value), IBK::MSG_WARNING, FUNC_ID);

	std::string modelDataBlock;
	// transform strings of preV6 models into new model data block format

	// exact test for GAUSS possible because GAUSS is still a keyword and should be extracted externally
	if( value == "GAUSS") {
		modelDataBlock = transferPreV6GAUSSModel(next_line);
	}
	// SP_MODEL is no keyword anymore, therefore test of content
	if( value.find("SP_MODEL") != std::string::npos) {
		modelDataBlock = transferPreV6SPModel(next_line);
	}

	// create new model
	ModelDataContainer model;
	model.read( modelDataBlock );

	// add model to data structure
	m_models[value] = model;
}

void MaterialFunctionBase::readModelsASCII( const std::string& next_line, const std::string& value ) {
	const char * const FUNC_ID = "[MaterialFunctionBase::readModelsASCII]";

	// value contains name of function

	// check for allready defined models
	if (m_models.find(value) != m_models.end())
		IBK::IBK_Message(IBK::FormatString("Multiple definitions of model identifier '%1'!").arg(value), IBK::MSG_WARNING, FUNC_ID);

	std::string modelDataBlock;
	// transform strings of preV6 models into new model data block format

	// exact test for GAUSS possible because GAUSS is still a keyword and should be extracted externally
	if( value == "GAUSS") {
		modelDataBlock = transferPreV6GAUSSModel(next_line);
	}
	// SP_MODEL is no keyword anymore, therefore test of content
	if( value.find("SP_MODEL") != std::string::npos) {
		modelDataBlock = transferPreV6SPModel(next_line);
	}

	// create new model
	ModelDataContainer model;
	model.read( modelDataBlock );

	// add model to data structure
	m_models[value] = model;
}


void MaterialFunctionBase::readModelsBinary( std::istream& in ) {

	size_t tmpSize;
	in.read(reinterpret_cast<char *>(&tmpSize), sizeof(size_t));
	for (size_t i = 0; i < tmpSize; ++i ){
		std::string tmpString;
		IBK::read_string_binary( in, tmpString, 10000 );
		ModelDataContainer tmpModel;
		tmpModel.readBinary(in);
		m_models[tmpString] = tmpModel;
	}
}


} // namespace MM

