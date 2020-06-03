#include "MM_Utils.h"

#include <limits>
#include <algorithm>

#include <IBK_Parameter.h>
#include <IBK_StringUtils.h>
#include <IBK_physics.h>
#include <IBK_Constants.h>

#include "MM_MoistureStorage.h"
#include "MM_MoistureTransport.h"
#include "MM_Constants.h"
#include "MM_KeywordList.h"

namespace MM {

bool MaterialSorter::operator()(const MaterialFile& lhs, const MaterialFile& rhs) {
	//lhs.second.m_identification.m_cat.setDefaultLanguage(m_language);
	//rhs.second.m_identification.m_cat.setDefaultLanguage(m_language);
	switch(m_kind)
	{
		case Filename: return lhs.first < rhs.first;
		case Name: {
			std::string lhsstr(lhs.second.m_identification.m_name.string(m_language, true));
			std::string rhsstr(rhs.second.m_identification.m_name.string(m_language, true));
			if( lhsstr == rhsstr) {
				MaterialSorter tmpSorter(MaterialSorter::Rho);
				return tmpSorter.operator ()(lhs, rhs);
			}
			return IBK::string_nocase_less(lhsstr, rhsstr);
		}
		case Category: {
			if( lhs.second.m_identification.m_cat == rhs.second.m_identification.m_cat) {
				MaterialSorter tmpSorter(MaterialSorter::Name);
				return tmpSorter.operator ()(lhs, rhs);
			}
			return lhs.second.m_identification.m_cat < rhs.second.m_identification.m_cat;
		}
		case Source: {
			std::string lhsstr(lhs.second.m_identification.m_laboratory.string(m_language));
			std::string rhsstr(rhs.second.m_identification.m_laboratory.string(m_language));
			if( lhsstr == rhsstr) {
				MaterialSorter tmpSorter(MaterialSorter::Name);
				return tmpSorter.operator ()(lhs, rhs);
			}
			return IBK::string_nocase_less(lhsstr, rhsstr);
		}
		case Producer: {
			std::string lhsstr(lhs.second.m_identification.m_producer.string(m_language));
			std::string rhsstr(rhs.second.m_identification.m_producer.string(m_language));
			if( lhsstr == rhsstr) {
				MaterialSorter tmpSorter(MaterialSorter::Name);
				return tmpSorter.operator ()(lhs, rhs);
			}
			return IBK::string_nocase_less(lhsstr, rhsstr);
		}
		case Rho: {
			double rhol = lhs.second.m_paraStorage[MM::Material::MP_RHO].get_value("kg/m3");
			double rhor = rhs.second.m_paraStorage[MM::Material::MP_RHO].get_value("kg/m3");
			if( rhol == rhor) {
				std::string lhsstr(lhs.second.m_identification.m_name.string(m_language, true));
				std::string rhsstr(rhs.second.m_identification.m_name.string(m_language, true));
				return IBK::string_nocase_less(lhsstr, rhsstr);
			}
			return rhol < rhor;
		}
		case Lambda: {
			double laml = lhs.second.m_paraTransport[MM::Material::MP_LAMBDA].get_value("W/mK");
			double lamr = rhs.second.m_paraTransport[MM::Material::MP_LAMBDA].get_value("W/mK");
			if( laml == lamr) {
				std::string lhsstr(lhs.second.m_identification.m_name.string(m_language, true));
				std::string rhsstr(rhs.second.m_identification.m_name.string(m_language, true));
				return IBK::string_nocase_less(lhsstr, rhsstr);
			}
			return laml < lamr;
		}
		case Mew: {
			double mewl = lhs.second.m_paraTransport[MM::Material::MP_MEW].get_value();
			double mewr = rhs.second.m_paraTransport[MM::Material::MP_MEW].get_value();
			if( mewl == mewr) {
				std::string lhsstr(lhs.second.m_identification.m_name.string(m_language, true));
				std::string rhsstr(rhs.second.m_identification.m_name.string(m_language, true));
				return IBK::string_nocase_less(lhsstr, rhsstr);
			}
			return mewl < mewr;
		}
		default: return lhs.first < rhs.first;
	}
}
//---------------------------------------------------------------------------

bool MaterialFinderByDBType::operator()(const MaterialFile& val) const {
	if( (int)m_dbtype == IBK::MATDB_TYPE_ALL)
		return true;
	return std::find(val.second.m_identification.m_DBtypes.begin(),
					 val.second.m_identification.m_DBtypes.end(), m_dbtype) !=
					 val.second.m_identification.m_DBtypes.end();
}
//---------------------------------------------------------------------------


bool checkStorageParameter(const Material * mat,
							 Material::ParaStorage p,
							 const std::string & modelContext,
							 const char * const & funcID,
							 double minVal, double maxVal)
{
	// check if parameter is given
	if (mat->m_paraStorage[p].name.empty()) {
		IBK::IBK_Message( IBK::FormatString("Parameter %1 missing for %2.")
						  .arg(KeywordList::Keyword("Material::ParaStorage", p))
						  .arg(modelContext),
						  IBK::MSG_WARNING, funcID, 3);
		return false;
	}
	// check for valid ranges
	double val = mat->m_paraStorage[p].value;
	if (val < minVal) {
		IBK::IBK_Message( IBK::FormatString("Parameter %1 (=%2) should be >= %3.")
						  .arg(KeywordList::Keyword("Material::ParaStorage", p))
						  .arg(val)
						  .arg(minVal),
						  IBK::MSG_WARNING, funcID, 3);
		return false;
	}
	if (val > maxVal) {
		IBK::IBK_Message( IBK::FormatString("Parameter %1 (=%2) should be <= %3.")
						  .arg(KeywordList::Keyword("Material::ParaStorage", p))
						  .arg(val)
						  .arg(maxVal),
						  IBK::MSG_WARNING, funcID, 3);
		return false;
	}
	return true;
}
//---------------------------------------------------------------------------


bool checkTransportParameter(const Material * mat,
							 Material::ParaTransport p,
							 const std::string & modelContext,
							 const char * const & funcID,
							 double minVal, double maxVal)
{
	// check if parameter is given
	if (mat->m_paraTransport[p].name.empty()) {
		IBK::IBK_Message( IBK::FormatString("Parameter %1 missing for %2.")
						  .arg(KeywordList::Keyword("Material::ParaTransport", p))
						  .arg(modelContext),
						  IBK::MSG_WARNING, funcID, 3);
		return false;
	}
	// check for valid ranges
	double val = mat->m_paraTransport[p].value;
	if (val < minVal) {
		IBK::IBK_Message( IBK::FormatString("Parameter %1 (=%2) should be >= %3.")
						  .arg(KeywordList::Keyword("Material::ParaTransport", p))
						  .arg(val)
						  .arg(minVal),
						  IBK::MSG_WARNING, funcID, 3);
		return false;
	}
	if (val > maxVal) {
		IBK::IBK_Message( IBK::FormatString("Parameter %1 (=%2) should be <= %3.")
						  .arg(KeywordList::Keyword("Material::ParaTransport", p))
						  .arg(val)
						  .arg(maxVal),
						  IBK::MSG_WARNING, funcID, 3);
		return false;
	}
	return true;
}
//---------------------------------------------------------------------------

bool isCONDReady(const MM::Material& material, bool testCategory) {
	/// \todo also check for identification parameters, like name, category and producer

	if (material.m_paraStorage[MM::Material::MP_RHO].name.empty())
		return false;
	if (material.m_paraStorage[MM::Material::MP_RHO].value <= 0.0)
		return false;

	if (material.m_paraTransport[MM::Material::MP_LAMBDA].name.empty())
		return false;
	if (material.m_paraTransport[MM::Material::MP_LAMBDA].value <= 0.0)
		return false;

	if (material.m_paraTransport[MM::Material::MP_AW].name.empty())
		return false;

	if (material.m_paraTransport[MM::Material::MP_MEW].name.empty())
		return false;
	if (material.m_paraTransport[MM::Material::MP_MEW].value <= 0.0)
		return false;

	if (material.m_paraStorage[MM::Material::MP_THETA_EFF].name.empty())
		return false;
	if (material.m_paraStorage[MM::Material::MP_THETA_EFF].value <= 0.0)
		return false;


	if (material.m_paraStorage[MM::Material::MP_THETA_80].name.empty())
		return false;
	if (material.m_paraStorage[MM::Material::MP_THETA_80].value <= 0.0)
		return false;

	if (material.m_paraStorage[MM::Material::MP_THETA_80].value >=
		material.m_paraStorage[MM::Material::MP_THETA_EFF].value)
		return false;

	if (testCategory)
		if (material.m_identification.m_cat.isOldStyle())
			return false;

	return true;
}
//---------------------------------------------------------------------------

bool isRoomModelReady(const MM::Material& material) {
	return (material.capabilities() & MM::Material::TT_Thermal) > 0;
}
//---------------------------------------------------------------------------

bool isDELPHINReady(const MM::Material & material) {
	if( (material.capabilities() & MM::Material::TT_Thermal) == 0)
		return false;

	if( !(material.m_identification.m_flags & MM::MaterialIdentification::WATER_TIGHT) &&
			(material.capabilities() & MM::Material::TT_Liquid) == 0)
		return false;
	if( !(material.m_identification.m_flags & MM::MaterialIdentification::VAPOR_TIGHT) &&
			(material.capabilities() & MM::Material::TT_Vapour) == 0)
		return false;

	return true;
}
//---------------------------------------------------------------------------

bool hasKl_Thetal(const MM::Material& material, bool monotonyCheck) {
	if( material.m_identification.m_flags & MM::MaterialIdentification::WATER_TIGHT)
		return false;

	if((material.capabilities() & MM::Material::TT_Liquid) == 0)
		return false;

	if(material.m_moistureTransport[MM::U_DIRECTION].liquidTransportType() != MM::MoistureTransport::KL_THETAL_SPLINE)
		return false;

	if(!monotonyCheck)
		return true;

	// check that all slopes are positive or 0
	const std::vector<double>& slopes = material.m_moistureTransport[MM::U_DIRECTION].m_spline[MM::MoistureTransport::F_Kl_Thetal].slopes();
	for (size_t i=0, end = slopes.size(); i<end; ++i) {
		if (slopes[i] < 0) {
			return false;
		}
	}
	return true;
}

bool hasDl_Thetal(const MM::Material& material, bool monotonyCheck) {
	if( material.m_identification.m_flags & MM::MaterialIdentification::WATER_TIGHT)
		return false;

	if((material.capabilities() & MM::Material::TT_Liquid) == 0)
		return false;

	if(material.m_moistureTransport[MM::U_DIRECTION].liquidTransportType() != MM::MoistureTransport::DL_THETAL_SPLINE)
		return false;

	if(!monotonyCheck)
		return true;

	// check that all slopes are positive or 0
	const std::vector<double>& slopes = material.m_moistureTransport[MM::U_DIRECTION].m_spline[MM::MoistureTransport::F_Dl_Thetal].slopes();
	for (size_t i=0, end = slopes.size(); i<end; ++i) {
		if (slopes[i] < 0) {
			return false;
		}
	}
	return true;
}

bool hasThetal_RH(const MM::Material& material) {
	if( material.m_identification.m_flags & MM::MaterialIdentification::WATER_TIGHT &&
		material.m_identification.m_flags & MM::MaterialIdentification::VAPOR_TIGHT	)
		return false;

	if(material.m_moistureStorage.storageType() == MM::MoistureStorage::ST_THETA_L_RH_SPLINE)
		return true;

	return false;
}

double materialDistance(const MM::Material& lhs, const MM::BaseParameter& rhs, int usedPara) {
	size_t count(0);

	double dist(0);
	if( usedPara & UPFD_Rho) {
		double vl = lhs.m_paraStorage[MM::Material::MP_RHO].value;
		double vr = rhs.rho;
		if( vl != 0 && vr > -1) {
			dist += std::fabs(vl - vr) / vl;
			++count;
		}
	}

	if( usedPara & UPFD_Lambda) {
		double vl = lhs.m_paraTransport[MM::Material::MP_LAMBDA].value;
		double vr = rhs.lambda;
		if( vl != 0 && vr > -1) {
			dist += std::fabs(vl - vr) / vl;
			++count;
		}
	}

	if( usedPara & UPFD_Mew) {
		double vl = lhs.m_paraTransport[MM::Material::MP_MEW].value;
		double vr = rhs.mew;
		if( vl != 0 && vr > -1) {
			dist += std::fabs(vl - vr) / vl;
			++count;
		}
	}

	if( usedPara & UPFD_Aw) {
		const IBK::Parameter& awl(lhs.m_paraTransport[MM::Material::MP_AW]);
		double vl = awl.get_value(awl.unit()) > 0 ? awl.value : 1.0e-10;
		double vr = rhs.aw;
		if( vl != 0 && vr > -1) {
			dist += std::fabs(vl - vr) / vl;
			++count;
		}
	}

	if( usedPara & UPFD_Theta80) {
		double vl = lhs.m_paraStorage[MM::Material::MP_THETA_80].value;
		double vr = rhs.theta80;
		if( vl != 0 && vr > -1) {
			dist += std::fabs(vl - vr) / vl;
			++count;
		}
	}

	if( usedPara & UPFD_ThetaPor) {
		double vl = lhs.m_paraStorage[MM::Material::MP_THETA_POR].value;
		double vr = rhs.thetaPor;
		if( vl != 0 && vr != 0) {
			dist += std::fabs(vl - vr) / vl;
			++count;
		}
	}

	if( usedPara & UPFD_ThetaEff) {
		double vl = lhs.m_paraStorage[MM::Material::MP_THETA_EFF].value;
		double vr = rhs.thetaEff;
		if( vl != 0 && vr != 0) {
			dist += std::fabs(vl - vr) / vl;
			++count;
		}
	}

	if( count == 0)
		return std::numeric_limits<double>::max();

	return dist / count;
}

double materialDistance(const MM::Material& lhs, const MM::Material& rhs, UsedParameterForDistance usedPara) {
	bool lc = isCONDReady(lhs, false);
	bool rc = isCONDReady(rhs, false);
	if( !lc || !rc)
		return std::numeric_limits<double>::max();

	bool onlyDelphinReady = usedPara & UPFD_DELPHINData;
	if( onlyDelphinReady) {
		bool lc = isDELPHINReady(lhs);
		bool rc = isDELPHINReady(rhs);
		// if left hand material is not Delphin ready no check take place
		if( !lc) {
			onlyDelphinReady = false;
		}
		else {
			if( !rc)
				return std::numeric_limits<double>::max();
		}
	}
	size_t count(0);
	double dist(0);
	double maxdist(0);
	if( usedPara & UPFD_Rho) {
		double vl = lhs.m_paraStorage[MM::Material::MP_RHO].value;
		double vr = rhs.m_paraStorage[MM::Material::MP_RHO].value;
		if( vl != 0 && vr != 0) {
			dist += std::fabs(vl - vr) / vl;
			maxdist = std::max(std::fabs(vl - vr) / vl, maxdist);
			++count;
		}
	}

	if( usedPara & UPFD_Lambda) {
		double vl = lhs.m_paraTransport[MM::Material::MP_LAMBDA].value;
		double vr = rhs.m_paraTransport[MM::Material::MP_LAMBDA].value;
		if( vl != 0 && vr != 0) {
			dist += std::fabs(vl - vr) / vl;
			maxdist = std::max(std::fabs(vl - vr) / vl, maxdist);
			++count;
		}
	}

	if( usedPara & UPFD_Mew) {
		double vl = lhs.m_paraTransport[MM::Material::MP_MEW].value;
		double vr = rhs.m_paraTransport[MM::Material::MP_MEW].value;
		if( vl != 0 && vr != 0) {
			dist += std::fabs(vl - vr) / vl;
			maxdist = std::max(std::fabs(vl - vr) / vl, maxdist);
			++count;
		}
	}

	if( usedPara & UPFD_Aw) {
		const IBK::Parameter& awl(lhs.m_paraTransport[MM::Material::MP_AW]);
		double vl = awl.get_value(awl.unit()) > 0 ? awl.value : 1.0e-10;
		const IBK::Parameter& awr(rhs.m_paraTransport[MM::Material::MP_AW]);
		double vr = awr.get_value(awr.unit()) > 0 ? awr.value : 1.0e-10;
		if( vl != 0 && vr != 0) {
			dist += std::fabs(vl - vr) / vl;
			maxdist = std::max(std::fabs(vl - vr) / vl, maxdist);
			++count;
		}
	}

	if( usedPara & UPFD_Theta80) {
		double vl = lhs.m_paraStorage[MM::Material::MP_THETA_80].value;
		double vr = rhs.m_paraStorage[MM::Material::MP_THETA_80].value;
		if( vl != 0 && vr != 0) {
			dist += std::fabs(vl - vr) / vl;
			maxdist = std::max(std::fabs(vl - vr) / vl, maxdist);
			++count;
		}
	}

	if( (usedPara & UPFD_ThetaPor) || (usedPara & UPFD_ThetaEff)) {
		double vl = lhs.m_paraStorage[MM::Material::MP_THETA_POR].value;
		double vr = rhs.m_paraStorage[MM::Material::MP_THETA_POR].value;
		if(lhs.m_paraStorage[MM::Material::MP_THETA_POR].name.empty())
			vl = lhs.m_paraStorage[MM::Material::MP_THETA_EFF].value;
		if(rhs.m_paraStorage[MM::Material::MP_THETA_POR].name.empty())
			vr = rhs.m_paraStorage[MM::Material::MP_THETA_EFF].value;
		if( vl != 0 && vr != 0) {
			dist += std::fabs(vl - vr) / vl;
			maxdist = std::max(std::fabs(vl - vr) / vl, maxdist);
			++count;
		}
	}

	bool moistStorageLhs = lhs.m_moistureStorage.storageType() != MoistureStorage::NUM_MoistureStorageType;
	bool moistStorageRhs = rhs.m_moistureStorage.storageType() != MoistureStorage::NUM_MoistureStorageType;
	if(moistStorageLhs != moistStorageRhs)
		return std::numeric_limits<double>::max();

	// check for storage function
	if( onlyDelphinReady && moistStorageLhs && moistStorageRhs) {
		try {
			double minpC = 0;
			double maxpC = 8;
			int NUM = 100;
			double locDist = 0;
			for( int i=0; i<NUM; ++i) {
				double pC = (maxpC - minpC) / (NUM - 1) * i + minpC;
				double pl = -1.0 * std::pow(10, pC);
				double pll = lhs.m_moistureStorage.Thetal_pl(pl);
				double pl2 = rhs.m_moistureStorage.Thetal_pl(pl);
				locDist += std::fabs(pll - pl2) / pll;
			}
			dist += locDist / NUM;
			maxdist = std::max(locDist / NUM, maxdist);
		}
		catch(...) {
			return std::numeric_limits<double>::max();
		}
	}
	if( count == 0)
		return std::numeric_limits<double>::max();

//	return dist / count;
	return maxdist;
}

/*
void appendToMaterialIndexMap(const MM::MaterialList & mlist,
							  const std::map<std::string, std::string> & pathPlaceHolders,
							  std::map<unsigned int, MM::Material> & matMap,
							  std::vector<std::string> & matFileErrorList,
							  std::map<unsigned int, unsigned int>	& newOldIdMap,
							  const std::string& bracket)
{
	// loop through all materials in the list
	matFileErrorList.clear();
	newOldIdMap.clear();
	MaterialList conflictingIDMaterials;
	for (unsigned int i=0; i<mlist.size(); ++i) {
		MM::Material m = mlist[i];	// create a copy on purpose
		// check if external reference
		if (!m.m_filename.empty()) {
			// substitute path placeholders
			std::string fullpath = IBK::replace_path_placeholders(m.m_filename, pathPlaceHolders, bracket);
			// load material
			try {
				m.readFromFile(fullpath);
			}
			catch (std::exception & ex) {
				IBK::IBK_Message(ex.what(), IBK::MSG_ERROR);
				matFileErrorList.push_back(m.m_filename);
				continue;
			}
		}
		else {
			// now we have a material object m with full data and its ID set in m.m_id
			if (m.m_id == 0) {
				IBK::IBK_Message( IBK::FormatString("Material '%1' has invalid/no ID.").arg(m.m_name.string()), IBK::MSG_ERROR);
				continue;
			}
		}
		// check if ID already exists in mapping
		if (m.m_id >= MM::USER_MAT_ID() && matMap.find(m.m_id) != matMap.end()) {
			// store material in list with conflicting IDs
			conflictingIDMaterials.push_back(m);
		}
		else {
			// store material in map
			matMap[m.m_id] = m;
		}
	}
	if (!conflictingIDMaterials.empty()) {
		// determine largest ID, add an offset of 10000 and set new ID
		unsigned int maxID = 0;
		for (std::map<unsigned int, MM::Material>::const_iterator it = matMap.begin(); it != matMap.end(); ++it) {
			maxID = std::max(maxID, it->first);
		}
		maxID += 10000; // start ID for our renaming action
		for (unsigned int i=0; i<conflictingIDMaterials.size(); ++i) {
			unsigned int currentId = maxID++;
			newOldIdMap[currentId] = conflictingIDMaterials[i].m_id;
			matMap[currentId] = conflictingIDMaterials[i];
		}
	}
}
*/





std::ostream& write_keyword(std::ostream& out, const std::string& istr, unsigned int len, const char * const kw) {
	return (out << istr << std::setw(len) << std::left << kw << " = ");
}
// ----------------------------------------------------------------------------


std::string currentNameFromD5_6Name(const std::string& oldName) {
	if( IBK::string_nocase_find(oldName, "AAC Haniel - Xella.mat") != std::string::npos)
		return "AutoclavedAeratedConcrete_1.m6";
	if( IBK::string_nocase_find(oldName, "Autoclaved Aerated Concrete.mat") != std::string::npos)
		return "AutoclavedAeratedConcrete_1.m6";
	if( IBK::string_nocase_find(oldName, "Adhesive Covering Plaster.mat") != std::string::npos)
		return "AdhesiveCoveringPlaster_2.m6";
	if( IBK::string_nocase_find(oldName, "Air Layer 3 mm (closed) - vertical.mat") != std::string::npos)
		return "AirGapVertical03mm_13.m6";
	if( IBK::string_nocase_find(oldName, "Air Layer 5 mm (closed) - vertical.mat") != std::string::npos)
		return "AirGapVertical05mm_14.m6";
	if( IBK::string_nocase_find(oldName, "Air Layer 10 mm (closed) - vertical.mat") != std::string::npos)
		return "AirGapVertical10mm_15.m6";
	if( IBK::string_nocase_find(oldName, "Air Layer 25 mm (closed) - vertical.mat") != std::string::npos)
		return "AirGapVertical25mm_16.m6";
	if( IBK::string_nocase_find(oldName, "Air Layer 40 mm (closed) - vertical.mat") != std::string::npos)
		return "AirGapVertical40mm_17.m6";
	if( IBK::string_nocase_find(oldName, "Air Layer 50 mm (closed) - vertical.mat") != std::string::npos)
		return "AirGapVertical50mm_18.m6";
	if( IBK::string_nocase_find(oldName, "Bitumen.mat") != std::string::npos)
		return "Bitumen_28.m6";
	if( IBK::string_nocase_find(oldName, "Brick Bernhard.mat") != std::string::npos)
		return "BrickBernhard_33.m6";
	if( IBK::string_nocase_find(oldName, "Brick Joens.mat") != std::string::npos)
		return "BrickJoens_34.m6";
	if( IBK::string_nocase_find(oldName, "Calciumsilicate.mat") != std::string::npos)
		return "CalciumSilicateBoard_37.m6";
	if( IBK::string_nocase_find(oldName, "Cellulose_Insulation.mat") != std::string::npos)
		return "CelluloseInsulation_45.m6";
	if( IBK::string_nocase_find(oldName, "Clay Tu2_PV 0_439.mat") != std::string::npos)
		return "ClayTu2PV439_52.m6";
	if( IBK::string_nocase_find(oldName, "Concrete_C20_25.mat") != std::string::npos)
		return "Concrete_58.m6";
	if( IBK::string_nocase_find(oldName, "Floating Floor Screed.mat") != std::string::npos)
		return "FloatingFloorScreed_69.m6";
	if( IBK::string_nocase_find(oldName, "Foamglas.mat") != std::string::npos)
		return "Foamglas_70.m6";
	if( IBK::string_nocase_find(oldName, "MULTIPOR adhesive.mat") != std::string::npos)
		return "GlueMortar_77.m6";
	if( IBK::string_nocase_find(oldName, "Gypsum Board.mat") != std::string::npos)
		return "GypsumBoard_81.m6";
	if( IBK::string_nocase_find(oldName, "Haacke_Cellco_KAPI.mat") != std::string::npos)
		return "HaackeCellcoKAPI_90.m6";
	if( IBK::string_nocase_find(oldName, "Haniel Fermacell (Fa. Xella International).mat") != std::string::npos)
		return "FermacellXella_91.m6";
	if( IBK::string_nocase_find(oldName, "Historical Brick Cluster (edge).mat") != std::string::npos)
		return "HistoricalBrickClusterEdge_96.m6";
	if( IBK::string_nocase_find(oldName, "Historical Brick Cluster 4-DD.mat") != std::string::npos)
		return "HistoricalBrickCluster4DD_97.m6";
	if( IBK::string_nocase_find(oldName, "Historical Brick Cluster.mat") != std::string::npos)
		return "HistoricalBrickCluster_98.m6";
	if( IBK::string_nocase_find(oldName, "Historical clay mortar.mat") != std::string::npos)
		return "HistoricalClayMortar_103.m6";
	if( IBK::string_nocase_find(oldName, "Insulation plaster.mat") != std::string::npos)
		return "InsulationPlaster_117.m6";
	if( IBK::string_nocase_find(oldName, "Insulation-Clay-Cork-FW.mat") != std::string::npos)
		return "InsulationClayCorkFW_119.m6";
	if( IBK::string_nocase_find(oldName, "Climate Plaster (Fa. Hufgard Tubag).mat") != std::string::npos)
		return "KlimaputzMaschinenputzTubag_124.m6";
	if( IBK::string_nocase_find(oldName, "Climate plaster.mat") != std::string::npos)
		return "KlimaputzMaschinenputzTubag_124.m6";
	if( IBK::string_nocase_find(oldName, "LightClayMortar.mat") != std::string::npos)
		return "LightClayMortar_130.m6";
	if( IBK::string_nocase_find(oldName, "Lime-cement mortar.mat") != std::string::npos)
		return "LimeCementMortar_143.m6";
	if( IBK::string_nocase_find(oldName, "Lime Cement Plaster (Transputz SG).mat") != std::string::npos)
		return "LimeCementPlasterTransputzSG_145.m6";
	if( IBK::string_nocase_find(oldName, "Lime Plaster (hist.).mat") != std::string::npos)
		return "LimePlasterHist_148.m6";
	if( IBK::string_nocase_find(oldName, "Lime plaster for fettling machine.mat") != std::string::npos)
		return "LimePlasterForFettlingMachine_149.m6";
	if( IBK::string_nocase_find(oldName, "Lime plaster for fine surface coating.mat") != std::string::npos)
		return "LimePlasterForFineSurfaceCoating_150.m6";
	if( IBK::string_nocase_find(oldName, "Lime sand brick industrial.mat") != std::string::npos)
		return "LimeSandBrickIndustrial_152.m6";
	if( IBK::string_nocase_find(oldName, "Lime sand brick traditional.mat") != std::string::npos)
		return "LimeSandBrickTraditional_153.m6";
	if( IBK::string_nocase_find(oldName, "Limesandstone.mat") != std::string::npos)
		return "LimeSandBrickTraditional_153.m6";
	if( IBK::string_nocase_find(oldName, "Lime Trass Light Mortar.mat") != std::string::npos)
		return "LimeTrassLightPlaster_154.m6";
	if( IBK::string_nocase_find(oldName, "Loam LMH (Hist.).mat") != std::string::npos)
		return "LoamLMHHist_156.m6";
	if( IBK::string_nocase_find(oldName, "Minerally insulation board.mat") != std::string::npos)
		return "MineralInsulationBoard_164.m6";
	if( IBK::string_nocase_find(oldName, "Mineralic insulation plaster.mat") != std::string::npos)
		return "MineralInsulationPlaster_169.m6";
	if( IBK::string_nocase_find(oldName, "Mineral Wool.mat") != std::string::npos)
		return "MineralWool_170.m6";
	if( IBK::string_nocase_find(oldName, "OSB Board.mat") != std::string::npos)
		return "OSBBoard_172.m6";
	if( IBK::string_nocase_find(oldName, "Particle board.mat") != std::string::npos)
		return "ParticleBoard_173.m6";
	if( IBK::string_nocase_find(oldName, "PE-Foil.mat") != std::string::npos)
		return "PEFoil_174.m6";
	if( IBK::string_nocase_find(oldName, "PSFoam-expanded.mat") != std::string::npos)
		return "PolystyreneBoardExpanded_186.m6";
	if( IBK::string_nocase_find(oldName, "PSFoam-extruded.mat") != std::string::npos)
		return "PolystyreneBoardExtruded_189.m6";
	if( IBK::string_nocase_find(oldName, "PU-Foam.mat") != std::string::npos)
		return "PolyurethaneBoard_193.m6";
	if( IBK::string_nocase_find(oldName, "Restoration Render.mat") != std::string::npos)
		return "RestorationRender_210.m6";
	if( IBK::string_nocase_find(oldName, "Roofing membrane.mat") != std::string::npos)
		return "RoofingMembrane_211.m6";
	if( IBK::string_nocase_find(oldName, "Sand mS_PV 0_413.mat") != std::string::npos)
		return "Sand_396.m6";
	if( IBK::string_nocase_find(oldName, "Sandstone (Cotta).mat") != std::string::npos)
		return "SandstoneCotta_219.m6";
	if( IBK::string_nocase_find(oldName, "Sandstone (Posta).mat") != std::string::npos)
		return "SandstonePosta_220.m6";
	if( IBK::string_nocase_find(oldName, "Sandstone (Reinhardsdorf).mat") != std::string::npos)
		return "SandstoneReinhardsdorf_221.m6";
	if( IBK::string_nocase_find(oldName, "Silt Ut4_PV 0_38.mat") != std::string::npos)
		return "Silt_400.m6";
	if( IBK::string_nocase_find(oldName, "Silt Ut4_PV 0_468.mat") != std::string::npos)
		return "Silt_400.m6";
	if( IBK::string_nocase_find(oldName, "Spruce SW_Fi across grain.mat") != std::string::npos)
		return "SpruceSWFiAcrossGrain_235.m6";
	if( IBK::string_nocase_find(oldName, "Steel.mat") != std::string::npos)
		return "Steel_238.m6";
	if( IBK::string_nocase_find(oldName, "Surface coating plaster.mat") != std::string::npos)
		return "SurfaceCoatingPlaster_242.m6";
	if( IBK::string_nocase_find(oldName, "Trass-Mineral Insulation Plaster.mat") != std::string::npos)
		return "TrassMineralInsulationPlaster_251.m6";
	if( IBK::string_nocase_find(oldName, "Vapour Retarder (open).mat") != std::string::npos)
		return "VapourRetarderOpen_258.m6";
	if( IBK::string_nocase_find(oldName, "Weathered granite.mat") != std::string::npos)
		return "WeatheredGranite_263.m6";
	if( IBK::string_nocase_find(oldName, "Wienerberger Normal Brick.mat") != std::string::npos)
		return "WienerbergerNormalBrick_264.m6";
	if( IBK::string_nocase_find(oldName, "Wood fibre insulation board.mat") != std::string::npos)
		return "WoodFibreInsulationBoard_270.m6";
	if( IBK::string_nocase_find(oldName, "Wood Fibreboard.mat") != std::string::npos)
		return "WoodFibreBoard_272.m6";
	if( IBK::string_nocase_find(oldName, "Wood wool cement board.mat") != std::string::npos)
		return "WoodWoolCementBoard_275.m6";
	if( IBK::string_nocase_find(oldName, "WoodCementBoard.mat") != std::string::npos)
		return "WoodCementBoard_279.m6";
	return "";
}

std::string currentNameFromD5_7Name(const std::string& oldName) {
	if( IBK::string_nocase_find(oldName, "AAC Haniel - Xella.mat") != std::string::npos)
		return "AutoclavedAeratedConcrete_1.m6";
	if( IBK::string_nocase_find(oldName, "Adhesive Covering Plaster.mat") != std::string::npos)
		return "AdhesiveCoveringPlaster_2.m6";
	if( IBK::string_nocase_find(oldName, "Aerated Concrete.mat") != std::string::npos)
		return "AeratedConcrete_3.m6";    // not Delphin ready
	if( IBK::string_nocase_find(oldName, "Aerated Concrete_1.mat") != std::string::npos)
		return "AeratedConcrete_4.m6";      // not Delphin ready
	if( IBK::string_nocase_find(oldName, "Aerated Concrete_2.mat") != std::string::npos)
		return "AeratedConcrete_5.m6";      // not Delphin ready
	if( IBK::string_nocase_find(oldName, "Aerated Concrete_3.mat") != std::string::npos)
		return "AeratedConcrete_6.m6";      // not Delphin ready
	if( IBK::string_nocase_find(oldName, "Aerated Concrete_4.mat") != std::string::npos)
		return "AeratedConcrete_7.m6";      // not Delphin ready
	if( IBK::string_nocase_find(oldName, "Aerated Concrete_5.mat") != std::string::npos)
		return "AeratedConcrete_8.m6";      // not Delphin ready
	if( IBK::string_nocase_find(oldName, "Air gap (horizontal) 05 mm.mat") != std::string::npos)
		return "AirGapHorizontal05mm_9.m6";
	if( IBK::string_nocase_find(oldName, "Air gap (horizontal) 10 mm.mat") != std::string::npos)
		return "AirGapHorizontal10mm_10.m6";
	if( IBK::string_nocase_find(oldName, "Air gap (horizontal) 25 mm.mat") != std::string::npos)
		return "AirGapHorizontal25mm_11.m6";
	if( IBK::string_nocase_find(oldName, "Air gap (horizontal) 50 mm.mat") != std::string::npos)
		return "AirGapHorizontal50mm_12.m6";
	if( IBK::string_nocase_find(oldName, "Air gap (vertical) 03mm.mat") != std::string::npos)
		return "AirGapVertical03mm_13.m6";
	if( IBK::string_nocase_find(oldName, "Air gap (vertical) 05mm.mat") != std::string::npos)
		return "AirGapVertical05mm_14.m6";
	if( IBK::string_nocase_find(oldName, "Air gap (vertical) 10mm.mat") != std::string::npos)
		return "AirGapVertical10mm_15.m6";
	if( IBK::string_nocase_find(oldName, "Air gap (vertical) 25mm.mat") != std::string::npos)
		return "AirGapVertical25mm_16.m6";
	if( IBK::string_nocase_find(oldName, "Air gap (vertical) 40 mm.mat") != std::string::npos)
		return "AirGapVertical40mm_17.m6";
	if( IBK::string_nocase_find(oldName, "Air gap (vertical) 50 mm.mat") != std::string::npos)
		return "AirGapVertical50mm_18.m6";
	if( IBK::string_nocase_find(oldName, "Aluminium Foil.mat") != std::string::npos)
		return "AluminiumFoil_19.m6";      // not Delphin ready
	if( IBK::string_nocase_find(oldName, "Anhydrite Flowing Screed.mat") != std::string::npos)
		return "AnhydriteFlowingScreed_20.m6";      // not Delphin ready
	if( IBK::string_nocase_find(oldName, "Ansetzm""\xF6""rtel_SLP_Remmers_Baustoffe.mat") != std::string::npos)
		return "AnsetzmoertelSLPRemmers_21.m6";
	if( IBK::string_nocase_find(oldName, "Asbestos Cement Board.mat") != std::string::npos)
		return "AsbestosCementBoard_22.m6";      // not Delphin ready
	if( IBK::string_nocase_find(oldName, "Asbestos Cement Board_1.mat") != std::string::npos)
		return "AsbestosCementBoard_23.m6";       // not Delphin ready
	if( IBK::string_nocase_find(oldName, "Asphalt.mat") != std::string::npos)
		return "Asphalt_24.m6";     // not Delphin ready
	if( IBK::string_nocase_find(oldName, "Bitumen Roof Sheeting.mat") != std::string::npos)
		return "BitumenRoofSheeting_25.m6";     // not Delphin ready
	if( IBK::string_nocase_find(oldName, "Bitumen Roof Sheeting_1.mat") != std::string::npos)
		return "BitumenRoofSheeting_26.m6";     // not Delphin ready
	if( IBK::string_nocase_find(oldName, "Bitumen.mat") != std::string::npos)
		return "Bitumen_27.m6";     // not Delphin ready
	if( IBK::string_nocase_find(oldName, "Bitumen_d5.mat") != std::string::npos)
		return "Bitumen_28.m6";
	if( IBK::string_nocase_find(oldName, "Blast-furnace Slag Concrete.mat") != std::string::npos)
		return "BlastFurnaceSlagConcrete_29.m6";     // not Delphin ready
	if( IBK::string_nocase_find(oldName, "Blast-furnace Slag Concrete_1.mat") != std::string::npos)
		return "BlastFurnaceSlagConcrete_30.m6";     // not Delphin ready
	if( IBK::string_nocase_find(oldName, "Blast-furnace Slag Concrete_2.mat") != std::string::npos)
		return "BlastFurnaceSlagConcrete_31.m6";     // not Delphin ready
	if( IBK::string_nocase_find(oldName, "Blast-furnace Slag Concrete_3.mat") != std::string::npos)
		return "BlastFurnaceSlagConcrete_32.m6";     // not Delphin ready
	if( IBK::string_nocase_find(oldName, "Brick Bernhard.mat") != std::string::npos)
		return "BrickBernhard_33.m6";
	if( IBK::string_nocase_find(oldName, "Brick Joens.mat") != std::string::npos)
		return "BrickJoens_34.m6";
	if( IBK::string_nocase_find(oldName, "Build-in Insulation Plaster.mat") != std::string::npos)
		return "BuildInInsulationPlaster_35.m6";     // not Delphin ready
	if( IBK::string_nocase_find(oldName, "Calcium Silicate Board.mat") != std::string::npos)
		return "CalciumSilicateBoard_37.m6";
	if( IBK::string_nocase_find(oldName, "Calcium Silicate Board_1.mat") != std::string::npos)
		return "CalciumSilicateBoard_37.m6";
	if( IBK::string_nocase_find(oldName, "Calcium Silicate Board_2.mat") != std::string::npos)
		return "CalciumSilicateBoard_38.m6";     // not Delphin ready
	if( IBK::string_nocase_find(oldName, "Calcium Silicate Board_3.mat") != std::string::npos)
		return "CalciumSilicateBoard_39.m6";     // not Delphin ready
	if( IBK::string_nocase_find(oldName, "Calcium Silicate Board_4.mat") != std::string::npos)
		return "CalciumSilicateBoard_40.m6";     // not Delphin ready
	if( IBK::string_nocase_find(oldName, "Calcium Silicate Board_5.mat") != std::string::npos)
		return "CalciumSilicateBoard_41.m6";     // not Delphin ready
	if( IBK::string_nocase_find(oldName, "Calcium Silicate Board_Epatherm.mat") != std::string::npos)
		return "CalciumSilicateBoardEpatherm_42.m6";     // not Delphin ready
	if( IBK::string_nocase_find(oldName, "Calsitherm_Kp_Kleber_Schomburg.mat") != std::string::npos)
		return "CalsithermKpKleberSchomburg_43.m6";     // not Delphin ready
	if( IBK::string_nocase_find(oldName, "Cellular Glass.mat") != std::string::npos)
		return "CellularGlass_44.m6";     // not Delphin ready
	if( IBK::string_nocase_find(oldName, "Cellulose_Insulation.mat") != std::string::npos)
		return "CelluloseInsulation_45.m6";
	if( IBK::string_nocase_find(oldName, "Cement Flooring.mat") != std::string::npos)
		return "CementFlooring_46.m6";     // not Delphin ready
	if( IBK::string_nocase_find(oldName, "Cement Plaster.mat") != std::string::npos)
		return "CementPlaster_47.m6";     // not Delphin ready
	if( IBK::string_nocase_find(oldName, "Cement Plaster_1.mat") != std::string::npos)
		return "CementPlaster_48.m6";     // not Delphin ready
	if( IBK::string_nocase_find(oldName, "Ceramics And Glass Mosaic.mat") != std::string::npos)
		return "CeramicsAndGlassMosaic_49.m6";     // not Delphin ready
	if( IBK::string_nocase_find(oldName, "Clay Mortar (hist.).mat") != std::string::npos)
		return "ClayMortarHist_50.m6";     // not Delphin ready
	if( IBK::string_nocase_find(oldName, "Clay Plaster.mat") != std::string::npos)
		return "ClayPlaster_51.m6";     // not Delphin ready
	if( IBK::string_nocase_find(oldName, "Clay Tu2_PV 0_439.mat") != std::string::npos)
		return "Clay_392.m6";
	if( IBK::string_nocase_find(oldName, "Clincker Brick.mat") != std::string::npos)
		return "ClinckerBrick_53.m6";     // not Delphin ready
	if( IBK::string_nocase_find(oldName, "Concrete.mat") != std::string::npos)
		return "Concrete_54.m6";     // not Delphin ready
	if( IBK::string_nocase_find(oldName, "Concrete_1.mat") != std::string::npos)
		return "Concrete_55.m6";     // not Delphin ready
	if( IBK::string_nocase_find(oldName, "Concrete_2.mat") != std::string::npos)
		return "Concrete_56.m6";     // not Delphin ready
	if( IBK::string_nocase_find(oldName, "Concrete_3.mat") != std::string::npos)
		return "Concrete_57.m6";     // not Delphin ready
	if( IBK::string_nocase_find(oldName, "Concrete_C20_25.mat") != std::string::npos)
		return "Concrete_58.m6";
	if( IBK::string_nocase_find(oldName, "Coniferous Wood.mat") != std::string::npos)
		return "ConiferousWood_59.m6";     // not Delphin ready
	if( IBK::string_nocase_find(oldName, "Copper.mat") != std::string::npos)
		return "Copper_60.m6";
	if( IBK::string_nocase_find(oldName, "Cork Board.mat") != std::string::npos)
		return "CorkBoard_61.m6";     // not Delphin ready
	if( IBK::string_nocase_find(oldName, "Cork.mat") != std::string::npos)
		return "Cork_62.m6";     // not Delphin ready
	if( IBK::string_nocase_find(oldName, "Cork_1.mat") != std::string::npos)
		return "Cork_63.m6";     // not Delphin ready
	if( IBK::string_nocase_find(oldName, "Diatomite Brick.mat") != std::string::npos)
		return "DiatomiteBrick_64.m6";     // not Delphin ready
	if( IBK::string_nocase_find(oldName, "Dichtspachtel_Remmers.mat") != std::string::npos)
		return "DichtspachtelRemmers_65.m6";     // not Delphin ready
	if( IBK::string_nocase_find(oldName, "Fliesenkleber_FKA_Remmers_Baustoffe.mat") != std::string::npos)
		return "FliesenkleberFKARemmers_66.m6";
	if( IBK::string_nocase_find(oldName, "Fliesenkleber_FKB_Remmers_Baustoffe.mat") != std::string::npos)
		return "FliesenkleberFKBRemmers_67.m6";
	if( IBK::string_nocase_find(oldName, "Flie""\xDF""estrich_Knauf_FE50.mat") != std::string::npos)
		return "FliessestrichKnaufFE50_68.m6";
	if( IBK::string_nocase_find(oldName, "Floating Floor Screed.mat") != std::string::npos)
		return "FloatingFloorScreed_69.m6";
	if( IBK::string_nocase_find(oldName, "Foamglas.mat") != std::string::npos)
		return "Foamglas_70.m6";
	if( IBK::string_nocase_find(oldName, "Gipsputz_MAXIT_MX_GP_GS.mat") != std::string::npos)
		return "GipsputzMAXIT_71.m6";
	if( IBK::string_nocase_find(oldName, "Glass.mat") != std::string::npos)
		return "Glass_72.m6";
	if( IBK::string_nocase_find(oldName, "Glue Mortar (Climate Board).mat") != std::string::npos)
		return "GlueMortarForClimateBoard_73.m6";     // not Delphin ready
	if( IBK::string_nocase_find(oldName, "Glue Mortar (For Climate Board).mat") != std::string::npos)
		return "GlueMortarForClimateBoard_74.m6";     // not Delphin ready
	if( IBK::string_nocase_find(oldName, "Glue Mortar (For Climate Board)_2.mat") != std::string::npos)
		return "GlueMortarForClimateBoard_75.m6";     // not Delphin ready
	if( IBK::string_nocase_find(oldName, "Glue Mortar (For Mineral Insulation Board).mat") != std::string::npos)
		return "GlueMortarForMineralInsulationBoard_76.m6";     // not Delphin ready
	if( IBK::string_nocase_find(oldName, "Glue Mortar.mat") != std::string::npos)
		return "GlueMortar_77.m6";
	if( IBK::string_nocase_find(oldName, "Granite.mat") != std::string::npos)
		return "Granite_78.m6";     // not Delphin ready
	if( IBK::string_nocase_find(oldName, "Gypsum Board.mat") != std::string::npos)
		return "GypsumBoard_79.m6";     // not Delphin ready
	if( IBK::string_nocase_find(oldName, "Gypsum Board_1.mat") != std::string::npos)
		return "GypsumBoard_80.m6";     // not Delphin ready
	if( IBK::string_nocase_find(oldName, "Gypsum Board_d5.mat") != std::string::npos)
		return "GypsumBoard_81.m6";
	if( IBK::string_nocase_find(oldName, "Gypsum Plaster.mat") != std::string::npos)
		return "GypsumPlaster_82.m6";     // not Delphin ready
	if( IBK::string_nocase_find(oldName, "Gypsum Plaster_2.mat") != std::string::npos)
		return "GypsumPlaster_83.m6";     // not Delphin ready
	if( IBK::string_nocase_find(oldName, "Gypsum Plaster_3.mat") != std::string::npos)
		return "GypsumPlaster_84.m6";     // not Delphin ready
	if( IBK::string_nocase_find(oldName, "Gypsum Plaster_4.mat") != std::string::npos)
		return "GypsumPlaster_85.m6";     // not Delphin ready
	if( IBK::string_nocase_find(oldName, "HaackeCellco_MatC.mat") != std::string::npos)
		return "HaackeCellcoMatC_86.m6";
	if( IBK::string_nocase_find(oldName, "HaackeCellco_MatE.mat") != std::string::npos)
		return "HaackeCellcoMatE_87.m6";
	if( IBK::string_nocase_find(oldName, "Haacke_Cellco_KAPI.mat") != std::string::npos)
		return "HaackeCellcoKAPI_88.m6";     // not Delphin ready
	if( IBK::string_nocase_find(oldName, "Haacke_Cellco_KAPII.mat") != std::string::npos)
		return "HaackeCellcoKAPII_89.m6";
	if( IBK::string_nocase_find(oldName, "Haacke_Cellco_KAPI_1.mat") != std::string::npos)
		return "HaackeCellcoKAPI_90.m6";
	if( IBK::string_nocase_find(oldName, "Haniel Fermacell (Fa. Xella International).mat") != std::string::npos)
		return "FermacellXella_91.m6";
	if( IBK::string_nocase_find(oldName, "Hard-burnt Brick.mat") != std::string::npos)
		return "HardBurntBrick_92.m6";     // not Delphin ready
	if( IBK::string_nocase_find(oldName, "Hard-grey-burnt Brick.mat") != std::string::npos)
		return "HardGreyBurntBrick_93.m6";     // not Delphin ready
	if( IBK::string_nocase_find(oldName, "Hard-grey-burnt Brick_1.mat") != std::string::npos)
		return "HardGreyBurntBrick_94.m6";     // not Delphin ready
	if( IBK::string_nocase_find(oldName, "Hardwood.mat") != std::string::npos)
		return "Hardwood_95.m6";     // not Delphin ready
	if( IBK::string_nocase_find(oldName, "Historical Brick Cluster (edge).mat") != std::string::npos)
		return "HistoricalBrickClusterEdge_96.m6";
	if( IBK::string_nocase_find(oldName, "Historical Brick Cluster 4-DD.mat") != std::string::npos)
		return "HistoricalBrickCluster4DD_97.m6";
	if( IBK::string_nocase_find(oldName, "Historical Brick Cluster.mat") != std::string::npos)
		return "AltbauziegelDresdenZP_504.m6";
	if( IBK::string_nocase_find(oldName, "Historical Brick.mat") != std::string::npos)
		return "AltbauziegelDresdenZE_493.m6";
	if( IBK::string_nocase_find(oldName, "Historical Brick_1.mat") != std::string::npos)
		return "HistoricalBrick_100.m6";     // not Delphin ready
	if( IBK::string_nocase_find(oldName, "Historical Brick_2.mat") != std::string::npos)
		return "HistoricalBrick_101.m6";     // not Delphin ready
	if( IBK::string_nocase_find(oldName, "Historical Brick_3.mat") != std::string::npos)
		return "HistoricalBrick_102.m6";     // not Delphin ready
	if( IBK::string_nocase_find(oldName, "Historical clay mortar.mat") != std::string::npos)
		return "HistoricalClayMortar_103.m6";
	if( IBK::string_nocase_find(oldName, "Historical Clincker-Surface.mat") != std::string::npos)
		return "HistoricalClinckerSurface_104.m6";     // not Delphin ready
	if( IBK::string_nocase_find(oldName, "Historical Clincker.mat") != std::string::npos)
		return "HistoricalClincker_105.m6";     // not Delphin ready
	if( IBK::string_nocase_find(oldName, "Historical Clincker_1.mat") != std::string::npos)
		return "AltbauziegelDresdenZF_494.m6";
	if( IBK::string_nocase_find(oldName, "Innenputz_Klimasan_KSPe.mat") != std::string::npos)
		return "KlimasanI_419.m6";
	if( IBK::string_nocase_find(oldName, "Insulating Clay.mat") != std::string::npos)
		return "InsulatingClay_108.m6";     // not Delphin ready
	if( IBK::string_nocase_find(oldName, "Insulating Clay_1.mat") != std::string::npos)
		return "InsulatingClay_109.m6";     // not Delphin ready
	if( IBK::string_nocase_find(oldName, "Insulating Clay_3.mat") != std::string::npos)
		return "HaackeCellcoWaermedaemmlehmKorkBigPack_414.m6";
	if( IBK::string_nocase_find(oldName, "Insulating Clay_5.mat") != std::string::npos)
		return "InsulatingClay_111.m6";     // not Delphin ready
	if( IBK::string_nocase_find(oldName, "Insulating Clay_6.mat") != std::string::npos)
		return "InsulatingClay_112.m6";     // not Delphin ready
	if( IBK::string_nocase_find(oldName, "Insulation Concrete.mat") != std::string::npos)
		return "InsulationConcrete_113.m6";     // not Delphin ready
	if( IBK::string_nocase_find(oldName, "Insulation Loam (Corc).mat") != std::string::npos)
		return "InsulationLoamCorc_114.m6";
	if( IBK::string_nocase_find(oldName, "Insulation Plaster.mat") != std::string::npos)
		return "InsulationPlaster_115.m6";     // not Delphin ready
	if( IBK::string_nocase_find(oldName, "Insulation Plaster_1.mat") != std::string::npos)
		return "InsulationPlaster_116.m6";     // not Delphin ready
	if( IBK::string_nocase_find(oldName, "Insulation plaster_d5.mat") != std::string::npos)
		return "InsulationPlaster_117.m6";
	if( IBK::string_nocase_find(oldName, "Insulation Stone.mat") != std::string::npos)
		return "InsulationStone_118.m6";     // not Delphin ready
	if( IBK::string_nocase_find(oldName, "Insulation-Clay-Cork-FW.mat") != std::string::npos)
		return "InsulationClayCorkFW_119.m6";
	if( IBK::string_nocase_find(oldName, "IQfill.mat") != std::string::npos)
		return "IQfill_120.m6";     // not Delphin ready
	if( IBK::string_nocase_find(oldName, "IQtherm.mat") != std::string::npos)
		return "RemmersiQTherm_438.m6";
	if( IBK::string_nocase_find(oldName, "IQtop.mat") != std::string::npos)
		return "RemmersiQTop_439.m6";
	if( IBK::string_nocase_find(oldName, "Klebemoertel_leicht_Schommburg.mat") != std::string::npos)
		return "KlebemoertelLeichtSchommburg_123.m6";
	if( IBK::string_nocase_find(oldName, "Klimaputz_Maschinenputz_Tubag.mat") != std::string::npos)
		return "KlimaputzMaschinenputzTubag_124.m6";
	if( IBK::string_nocase_find(oldName, "Klimaputz_MKK_Quickmix.mat") != std::string::npos)
		return "KlimaputzMKKQuickmix_125.m6";
	if( IBK::string_nocase_find(oldName, "Lean Concrete.mat") != std::string::npos)
		return "LeanConcrete_126.m6";     // not Delphin ready
	if( IBK::string_nocase_find(oldName, "Lean Concrete_1.mat") != std::string::npos)
		return "LeanConcrete_127.m6";     // not Delphin ready
	if( IBK::string_nocase_find(oldName, "Lehmm""\xF6""rtel_LMH.mat") != std::string::npos)
		return "LehmmoertelLMH_128.m6";
	if( IBK::string_nocase_find(oldName, "Light-weight Wood Wool Magnesite Board.mat") != std::string::npos)
		return "LightWeightWoodWoolMagnesiteBoard_129.m6";     // not Delphin ready
	if( IBK::string_nocase_find(oldName, "LightClayMortar.mat") != std::string::npos)
		return "LightClayMortar_130.m6";
	if( IBK::string_nocase_find(oldName, "Lightweight Concrete.mat") != std::string::npos)
		return "LightweightConcrete_131.m6";     // not Delphin ready
	if( IBK::string_nocase_find(oldName, "Lightweight Concrete_1.mat") != std::string::npos)
		return "LightweightConcrete_132.m6";     // not Delphin ready
	if( IBK::string_nocase_find(oldName, "Lightweight Concrete_2.mat") != std::string::npos)
		return "LightweightConcrete_133.m6";     // not Delphin ready
	if( IBK::string_nocase_find(oldName, "Lightweight Concrete_3.mat") != std::string::npos)
		return "LightweightConcrete_134.m6";     // not Delphin ready
	if( IBK::string_nocase_find(oldName, "Lightweight Concrete_4.mat") != std::string::npos)
		return "LightweightConcrete_135.m6";     // not Delphin ready
	if( IBK::string_nocase_find(oldName, "Lightweight Concrete_5.mat") != std::string::npos)
		return "LightweightConcrete_136.m6";     // not Delphin ready
	if( IBK::string_nocase_find(oldName, "Lightweight Concrete_6.mat") != std::string::npos)
		return "LightweightConcrete_137.m6";     // not Delphin ready
	if( IBK::string_nocase_find(oldName, "Lightweight Concrete_7.mat") != std::string::npos)
		return "LightweightConcrete_138.m6";     // not Delphin ready
	if( IBK::string_nocase_find(oldName, "Lightweight-clay_mortar.mat") != std::string::npos)
		return "LightweightClayMortar_139.m6";     // not Delphin ready
	if( IBK::string_nocase_find(oldName, "Lightweight-clay_mortar_1.mat") != std::string::npos)
		return "LightweightClayMortar_140.m6";     // not Delphin ready
	if( IBK::string_nocase_find(oldName, "Lime Cement Mortar.mat") != std::string::npos)
		return "LimeCementMortar_141.m6";     // not Delphin ready
	if( IBK::string_nocase_find(oldName, "Lime Cement Mortar_1.mat") != std::string::npos)
		return "LimeCementMortar_142.m6";     // not Delphin ready
	if( IBK::string_nocase_find(oldName, "Lime Cement Mortar_2.mat") != std::string::npos)
		return "LimeCementMortar_143.m6";
	if( IBK::string_nocase_find(oldName, "Lime Cement Plaster (lightweight).mat") != std::string::npos)
		return "LimeCementPlasterLightweight_144.m6";     // not Delphin ready
	if( IBK::string_nocase_find(oldName, "Lime Cement Plaster (Transputz SG).mat") != std::string::npos)
		return "LimeCementPlasterTransputzSG_145.m6";
	if( IBK::string_nocase_find(oldName, "Lime Cement Plaster.mat") != std::string::npos)
		return "LimeCementPlaster_146.m6";     // not Delphin ready
	if( IBK::string_nocase_find(oldName, "Lime Cement Plaster_1.mat") != std::string::npos)
		return "LimeCementPlaster_147.m6";     // not Delphin ready
	if( IBK::string_nocase_find(oldName, "Lime Plaster (hist.).mat") != std::string::npos)
		return "LimePlasterHist_148.m6";
	if( IBK::string_nocase_find(oldName, "Lime plaster for fettling machine.mat") != std::string::npos)
		return "LimePlasterForFettlingMachine_149.m6";
	if( IBK::string_nocase_find(oldName, "Lime plaster for fine surface coating.mat") != std::string::npos)
		return "LimePlasterForFineSurfaceCoating_150.m6";
	if( IBK::string_nocase_find(oldName, "Lime Plaster.mat") != std::string::npos)
		return "LimePlaster_151.m6";     // not Delphin ready
	if( IBK::string_nocase_find(oldName, "Lime sand brick industrial.mat") != std::string::npos)
		return "LimeSandBrickIndustrial_152.m6";
	if( IBK::string_nocase_find(oldName, "Lime sand brick traditional.mat") != std::string::npos)
		return "LimeSandBrickTraditional_153.m6";
	if( IBK::string_nocase_find(oldName, "Lime Trass Light Plaster.mat") != std::string::npos)
		return "LimeTrassLightPlaster_154.m6";
	if( IBK::string_nocase_find(oldName, "Linoleum.mat") != std::string::npos)
		return "Linoleum_155.m6";     // not Delphin ready
	if( IBK::string_nocase_find(oldName, "Loam LMH (Hist.).mat") != std::string::npos)
		return "LoamLMHHist_156.m6";
	if( IBK::string_nocase_find(oldName, "Marble.mat") != std::string::npos)
		return "Marble_157.m6";     // not Delphin ready
	if( IBK::string_nocase_find(oldName, "Maxit_Flie""\xE1""beton908FB_G_908.mat") != std::string::npos)
		return "MaxitFliessbeton908FBG908_158.m6";
	if( IBK::string_nocase_find(oldName, "Maxit_Leichtbeton915FB_G_LB15.mat") != std::string::npos)
		return "MaxitLeichtbeton915FBGLB15_159.m6";
	if( IBK::string_nocase_find(oldName, "Maxit_Porengrundputz_MXGP.mat") != std::string::npos)
		return "MaxitPorengrundputzMXGP_160.m6";
	if( IBK::string_nocase_find(oldName, "Maxit_Sanierspritzbewurf_MXSV.mat") != std::string::npos)
		return "MaxitSanierspritzbewurfMXSV_161.m6";
	if( IBK::string_nocase_find(oldName, "Medium Density Fibreboard.mat") != std::string::npos)
		return "MediumDensityFibreboard_162.m6";     // not Delphin ready
	if( IBK::string_nocase_find(oldName, "Medium Density Fibreboard_2.mat") != std::string::npos)
		return "MediumDensityFibreboard_163.m6";     // not Delphin ready
	if( IBK::string_nocase_find(oldName, "Mineral Insulation Board_3.mat") != std::string::npos)
		return "MineralInsulationBoard_164.m6";
	if( IBK::string_nocase_find(oldName, "Mineral Insulation Board_Dennert.mat") != std::string::npos)
		return "MineralInsulationBoardDennert_165.m6";     // not Delphin ready
	if( IBK::string_nocase_find(oldName, "Mineral Insulation Board_Dennert_1.mat") != std::string::npos)
		return "MineralInsulationBoardDennert_166.m6";     // not Delphin ready
	if( IBK::string_nocase_find(oldName, "Mineral Insulation Board_Dennert_2.mat") != std::string::npos)
		return "MineralInsulationBoardDennert_167.m6";     // not Delphin ready
	if( IBK::string_nocase_find(oldName, "Mineral Insulation Board_Dennert_3.mat") != std::string::npos)
		return "MineralInsulationBoardDennert_168.m6";     // not Delphin ready
	if( IBK::string_nocase_find(oldName, "Mineral insulation plaster.mat") != std::string::npos)
		return "TrassMineralInsulationPlaster_251.m6";
	if( IBK::string_nocase_find(oldName, "Mineral Wool_d5.mat") != std::string::npos)
		return "MineralWool_282.m6";
	if( IBK::string_nocase_find(oldName, "Mould Renovating Plaster.mat") != std::string::npos)
		return "MouldRenovatingPlaster_171.m6";    // not Delphin ready
	if( IBK::string_nocase_find(oldName, "OSB Board_d5.mat") != std::string::npos)
		return "OSBBoard_172.m6";
	if( IBK::string_nocase_find(oldName, "Particle board.mat") != std::string::npos)
		return "ParticleBoard_173.m6";
	if( IBK::string_nocase_find(oldName, "PE-Foil.mat") != std::string::npos)
		return "PEFoil_174.m6";
	if( IBK::string_nocase_find(oldName, "Perlite bitumenummantelt.mat") != std::string::npos)
		return "PerliteBitumenummantelt_175.m6";    // not Delphin ready
	if( IBK::string_nocase_find(oldName, "Perlite Clay Stone.mat") != std::string::npos)
		return "PerliteClayStone_176.m6";    // not Delphin ready
	if( IBK::string_nocase_find(oldName, "Perlite Insulation Board.mat") != std::string::npos)
		return "PerliteInsulationBoard_177.m6";    // not Delphin ready
	if( IBK::string_nocase_find(oldName, "Perlite Insulation Board_1.mat") != std::string::npos)
		return "PerliteInsulationBoard_178.m6";    // not Delphin ready
	if( IBK::string_nocase_find(oldName, "Phenolic_resin_rigid_foam.mat") != std::string::npos)
		return "PhenolicResinRigidFoam_179.m6";    // not Delphin ready
	if( IBK::string_nocase_find(oldName, "Pine.mat") != std::string::npos)
		return "Pine_180.m6";    // not Delphin ready
	if( IBK::string_nocase_find(oldName, "Plump.mat") != std::string::npos)
		return "Plump_181.m6";
	if( IBK::string_nocase_find(oldName, "Polyester Board.mat") != std::string::npos)
		return "PolyesterBoard_182.m6";
	if( IBK::string_nocase_find(oldName, "Polyethen.mat") != std::string::npos)
		return "Polyethen_183.m6";
	if( IBK::string_nocase_find(oldName, "Polymethyl Acrylate.mat") != std::string::npos)
		return "PolymethylAcrylate_184.m6";
	if( IBK::string_nocase_find(oldName, "Polypropen.mat") != std::string::npos)
		return "Polypropen_185.m6";
	if( IBK::string_nocase_find(oldName, "Polystyrene Board - Expanded.mat") != std::string::npos)
		return "PolystyreneBoardExpanded_186.m6";
	if( IBK::string_nocase_find(oldName, "Polystyrene Board - Expanded_1.mat") != std::string::npos)
		return "PolystyreneBoardExpanded_187.m6";
	if( IBK::string_nocase_find(oldName, "Polystyrene Board - Extruded.mat") != std::string::npos)
		return "PolystyreneBoardExtruded_188.m6";
	if( IBK::string_nocase_find(oldName, "Polystyrene Board - Extruded_1.mat") != std::string::npos)
		return "PolystyreneBoardExtruded_189.m6";
	if( IBK::string_nocase_find(oldName, "Polystyrene Foam Concrete.mat") != std::string::npos)
		return "PolystyreneFoamConcrete_190.m6";    // not Delphin ready
	if( IBK::string_nocase_find(oldName, "Polystyrene Foam Concrete_1.mat") != std::string::npos)
		return "PolystyreneFoamConcrete_191.m6";    // not Delphin ready
	if( IBK::string_nocase_find(oldName, "Polystyrene Foam Concrete_2.mat") != std::string::npos)
		return "PolystyreneFoamConcrete_192.m6";    // not Delphin ready
	if( IBK::string_nocase_find(oldName, "Polyurethane boards.mat") != std::string::npos)
		return "PolyurethaneBoard_193.m6";
	if( IBK::string_nocase_find(oldName, "Polyurethane boards_1.mat") != std::string::npos)
		return "PolyurethaneBoard_194.m6";
	if( IBK::string_nocase_find(oldName, "Polyurethane-foam.mat") != std::string::npos)
		return "PolyurethaneFoam_195.m6";
	if( IBK::string_nocase_find(oldName, "Pumice Concrete.mat") != std::string::npos)
		return "PumiceConcrete_196.m6";    // not Delphin ready
	if( IBK::string_nocase_find(oldName, "Pumice Concrete_1.mat") != std::string::npos)
		return "PumiceConcrete_197.m6";    // not Delphin ready
	if( IBK::string_nocase_find(oldName, "PVC Floor Covering.mat") != std::string::npos)
		return "PVCFloorCovering_198.m6";
	if( IBK::string_nocase_find(oldName, "PVC Foam.mat") != std::string::npos)
		return "PVCFoam_199.m6";
	if( IBK::string_nocase_find(oldName, "PVC Foil.mat") != std::string::npos)
		return "PVCFoil_200.m6";
	if( IBK::string_nocase_find(oldName, "Quartz Glass.mat") != std::string::npos)
		return "QuartzGlass_201.m6";
	if( IBK::string_nocase_find(oldName, "Rammed Insulating Clay.mat") != std::string::npos)
		return "HaackeCellcoWaermedaemmlehmKorkBigPack_414.m6";
	if( IBK::string_nocase_find(oldName, "Rammed Insulating Clay_1.mat") != std::string::npos)
		return "RammedInsulatingClay_202.m6";    // not Delphin ready
	if( IBK::string_nocase_find(oldName, "Red-burnt Brick.mat") != std::string::npos)
		return "RedBurntBrick_203.m6";    // not Delphin ready
	if( IBK::string_nocase_find(oldName, "Red-burnt Brick_1.mat") != std::string::npos)
		return "RedBurntBrick_204.m6";    // not Delphin ready
	if( IBK::string_nocase_find(oldName, "Red-burnt Brick_2.mat") != std::string::npos)
		return "RedBurntBrick_205.m6";    // not Delphin ready
	if( IBK::string_nocase_find(oldName, "Reed Mat.mat") != std::string::npos)
		return "ReedMat_206.m6";    // not Delphin ready
	if( IBK::string_nocase_find(oldName, "Reed Mat_1.mat") != std::string::npos)
		return "ReedMat_207.m6";    // not Delphin ready
	if( IBK::string_nocase_find(oldName, "Reed Straw Chaffing Board.mat") != std::string::npos)
		return "ReedStrawChaffingBoard_208.m6";    // not Delphin ready
	if( IBK::string_nocase_find(oldName, "Renovating Plaster (lightweight).mat") != std::string::npos)
		return "RenovatingPlasterLightweight_209.m6";    // not Delphin ready
	if( IBK::string_nocase_find(oldName, "Restoration Render.mat") != std::string::npos)
		return "RestorationRender_210.m6";
	if( IBK::string_nocase_find(oldName, "Roofing membrane.mat") != std::string::npos)
		return "RoofingMembrane_211.m6";
	if( IBK::string_nocase_find(oldName, "Sand mS_PV 0_413.mat") != std::string::npos)
		return "Sand_396.m6";
	if( IBK::string_nocase_find(oldName, "Sand-lime Brick Masonry.mat") != std::string::npos)
		return "SandLimeBrickMasonry_213.m6";    // not Delphin ready
	if( IBK::string_nocase_find(oldName, "Sand-lime Brick.mat") != std::string::npos)
		return "XellaKalksandstein_511.m6";
	if( IBK::string_nocase_find(oldName, "Sand-lime Brick_1.mat") != std::string::npos)
		return "SandLimeBrick_215.m6";   // not Delphin ready
	if( IBK::string_nocase_find(oldName, "Sand-lime Brick_2.mat") != std::string::npos)
		return "SandLimeBrick_216.m6";   // not Delphin ready
	if( IBK::string_nocase_find(oldName, "Sand-lime Brick_3.mat") != std::string::npos)
		return "SandLimeBrick_217.m6";   // not Delphin ready
	if( IBK::string_nocase_find(oldName, "Sand-lime Brick_4.mat") != std::string::npos)
		return "SandLimeBrick_218.m6";   // not Delphin ready
	if( IBK::string_nocase_find(oldName, "Sandstone (Cotta).mat") != std::string::npos)
		return "SandstoneCotta_219.m6";
	if( IBK::string_nocase_find(oldName, "Sandstone (Posta).mat") != std::string::npos)
		return "SandstonePosta_220.m6";
	if( IBK::string_nocase_find(oldName, "Sandstone (Reinhardsdorf).mat") != std::string::npos)
		return "SandstoneReinhardsdorf_221.m6";
	if( IBK::string_nocase_find(oldName, "Sandstone.mat") != std::string::npos)
		return "SandsteinRuethen_469.m6";
	if( IBK::string_nocase_find(oldName, "Sandstone_1.mat") != std::string::npos)
		return "SandsteinBadBentheim_465.m6";
	if( IBK::string_nocase_find(oldName, "Sandstone_10.mat") != std::string::npos)
		return "Sandstone_224.m6";   // not Delphin ready
	if( IBK::string_nocase_find(oldName, "Sandstone_3.mat") != std::string::npos)
		return "Sandstone_225.m6";   // not Delphin ready
	if( IBK::string_nocase_find(oldName, "Sandstone_4.mat") != std::string::npos)
		return "SandsteinHildesheim_466.m6";
	if( IBK::string_nocase_find(oldName, "Sandstone_7.mat") != std::string::npos)
		return "Sandstone_227.m6";   // not Delphin ready
	if( IBK::string_nocase_find(oldName, "Sandstone_9.mat") != std::string::npos)
		return "Sandstone_228.m6";   // not Delphin ready
	if( IBK::string_nocase_find(oldName, "SanierHaftGl""\xE4""ttputz_Tubag_SHG.mat") != std::string::npos)
		return "SanierHaftGlaettputzTubagSHG_229.m6";
	if( IBK::string_nocase_find(oldName, "Schimmelsanierputz_AISIT_Remmers_Baustoffe.mat") != std::string::npos)
		return "SchimmelsanierputzAISITRemmers_230.m6";
	if( IBK::string_nocase_find(oldName, "Seccopor_Innenputz_ Keimfarben.mat") != std::string::npos)
		return "SeccoporInnenputzKeimfarben_231.m6";
	if( IBK::string_nocase_find(oldName, "Silt Ut4_PV 0_38.mat") != std::string::npos)
		return "Silt_400.m6";
	if( IBK::string_nocase_find(oldName, "Silt Ut4_PV 0_468.mat") != std::string::npos)
		return "Silt_400.m6";
	if( IBK::string_nocase_find(oldName, "Solid Brick.mat") != std::string::npos)
		return "CeramicBrick_513.m6";
	if( IBK::string_nocase_find(oldName, "Spruce SW_Fi across grain.mat") != std::string::npos)
		return "SpruceSWFiAcrossGrain_235.m6";
	if( IBK::string_nocase_find(oldName, "Spruce.mat") != std::string::npos)
		return "Spruce_236.m6";   // not Delphin ready
	if( IBK::string_nocase_find(oldName, "Standard Building Brick_1.mat") != std::string::npos)
		return "BrickWienerberger_512.m6";
	if( IBK::string_nocase_find(oldName, "Steel.mat") != std::string::npos)
		return "Steel_238.m6";
	if( IBK::string_nocase_find(oldName, "Straw Chaffing Board.mat") != std::string::npos)
		return "StrawChaffingBoard_239.m6";   // not Delphin ready
	if( IBK::string_nocase_find(oldName, "Straw Clay.mat") != std::string::npos)
		return "StrawClay_240.m6";   // not Delphin ready
	if( IBK::string_nocase_find(oldName, "Sulfatex_Remmers.mat") != std::string::npos)
		return "Sulfatexschlaemme_488.m6";
	if( IBK::string_nocase_find(oldName, "Surface coating plaster.mat") != std::string::npos)
		return "SurfaceCoatingPlaster_242.m6";
	if( IBK::string_nocase_find(oldName, "Synthetic Resin Plaster_1.mat") != std::string::npos)
		return "SyntheticResinPlaster_243.m6";   // not Delphin ready
	if( IBK::string_nocase_find(oldName, "Teak.mat") != std::string::npos)
		return "Teak_244.m6";   // not Delphin ready
	if( IBK::string_nocase_find(oldName, "TecTem""\xA9"" Flaechenspachtel_Knauf_Perlite.mat") != std::string::npos)
		return "TecTemFlaechenspachtelKnaufPerlite_245.m6";
	if( IBK::string_nocase_find(oldName, "TecTem""\xAE"" Flaechenspachtel_Knauf_Perlite.mat") != std::string::npos)
		return "TecTemFlaechenspachtelKnaufPerlite_245.m6";
	if( IBK::string_nocase_find(oldName, "Tiles With Mortar.mat") != std::string::npos)
		return "TilesWithMortar_246.m6";   // not Delphin ready
	if( IBK::string_nocase_find(oldName, "Tiles With Mortar_1.mat") != std::string::npos)
		return "TilesWithMortar_247.m6";   // not Delphin ready
	if( IBK::string_nocase_find(oldName, "Timber Frame Mortar.mat") != std::string::npos)
		return "TimberFrameMortar_248.m6";   // not Delphin ready
	if( IBK::string_nocase_find(oldName, "TransputzSG_Hydroment_(Baustelle).mat") != std::string::npos)
		return "TransputzSGHydromentBaustelle_249.m6";
	if( IBK::string_nocase_find(oldName, "Trass-lime Mineral Insulation Plaster.mat") != std::string::npos)
		return "TrassLimeMineralInsulationPlaster_250.m6";   // not Delphin ready
	if( IBK::string_nocase_find(oldName, "Trass-Mineral Insulation Plaster.mat") != std::string::npos)
		return "TrassMineralInsulationPlaster_251.m6";
	if( IBK::string_nocase_find(oldName, "TrassKalkFeinputz_Tubag_TKF.mat") != std::string::npos)
		return "TrassKalkFeinputzTubagTKF_252.m6";
	if( IBK::string_nocase_find(oldName, "Trasskalk_MLP_Tubag_TKM.mat") != std::string::npos)
		return "TrasskalkMLPTubagTKM_253.m6";
	if( IBK::string_nocase_find(oldName, "Triplex-Multiplex Board.mat") != std::string::npos)
		return "TriplexMultiplexBoard_254.m6";   // not Delphin ready
	if( IBK::string_nocase_find(oldName, "Tuff Stone.mat") != std::string::npos)
		return "TuffStone_255.m6";   // not Delphin ready
	if( IBK::string_nocase_find(oldName, "Urea-formaldehyde Foam.mat") != std::string::npos)
		return "UreaFormaldehydeFoam_256.m6";   // not Delphin ready
	if( IBK::string_nocase_find(oldName, "Urea-formaldehyde Foam_1.mat") != std::string::npos)
		return "UreaFormaldehydeFoam_257.m6";   // not Delphin ready
	if( IBK::string_nocase_find(oldName, "Vapour Retarder (open).mat") != std::string::npos)
		return "VapourRetarderOpen_258.m6";
	if( IBK::string_nocase_find(oldName, "Vertically Perforated Brick.mat") != std::string::npos)
		return "VerticallyPerforatedBrick_259.m6";  // not Delphin ready
	if( IBK::string_nocase_find(oldName, "Vertically Perforated Brick_1.mat") != std::string::npos)
		return "VerticallyPerforatedBrick_260.m6";  // not Delphin ready
	if( IBK::string_nocase_find(oldName, "Vulcanized Rubber - Expanded.mat") != std::string::npos)
		return "VulcanizedRubberExpanded_261.m6";  // not Delphin ready
	if( IBK::string_nocase_find(oldName, "Vulcanized Rubber.mat") != std::string::npos)
		return "VulcanizedRubber_262.m6";  // not Delphin ready
	if( IBK::string_nocase_find(oldName, "Weathered granite.mat") != std::string::npos)
		return "WeatheredGranite_263.m6";
	if( IBK::string_nocase_find(oldName, "Wienerberger Normal Brick.mat") != std::string::npos)
		return "WienerbergerNormalBrick_264.m6";
	if( IBK::string_nocase_find(oldName, "Wood Cement Particle Board.mat") != std::string::npos)
		return "WoodCementParticleBoard_265.m6";  // not Delphin ready
	if( IBK::string_nocase_find(oldName, "Wood Fiber Insulation Board.mat") != std::string::npos)
		return "WoodFiberInsulationBoard_266.m6";  // not Delphin ready
	if( IBK::string_nocase_find(oldName, "Wood Fiber Insulation Board_2.mat") != std::string::npos)
		return "WoodFiberInsulationBoard_267.m6";  // not Delphin ready
	if( IBK::string_nocase_find(oldName, "Wood Fiber Insulation Board_Pavatex2.mat") != std::string::npos)
		return "WoodFiberInsulationBoardPavatex_268.m6";  // not Delphin ready
	if( IBK::string_nocase_find(oldName, "Wood Fiberboard.mat") != std::string::npos)
		return "WoodFiberBoard_269.m6";  // not Delphin ready
	if( IBK::string_nocase_find(oldName, "Wood fibre insulation board.mat") != std::string::npos)
		return "WoodFibreInsulationBoard_270.m6";
	if( IBK::string_nocase_find(oldName, "Wood fibre insulation board_Pavatex1.mat") != std::string::npos)
		return "WoodFibreInsulationBoardPavatex_271.m6";  // not Delphin ready
	if( IBK::string_nocase_find(oldName, "Wood Fibreboard.mat") != std::string::npos)
		return "WoodFibreBoard_272.m6";
	if( IBK::string_nocase_find(oldName, "Wood Particle Board.mat") != std::string::npos)
		return "WoodParticleBoard_273.m6";  // not Delphin ready
	if( IBK::string_nocase_find(oldName, "Wood Particle Board_1.mat") != std::string::npos)
		return "WoodParticleBoard_274.m6";  // not Delphin ready
	if( IBK::string_nocase_find(oldName, "Wood wool cement board.mat") != std::string::npos)
		return "WoodWoolCementBoard_275.m6";
	if( IBK::string_nocase_find(oldName, "Wood Wool Light Building Board.mat") != std::string::npos)
		return "WoodWoolLightBuildingBoard_276.m6";  // not Delphin ready
	if( IBK::string_nocase_find(oldName, "Wood Wool Light Building Board_1.mat") != std::string::npos)
		return "WoodWoolLightBuildingBoard_277.m6";  // not Delphin ready
	if( IBK::string_nocase_find(oldName, "Wood Wool Light Building Board_2.mat") != std::string::npos)
		return "WoodWoolLightBuildingBoard_278.m6";  // not Delphin ready
	if( IBK::string_nocase_find(oldName, "WoodCementBoard.mat") != std::string::npos)
		return "WoodCementBoard_279.m6";
	if( IBK::string_nocase_find(oldName, "Zinc.mat") != std::string::npos)
		return "Zinc_280.m6";
	return "";
}

// ----------------------------------------------------------------------------

std::ostream& write_spline(std::ostream& out, const std::string& istr, unsigned int len,
	const IBK::LinearSpline & sp, const std::string& fn_name)
{
	if (!sp.empty()) {
		out << istr << std::setw(len) << std::left << "FUNCTION = " << fn_name << std::endl;
		sp.write(out, static_cast<unsigned int>(istr.size())+4);
	}
	return out;
}


} // namespace MM


