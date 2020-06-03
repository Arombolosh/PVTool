#include "MM_KeywordList.h"

#include <map>
#include <limits>
#include <iostream>

#include <IBK_FormatString.h>
#include <IBK_Exception.h>


namespace MM {
	/*! Holds the string to print as error when an invalid keyword is encountered. */
	const char * const INVALID_KEYWORD_INDEX_STRING = "KEYWORD_ERROR_STRING: Invalid type index";

	/*! Holds a list of all enum types/categories. */
	const char * const ENUM_TYPES[34] = {
		"AirTransport::Functions",
		"Foils::Ident",
		"Foils::Param",
		"Foils::Functions",
		"MaterialIdentification::Ident",
		"MaterialIdentification::Flags",
		"MaterialIdentification::SamplingMethods",
		"MaterialIdentification::FlagsPreV6",
		"MaterialIdentification::HatchingType",
		"Material::Sections",
		"Material::SectionsPreV6",
		"Material::ParaStorage",
		"Material::ParaTransport",
		"Material::ParaMechanical",
		"MaterialCategory::CategoryID",
		"MeasuredData::Sections",
		"MeasuredData::WaterRetentionValues",
		"MeasuredData::WaterVapourPermeabilityValues",
		"MeasuredData::LiquidWaterConductivityValues",
		"MeasuredData::WaterUptakeValues",
		"MeasuredData::DryingValues0",
		"MeasuredData::DryingValues1",
		"MeasuredData::GeometryKeys",
		"MeasuredData::ExperimentKeys",
		"MeasuredData::Experiments",
		"ModelHandler::ModelIdentifier",
		"MoistureStorage::Functions",
		"MoistureStorage::ModelTypes",
		"MoistureTransport::Functions",
		"MoistureTransport::ModelTypes",
		"ThermalStorage::Functions",
		"ThermalTransport::Functions",
		"VaporContactResistanceModel::Param",
		"VaporContactResistanceModel::Functions"
	};

	/*! Converts a category string to respective enumeration value. */ 
	int enum2index(const std::string & enumtype) {
		for (int i=0; i<34; ++i) {
			if (enumtype == ENUM_TYPES[i]) return i;
		}
		//std::cerr << "Unknown enumeration type '" << enumtype<< "'." << std::endl;
		return -1;
	}
	

	/*! Returns a keyword string for a given category (typenum) and type number t. */ 
	const char * theKeyword(int typenum, int t) {
		switch (typenum) {
			// AirTransport::Functions 
			case 0 :
			switch (t) { 
				case 0 : return "Kg(Theta_l)";
			} break; 
			// Foils::Ident 
			case 1 :
			switch (t) { 
				case 0 : return "NAME";
				case 1 : return "ID_PLUS";
				case 2 : return "ID_MINUS";
				case 3 : return "COMMENT";
			} break; 
			// Foils::Param 
			case 2 :
			switch (t) { 
				case 0 : return "SD_VALUE";
				case 1 : return "WEIGHT_FACTOR_POS";
				case 2 : return "WEIGHT_FACTOR_NEG";
				case 3 : return "SCALING_A_POS";
				case 4 : return "SCALING_B_POS";
				case 5 : return "SCALING_C_POS";
				case 6 : return "SCALING_A_NEG";
				case 7 : return "SCALING_B_NEG";
				case 8 : return "SCALING_C_NEG";
			} break; 
			// Foils::Functions 
			case 3 :
			switch (t) { 
				case 0 : return "sd+(rh)";
				case 1 : return "sd-(rh)";
			} break; 
			// MaterialIdentification::Ident 
			case 4 :
			switch (t) { 
				case 0 : return "NAME";
				case 1 : return "PRODUCER";
				case 2 : return "PRODUCT_ID";
				case 3 : return "INVESTIGATOR";
				case 4 : return "LABORATORY";
				case 5 : return "DATE";
				case 6 : return "SAMPLING_DATE";
				case 7 : return "PRODUCTION_DATE";
				case 8 : return "SAMPLE_ID";
				case 9 : return "SAMPLING_METHOD";
				case 10 : return "COUNTRY";
				case 11 : return "COLOUR";
				case 12 : return "FLAGS";
				case 13 : return "USE_INSTEAD";
				case 14 : return "COPYRIGHT";
				case 15 : return "CATEGORY";
				case 16 : return "COMMENTS";
				case 17 : return "U_DIRECTION";
				case 18 : return "V_DIRECTION";
				case 19 : return "W_DIRECTION";
				case 20 : return "DBTYPE";
				case 21 : return "SIGNATURE_SOURCE";
				case 22 : return "QUALITY_FLAGS";
				case 23 : return "DATA_SHEET_SOURCE";
				case 24 : return "HATCHING";
				case 25 : return "AQUISITION_ID";
			} break; 
			// MaterialIdentification::Flags 
			case 5 :
			switch (t) { 
				case 1 : return "AIR";
				case 2 : return "AIR_TIGHT";
				case 4 : return "VAPOR_TIGHT";
				case 8 : return "WATER_TIGHT";
				case 16 : return "WOOD";
				case 32 : return "WOOD_BASED";
				case 64 : return "FOILS";
			} break; 
			// MaterialIdentification::SamplingMethods 
			case 6 :
			switch (t) { 
				case 1 : return "S_CAST_CUT_PRODUCER";
				case 2 : return "S_CAST_PRODUCER";
				case 3 : return "S_CAST_CUT_IBK";
				case 4 : return "S_CAST_IBK";
			} break; 
			// MaterialIdentification::FlagsPreV6 
			case 7 :
			switch (t) { 
				case 0 : return "AIR";
				case 1 : return "AIRTIGHT";
				case 2 : return "VAPORTIGHT";
				case 3 : return "WATERTIGHT";
				case 4 : return "WOOD";
				case 5 : return "WOODBASED";
				case 6 : return "ONLY_HYGROSCOPIC";
			} break; 
			// MaterialIdentification::HatchingType 
			case 8 :
			switch (t) { 
				case 0 : return "Ground";
				case 1 : return "Gravel";
				case 2 : return "Sand";
				case 3 : return "ConcreteNotReinforced";
				case 4 : return "ConcreteReinforced";
				case 5 : return "Masonry";
				case 6 : return "WoodTransverseToFibre";
				case 7 : return "WoodLongitudinal";
				case 8 : return "Metal";
				case 9 : return "MortarPlaster";
				case 10 : return "Insulation";
				case 11 : return "SealingFoil";
				case 12 : return "Sealant";
				case 13 : return "None";
			} break; 
			// Material::Sections 
			case 9 :
			switch (t) { 
				case 0 : return "IDENTIFICATION";
				case 1 : return "STORAGE_BASE_PARAMETERS";
				case 2 : return "TRANSPORT_BASE_PARAMETERS";
				case 3 : return "MECHANICAL_BASE_PARAMETERS";
				case 4 : return "THERMAL_STORAGE";
				case 5 : return "MOISTURE_STORAGE";
				case 6 : return "MOISTURE_TRANSPORT";
				case 7 : return "HEAT_TRANSPORT";
				case 8 : return "AIR_TRANSPORT";
				case 9 : return "MOISTURE_TRANSPORT_V";
				case 10 : return "HEAT_TRANSPORT_V";
				case 11 : return "AIR_TRANSPORT_V";
				case 12 : return "MOISTURE_TRANSPORT_W";
				case 13 : return "HEAT_TRANSPORT_W";
				case 14 : return "AIR_TRANSPORT_W";
				case 15 : return "MEASURED_DATA";
				case 16 : return "VAPOR_CONTACT_RESISTANCE";
			} break; 
			// Material::SectionsPreV6 
			case 10 :
			switch (t) { 
				case 0 : return "MATERIAL";
				case 1 : return "IDENTIFICATION";
				case 2 : return "GENERAL";
				case 3 : return "MRC";
				case 4 : return "MOISTTRANS";
				case 5 : return "HEATTRANS";
				case 6 : return "AIRTRANS";
				case 7 : return "PARAMETER";
				case 8 : return "PCMMATERIAL";
			} break; 
			// Material::ParaStorage 
			case 11 :
			switch (t) { 
				case 0 : return "RHO";
				case 1 : return "CE";
				case 2 : return "THETA_POR";
				case 3 : return "THETA_EFF";
				case 4 : return "THETA_CAP";
				case 5 : return "THETA_LIM_HYG";
				case 6 : return "THETA_80";
			} break; 
			// Material::ParaTransport 
			case 12 :
			switch (t) { 
				case 0 : return "LAMBDA";
				case 1 : return "AW";
				case 2 : return "MEW";
				case 3 : return "SD";
				case 4 : return "KLEFF";
				case 5 : return "DLEFF";
				case 6 : return "KG";
				case 7 : return "LAMBDA_V";
				case 8 : return "AW_V";
				case 9 : return "MEW_V";
				case 10 : return "SD_V";
				case 11 : return "KLEFF_V";
				case 12 : return "DLEFF_V";
				case 13 : return "KG_V";
				case 14 : return "LAMBDA_W";
				case 15 : return "AW_W";
				case 16 : return "MEW_W";
				case 17 : return "SD_W";
				case 18 : return "KLEFF_W";
				case 19 : return "DLEFF_W";
				case 20 : return "KG_W";
				case 21 : return "LAMBDA_DESIGN";
			} break; 
			// Material::ParaMechanical 
			case 13 :
			switch (t) { 
				case 0 : return "ALPHA";
				case 1 : return "BETA_THETA_L";
				case 2 : return "BETA_THETA_L2";
				case 3 : return "ALPHA_V";
				case 4 : return "BETA_THETA_L_V";
				case 5 : return "BETA_THETA_L2_V";
				case 6 : return "ALPHA_W";
				case 7 : return "BETA_THETA_L_W";
				case 8 : return "BETA_THETA_L2_W";
			} break; 
			// MaterialCategory::CategoryID 
			case 14 :
			switch (t) { 
				case 0 : return "INVALID";
				case 1 : return "COATING";
				case 2 : return "PLASTER";
				case 3 : return "BRICK";
				case 4 : return "NATURAL_STONES";
				case 5 : return "CONCRETE";
				case 6 : return "INSULATION";
				case 7 : return "BUILDING_BOARDS";
				case 8 : return "TIMBER";
				case 9 : return "NATURAL_MATERIALS";
				case 10 : return "SOIL";
				case 11 : return "CLADDING";
				case 12 : return "FOILS";
				case 13 : return "MISC";
			} break; 
			// MeasuredData::Sections 
			case 15 :
			switch (t) { 
				case 0 : return "GEOMETRY";
				case 1 : return "EXPERIMENTS";
			} break; 
			// MeasuredData::WaterRetentionValues 
			case 16 :
			switch (t) { 
				case 0 : return "NON_LOG_PC";
				case 1 : return "TEMPERATURE";
				case 2 : return "THETA_L";
				case 3 : return "STD_DEVIATION";
				case 4 : return "MINIMUM";
				case 5 : return "MAXIMUM";
			} break; 
			// MeasuredData::WaterVapourPermeabilityValues 
			case 17 :
			switch (t) { 
				case 0 : return "PHI_INSIDE";
				case 1 : return "PHI_OUTSIDE";
				case 2 : return "MEW";
				case 3 : return "STD_DEVIATION";
				case 4 : return "MINIMUM";
				case 5 : return "MAXIMUM";
			} break; 
			// MeasuredData::LiquidWaterConductivityValues 
			case 18 :
			switch (t) { 
				case 0 : return "STD_DEVIATION";
				case 1 : return "MINIMUM";
				case 2 : return "MAXIMUM";
			} break; 
			// MeasuredData::WaterUptakeValues 
			case 19 :
			switch (t) { 
				case 0 : return "MASS";
			} break; 
			// MeasuredData::DryingValues0 
			case 20 :
			switch (t) { 
				case 0 : return "WEIGHT";
				case 1 : return "WIDTH_0";
				case 2 : return "LENGTH_0";
				case 3 : return "HEIGHT_0";
				case 4 : return "WIDTH_1";
				case 5 : return "LENGTH_1";
				case 6 : return "HEIGHT_1";
			} break; 
			// MeasuredData::DryingValues1 
			case 21 :
			switch (t) { 
				case 0 : return "BC_TEMPERATURE_0";
				case 1 : return "BC_RH_0";
				case 2 : return "BC_TEMPERATURE_1";
				case 3 : return "BC_RH_1";
				case 4 : return "SAMPLE_TEMPERATURE_0";
				case 5 : return "SAMPLE_TEMPERATURE_1";
				case 6 : return "SAMPLE_TEMPERATURE_2";
				case 7 : return "SAMPLE_TEMPERATURE_3";
				case 8 : return "SAMPLE_TEMPERATURE_4";
				case 9 : return "SAMPLE_TEMPERATURE_5";
			} break; 
			// MeasuredData::GeometryKeys 
			case 22 :
			switch (t) { 
				case 0 : return "SPECIMEN";
				case 1 : return "CYLINDRIC";
				case 2 : return "WIDTH_0";
				case 3 : return "WIDTH_1";
				case 4 : return "WIDTH_2";
				case 5 : return "WIDTH_3";
				case 6 : return "LENGTH_0";
				case 7 : return "LENGTH_1";
				case 8 : return "LENGTH_2";
				case 9 : return "LENGTH_3";
				case 10 : return "HEIGHT_0";
				case 11 : return "HEIGHT_1";
				case 12 : return "HEIGHT_2";
				case 13 : return "HEIGHT_3";
			} break; 
			// MeasuredData::ExperimentKeys 
			case 23 :
			switch (t) { 
				case 0 : return "SPECIMEN";
				case 1 : return "EXPERIMENT";
				case 2 : return "X_VALUES";
				case 3 : return "NON_LOG_PC";
				case 4 : return "TEMPERATURE";
				case 5 : return "THETA_L";
				case 6 : return "PHI_INSIDE";
				case 7 : return "PHI_OUTSIDE";
				case 8 : return "MEW";
				case 9 : return "MINIMUM";
				case 10 : return "MAXIMUM";
				case 11 : return "STD_DEVIATION";
				case 12 : return "WEIGHT";
				case 13 : return "WIDTH_0";
				case 14 : return "LENGTH_0";
				case 15 : return "HEIGHT_0";
				case 16 : return "WIDTH_1";
				case 17 : return "LENGTH_1";
				case 18 : return "HEIGHT_1";
				case 19 : return "DATE_BC";
				case 20 : return "BC_TEMPERATURE_0";
				case 21 : return "BC_RH_0";
				case 22 : return "BC_TEMPERATURE_1";
				case 23 : return "BC_RH_1";
				case 24 : return "SAMPLE_TEMPERATURE_0";
				case 25 : return "SAMPLE_TEMPERATURE_1";
				case 26 : return "SAMPLE_TEMPERATURE_2";
				case 27 : return "SAMPLE_TEMPERATURE_3";
				case 28 : return "SAMPLE_TEMPERATURE_4";
				case 29 : return "SAMPLE_TEMPERATURE_5";
			} break; 
			// MeasuredData::Experiments 
			case 24 :
			switch (t) { 
				case 1 : return "WET_CUP";
				case 2 : return "MED_CUP";
				case 4 : return "DRY_CUP";
				case 8 : return "WATER_RETENTION";
				case 16 : return "SORPTION_ISOTHERM";
				case 32 : return "WATER_UPTAKE";
				case 64 : return "DRYING_0";
				case 128 : return "DRYING_1";
			} break; 
			// ModelHandler::ModelIdentifier 
			case 25 :
			switch (t) { 
				case 0 : return "GAUSS";
				case 1 : return "SERIAL_PARALLEL";
			} break; 
			// MoistureStorage::Functions 
			case 26 :
			switch (t) { 
				case 0 : return "Theta_l(pC)_de";
				case 1 : return "pC(Theta_l)_de";
				case 2 : return "Theta_l(RH)";
				case 3 : return "Theta_l(pC)_ad";
				case 4 : return "pC(Theta_l)_ad";
				case 5 : return "mwu(T)";
			} break; 
			// MoistureStorage::ModelTypes 
			case 27 :
			switch (t) { 
				case 0 : return "GAUSS";
				case 1 : return "VAN_GENUCHTEN";
			} break; 
			// MoistureTransport::Functions 
			case 28 :
			switch (t) { 
				case 0 : return "lgKl(Theta_l)";
				case 1 : return "lgDl(Theta_l)";
				case 2 : return "lgKv(Theta_l)";
				case 3 : return "mew(RH)";
			} break; 
			// MoistureTransport::ModelTypes 
			case 29 :
			switch (t) { 
				case 0 : return "SERIAL_PARALLEL";
			} break; 
			// ThermalStorage::Functions 
			case 30 :
			switch (t) { 
				case 0 : return "u(T)";
			} break; 
			// ThermalTransport::Functions 
			case 31 :
			switch (t) { 
				case 0 : return "lambda(Theta_l)";
				case 1 : return "lambda(T)";
			} break; 
			// VaporContactResistanceModel::Param 
			case 32 :
			switch (t) { 
				case 0 : return "WEIGHT_FACTOR_POS";
				case 1 : return "WEIGHT_FACTOR_NEG";
				case 2 : return "SCALING_A_POS";
				case 3 : return "SCALING_B_POS";
				case 4 : return "SCALING_C_POS";
				case 5 : return "SCALING_A_NEG";
				case 6 : return "SCALING_B_NEG";
				case 7 : return "SCALING_C_NEG";
			} break; 
			// VaporContactResistanceModel::Functions 
			case 33 :
			switch (t) { 
				case 0 : return "sd+(rh)";
				case 1 : return "sd-(rh)";
			} break; 
		} // switch
		return INVALID_KEYWORD_INDEX_STRING;
	}

	/*! Returns all keywords including deprecated for a given category (typenum) and type number (t). */ 
	const char * allKeywords(int typenum, int t) {
		switch (typenum) {
			// AirTransport::Functions 
			case 0 :
			switch (t) { 
				case 0 : return "Kg(Theta_l) Kg(Ol)";
			} break; 
			// Foils::Ident 
			case 1 :
			switch (t) { 
				case 0 : return "NAME";
				case 1 : return "ID_PLUS";
				case 2 : return "ID_MINUS";
				case 3 : return "COMMENT";
			} break; 
			// Foils::Param 
			case 2 :
			switch (t) { 
				case 0 : return "SD_VALUE";
				case 1 : return "WEIGHT_FACTOR_POS";
				case 2 : return "WEIGHT_FACTOR_NEG";
				case 3 : return "SCALING_A_POS";
				case 4 : return "SCALING_B_POS";
				case 5 : return "SCALING_C_POS";
				case 6 : return "SCALING_A_NEG";
				case 7 : return "SCALING_B_NEG";
				case 8 : return "SCALING_C_NEG";
			} break; 
			// Foils::Functions 
			case 3 :
			switch (t) { 
				case 0 : return "sd+(rh)";
				case 1 : return "sd-(rh)";
			} break; 
			// MaterialIdentification::Ident 
			case 4 :
			switch (t) { 
				case 0 : return "NAME";
				case 1 : return "PRODUCER";
				case 2 : return "PRODUCT_ID";
				case 3 : return "INVESTIGATOR";
				case 4 : return "LABORATORY SOURCE";
				case 5 : return "DATE";
				case 6 : return "SAMPLING_DATE";
				case 7 : return "PRODUCTION_DATE";
				case 8 : return "SAMPLE_ID";
				case 9 : return "SAMPLING_METHOD";
				case 10 : return "COUNTRY";
				case 11 : return "COLOUR COLOR";
				case 12 : return "FLAGS";
				case 13 : return "USE_INSTEAD";
				case 14 : return "COPYRIGHT";
				case 15 : return "CATEGORY";
				case 16 : return "COMMENTS";
				case 17 : return "U_DIRECTION";
				case 18 : return "V_DIRECTION";
				case 19 : return "W_DIRECTION";
				case 20 : return "DBTYPE";
				case 21 : return "SIGNATURE_SOURCE";
				case 22 : return "QUALITY_FLAGS";
				case 23 : return "DATA_SHEET_SOURCE";
				case 24 : return "HATCHING";
				case 25 : return "AQUISITION_ID";
			} break; 
			// MaterialIdentification::Flags 
			case 5 :
			switch (t) { 
				case 1 : return "AIR";
				case 2 : return "AIR_TIGHT";
				case 4 : return "VAPOR_TIGHT";
				case 8 : return "WATER_TIGHT";
				case 16 : return "WOOD";
				case 32 : return "WOOD_BASED";
				case 64 : return "FOILS";
			} break; 
			// MaterialIdentification::SamplingMethods 
			case 6 :
			switch (t) { 
				case 1 : return "S_CAST_CUT_PRODUCER";
				case 2 : return "S_CAST_PRODUCER";
				case 3 : return "S_CAST_CUT_IBK";
				case 4 : return "S_CAST_IBK";
			} break; 
			// MaterialIdentification::FlagsPreV6 
			case 7 :
			switch (t) { 
				case 0 : return "AIR";
				case 1 : return "AIRTIGHT";
				case 2 : return "VAPORTIGHT";
				case 3 : return "WATERTIGHT";
				case 4 : return "WOOD";
				case 5 : return "WOODBASED";
				case 6 : return "ONLY_HYGROSCOPIC";
			} break; 
			// MaterialIdentification::HatchingType 
			case 8 :
			switch (t) { 
				case 0 : return "Ground";
				case 1 : return "Gravel";
				case 2 : return "Sand";
				case 3 : return "ConcreteNotReinforced";
				case 4 : return "ConcreteReinforced";
				case 5 : return "Masonry";
				case 6 : return "WoodTransverseToFibre";
				case 7 : return "WoodLongitudinal";
				case 8 : return "Metal";
				case 9 : return "MortarPlaster";
				case 10 : return "Insulation";
				case 11 : return "SealingFoil";
				case 12 : return "Sealant";
				case 13 : return "None";
			} break; 
			// Material::Sections 
			case 9 :
			switch (t) { 
				case 0 : return "IDENTIFICATION";
				case 1 : return "STORAGE_BASE_PARAMETERS";
				case 2 : return "TRANSPORT_BASE_PARAMETERS";
				case 3 : return "MECHANICAL_BASE_PARAMETERS";
				case 4 : return "THERMAL_STORAGE";
				case 5 : return "MOISTURE_STORAGE";
				case 6 : return "MOISTURE_TRANSPORT";
				case 7 : return "HEAT_TRANSPORT";
				case 8 : return "AIR_TRANSPORT";
				case 9 : return "MOISTURE_TRANSPORT_V";
				case 10 : return "HEAT_TRANSPORT_V";
				case 11 : return "AIR_TRANSPORT_V";
				case 12 : return "MOISTURE_TRANSPORT_W";
				case 13 : return "HEAT_TRANSPORT_W";
				case 14 : return "AIR_TRANSPORT_W";
				case 15 : return "MEASURED_DATA";
				case 16 : return "VAPOR_CONTACT_RESISTANCE";
			} break; 
			// Material::SectionsPreV6 
			case 10 :
			switch (t) { 
				case 0 : return "MATERIAL";
				case 1 : return "IDENTIFICATION";
				case 2 : return "GENERAL";
				case 3 : return "MRC";
				case 4 : return "MOISTTRANS";
				case 5 : return "HEATTRANS";
				case 6 : return "AIRTRANS";
				case 7 : return "PARAMETER";
				case 8 : return "PCMMATERIAL";
			} break; 
			// Material::ParaStorage 
			case 11 :
			switch (t) { 
				case 0 : return "RHO";
				case 1 : return "CE			CET";
				case 2 : return "THETA_POR	OPOR";
				case 3 : return "THETA_EFF	OEFF";
				case 4 : return "THETA_CAP	OCAP";
				case 5 : return "THETA_LIM_HYG";
				case 6 : return "THETA_80	O80";
			} break; 
			// Material::ParaTransport 
			case 12 :
			switch (t) { 
				case 0 : return "LAMBDA";
				case 1 : return "AW";
				case 2 : return "MEW";
				case 3 : return "SD";
				case 4 : return "KLEFF	KLSAT";
				case 5 : return "DLEFF";
				case 6 : return "KG		KGDRY";
				case 7 : return "LAMBDA_V";
				case 8 : return "AW_V";
				case 9 : return "MEW_V";
				case 10 : return "SD_V";
				case 11 : return "KLEFF_V";
				case 12 : return "DLEFF_V";
				case 13 : return "KG_V";
				case 14 : return "LAMBDA_W";
				case 15 : return "AW_W";
				case 16 : return "MEW_W";
				case 17 : return "SD_W";
				case 18 : return "KLEFF_W";
				case 19 : return "DLEFF_W";
				case 20 : return "KG_W";
				case 21 : return "LAMBDA_DESIGN";
			} break; 
			// Material::ParaMechanical 
			case 13 :
			switch (t) { 
				case 0 : return "ALPHA";
				case 1 : return "BETA_THETA_L";
				case 2 : return "BETA_THETA_L2";
				case 3 : return "ALPHA_V";
				case 4 : return "BETA_THETA_L_V";
				case 5 : return "BETA_THETA_L2_V";
				case 6 : return "ALPHA_W";
				case 7 : return "BETA_THETA_L_W";
				case 8 : return "BETA_THETA_L2_W";
			} break; 
			// MaterialCategory::CategoryID 
			case 14 :
			switch (t) { 
				case 0 : return "INVALID";
				case 1 : return "COATING";
				case 2 : return "PLASTER";
				case 3 : return "BRICK";
				case 4 : return "NATURAL_STONES";
				case 5 : return "CONCRETE";
				case 6 : return "INSULATION";
				case 7 : return "BUILDING_BOARDS";
				case 8 : return "TIMBER";
				case 9 : return "NATURAL_MATERIALS";
				case 10 : return "SOIL";
				case 11 : return "CLADDING";
				case 12 : return "FOILS";
				case 13 : return "MISC";
			} break; 
			// MeasuredData::Sections 
			case 15 :
			switch (t) { 
				case 0 : return "GEOMETRY";
				case 1 : return "EXPERIMENTS";
			} break; 
			// MeasuredData::WaterRetentionValues 
			case 16 :
			switch (t) { 
				case 0 : return "NON_LOG_PC";
				case 1 : return "TEMPERATURE";
				case 2 : return "THETA_L";
				case 3 : return "STD_DEVIATION";
				case 4 : return "MINIMUM";
				case 5 : return "MAXIMUM";
			} break; 
			// MeasuredData::WaterVapourPermeabilityValues 
			case 17 :
			switch (t) { 
				case 0 : return "PHI_INSIDE";
				case 1 : return "PHI_OUTSIDE";
				case 2 : return "MEW";
				case 3 : return "STD_DEVIATION";
				case 4 : return "MINIMUM";
				case 5 : return "MAXIMUM";
			} break; 
			// MeasuredData::LiquidWaterConductivityValues 
			case 18 :
			switch (t) { 
				case 0 : return "STD_DEVIATION";
				case 1 : return "MINIMUM";
				case 2 : return "MAXIMUM";
			} break; 
			// MeasuredData::WaterUptakeValues 
			case 19 :
			switch (t) { 
				case 0 : return "MASS";
			} break; 
			// MeasuredData::DryingValues0 
			case 20 :
			switch (t) { 
				case 0 : return "WEIGHT";
				case 1 : return "WIDTH_0";
				case 2 : return "LENGTH_0";
				case 3 : return "HEIGHT_0";
				case 4 : return "WIDTH_1";
				case 5 : return "LENGTH_1";
				case 6 : return "HEIGHT_1";
			} break; 
			// MeasuredData::DryingValues1 
			case 21 :
			switch (t) { 
				case 0 : return "BC_TEMPERATURE_0";
				case 1 : return "BC_RH_0";
				case 2 : return "BC_TEMPERATURE_1";
				case 3 : return "BC_RH_1";
				case 4 : return "SAMPLE_TEMPERATURE_0";
				case 5 : return "SAMPLE_TEMPERATURE_1";
				case 6 : return "SAMPLE_TEMPERATURE_2";
				case 7 : return "SAMPLE_TEMPERATURE_3";
				case 8 : return "SAMPLE_TEMPERATURE_4";
				case 9 : return "SAMPLE_TEMPERATURE_5";
			} break; 
			// MeasuredData::GeometryKeys 
			case 22 :
			switch (t) { 
				case 0 : return "SPECIMEN";
				case 1 : return "CYLINDRIC";
				case 2 : return "WIDTH_0";
				case 3 : return "WIDTH_1";
				case 4 : return "WIDTH_2";
				case 5 : return "WIDTH_3";
				case 6 : return "LENGTH_0";
				case 7 : return "LENGTH_1";
				case 8 : return "LENGTH_2";
				case 9 : return "LENGTH_3";
				case 10 : return "HEIGHT_0";
				case 11 : return "HEIGHT_1";
				case 12 : return "HEIGHT_2";
				case 13 : return "HEIGHT_3";
			} break; 
			// MeasuredData::ExperimentKeys 
			case 23 :
			switch (t) { 
				case 0 : return "SPECIMEN";
				case 1 : return "EXPERIMENT";
				case 2 : return "X_VALUES";
				case 3 : return "NON_LOG_PC";
				case 4 : return "TEMPERATURE";
				case 5 : return "THETA_L";
				case 6 : return "PHI_INSIDE";
				case 7 : return "PHI_OUTSIDE";
				case 8 : return "MEW";
				case 9 : return "MINIMUM";
				case 10 : return "MAXIMUM";
				case 11 : return "STD_DEVIATION";
				case 12 : return "WEIGHT";
				case 13 : return "WIDTH_0";
				case 14 : return "LENGTH_0";
				case 15 : return "HEIGHT_0";
				case 16 : return "WIDTH_1";
				case 17 : return "LENGTH_1";
				case 18 : return "HEIGHT_1";
				case 19 : return "DATE_BC";
				case 20 : return "BC_TEMPERATURE_0";
				case 21 : return "BC_RH_0";
				case 22 : return "BC_TEMPERATURE_1";
				case 23 : return "BC_RH_1";
				case 24 : return "SAMPLE_TEMPERATURE_0";
				case 25 : return "SAMPLE_TEMPERATURE_1";
				case 26 : return "SAMPLE_TEMPERATURE_2";
				case 27 : return "SAMPLE_TEMPERATURE_3";
				case 28 : return "SAMPLE_TEMPERATURE_4";
				case 29 : return "SAMPLE_TEMPERATURE_5";
			} break; 
			// MeasuredData::Experiments 
			case 24 :
			switch (t) { 
				case 1 : return "WET_CUP";
				case 2 : return "MED_CUP";
				case 4 : return "DRY_CUP";
				case 8 : return "WATER_RETENTION";
				case 16 : return "SORPTION_ISOTHERM";
				case 32 : return "WATER_UPTAKE";
				case 64 : return "DRYING_0";
				case 128 : return "DRYING_1";
			} break; 
			// ModelHandler::ModelIdentifier 
			case 25 :
			switch (t) { 
				case 0 : return "GAUSS";
				case 1 : return "SERIAL_PARALLEL";
			} break; 
			// MoistureStorage::Functions 
			case 26 :
			switch (t) { 
				case 0 : return "Theta_l(pC)_de  Ol(pC)";
				case 1 : return "pC(Theta_l)_de  pC(Ol)";
				case 2 : return "Theta_l(RH)  Theta_l(RH)_de Ol(RH)";
				case 3 : return "Theta_l(pC)_ad";
				case 4 : return "pC(Theta_l)_ad";
				case 5 : return "mwu(T)";
			} break; 
			// MoistureStorage::ModelTypes 
			case 27 :
			switch (t) { 
				case 0 : return "GAUSS";
				case 1 : return "VAN_GENUCHTEN";
			} break; 
			// MoistureTransport::Functions 
			case 28 :
			switch (t) { 
				case 0 : return "lgKl(Theta_l) lgKl(Ol)";
				case 1 : return "lgDl(Theta_l) lgDl(Ol)";
				case 2 : return "lgKv(Theta_l) lgKv(Ol)";
				case 3 : return "mew(RH) My(Phi)";
			} break; 
			// MoistureTransport::ModelTypes 
			case 29 :
			switch (t) { 
				case 0 : return "SERIAL_PARALLEL SP_MODEL";
			} break; 
			// ThermalStorage::Functions 
			case 30 :
			switch (t) { 
				case 0 : return "u(T)";
			} break; 
			// ThermalTransport::Functions 
			case 31 :
			switch (t) { 
				case 0 : return "lambda(Theta_l) lambda(Ol)";
				case 1 : return "lambda(T)";
			} break; 
			// VaporContactResistanceModel::Param 
			case 32 :
			switch (t) { 
				case 0 : return "WEIGHT_FACTOR_POS";
				case 1 : return "WEIGHT_FACTOR_NEG";
				case 2 : return "SCALING_A_POS";
				case 3 : return "SCALING_B_POS";
				case 4 : return "SCALING_C_POS";
				case 5 : return "SCALING_A_NEG";
				case 6 : return "SCALING_B_NEG";
				case 7 : return "SCALING_C_NEG";
			} break; 
			// VaporContactResistanceModel::Functions 
			case 33 :
			switch (t) { 
				case 0 : return "sd+(rh)";
				case 1 : return "sd-(rh)";
			} break; 
		} // switch
		return INVALID_KEYWORD_INDEX_STRING;
	}

	const char * KeywordList::Description(const char * const enumtype, int t, bool * no_description) {
		if (no_description != NULL)
			*no_description = false; // we are optimistic
		switch (enum2index(enumtype)) {
			// AirTransport::Functions 
			case 0 :
			switch (t) { 
				case 0 : if (no_description != NULL) *no_description = true; return "Kg(Theta_l) Kg(Ol)";
			} break; 
			// Foils::Ident 
			case 1 :
			switch (t) { 
				case 0 : return "Foil name";
				case 1 : return "Material ID for sd-value and/or sd+(rh)";
				case 2 : return "Material ID for sd-(rh)";
				case 3 : return "Some comments and explanations.";
			} break; 
			// Foils::Param 
			case 2 :
			switch (t) { 
				case 0 : return "sd-value";
				case 1 : return "Weighting factor for adaptive foil model positive direction";
				case 2 : return "Weighting factor for adaptive foil model negative direction";
				case 3 : return "Parameter a for square scaling function for adaptive foil model positive direction";
				case 4 : return "Parameter b for square scaling function for adaptive foil model positive direction";
				case 5 : return "Parameter c for square scaling function for adaptive foil model positive direction";
				case 6 : return "Parameter a for square scaling function for adaptive foil model negative direction";
				case 7 : return "Parameter b for square scaling function for adaptive foil model negative direction";
				case 8 : return "Parameter c for square scaling function for adaptive foil model negative direction";
			} break; 
			// Foils::Functions 
			case 3 :
			switch (t) { 
				case 0 : if (no_description != NULL) *no_description = true; return "sd+(rh)";
				case 1 : if (no_description != NULL) *no_description = true; return "sd-(rh)";
			} break; 
			// MaterialIdentification::Ident 
			case 4 :
			switch (t) { 
				case 0 : if (no_description != NULL) *no_description = true; return "NAME";
				case 1 : if (no_description != NULL) *no_description = true; return "PRODUCER";
				case 2 : if (no_description != NULL) *no_description = true; return "PRODUCT_ID";
				case 3 : if (no_description != NULL) *no_description = true; return "INVESTIGATOR";
				case 4 : if (no_description != NULL) *no_description = true; return "LABORATORY SOURCE";
				case 5 : if (no_description != NULL) *no_description = true; return "DATE";
				case 6 : if (no_description != NULL) *no_description = true; return "SAMPLING_DATE";
				case 7 : if (no_description != NULL) *no_description = true; return "PRODUCTION_DATE";
				case 8 : if (no_description != NULL) *no_description = true; return "SAMPLE_ID";
				case 9 : if (no_description != NULL) *no_description = true; return "SAMPLING_METHOD";
				case 10 : if (no_description != NULL) *no_description = true; return "COUNTRY";
				case 11 : if (no_description != NULL) *no_description = true; return "COLOUR COLOR";
				case 12 : if (no_description != NULL) *no_description = true; return "FLAGS";
				case 13 : if (no_description != NULL) *no_description = true; return "USE_INSTEAD";
				case 14 : if (no_description != NULL) *no_description = true; return "COPYRIGHT";
				case 15 : if (no_description != NULL) *no_description = true; return "CATEGORY";
				case 16 : if (no_description != NULL) *no_description = true; return "COMMENTS";
				case 17 : if (no_description != NULL) *no_description = true; return "U_DIRECTION";
				case 18 : if (no_description != NULL) *no_description = true; return "V_DIRECTION";
				case 19 : if (no_description != NULL) *no_description = true; return "W_DIRECTION";
				case 20 : if (no_description != NULL) *no_description = true; return "DBTYPE";
				case 21 : if (no_description != NULL) *no_description = true; return "SIGNATURE_SOURCE";
				case 22 : if (no_description != NULL) *no_description = true; return "QUALITY_FLAGS";
				case 23 : if (no_description != NULL) *no_description = true; return "DATA_SHEET_SOURCE";
				case 24 : if (no_description != NULL) *no_description = true; return "HATCHING";
				case 25 : if (no_description != NULL) *no_description = true; return "AQUISITION_ID";
			} break; 
			// MaterialIdentification::Flags 
			case 5 :
			switch (t) { 
				case 1 : return "Material is air.";
				case 2 : return "No gas transport through the material possible.";
				case 4 : return "No water vapor transport through the material.";
				case 8 : return "No liquid water transport through the materials.";
				case 16 : return "Material is wood (timber)";
				case 32 : return "Material is based on wood (e.g. wood fiber board)";
				case 64 : return "Material is a foil or vapor retarder. See in comments for additional informations.";
			} break; 
			// MaterialIdentification::SamplingMethods 
			case 6 :
			switch (t) { 
				case 1 : return "casted and cutted by producer";
				case 2 : return "casted by producer";
				case 3 : return "casted and cutted by IBK";
				case 4 : return "casted by IBK";
			} break; 
			// MaterialIdentification::FlagsPreV6 
			case 7 :
			switch (t) { 
				case 0 : if (no_description != NULL) *no_description = true; return "AIR";
				case 1 : if (no_description != NULL) *no_description = true; return "AIRTIGHT";
				case 2 : if (no_description != NULL) *no_description = true; return "VAPORTIGHT";
				case 3 : if (no_description != NULL) *no_description = true; return "WATERTIGHT";
				case 4 : if (no_description != NULL) *no_description = true; return "WOOD";
				case 5 : if (no_description != NULL) *no_description = true; return "WOODBASED";
				case 6 : if (no_description != NULL) *no_description = true; return "ONLY_HYGROSCOPIC";
			} break; 
			// MaterialIdentification::HatchingType 
			case 8 :
			switch (t) { 
				case 0 : return "code according DIN ISO 128-50 no, DIN 1356-1 8.1";
				case 1 : return "code according DIN ISO 128-50 SN4";
				case 2 : return "code according DIN ISO 128-50 SN5";
				case 3 : return "code according DIN ISO 128-50 SN47";
				case 4 : return "code according DIN ISO 128-50 SN46";
				case 5 : return "code according DIN ISO 128-50 SN41";
				case 6 : return "code according DIN ISO 128-50 SN21";
				case 7 : return "code according DIN ISO 128-50 SN22";
				case 8 : return "code according DIN ISO 128-50 SM";
				case 9 : return "code according DIN ISO 128-50 no, DIN 1356-1 8.10";
				case 10 : return "code according DIN ISO 128-50 SN55";
				case 11 : return "code according DIN ISO 128-50 SN59";
				case 12 : return "code according DIN ISO 128-50 SN24";
				case 13 : if (no_description != NULL) *no_description = true; return "None";
			} break; 
			// Material::Sections 
			case 9 :
			switch (t) { 
				case 0 : if (no_description != NULL) *no_description = true; return "IDENTIFICATION";
				case 1 : if (no_description != NULL) *no_description = true; return "STORAGE_BASE_PARAMETERS";
				case 2 : if (no_description != NULL) *no_description = true; return "TRANSPORT_BASE_PARAMETERS";
				case 3 : if (no_description != NULL) *no_description = true; return "MECHANICAL_BASE_PARAMETERS";
				case 4 : if (no_description != NULL) *no_description = true; return "THERMAL_STORAGE";
				case 5 : if (no_description != NULL) *no_description = true; return "MOISTURE_STORAGE";
				case 6 : if (no_description != NULL) *no_description = true; return "MOISTURE_TRANSPORT";
				case 7 : if (no_description != NULL) *no_description = true; return "HEAT_TRANSPORT";
				case 8 : if (no_description != NULL) *no_description = true; return "AIR_TRANSPORT";
				case 9 : if (no_description != NULL) *no_description = true; return "MOISTURE_TRANSPORT_V";
				case 10 : if (no_description != NULL) *no_description = true; return "HEAT_TRANSPORT_V";
				case 11 : if (no_description != NULL) *no_description = true; return "AIR_TRANSPORT_V";
				case 12 : if (no_description != NULL) *no_description = true; return "MOISTURE_TRANSPORT_W";
				case 13 : if (no_description != NULL) *no_description = true; return "HEAT_TRANSPORT_W";
				case 14 : if (no_description != NULL) *no_description = true; return "AIR_TRANSPORT_W";
				case 15 : if (no_description != NULL) *no_description = true; return "MEASURED_DATA";
				case 16 : if (no_description != NULL) *no_description = true; return "VAPOR_CONTACT_RESISTANCE";
			} break; 
			// Material::SectionsPreV6 
			case 10 :
			switch (t) { 
				case 0 : if (no_description != NULL) *no_description = true; return "MATERIAL";
				case 1 : if (no_description != NULL) *no_description = true; return "IDENTIFICATION";
				case 2 : if (no_description != NULL) *no_description = true; return "GENERAL";
				case 3 : if (no_description != NULL) *no_description = true; return "MRC";
				case 4 : if (no_description != NULL) *no_description = true; return "MOISTTRANS";
				case 5 : if (no_description != NULL) *no_description = true; return "HEATTRANS";
				case 6 : if (no_description != NULL) *no_description = true; return "AIRTRANS";
				case 7 : if (no_description != NULL) *no_description = true; return "PARAMETER";
				case 8 : if (no_description != NULL) *no_description = true; return "PCMMATERIAL";
			} break; 
			// Material::ParaStorage 
			case 11 :
			switch (t) { 
				case 0 : return "Bulk density of dry material";
				case 1 : return "Specific heat capacity of dry material";
				case 2 : return "Open porosity";
				case 3 : return "Effective saturation (long term process)";
				case 4 : return "Capillary saturation content (short term process)";
				case 5 : return "Maxiumum moisture content allowed (material must not get wet)";
				case 6 : return "Hygroscopic sorption value at RH=80%";
			} break; 
			// Material::ParaTransport 
			case 12 :
			switch (t) { 
				case 0 : return "Thermal conductivity (dry)";
				case 1 : return "Water uptake coefficient";
				case 2 : return "Water vapor diffusion resistance factor";
				case 3 : return "Diffusiun equivalent air layer thickness";
				case 4 : return "Liquid water conductivity at effective saturation";
				case 5 : return "Liquid water diffusivity at effective saturation";
				case 6 : return "Air permeability";
				case 7 : return "Thermal conductivity (dry) in material direction v";
				case 8 : return "Water uptake coefficient in material direction v";
				case 9 : return "Water vapor diffusion resistance factor in material direction v";
				case 10 : return "Diffusiun equivalent air layer thickness in material direction v";
				case 11 : return "Liquid water conductivity at effective saturation in material direction v";
				case 12 : return "Liquid water diffusivity at effective saturation in material direction v";
				case 13 : return "Air permeability in material direction v";
				case 14 : return "Thermal conductivity (dry) in material direction w";
				case 15 : return "Water uptake coefficient in material direction w";
				case 16 : return "Water vapor diffusion resistance factor in material direction w";
				case 17 : return "Diffusiun equivalent air layer thickness in material direction w";
				case 18 : return "Liquid water conductivity at effective saturation in material direction w";
				case 19 : return "Liquid water diffusivity at effective saturation in material direction w";
				case 20 : return "Air permeability in material direction w";
				case 21 : return "Design value of thermal conductivity";
			} break; 
			// Material::ParaMechanical 
			case 13 :
			switch (t) { 
				case 0 : return "Thermal expansion coefficient";
				case 1 : return "Hygric expansion coefficient (volumetric moisture content)";
				case 2 : return "Fiber saturation point (wood)";
				case 3 : return "Thermal expansion coefficient in material direction v";
				case 4 : return "Hygric expansion coefficient (volumetric moisture content) in material direction v";
				case 5 : return "Fiber saturation point (wood) in material direction v";
				case 6 : return "Thermal expansion coefficient in material direction w";
				case 7 : return "Hygric expansion coefficient (volumetric moisture content) in material direction w";
				case 8 : return "Fiber saturation point (wood) in material direction w";
			} break; 
			// MaterialCategory::CategoryID 
			case 14 :
			switch (t) { 
				case 0 : return "Invalid Category";
				case 1 : return "Coating";
				case 2 : return "Plaster and mortar";
				case 3 : return "Building brick";
				case 4 : return "Natural stones";
				case 5 : return "Concrete containing building materials";
				case 6 : return "Insulation materials";
				case 7 : return "Building boards";
				case 8 : return "Timber";
				case 9 : return "Natural materials";
				case 10 : return "Soil";
				case 11 : return "Cladding panels and ceramic tiles";
				case 12 : return "Foils and waterproofing products";
				case 13 : return "Miscellaneous";
			} break; 
			// MeasuredData::Sections 
			case 15 :
			switch (t) { 
				case 0 : if (no_description != NULL) *no_description = true; return "GEOMETRY";
				case 1 : if (no_description != NULL) *no_description = true; return "EXPERIMENTS";
			} break; 
			// MeasuredData::WaterRetentionValues 
			case 16 :
			switch (t) { 
				case 0 : if (no_description != NULL) *no_description = true; return "NON_LOG_PC";
				case 1 : if (no_description != NULL) *no_description = true; return "TEMPERATURE";
				case 2 : if (no_description != NULL) *no_description = true; return "THETA_L";
				case 3 : if (no_description != NULL) *no_description = true; return "STD_DEVIATION";
				case 4 : if (no_description != NULL) *no_description = true; return "MINIMUM";
				case 5 : if (no_description != NULL) *no_description = true; return "MAXIMUM";
			} break; 
			// MeasuredData::WaterVapourPermeabilityValues 
			case 17 :
			switch (t) { 
				case 0 : if (no_description != NULL) *no_description = true; return "PHI_INSIDE";
				case 1 : if (no_description != NULL) *no_description = true; return "PHI_OUTSIDE";
				case 2 : if (no_description != NULL) *no_description = true; return "MEW";
				case 3 : if (no_description != NULL) *no_description = true; return "STD_DEVIATION";
				case 4 : if (no_description != NULL) *no_description = true; return "MINIMUM";
				case 5 : if (no_description != NULL) *no_description = true; return "MAXIMUM";
			} break; 
			// MeasuredData::LiquidWaterConductivityValues 
			case 18 :
			switch (t) { 
				case 0 : if (no_description != NULL) *no_description = true; return "STD_DEVIATION";
				case 1 : if (no_description != NULL) *no_description = true; return "MINIMUM";
				case 2 : if (no_description != NULL) *no_description = true; return "MAXIMUM";
			} break; 
			// MeasuredData::WaterUptakeValues 
			case 19 :
			switch (t) { 
				case 0 : return "Mass";
			} break; 
			// MeasuredData::DryingValues0 
			case 20 :
			switch (t) { 
				case 0 : if (no_description != NULL) *no_description = true; return "WEIGHT";
				case 1 : if (no_description != NULL) *no_description = true; return "WIDTH_0";
				case 2 : if (no_description != NULL) *no_description = true; return "LENGTH_0";
				case 3 : if (no_description != NULL) *no_description = true; return "HEIGHT_0";
				case 4 : if (no_description != NULL) *no_description = true; return "WIDTH_1";
				case 5 : if (no_description != NULL) *no_description = true; return "LENGTH_1";
				case 6 : if (no_description != NULL) *no_description = true; return "HEIGHT_1";
			} break; 
			// MeasuredData::DryingValues1 
			case 21 :
			switch (t) { 
				case 0 : return "Boundary Temperature";
				case 1 : return "Boundary Relative Humidity";
				case 2 : return "Boundary Temperature";
				case 3 : return "Boundary  Relative Humidity";
				case 4 : return "Sample Temperature";
				case 5 : return "Sample Temperature";
				case 6 : return "Sample Temperature";
				case 7 : return "Sample Temperature";
				case 8 : return "Sample Temperature";
				case 9 : return "Sample Temperature";
			} break; 
			// MeasuredData::GeometryKeys 
			case 22 :
			switch (t) { 
				case 0 : if (no_description != NULL) *no_description = true; return "SPECIMEN";
				case 1 : if (no_description != NULL) *no_description = true; return "CYLINDRIC";
				case 2 : if (no_description != NULL) *no_description = true; return "WIDTH_0";
				case 3 : if (no_description != NULL) *no_description = true; return "WIDTH_1";
				case 4 : if (no_description != NULL) *no_description = true; return "WIDTH_2";
				case 5 : if (no_description != NULL) *no_description = true; return "WIDTH_3";
				case 6 : if (no_description != NULL) *no_description = true; return "LENGTH_0";
				case 7 : if (no_description != NULL) *no_description = true; return "LENGTH_1";
				case 8 : if (no_description != NULL) *no_description = true; return "LENGTH_2";
				case 9 : if (no_description != NULL) *no_description = true; return "LENGTH_3";
				case 10 : if (no_description != NULL) *no_description = true; return "HEIGHT_0";
				case 11 : if (no_description != NULL) *no_description = true; return "HEIGHT_1";
				case 12 : if (no_description != NULL) *no_description = true; return "HEIGHT_2";
				case 13 : if (no_description != NULL) *no_description = true; return "HEIGHT_3";
			} break; 
			// MeasuredData::ExperimentKeys 
			case 23 :
			switch (t) { 
				case 0 : if (no_description != NULL) *no_description = true; return "SPECIMEN";
				case 1 : if (no_description != NULL) *no_description = true; return "EXPERIMENT";
				case 2 : if (no_description != NULL) *no_description = true; return "X_VALUES";
				case 3 : if (no_description != NULL) *no_description = true; return "NON_LOG_PC";
				case 4 : if (no_description != NULL) *no_description = true; return "TEMPERATURE";
				case 5 : if (no_description != NULL) *no_description = true; return "THETA_L";
				case 6 : if (no_description != NULL) *no_description = true; return "PHI_INSIDE";
				case 7 : if (no_description != NULL) *no_description = true; return "PHI_OUTSIDE";
				case 8 : if (no_description != NULL) *no_description = true; return "MEW";
				case 9 : if (no_description != NULL) *no_description = true; return "MINIMUM";
				case 10 : if (no_description != NULL) *no_description = true; return "MAXIMUM";
				case 11 : if (no_description != NULL) *no_description = true; return "STD_DEVIATION";
				case 12 : if (no_description != NULL) *no_description = true; return "WEIGHT";
				case 13 : if (no_description != NULL) *no_description = true; return "WIDTH_0";
				case 14 : if (no_description != NULL) *no_description = true; return "LENGTH_0";
				case 15 : if (no_description != NULL) *no_description = true; return "HEIGHT_0";
				case 16 : if (no_description != NULL) *no_description = true; return "WIDTH_1";
				case 17 : if (no_description != NULL) *no_description = true; return "LENGTH_1";
				case 18 : if (no_description != NULL) *no_description = true; return "HEIGHT_1";
				case 19 : if (no_description != NULL) *no_description = true; return "DATE_BC";
				case 20 : if (no_description != NULL) *no_description = true; return "BC_TEMPERATURE_0";
				case 21 : if (no_description != NULL) *no_description = true; return "BC_RH_0";
				case 22 : if (no_description != NULL) *no_description = true; return "BC_TEMPERATURE_1";
				case 23 : if (no_description != NULL) *no_description = true; return "BC_RH_1";
				case 24 : if (no_description != NULL) *no_description = true; return "SAMPLE_TEMPERATURE_0";
				case 25 : if (no_description != NULL) *no_description = true; return "SAMPLE_TEMPERATURE_1";
				case 26 : if (no_description != NULL) *no_description = true; return "SAMPLE_TEMPERATURE_2";
				case 27 : if (no_description != NULL) *no_description = true; return "SAMPLE_TEMPERATURE_3";
				case 28 : if (no_description != NULL) *no_description = true; return "SAMPLE_TEMPERATURE_4";
				case 29 : if (no_description != NULL) *no_description = true; return "SAMPLE_TEMPERATURE_5";
			} break; 
			// MeasuredData::Experiments 
			case 24 :
			switch (t) { 
				case 1 : return "K_l(Thetal)";
				case 2 : return "K_l(Thetal)";
				case 4 : return "K_l(Thetal)";
				case 8 : if (no_description != NULL) *no_description = true; return "WATER_RETENTION";
				case 16 : if (no_description != NULL) *no_description = true; return "SORPTION_ISOTHERM";
				case 32 : if (no_description != NULL) *no_description = true; return "WATER_UPTAKE";
				case 64 : if (no_description != NULL) *no_description = true; return "DRYING_0";
				case 128 : if (no_description != NULL) *no_description = true; return "DRYING_1";
			} break; 
			// ModelHandler::ModelIdentifier 
			case 25 :
			switch (t) { 
				case 0 : if (no_description != NULL) *no_description = true; return "GAUSS";
				case 1 : if (no_description != NULL) *no_description = true; return "SERIAL_PARALLEL";
			} break; 
			// MoistureStorage::Functions 
			case 26 :
			switch (t) { 
				case 0 : if (no_description != NULL) *no_description = true; return "Theta_l(pC)_de  Ol(pC)";
				case 1 : if (no_description != NULL) *no_description = true; return "pC(Theta_l)_de  pC(Ol)";
				case 2 : if (no_description != NULL) *no_description = true; return "Theta_l(RH)  Theta_l(RH)_de Ol(RH)";
				case 3 : if (no_description != NULL) *no_description = true; return "Theta_l(pC)_ad";
				case 4 : if (no_description != NULL) *no_description = true; return "pC(Theta_l)_ad";
				case 5 : if (no_description != NULL) *no_description = true; return "mwu(T)";
			} break; 
			// MoistureStorage::ModelTypes 
			case 27 :
			switch (t) { 
				case 0 : return "Gauss MRC-Model";
				case 1 : return "van Genuchten MRC-Model";
			} break; 
			// MoistureTransport::Functions 
			case 28 :
			switch (t) { 
				case 0 : if (no_description != NULL) *no_description = true; return "lgKl(Theta_l) lgKl(Ol)";
				case 1 : if (no_description != NULL) *no_description = true; return "lgDl(Theta_l) lgDl(Ol)";
				case 2 : if (no_description != NULL) *no_description = true; return "lgKv(Theta_l) lgKv(Ol)";
				case 3 : if (no_description != NULL) *no_description = true; return "mew(RH) My(Phi)";
			} break; 
			// MoistureTransport::ModelTypes 
			case 29 :
			switch (t) { 
				case 0 : return "Serial Parallel vapor transport model";
			} break; 
			// ThermalStorage::Functions 
			case 30 :
			switch (t) { 
				case 0 : if (no_description != NULL) *no_description = true; return "u(T)";
			} break; 
			// ThermalTransport::Functions 
			case 31 :
			switch (t) { 
				case 0 : if (no_description != NULL) *no_description = true; return "lambda(Theta_l) lambda(Ol)";
				case 1 : if (no_description != NULL) *no_description = true; return "lambda(T)";
			} break; 
			// VaporContactResistanceModel::Param 
			case 32 :
			switch (t) { 
				case 0 : return "Weighting factor for adaptive foil model positive direction";
				case 1 : return "Weighting factor for adaptive foil model negative direction";
				case 2 : return "Parameter a for square scaling function for adaptive foil model positive direction";
				case 3 : return "Parameter b for square scaling function for adaptive foil model positive direction";
				case 4 : return "Parameter c for square scaling function for adaptive foil model positive direction";
				case 5 : return "Parameter a for square scaling function for adaptive foil model negative direction";
				case 6 : return "Parameter b for square scaling function for adaptive foil model negative direction";
				case 7 : return "Parameter c for square scaling function for adaptive foil model negative direction";
			} break; 
			// VaporContactResistanceModel::Functions 
			case 33 :
			switch (t) { 
				case 0 : if (no_description != NULL) *no_description = true; return "sd+(rh)";
				case 1 : if (no_description != NULL) *no_description = true; return "sd-(rh)";
			} break; 
		} // switch
		throw IBK::Exception(IBK::FormatString("Cannot determine description for enumeration type '%1' and index '%2'.")
			.arg(enumtype).arg(t), "[KeywordList::Description]");
	}

	const char * KeywordList::Unit(const char * const enumtype, int t) {
		switch (enum2index(enumtype)) {
			// AirTransport::Functions 
			case 0 :
			switch (t) { 
				case 0 : return "";
			} break; 
			// Foils::Ident 
			case 1 :
			switch (t) { 
				case 0 : return "";
				case 1 : return "";
				case 2 : return "";
				case 3 : return "";
			} break; 
			// Foils::Param 
			case 2 :
			switch (t) { 
				case 0 : return "m";
				case 1 : return "-";
				case 2 : return "-";
				case 3 : return "-";
				case 4 : return "-";
				case 5 : return "-";
				case 6 : return "-";
				case 7 : return "-";
				case 8 : return "-";
			} break; 
			// Foils::Functions 
			case 3 :
			switch (t) { 
				case 0 : return "";
				case 1 : return "";
			} break; 
			// MaterialIdentification::Ident 
			case 4 :
			switch (t) { 
				case 0 : return "";
				case 1 : return "";
				case 2 : return "";
				case 3 : return "";
				case 4 : return "";
				case 5 : return "";
				case 6 : return "";
				case 7 : return "";
				case 8 : return "";
				case 9 : return "";
				case 10 : return "";
				case 11 : return "";
				case 12 : return "";
				case 13 : return "";
				case 14 : return "";
				case 15 : return "";
				case 16 : return "";
				case 17 : return "";
				case 18 : return "";
				case 19 : return "";
				case 20 : return "";
				case 21 : return "";
				case 22 : return "";
				case 23 : return "";
				case 24 : return "";
				case 25 : return "";
			} break; 
			// MaterialIdentification::Flags 
			case 5 :
			switch (t) { 
				case 1 : return "";
				case 2 : return "";
				case 4 : return "";
				case 8 : return "";
				case 16 : return "";
				case 32 : return "";
				case 64 : return "";
			} break; 
			// MaterialIdentification::SamplingMethods 
			case 6 :
			switch (t) { 
				case 1 : return "";
				case 2 : return "";
				case 3 : return "";
				case 4 : return "";
			} break; 
			// MaterialIdentification::FlagsPreV6 
			case 7 :
			switch (t) { 
				case 0 : return "";
				case 1 : return "";
				case 2 : return "";
				case 3 : return "";
				case 4 : return "";
				case 5 : return "";
				case 6 : return "";
			} break; 
			// MaterialIdentification::HatchingType 
			case 8 :
			switch (t) { 
				case 0 : return "";
				case 1 : return "";
				case 2 : return "";
				case 3 : return "";
				case 4 : return "";
				case 5 : return "";
				case 6 : return "";
				case 7 : return "";
				case 8 : return "";
				case 9 : return "";
				case 10 : return "";
				case 11 : return "";
				case 12 : return "";
				case 13 : return "";
			} break; 
			// Material::Sections 
			case 9 :
			switch (t) { 
				case 0 : return "";
				case 1 : return "";
				case 2 : return "";
				case 3 : return "";
				case 4 : return "";
				case 5 : return "";
				case 6 : return "";
				case 7 : return "";
				case 8 : return "";
				case 9 : return "";
				case 10 : return "";
				case 11 : return "";
				case 12 : return "";
				case 13 : return "";
				case 14 : return "";
				case 15 : return "";
				case 16 : return "";
			} break; 
			// Material::SectionsPreV6 
			case 10 :
			switch (t) { 
				case 0 : return "";
				case 1 : return "";
				case 2 : return "";
				case 3 : return "";
				case 4 : return "";
				case 5 : return "";
				case 6 : return "";
				case 7 : return "";
				case 8 : return "";
			} break; 
			// Material::ParaStorage 
			case 11 :
			switch (t) { 
				case 0 : return "kg/m3";
				case 1 : return "J/kgK";
				case 2 : return "m3/m3";
				case 3 : return "m3/m3";
				case 4 : return "m3/m3";
				case 5 : return "m3/m3";
				case 6 : return "m3/m3";
			} break; 
			// Material::ParaTransport 
			case 12 :
			switch (t) { 
				case 0 : return "W/mK";
				case 1 : return "kg/m2s05";
				case 2 : return "-";
				case 3 : return "m";
				case 4 : return "s";
				case 5 : return "m2/s";
				case 6 : return "s";
				case 7 : return "W/mK";
				case 8 : return "kg/m2s05";
				case 9 : return "-";
				case 10 : return "m";
				case 11 : return "s";
				case 12 : return "m2/s";
				case 13 : return "s";
				case 14 : return "W/mK";
				case 15 : return "kg/m2s05";
				case 16 : return "-";
				case 17 : return "m";
				case 18 : return "s";
				case 19 : return "m2/s";
				case 20 : return "s";
				case 21 : return "W/mK";
			} break; 
			// Material::ParaMechanical 
			case 13 :
			switch (t) { 
				case 0 : return "mm/mK";
				case 1 : return "m3m/m3m";
				case 2 : return "m3/m3";
				case 3 : return "mm/mK";
				case 4 : return "m3m/m3m";
				case 5 : return "m3/m3";
				case 6 : return "mm/mK";
				case 7 : return "m3m/m3m";
				case 8 : return "m3/m3";
			} break; 
			// MaterialCategory::CategoryID 
			case 14 :
			switch (t) { 
				case 0 : return "";
				case 1 : return "";
				case 2 : return "";
				case 3 : return "";
				case 4 : return "";
				case 5 : return "";
				case 6 : return "";
				case 7 : return "";
				case 8 : return "";
				case 9 : return "";
				case 10 : return "";
				case 11 : return "";
				case 12 : return "";
				case 13 : return "";
			} break; 
			// MeasuredData::Sections 
			case 15 :
			switch (t) { 
				case 0 : return "";
				case 1 : return "";
			} break; 
			// MeasuredData::WaterRetentionValues 
			case 16 :
			switch (t) { 
				case 0 : return "Pa";
				case 1 : return "K";
				case 2 : return "m3/m3";
				case 3 : return "-";
				case 4 : return "m3/m3";
				case 5 : return "m3/m3";
			} break; 
			// MeasuredData::WaterVapourPermeabilityValues 
			case 17 :
			switch (t) { 
				case 0 : return "%";
				case 1 : return "%";
				case 2 : return "-";
				case 3 : return "-";
				case 4 : return "-";
				case 5 : return "-";
			} break; 
			// MeasuredData::LiquidWaterConductivityValues 
			case 18 :
			switch (t) { 
				case 0 : return "-";
				case 1 : return "s";
				case 2 : return "s";
			} break; 
			// MeasuredData::WaterUptakeValues 
			case 19 :
			switch (t) { 
				case 0 : return "Kg";
			} break; 
			// MeasuredData::DryingValues0 
			case 20 :
			switch (t) { 
				case 0 : return "m";
				case 1 : return "m";
				case 2 : return "m";
				case 3 : return "m";
				case 4 : return "m";
				case 5 : return "m";
				case 6 : return "m";
			} break; 
			// MeasuredData::DryingValues1 
			case 21 :
			switch (t) { 
				case 0 : return "K";
				case 1 : return "%";
				case 2 : return "K";
				case 3 : return "%";
				case 4 : return "K";
				case 5 : return "K";
				case 6 : return "K";
				case 7 : return "K";
				case 8 : return "K";
				case 9 : return "K";
			} break; 
			// MeasuredData::GeometryKeys 
			case 22 :
			switch (t) { 
				case 0 : return "";
				case 1 : return "";
				case 2 : return "";
				case 3 : return "";
				case 4 : return "";
				case 5 : return "";
				case 6 : return "";
				case 7 : return "";
				case 8 : return "";
				case 9 : return "";
				case 10 : return "";
				case 11 : return "";
				case 12 : return "";
				case 13 : return "";
			} break; 
			// MeasuredData::ExperimentKeys 
			case 23 :
			switch (t) { 
				case 0 : return "";
				case 1 : return "";
				case 2 : return "";
				case 3 : return "Pa";
				case 4 : return "K";
				case 5 : return "m3/m3";
				case 6 : return "%";
				case 7 : return "%";
				case 8 : return "-";
				case 9 : return "-";
				case 10 : return "-";
				case 11 : return "-";
				case 12 : return "m";
				case 13 : return "m";
				case 14 : return "m";
				case 15 : return "m";
				case 16 : return "m";
				case 17 : return "m";
				case 18 : return "m";
				case 19 : return "s";
				case 20 : return "K";
				case 21 : return "%";
				case 22 : return "K";
				case 23 : return "%";
				case 24 : return "K";
				case 25 : return "K";
				case 26 : return "K";
				case 27 : return "K";
				case 28 : return "K";
				case 29 : return "K";
			} break; 
			// MeasuredData::Experiments 
			case 24 :
			switch (t) { 
				case 1 : return "";
				case 2 : return "";
				case 4 : return "";
				case 8 : return "";
				case 16 : return "";
				case 32 : return "";
				case 64 : return "";
				case 128 : return "";
			} break; 
			// ModelHandler::ModelIdentifier 
			case 25 :
			switch (t) { 
				case 0 : return "";
				case 1 : return "";
			} break; 
			// MoistureStorage::Functions 
			case 26 :
			switch (t) { 
				case 0 : return "";
				case 1 : return "";
				case 2 : return "";
				case 3 : return "";
				case 4 : return "";
				case 5 : return "";
			} break; 
			// MoistureStorage::ModelTypes 
			case 27 :
			switch (t) { 
				case 0 : return "";
				case 1 : return "";
			} break; 
			// MoistureTransport::Functions 
			case 28 :
			switch (t) { 
				case 0 : return "";
				case 1 : return "";
				case 2 : return "";
				case 3 : return "";
			} break; 
			// MoistureTransport::ModelTypes 
			case 29 :
			switch (t) { 
				case 0 : return "";
			} break; 
			// ThermalStorage::Functions 
			case 30 :
			switch (t) { 
				case 0 : return "";
			} break; 
			// ThermalTransport::Functions 
			case 31 :
			switch (t) { 
				case 0 : return "";
				case 1 : return "";
			} break; 
			// VaporContactResistanceModel::Param 
			case 32 :
			switch (t) { 
				case 0 : return "-";
				case 1 : return "-";
				case 2 : return "-";
				case 3 : return "-";
				case 4 : return "-";
				case 5 : return "-";
				case 6 : return "-";
				case 7 : return "-";
			} break; 
			// VaporContactResistanceModel::Functions 
			case 33 :
			switch (t) { 
				case 0 : return "";
				case 1 : return "";
			} break; 
		} // switch
		throw IBK::Exception(IBK::FormatString("Cannot determine default unit for enumeration type '%1' and index '%2'.")
			.arg(enumtype).arg(t), "[KeywordList::Unit]");
	}

	const char * KeywordList::Color(const char * const enumtype, int t) {
		switch (enum2index(enumtype)) {
			// AirTransport::Functions 
			case 0 :
			switch (t) { 
				case 0 : return "#FFFFFF";
			} break; 
			// Foils::Ident 
			case 1 :
			switch (t) { 
				case 0 : return "#FFFFFF";
				case 1 : return "#FFFFFF";
				case 2 : return "#FFFFFF";
				case 3 : return "#FFFFFF";
			} break; 
			// Foils::Param 
			case 2 :
			switch (t) { 
				case 0 : return "#FFFFFF";
				case 1 : return "#FFFFFF";
				case 2 : return "#FFFFFF";
				case 3 : return "#FFFFFF";
				case 4 : return "#FFFFFF";
				case 5 : return "#FFFFFF";
				case 6 : return "#FFFFFF";
				case 7 : return "#FFFFFF";
				case 8 : return "#FFFFFF";
			} break; 
			// Foils::Functions 
			case 3 :
			switch (t) { 
				case 0 : return "#FFFFFF";
				case 1 : return "#FFFFFF";
			} break; 
			// MaterialIdentification::Ident 
			case 4 :
			switch (t) { 
				case 0 : return "#FFFFFF";
				case 1 : return "#FFFFFF";
				case 2 : return "#FFFFFF";
				case 3 : return "#FFFFFF";
				case 4 : return "#FFFFFF";
				case 5 : return "#FFFFFF";
				case 6 : return "#FFFFFF";
				case 7 : return "#FFFFFF";
				case 8 : return "#FFFFFF";
				case 9 : return "#FFFFFF";
				case 10 : return "#FFFFFF";
				case 11 : return "#FFFFFF";
				case 12 : return "#FFFFFF";
				case 13 : return "#FFFFFF";
				case 14 : return "#FFFFFF";
				case 15 : return "#FFFFFF";
				case 16 : return "#FFFFFF";
				case 17 : return "#FFFFFF";
				case 18 : return "#FFFFFF";
				case 19 : return "#FFFFFF";
				case 20 : return "#FFFFFF";
				case 21 : return "#FFFFFF";
				case 22 : return "#FFFFFF";
				case 23 : return "#FFFFFF";
				case 24 : return "#FFFFFF";
				case 25 : return "#FFFFFF";
			} break; 
			// MaterialIdentification::Flags 
			case 5 :
			switch (t) { 
				case 1 : return "#FFFFFF";
				case 2 : return "#FFFFFF";
				case 4 : return "#FFFFFF";
				case 8 : return "#FFFFFF";
				case 16 : return "#FFFFFF";
				case 32 : return "#FFFFFF";
				case 64 : return "#FFFFFF";
			} break; 
			// MaterialIdentification::SamplingMethods 
			case 6 :
			switch (t) { 
				case 1 : return "#FFFFFF";
				case 2 : return "#FFFFFF";
				case 3 : return "#FFFFFF";
				case 4 : return "#FFFFFF";
			} break; 
			// MaterialIdentification::FlagsPreV6 
			case 7 :
			switch (t) { 
				case 0 : return "#FFFFFF";
				case 1 : return "#FFFFFF";
				case 2 : return "#FFFFFF";
				case 3 : return "#FFFFFF";
				case 4 : return "#FFFFFF";
				case 5 : return "#FFFFFF";
				case 6 : return "#FFFFFF";
			} break; 
			// MaterialIdentification::HatchingType 
			case 8 :
			switch (t) { 
				case 0 : return "#FFFFFF";
				case 1 : return "#FFFFFF";
				case 2 : return "#FFFFFF";
				case 3 : return "#FFFFFF";
				case 4 : return "#FFFFFF";
				case 5 : return "#FFFFFF";
				case 6 : return "#FFFFFF";
				case 7 : return "#FFFFFF";
				case 8 : return "#FFFFFF";
				case 9 : return "#FFFFFF";
				case 10 : return "#FFFFFF";
				case 11 : return "#FFFFFF";
				case 12 : return "#FFFFFF";
				case 13 : return "#FFFFFF";
			} break; 
			// Material::Sections 
			case 9 :
			switch (t) { 
				case 0 : return "#FFFFFF";
				case 1 : return "#FFFFFF";
				case 2 : return "#FFFFFF";
				case 3 : return "#FFFFFF";
				case 4 : return "#FFFFFF";
				case 5 : return "#FFFFFF";
				case 6 : return "#FFFFFF";
				case 7 : return "#FFFFFF";
				case 8 : return "#FFFFFF";
				case 9 : return "#FFFFFF";
				case 10 : return "#FFFFFF";
				case 11 : return "#FFFFFF";
				case 12 : return "#FFFFFF";
				case 13 : return "#FFFFFF";
				case 14 : return "#FFFFFF";
				case 15 : return "#FFFFFF";
				case 16 : return "#FFFFFF";
			} break; 
			// Material::SectionsPreV6 
			case 10 :
			switch (t) { 
				case 0 : return "#FFFFFF";
				case 1 : return "#FFFFFF";
				case 2 : return "#FFFFFF";
				case 3 : return "#FFFFFF";
				case 4 : return "#FFFFFF";
				case 5 : return "#FFFFFF";
				case 6 : return "#FFFFFF";
				case 7 : return "#FFFFFF";
				case 8 : return "#FFFFFF";
			} break; 
			// Material::ParaStorage 
			case 11 :
			switch (t) { 
				case 0 : return "#FFFFFF";
				case 1 : return "#FFFFFF";
				case 2 : return "#FFFFFF";
				case 3 : return "#FFFFFF";
				case 4 : return "#FFFFFF";
				case 5 : return "#FFFFFF";
				case 6 : return "#FFFFFF";
			} break; 
			// Material::ParaTransport 
			case 12 :
			switch (t) { 
				case 0 : return "#FFFFFF";
				case 1 : return "#FFFFFF";
				case 2 : return "#FFFFFF";
				case 3 : return "#FFFFFF";
				case 4 : return "#FFFFFF";
				case 5 : return "#FFFFFF";
				case 6 : return "#FFFFFF";
				case 7 : return "#FFFFFF";
				case 8 : return "#FFFFFF";
				case 9 : return "#FFFFFF";
				case 10 : return "#FFFFFF";
				case 11 : return "#FFFFFF";
				case 12 : return "#FFFFFF";
				case 13 : return "#FFFFFF";
				case 14 : return "#FFFFFF";
				case 15 : return "#FFFFFF";
				case 16 : return "#FFFFFF";
				case 17 : return "#FFFFFF";
				case 18 : return "#FFFFFF";
				case 19 : return "#FFFFFF";
				case 20 : return "#FFFFFF";
				case 21 : return "#FFFFFF";
			} break; 
			// Material::ParaMechanical 
			case 13 :
			switch (t) { 
				case 0 : return "#FFFFFF";
				case 1 : return "#FFFFFF";
				case 2 : return "#FFFFFF";
				case 3 : return "#FFFFFF";
				case 4 : return "#FFFFFF";
				case 5 : return "#FFFFFF";
				case 6 : return "#FFFFFF";
				case 7 : return "#FFFFFF";
				case 8 : return "#FFFFFF";
			} break; 
			// MaterialCategory::CategoryID 
			case 14 :
			switch (t) { 
				case 0 : return "#FFFFFF";
				case 1 : return "#FFFFFF";
				case 2 : return "#FFFFFF";
				case 3 : return "#FFFFFF";
				case 4 : return "#FFFFFF";
				case 5 : return "#FFFFFF";
				case 6 : return "#FFFFFF";
				case 7 : return "#FFFFFF";
				case 8 : return "#FFFFFF";
				case 9 : return "#FFFFFF";
				case 10 : return "#FFFFFF";
				case 11 : return "#FFFFFF";
				case 12 : return "#FFFFFF";
				case 13 : return "#FFFFFF";
			} break; 
			// MeasuredData::Sections 
			case 15 :
			switch (t) { 
				case 0 : return "#FFFFFF";
				case 1 : return "#FFFFFF";
			} break; 
			// MeasuredData::WaterRetentionValues 
			case 16 :
			switch (t) { 
				case 0 : return "#FFFFFF";
				case 1 : return "#FFFFFF";
				case 2 : return "#FFFFFF";
				case 3 : return "#FFFFFF";
				case 4 : return "#FFFFFF";
				case 5 : return "#FFFFFF";
			} break; 
			// MeasuredData::WaterVapourPermeabilityValues 
			case 17 :
			switch (t) { 
				case 0 : return "#FFFFFF";
				case 1 : return "#FFFFFF";
				case 2 : return "#FFFFFF";
				case 3 : return "#FFFFFF";
				case 4 : return "#FFFFFF";
				case 5 : return "#FFFFFF";
			} break; 
			// MeasuredData::LiquidWaterConductivityValues 
			case 18 :
			switch (t) { 
				case 0 : return "#FFFFFF";
				case 1 : return "#FFFFFF";
				case 2 : return "#FFFFFF";
			} break; 
			// MeasuredData::WaterUptakeValues 
			case 19 :
			switch (t) { 
				case 0 : return "#FFFFFF";
			} break; 
			// MeasuredData::DryingValues0 
			case 20 :
			switch (t) { 
				case 0 : return "#FFFFFF";
				case 1 : return "#FFFFFF";
				case 2 : return "#FFFFFF";
				case 3 : return "#FFFFFF";
				case 4 : return "#FFFFFF";
				case 5 : return "#FFFFFF";
				case 6 : return "#FFFFFF";
			} break; 
			// MeasuredData::DryingValues1 
			case 21 :
			switch (t) { 
				case 0 : return "#FFFFFF";
				case 1 : return "#FFFFFF";
				case 2 : return "#FFFFFF";
				case 3 : return "#FFFFFF";
				case 4 : return "#FFFFFF";
				case 5 : return "#FFFFFF";
				case 6 : return "#FFFFFF";
				case 7 : return "#FFFFFF";
				case 8 : return "#FFFFFF";
				case 9 : return "#FFFFFF";
			} break; 
			// MeasuredData::GeometryKeys 
			case 22 :
			switch (t) { 
				case 0 : return "#FFFFFF";
				case 1 : return "#FFFFFF";
				case 2 : return "#FFFFFF";
				case 3 : return "#FFFFFF";
				case 4 : return "#FFFFFF";
				case 5 : return "#FFFFFF";
				case 6 : return "#FFFFFF";
				case 7 : return "#FFFFFF";
				case 8 : return "#FFFFFF";
				case 9 : return "#FFFFFF";
				case 10 : return "#FFFFFF";
				case 11 : return "#FFFFFF";
				case 12 : return "#FFFFFF";
				case 13 : return "#FFFFFF";
			} break; 
			// MeasuredData::ExperimentKeys 
			case 23 :
			switch (t) { 
				case 0 : return "#FFFFFF";
				case 1 : return "#FFFFFF";
				case 2 : return "#FFFFFF";
				case 3 : return "#FFFFFF";
				case 4 : return "#FFFFFF";
				case 5 : return "#FFFFFF";
				case 6 : return "#FFFFFF";
				case 7 : return "#FFFFFF";
				case 8 : return "#FFFFFF";
				case 9 : return "#FFFFFF";
				case 10 : return "#FFFFFF";
				case 11 : return "#FFFFFF";
				case 12 : return "#FFFFFF";
				case 13 : return "#FFFFFF";
				case 14 : return "#FFFFFF";
				case 15 : return "#FFFFFF";
				case 16 : return "#FFFFFF";
				case 17 : return "#FFFFFF";
				case 18 : return "#FFFFFF";
				case 19 : return "#FFFFFF";
				case 20 : return "#FFFFFF";
				case 21 : return "#FFFFFF";
				case 22 : return "#FFFFFF";
				case 23 : return "#FFFFFF";
				case 24 : return "#FFFFFF";
				case 25 : return "#FFFFFF";
				case 26 : return "#FFFFFF";
				case 27 : return "#FFFFFF";
				case 28 : return "#FFFFFF";
				case 29 : return "#FFFFFF";
			} break; 
			// MeasuredData::Experiments 
			case 24 :
			switch (t) { 
				case 1 : return "#FFFFFF";
				case 2 : return "#FFFFFF";
				case 4 : return "#FFFFFF";
				case 8 : return "#FFFFFF";
				case 16 : return "#FFFFFF";
				case 32 : return "#FFFFFF";
				case 64 : return "#FFFFFF";
				case 128 : return "#FFFFFF";
			} break; 
			// ModelHandler::ModelIdentifier 
			case 25 :
			switch (t) { 
				case 0 : return "#FFFFFF";
				case 1 : return "#FFFFFF";
			} break; 
			// MoistureStorage::Functions 
			case 26 :
			switch (t) { 
				case 0 : return "#FFFFFF";
				case 1 : return "#FFFFFF";
				case 2 : return "#FFFFFF";
				case 3 : return "#FFFFFF";
				case 4 : return "#FFFFFF";
				case 5 : return "#FFFFFF";
			} break; 
			// MoistureStorage::ModelTypes 
			case 27 :
			switch (t) { 
				case 0 : return "#FFFFFF";
				case 1 : return "#FFFFFF";
			} break; 
			// MoistureTransport::Functions 
			case 28 :
			switch (t) { 
				case 0 : return "#FFFFFF";
				case 1 : return "#FFFFFF";
				case 2 : return "#FFFFFF";
				case 3 : return "#FFFFFF";
			} break; 
			// MoistureTransport::ModelTypes 
			case 29 :
			switch (t) { 
				case 0 : return "#FFFFFF";
			} break; 
			// ThermalStorage::Functions 
			case 30 :
			switch (t) { 
				case 0 : return "#FFFFFF";
			} break; 
			// ThermalTransport::Functions 
			case 31 :
			switch (t) { 
				case 0 : return "#FFFFFF";
				case 1 : return "#FFFFFF";
			} break; 
			// VaporContactResistanceModel::Param 
			case 32 :
			switch (t) { 
				case 0 : return "#FFFFFF";
				case 1 : return "#FFFFFF";
				case 2 : return "#FFFFFF";
				case 3 : return "#FFFFFF";
				case 4 : return "#FFFFFF";
				case 5 : return "#FFFFFF";
				case 6 : return "#FFFFFF";
				case 7 : return "#FFFFFF";
			} break; 
			// VaporContactResistanceModel::Functions 
			case 33 :
			switch (t) { 
				case 0 : return "#FFFFFF";
				case 1 : return "#FFFFFF";
			} break; 
		} // switch
		throw IBK::Exception(IBK::FormatString("Cannot determine color for enumeration type '%1' and index '%2'.")
			.arg(enumtype).arg(t), "[KeywordList::Color]");
	}

	double KeywordList::DefaultValue(const char * const enumtype, int t) {
		switch (enum2index(enumtype)) {
			// AirTransport::Functions 
			case 0 :
			switch (t) { 
				case 0 : return std::numeric_limits<double>::quiet_NaN();
			} break; 
			// Foils::Ident 
			case 1 :
			switch (t) { 
				case 0 : return std::numeric_limits<double>::quiet_NaN();
				case 1 : return std::numeric_limits<double>::quiet_NaN();
				case 2 : return std::numeric_limits<double>::quiet_NaN();
				case 3 : return std::numeric_limits<double>::quiet_NaN();
			} break; 
			// Foils::Param 
			case 2 :
			switch (t) { 
				case 0 : return std::numeric_limits<double>::quiet_NaN();
				case 1 : return std::numeric_limits<double>::quiet_NaN();
				case 2 : return std::numeric_limits<double>::quiet_NaN();
				case 3 : return std::numeric_limits<double>::quiet_NaN();
				case 4 : return std::numeric_limits<double>::quiet_NaN();
				case 5 : return std::numeric_limits<double>::quiet_NaN();
				case 6 : return std::numeric_limits<double>::quiet_NaN();
				case 7 : return std::numeric_limits<double>::quiet_NaN();
				case 8 : return std::numeric_limits<double>::quiet_NaN();
			} break; 
			// Foils::Functions 
			case 3 :
			switch (t) { 
				case 0 : return std::numeric_limits<double>::quiet_NaN();
				case 1 : return std::numeric_limits<double>::quiet_NaN();
			} break; 
			// MaterialIdentification::Ident 
			case 4 :
			switch (t) { 
				case 0 : return std::numeric_limits<double>::quiet_NaN();
				case 1 : return std::numeric_limits<double>::quiet_NaN();
				case 2 : return std::numeric_limits<double>::quiet_NaN();
				case 3 : return std::numeric_limits<double>::quiet_NaN();
				case 4 : return std::numeric_limits<double>::quiet_NaN();
				case 5 : return std::numeric_limits<double>::quiet_NaN();
				case 6 : return std::numeric_limits<double>::quiet_NaN();
				case 7 : return std::numeric_limits<double>::quiet_NaN();
				case 8 : return std::numeric_limits<double>::quiet_NaN();
				case 9 : return std::numeric_limits<double>::quiet_NaN();
				case 10 : return std::numeric_limits<double>::quiet_NaN();
				case 11 : return std::numeric_limits<double>::quiet_NaN();
				case 12 : return std::numeric_limits<double>::quiet_NaN();
				case 13 : return std::numeric_limits<double>::quiet_NaN();
				case 14 : return std::numeric_limits<double>::quiet_NaN();
				case 15 : return std::numeric_limits<double>::quiet_NaN();
				case 16 : return std::numeric_limits<double>::quiet_NaN();
				case 17 : return std::numeric_limits<double>::quiet_NaN();
				case 18 : return std::numeric_limits<double>::quiet_NaN();
				case 19 : return std::numeric_limits<double>::quiet_NaN();
				case 20 : return std::numeric_limits<double>::quiet_NaN();
				case 21 : return std::numeric_limits<double>::quiet_NaN();
				case 22 : return std::numeric_limits<double>::quiet_NaN();
				case 23 : return std::numeric_limits<double>::quiet_NaN();
				case 24 : return std::numeric_limits<double>::quiet_NaN();
				case 25 : return std::numeric_limits<double>::quiet_NaN();
			} break; 
			// MaterialIdentification::Flags 
			case 5 :
			switch (t) { 
				case 1 : return std::numeric_limits<double>::quiet_NaN();
				case 2 : return std::numeric_limits<double>::quiet_NaN();
				case 4 : return std::numeric_limits<double>::quiet_NaN();
				case 8 : return std::numeric_limits<double>::quiet_NaN();
				case 16 : return std::numeric_limits<double>::quiet_NaN();
				case 32 : return std::numeric_limits<double>::quiet_NaN();
				case 64 : return std::numeric_limits<double>::quiet_NaN();
			} break; 
			// MaterialIdentification::SamplingMethods 
			case 6 :
			switch (t) { 
				case 1 : return std::numeric_limits<double>::quiet_NaN();
				case 2 : return std::numeric_limits<double>::quiet_NaN();
				case 3 : return std::numeric_limits<double>::quiet_NaN();
				case 4 : return std::numeric_limits<double>::quiet_NaN();
			} break; 
			// MaterialIdentification::FlagsPreV6 
			case 7 :
			switch (t) { 
				case 0 : return std::numeric_limits<double>::quiet_NaN();
				case 1 : return std::numeric_limits<double>::quiet_NaN();
				case 2 : return std::numeric_limits<double>::quiet_NaN();
				case 3 : return std::numeric_limits<double>::quiet_NaN();
				case 4 : return std::numeric_limits<double>::quiet_NaN();
				case 5 : return std::numeric_limits<double>::quiet_NaN();
				case 6 : return std::numeric_limits<double>::quiet_NaN();
			} break; 
			// MaterialIdentification::HatchingType 
			case 8 :
			switch (t) { 
				case 0 : return std::numeric_limits<double>::quiet_NaN();
				case 1 : return std::numeric_limits<double>::quiet_NaN();
				case 2 : return std::numeric_limits<double>::quiet_NaN();
				case 3 : return std::numeric_limits<double>::quiet_NaN();
				case 4 : return std::numeric_limits<double>::quiet_NaN();
				case 5 : return std::numeric_limits<double>::quiet_NaN();
				case 6 : return std::numeric_limits<double>::quiet_NaN();
				case 7 : return std::numeric_limits<double>::quiet_NaN();
				case 8 : return std::numeric_limits<double>::quiet_NaN();
				case 9 : return std::numeric_limits<double>::quiet_NaN();
				case 10 : return std::numeric_limits<double>::quiet_NaN();
				case 11 : return std::numeric_limits<double>::quiet_NaN();
				case 12 : return std::numeric_limits<double>::quiet_NaN();
				case 13 : return std::numeric_limits<double>::quiet_NaN();
			} break; 
			// Material::Sections 
			case 9 :
			switch (t) { 
				case 0 : return std::numeric_limits<double>::quiet_NaN();
				case 1 : return std::numeric_limits<double>::quiet_NaN();
				case 2 : return std::numeric_limits<double>::quiet_NaN();
				case 3 : return std::numeric_limits<double>::quiet_NaN();
				case 4 : return std::numeric_limits<double>::quiet_NaN();
				case 5 : return std::numeric_limits<double>::quiet_NaN();
				case 6 : return std::numeric_limits<double>::quiet_NaN();
				case 7 : return std::numeric_limits<double>::quiet_NaN();
				case 8 : return std::numeric_limits<double>::quiet_NaN();
				case 9 : return std::numeric_limits<double>::quiet_NaN();
				case 10 : return std::numeric_limits<double>::quiet_NaN();
				case 11 : return std::numeric_limits<double>::quiet_NaN();
				case 12 : return std::numeric_limits<double>::quiet_NaN();
				case 13 : return std::numeric_limits<double>::quiet_NaN();
				case 14 : return std::numeric_limits<double>::quiet_NaN();
				case 15 : return std::numeric_limits<double>::quiet_NaN();
				case 16 : return std::numeric_limits<double>::quiet_NaN();
			} break; 
			// Material::SectionsPreV6 
			case 10 :
			switch (t) { 
				case 0 : return std::numeric_limits<double>::quiet_NaN();
				case 1 : return std::numeric_limits<double>::quiet_NaN();
				case 2 : return std::numeric_limits<double>::quiet_NaN();
				case 3 : return std::numeric_limits<double>::quiet_NaN();
				case 4 : return std::numeric_limits<double>::quiet_NaN();
				case 5 : return std::numeric_limits<double>::quiet_NaN();
				case 6 : return std::numeric_limits<double>::quiet_NaN();
				case 7 : return std::numeric_limits<double>::quiet_NaN();
				case 8 : return std::numeric_limits<double>::quiet_NaN();
			} break; 
			// Material::ParaStorage 
			case 11 :
			switch (t) { 
				case 0 : return std::numeric_limits<double>::quiet_NaN();
				case 1 : return std::numeric_limits<double>::quiet_NaN();
				case 2 : return std::numeric_limits<double>::quiet_NaN();
				case 3 : return std::numeric_limits<double>::quiet_NaN();
				case 4 : return std::numeric_limits<double>::quiet_NaN();
				case 5 : return std::numeric_limits<double>::quiet_NaN();
				case 6 : return std::numeric_limits<double>::quiet_NaN();
			} break; 
			// Material::ParaTransport 
			case 12 :
			switch (t) { 
				case 0 : return std::numeric_limits<double>::quiet_NaN();
				case 1 : return std::numeric_limits<double>::quiet_NaN();
				case 2 : return std::numeric_limits<double>::quiet_NaN();
				case 3 : return std::numeric_limits<double>::quiet_NaN();
				case 4 : return std::numeric_limits<double>::quiet_NaN();
				case 5 : return std::numeric_limits<double>::quiet_NaN();
				case 6 : return std::numeric_limits<double>::quiet_NaN();
				case 7 : return std::numeric_limits<double>::quiet_NaN();
				case 8 : return std::numeric_limits<double>::quiet_NaN();
				case 9 : return std::numeric_limits<double>::quiet_NaN();
				case 10 : return std::numeric_limits<double>::quiet_NaN();
				case 11 : return std::numeric_limits<double>::quiet_NaN();
				case 12 : return std::numeric_limits<double>::quiet_NaN();
				case 13 : return std::numeric_limits<double>::quiet_NaN();
				case 14 : return std::numeric_limits<double>::quiet_NaN();
				case 15 : return std::numeric_limits<double>::quiet_NaN();
				case 16 : return std::numeric_limits<double>::quiet_NaN();
				case 17 : return std::numeric_limits<double>::quiet_NaN();
				case 18 : return std::numeric_limits<double>::quiet_NaN();
				case 19 : return std::numeric_limits<double>::quiet_NaN();
				case 20 : return std::numeric_limits<double>::quiet_NaN();
				case 21 : return std::numeric_limits<double>::quiet_NaN();
			} break; 
			// Material::ParaMechanical 
			case 13 :
			switch (t) { 
				case 0 : return std::numeric_limits<double>::quiet_NaN();
				case 1 : return std::numeric_limits<double>::quiet_NaN();
				case 2 : return std::numeric_limits<double>::quiet_NaN();
				case 3 : return std::numeric_limits<double>::quiet_NaN();
				case 4 : return std::numeric_limits<double>::quiet_NaN();
				case 5 : return std::numeric_limits<double>::quiet_NaN();
				case 6 : return std::numeric_limits<double>::quiet_NaN();
				case 7 : return std::numeric_limits<double>::quiet_NaN();
				case 8 : return std::numeric_limits<double>::quiet_NaN();
			} break; 
			// MaterialCategory::CategoryID 
			case 14 :
			switch (t) { 
				case 0 : return std::numeric_limits<double>::quiet_NaN();
				case 1 : return std::numeric_limits<double>::quiet_NaN();
				case 2 : return std::numeric_limits<double>::quiet_NaN();
				case 3 : return std::numeric_limits<double>::quiet_NaN();
				case 4 : return std::numeric_limits<double>::quiet_NaN();
				case 5 : return std::numeric_limits<double>::quiet_NaN();
				case 6 : return std::numeric_limits<double>::quiet_NaN();
				case 7 : return std::numeric_limits<double>::quiet_NaN();
				case 8 : return std::numeric_limits<double>::quiet_NaN();
				case 9 : return std::numeric_limits<double>::quiet_NaN();
				case 10 : return std::numeric_limits<double>::quiet_NaN();
				case 11 : return std::numeric_limits<double>::quiet_NaN();
				case 12 : return std::numeric_limits<double>::quiet_NaN();
				case 13 : return std::numeric_limits<double>::quiet_NaN();
			} break; 
			// MeasuredData::Sections 
			case 15 :
			switch (t) { 
				case 0 : return std::numeric_limits<double>::quiet_NaN();
				case 1 : return std::numeric_limits<double>::quiet_NaN();
			} break; 
			// MeasuredData::WaterRetentionValues 
			case 16 :
			switch (t) { 
				case 0 : return std::numeric_limits<double>::quiet_NaN();
				case 1 : return std::numeric_limits<double>::quiet_NaN();
				case 2 : return std::numeric_limits<double>::quiet_NaN();
				case 3 : return std::numeric_limits<double>::quiet_NaN();
				case 4 : return std::numeric_limits<double>::quiet_NaN();
				case 5 : return std::numeric_limits<double>::quiet_NaN();
			} break; 
			// MeasuredData::WaterVapourPermeabilityValues 
			case 17 :
			switch (t) { 
				case 0 : return std::numeric_limits<double>::quiet_NaN();
				case 1 : return std::numeric_limits<double>::quiet_NaN();
				case 2 : return std::numeric_limits<double>::quiet_NaN();
				case 3 : return std::numeric_limits<double>::quiet_NaN();
				case 4 : return std::numeric_limits<double>::quiet_NaN();
				case 5 : return std::numeric_limits<double>::quiet_NaN();
			} break; 
			// MeasuredData::LiquidWaterConductivityValues 
			case 18 :
			switch (t) { 
				case 0 : return std::numeric_limits<double>::quiet_NaN();
				case 1 : return std::numeric_limits<double>::quiet_NaN();
				case 2 : return std::numeric_limits<double>::quiet_NaN();
			} break; 
			// MeasuredData::WaterUptakeValues 
			case 19 :
			switch (t) { 
				case 0 : return std::numeric_limits<double>::quiet_NaN();
			} break; 
			// MeasuredData::DryingValues0 
			case 20 :
			switch (t) { 
				case 0 : return std::numeric_limits<double>::quiet_NaN();
				case 1 : return std::numeric_limits<double>::quiet_NaN();
				case 2 : return std::numeric_limits<double>::quiet_NaN();
				case 3 : return std::numeric_limits<double>::quiet_NaN();
				case 4 : return std::numeric_limits<double>::quiet_NaN();
				case 5 : return std::numeric_limits<double>::quiet_NaN();
				case 6 : return std::numeric_limits<double>::quiet_NaN();
			} break; 
			// MeasuredData::DryingValues1 
			case 21 :
			switch (t) { 
				case 0 : return std::numeric_limits<double>::quiet_NaN();
				case 1 : return std::numeric_limits<double>::quiet_NaN();
				case 2 : return std::numeric_limits<double>::quiet_NaN();
				case 3 : return std::numeric_limits<double>::quiet_NaN();
				case 4 : return std::numeric_limits<double>::quiet_NaN();
				case 5 : return std::numeric_limits<double>::quiet_NaN();
				case 6 : return std::numeric_limits<double>::quiet_NaN();
				case 7 : return std::numeric_limits<double>::quiet_NaN();
				case 8 : return std::numeric_limits<double>::quiet_NaN();
				case 9 : return std::numeric_limits<double>::quiet_NaN();
			} break; 
			// MeasuredData::GeometryKeys 
			case 22 :
			switch (t) { 
				case 0 : return std::numeric_limits<double>::quiet_NaN();
				case 1 : return std::numeric_limits<double>::quiet_NaN();
				case 2 : return std::numeric_limits<double>::quiet_NaN();
				case 3 : return std::numeric_limits<double>::quiet_NaN();
				case 4 : return std::numeric_limits<double>::quiet_NaN();
				case 5 : return std::numeric_limits<double>::quiet_NaN();
				case 6 : return std::numeric_limits<double>::quiet_NaN();
				case 7 : return std::numeric_limits<double>::quiet_NaN();
				case 8 : return std::numeric_limits<double>::quiet_NaN();
				case 9 : return std::numeric_limits<double>::quiet_NaN();
				case 10 : return std::numeric_limits<double>::quiet_NaN();
				case 11 : return std::numeric_limits<double>::quiet_NaN();
				case 12 : return std::numeric_limits<double>::quiet_NaN();
				case 13 : return std::numeric_limits<double>::quiet_NaN();
			} break; 
			// MeasuredData::ExperimentKeys 
			case 23 :
			switch (t) { 
				case 0 : return std::numeric_limits<double>::quiet_NaN();
				case 1 : return std::numeric_limits<double>::quiet_NaN();
				case 2 : return std::numeric_limits<double>::quiet_NaN();
				case 3 : return std::numeric_limits<double>::quiet_NaN();
				case 4 : return std::numeric_limits<double>::quiet_NaN();
				case 5 : return std::numeric_limits<double>::quiet_NaN();
				case 6 : return std::numeric_limits<double>::quiet_NaN();
				case 7 : return std::numeric_limits<double>::quiet_NaN();
				case 8 : return std::numeric_limits<double>::quiet_NaN();
				case 9 : return std::numeric_limits<double>::quiet_NaN();
				case 10 : return std::numeric_limits<double>::quiet_NaN();
				case 11 : return std::numeric_limits<double>::quiet_NaN();
				case 12 : return std::numeric_limits<double>::quiet_NaN();
				case 13 : return std::numeric_limits<double>::quiet_NaN();
				case 14 : return std::numeric_limits<double>::quiet_NaN();
				case 15 : return std::numeric_limits<double>::quiet_NaN();
				case 16 : return std::numeric_limits<double>::quiet_NaN();
				case 17 : return std::numeric_limits<double>::quiet_NaN();
				case 18 : return std::numeric_limits<double>::quiet_NaN();
				case 19 : return std::numeric_limits<double>::quiet_NaN();
				case 20 : return std::numeric_limits<double>::quiet_NaN();
				case 21 : return std::numeric_limits<double>::quiet_NaN();
				case 22 : return std::numeric_limits<double>::quiet_NaN();
				case 23 : return std::numeric_limits<double>::quiet_NaN();
				case 24 : return std::numeric_limits<double>::quiet_NaN();
				case 25 : return std::numeric_limits<double>::quiet_NaN();
				case 26 : return std::numeric_limits<double>::quiet_NaN();
				case 27 : return std::numeric_limits<double>::quiet_NaN();
				case 28 : return std::numeric_limits<double>::quiet_NaN();
				case 29 : return std::numeric_limits<double>::quiet_NaN();
			} break; 
			// MeasuredData::Experiments 
			case 24 :
			switch (t) { 
				case 1 : return std::numeric_limits<double>::quiet_NaN();
				case 2 : return std::numeric_limits<double>::quiet_NaN();
				case 4 : return std::numeric_limits<double>::quiet_NaN();
				case 8 : return std::numeric_limits<double>::quiet_NaN();
				case 16 : return std::numeric_limits<double>::quiet_NaN();
				case 32 : return std::numeric_limits<double>::quiet_NaN();
				case 64 : return std::numeric_limits<double>::quiet_NaN();
				case 128 : return std::numeric_limits<double>::quiet_NaN();
			} break; 
			// ModelHandler::ModelIdentifier 
			case 25 :
			switch (t) { 
				case 0 : return std::numeric_limits<double>::quiet_NaN();
				case 1 : return std::numeric_limits<double>::quiet_NaN();
			} break; 
			// MoistureStorage::Functions 
			case 26 :
			switch (t) { 
				case 0 : return std::numeric_limits<double>::quiet_NaN();
				case 1 : return std::numeric_limits<double>::quiet_NaN();
				case 2 : return std::numeric_limits<double>::quiet_NaN();
				case 3 : return std::numeric_limits<double>::quiet_NaN();
				case 4 : return std::numeric_limits<double>::quiet_NaN();
				case 5 : return std::numeric_limits<double>::quiet_NaN();
			} break; 
			// MoistureStorage::ModelTypes 
			case 27 :
			switch (t) { 
				case 0 : return std::numeric_limits<double>::quiet_NaN();
				case 1 : return std::numeric_limits<double>::quiet_NaN();
			} break; 
			// MoistureTransport::Functions 
			case 28 :
			switch (t) { 
				case 0 : return std::numeric_limits<double>::quiet_NaN();
				case 1 : return std::numeric_limits<double>::quiet_NaN();
				case 2 : return std::numeric_limits<double>::quiet_NaN();
				case 3 : return std::numeric_limits<double>::quiet_NaN();
			} break; 
			// MoistureTransport::ModelTypes 
			case 29 :
			switch (t) { 
				case 0 : return std::numeric_limits<double>::quiet_NaN();
			} break; 
			// ThermalStorage::Functions 
			case 30 :
			switch (t) { 
				case 0 : return std::numeric_limits<double>::quiet_NaN();
			} break; 
			// ThermalTransport::Functions 
			case 31 :
			switch (t) { 
				case 0 : return std::numeric_limits<double>::quiet_NaN();
				case 1 : return std::numeric_limits<double>::quiet_NaN();
			} break; 
			// VaporContactResistanceModel::Param 
			case 32 :
			switch (t) { 
				case 0 : return std::numeric_limits<double>::quiet_NaN();
				case 1 : return std::numeric_limits<double>::quiet_NaN();
				case 2 : return std::numeric_limits<double>::quiet_NaN();
				case 3 : return std::numeric_limits<double>::quiet_NaN();
				case 4 : return std::numeric_limits<double>::quiet_NaN();
				case 5 : return std::numeric_limits<double>::quiet_NaN();
				case 6 : return std::numeric_limits<double>::quiet_NaN();
				case 7 : return std::numeric_limits<double>::quiet_NaN();
			} break; 
			// VaporContactResistanceModel::Functions 
			case 33 :
			switch (t) { 
				case 0 : return std::numeric_limits<double>::quiet_NaN();
				case 1 : return std::numeric_limits<double>::quiet_NaN();
			} break; 
		} // switch
		throw IBK::Exception(IBK::FormatString("Cannot determine default value for enumeration type '%1' and index '%2'.")
			.arg(enumtype).arg(t), "[KeywordList::DefaultValue]");
	}

	// number of entries in a keyword list 
	unsigned int KeywordList::Count(const char * const enumtype) {
		switch (enum2index(enumtype)) {
			// AirTransport::Functions 
			case 0 : return 1;
			// Foils::Ident 
			case 1 : return 4;
			// Foils::Param 
			case 2 : return 9;
			// Foils::Functions 
			case 3 : return 2;
			// MaterialIdentification::Ident 
			case 4 : return 26;
			// MaterialIdentification::Flags 
			case 5 : return 7;
			// MaterialIdentification::SamplingMethods 
			case 6 : return 4;
			// MaterialIdentification::FlagsPreV6 
			case 7 : return 7;
			// MaterialIdentification::HatchingType 
			case 8 : return 14;
			// Material::Sections 
			case 9 : return 17;
			// Material::SectionsPreV6 
			case 10 : return 9;
			// Material::ParaStorage 
			case 11 : return 7;
			// Material::ParaTransport 
			case 12 : return 22;
			// Material::ParaMechanical 
			case 13 : return 9;
			// MaterialCategory::CategoryID 
			case 14 : return 14;
			// MeasuredData::Sections 
			case 15 : return 2;
			// MeasuredData::WaterRetentionValues 
			case 16 : return 6;
			// MeasuredData::WaterVapourPermeabilityValues 
			case 17 : return 6;
			// MeasuredData::LiquidWaterConductivityValues 
			case 18 : return 3;
			// MeasuredData::WaterUptakeValues 
			case 19 : return 1;
			// MeasuredData::DryingValues0 
			case 20 : return 7;
			// MeasuredData::DryingValues1 
			case 21 : return 10;
			// MeasuredData::GeometryKeys 
			case 22 : return 14;
			// MeasuredData::ExperimentKeys 
			case 23 : return 30;
			// MeasuredData::Experiments 
			case 24 : return 8;
			// ModelHandler::ModelIdentifier 
			case 25 : return 2;
			// MoistureStorage::Functions 
			case 26 : return 6;
			// MoistureStorage::ModelTypes 
			case 27 : return 2;
			// MoistureTransport::Functions 
			case 28 : return 4;
			// MoistureTransport::ModelTypes 
			case 29 : return 1;
			// ThermalStorage::Functions 
			case 30 : return 1;
			// ThermalTransport::Functions 
			case 31 : return 2;
			// VaporContactResistanceModel::Param 
			case 32 : return 8;
			// VaporContactResistanceModel::Functions 
			case 33 : return 2;
		} // switch
		throw IBK::Exception(IBK::FormatString("Invalid enumeration type '%1'.")
			.arg(enumtype), "[KeywordList::Count]");
	}

	// max index for entries sharing a category in a keyword list 
	int KeywordList::MaxIndex(const char * const enumtype) {
		switch (enum2index(enumtype)) {
			// AirTransport::Functions 
			case 0 : return 0;
			// Foils::Ident 
			case 1 : return 3;
			// Foils::Param 
			case 2 : return 8;
			// Foils::Functions 
			case 3 : return 1;
			// MaterialIdentification::Ident 
			case 4 : return 25;
			// MaterialIdentification::Flags 
			case 5 : return 64;
			// MaterialIdentification::SamplingMethods 
			case 6 : return 4;
			// MaterialIdentification::FlagsPreV6 
			case 7 : return 6;
			// MaterialIdentification::HatchingType 
			case 8 : return 13;
			// Material::Sections 
			case 9 : return 16;
			// Material::SectionsPreV6 
			case 10 : return 8;
			// Material::ParaStorage 
			case 11 : return 6;
			// Material::ParaTransport 
			case 12 : return 21;
			// Material::ParaMechanical 
			case 13 : return 8;
			// MaterialCategory::CategoryID 
			case 14 : return 13;
			// MeasuredData::Sections 
			case 15 : return 1;
			// MeasuredData::WaterRetentionValues 
			case 16 : return 5;
			// MeasuredData::WaterVapourPermeabilityValues 
			case 17 : return 5;
			// MeasuredData::LiquidWaterConductivityValues 
			case 18 : return 2;
			// MeasuredData::WaterUptakeValues 
			case 19 : return 0;
			// MeasuredData::DryingValues0 
			case 20 : return 6;
			// MeasuredData::DryingValues1 
			case 21 : return 9;
			// MeasuredData::GeometryKeys 
			case 22 : return 13;
			// MeasuredData::ExperimentKeys 
			case 23 : return 29;
			// MeasuredData::Experiments 
			case 24 : return 128;
			// ModelHandler::ModelIdentifier 
			case 25 : return 1;
			// MoistureStorage::Functions 
			case 26 : return 5;
			// MoistureStorage::ModelTypes 
			case 27 : return 1;
			// MoistureTransport::Functions 
			case 28 : return 3;
			// MoistureTransport::ModelTypes 
			case 29 : return 0;
			// ThermalStorage::Functions 
			case 30 : return 0;
			// ThermalTransport::Functions 
			case 31 : return 1;
			// VaporContactResistanceModel::Param 
			case 32 : return 7;
			// VaporContactResistanceModel::Functions 
			case 33 : return 1;
		} // switch
		throw IBK::Exception(IBK::FormatString("Invalid enumeration type '%1'.")
			.arg(enumtype), "[KeywordList::MaxIndex]");
	}

	const char * KeywordList::Keyword(const char * const enumtype, int t) {
		const char * const kw = theKeyword(enum2index(enumtype), t);
		if (std::string(kw) == INVALID_KEYWORD_INDEX_STRING) {
			throw IBK::Exception(IBK::FormatString("Cannot determine keyword for enumeration type '%1' and index '%2'.")
				.arg(enumtype).arg(t), "[KeywordList::Keyword]");
		}
		return kw;
	}

	bool KeywordList::KeywordExists(const char * const enumtype, const std::string & kw) {
		int typenum = enum2index(enumtype);
		int i = 0;
		int maxIndexInCategory = MaxIndex( enumtype ); 
		for ( ; i <= maxIndexInCategory; ++i ) {
			std::string keywords = allKeywords(typenum, i);
			if (keywords == INVALID_KEYWORD_INDEX_STRING)
				continue;
			std::stringstream strm(keywords);
			int j = 0;
			std::string kws;
			while (strm >> kws) {
				if (kws == kw) {
					return true; // keyword exists
				}
				++j;
			}
		}
		return false; // nothing found keyword doesn't exist.
	}

	int KeywordList::Enumeration(const char * const enumtype, const std::string & kw, bool * deprecated) {
		int typenum = enum2index(enumtype);
		int i = 0;
		int maxIndexInCategory = MaxIndex( enumtype ); 
		for ( ; i <= maxIndexInCategory; ++i ) {
			std::string keywords = allKeywords(typenum, i);
			if (keywords == INVALID_KEYWORD_INDEX_STRING)
				continue;
			std::stringstream strm(keywords);
			int j = 0;
			std::string kws;
			while (strm >> kws) {
				if (kws == kw) {
					if (deprecated != NULL) {
						*deprecated = (j != 0);
					}
					return i;
				}
				++j;
			}
		} // for ( ; i < maxIndexInCategory; ++i ) {
		throw IBK::Exception(IBK::FormatString("Cannot determine enumeration value for "
			"enumeration type '%1' and keyword '%2'.")
			.arg(enumtype).arg(kw), "[KeywordList::Enumeration]");
	}

	bool KeywordList::CategoryExists(const char * const enumtype) {
		return enum2index(enumtype) != -1;
	}

} // namespace MM
