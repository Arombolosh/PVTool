#include <iostream>
#include <limits>

#include <IBK_assert.h>
#include <IBK_messages.h>
#include <IBK_MessageHandlerRegistry.h>

#include <IBK_ArgParser.h>
#include <DATAIO_DataIO.h>

#include "PVTool_Energy.h"

//#include "PCM_Material.h"

const char * const LONG_VERSION = "1.0.0";

IBK::UnitVector readDataIO(const IBK::Path &filename){
	DATAIO::DataIO data;

	data.read(filename);

	IBK::UnitVector unitVec;
	unitVec.m_data = data.columnValues(0);
	unitVec.m_unit = IBK::Unit(data.m_valueUnit);
	unitVec.m_name = data.m_quantity;
	return  unitVec;
}

void helpDocumentation(){
	std::cout << "Help Documentation" << std::endl;

}

void extractDataFromD6Results(const IBK::Path & path, IBK::UnitVector &temp, IBK::UnitVector &rad){
	//ToDo hier müssen die richtigen Datei geladen werden
	temp = readDataIO( path /"results/temp.d6o");
	rad = readDataIO(path / "results/rad.d6o");
	temp.convert(IBK::Unit("K"));
	rad.convert(IBK::Unit("W/m2"));
/*
	IBK::UnitVector results;
	results.m_unit = IBK::Unit("W");
	results.m_name = "PV-Energy";
	results.m_data = calcPVEnergy(unitVecTemp.m_data, unitVecGlobalRad.m_data, manuData);

	DATAIO::DataIO data;
	data.read(path /"results/temp.d6o");

	std::vector<std::vector<double>> resData(data.m_timepoints.size());

	IBK_ASSERT(results.size() != data.m_timepoints.size());

	for (size_t i=0; i<results.size(); ++i)
		resData[i].push_back(results[i]);

	data.swapData(data.m_timepoints, resData);
	IBK::Path resFile(path / "pvEnergy.d6o");
	if(resFile.exists()){
		std::cout << "Filename already exist." << std::endl;
	}
	else {
		data.m_filename = resFile;
		data.write();
	}*/

}

void writeResultData(const IBK::Path &path, const IBK::UnitVector &result){
	DATAIO::DataIO data;
	std::vector<std::vector<double>> resDataIO(result.size());
	for(size_t i=0; i<result.size(); ++i){
		data.m_timepoints.push_back(i*3600);
		resDataIO[i].push_back(result[i]);
	}
	data.setData(data.m_timepoints, resDataIO);
	data.m_filename = path / ("pvEnergy.d6o");
	data.m_quantity = result.m_unit.name();
	data.m_timeType = DATAIO::DataIO::TT_MEAN;
	data.m_timeUnit = "h";
	data.m_spaceType = DATAIO::DataIO::ST_SINGLE;
	data.m_quantityKeyword = "PV Energy";
	data.m_projectFileName = "PVTool";
	data.m_startYear = 2020;
	data.m_valueUnit = "W";
	data.m_isBinary = false;

	data.write();
}



int main(int argc, char* argv[]) {
	FUNCID(main);

	IBK::ArgParser args;

	args.m_appname = "PVEnergy";
	//option for values double, string, etc.
	args.addOption('f', "path", "Input path for d6o result folder.", "","");
	args.addOption(0, "verbosity-level", "Sets verbosity of tool output.", "<0..4>", "1");

	//flags for bool
	args.addFlag('v', "version", "Displays current version");

	args.parse(argc, argv);

	//help
	if (args.handleDefaultFlags(std::cout)){
		return EXIT_SUCCESS;
	}

	if (args.flagEnabled('v')){
		IBK::IBK_Message(IBK::FormatString("PVEnergy version: %1 \n").arg(LONG_VERSION));
		return EXIT_SUCCESS;
	}

	// set message verbosity level
	try {
		int verbosityLevel = IBK::string2valChecked<int>(args.option("verbosity-level"));
		IBK::MessageHandlerRegistry::instance().messageHandler()->setConsoleVerbosityLevel(verbosityLevel);
	} catch (IBK::Exception & ex) {
		ex.writeMsgStackToError();
		IBK::IBK_Message("Invalid argument to 'verbosity-level'.", IBK::MSG_ERROR, FUNC_ID);
		return EXIT_FAILURE;
	}

	// Example command lines:
	// pv_tool.exe vmp imp voc isc alpha beta gamma nSer refTemp Material T Rad
	// pv_tool.exe vmp imp voc isc alpha beta gamma nSer refTemp Material -f path
	//
	// With temperature: 32.24 9.27 39.75 9.76 0.00013 "-0.0031" "-0.41" 60 298.15 monoSi 30 300
	// With file:

	// case filepath?
	if (args.hasOption('f')) {
		if (args.args().size() != 12 ) {
			IBK::IBK_Message("Invalid command line, 11 positional arguments expected in addition to -f=<> option. Use --help.", IBK::MSG_ERROR);
			return EXIT_FAILURE;
		}
	}
	else {
		if (args.args().size() != 13 ) {
			IBK::IBK_Message("Invalid command line, 12 positional arguments expected. U --help.", IBK::MSG_ERROR);
			return EXIT_FAILURE;
		}
	}
	// Mind: args.args()[0] = path/to/PVEnergy.exe

	PVTOOL::Energy pvtool;
	PVTOOL::Energy::ManufactureData &manuData = pvtool.m_manuData;

	//read PV module data
	try {
		manuData.m_vmp =	IBK::string2valChecked<double>(argv[1]);
		manuData.m_imp =	IBK::string2valChecked<double>(argv[2]);
		manuData.m_voc =	IBK::string2valChecked<double>(argv[3]);
		manuData.m_isc =	IBK::string2valChecked<double>(argv[4]);
		manuData.m_alpha =	IBK::string2valChecked<double>(argv[5]);
		manuData.m_beta =	IBK::string2valChecked<double>(argv[6]);
		manuData.m_gamma =	IBK::string2valChecked<double>(argv[7]);
		manuData.m_nSer =	IBK::string2valChecked<int>(argv[8]);
		manuData.m_refTemp =	IBK::string2valChecked<double>(argv[9]);

		// TODO : material is not taken into account yet; Default monoSi
		if (args.args()[10] == "monoSi") {
			manuData.m_material = 0;
		}
		else {
			manuData.m_material = 0;
		}

		// compute derived parameters, may throw an exception
		pvtool.calcPhysicalParameterFromManufactureData();
	}
	catch (IBK::Exception  &ex) {
		ex.writeMsgStackToError();
		IBK::IBK_Message("Invalid PV-data provided as command line arguments.", IBK::MSG_ERROR, FUNC_ID);
		return EXIT_FAILURE;
	}


	// read temperature and radiation data from d6o files
	IBK::UnitVector tempVec, radVec, pvEnergyVec;
	if (args.hasOption('f')) {
		try {
			// attempt to read input data from d6o files in given directory
			extractDataFromD6Results(IBK::Path(args.option('f')), tempVec, radVec);
			// convert to required unit (may throw if source unit mismatches)
			tempVec.convert(IBK::Unit("K"));
			radVec.convert(IBK::Unit("W/m2"));
			pvEnergyVec.m_unit = IBK::Unit("W");
			pvtool.calcPVEnergy(tempVec.m_data, radVec.m_data, pvEnergyVec.m_data);
			// calculation of physical PV data
			writeResultData(IBK::Path("c:/temp/"), pvEnergyVec);
		}
		catch (IBK::Exception &ex) {
			ex.writeMsgStackToError();
			IBK::IBK_Message("Error during energy calculation!", IBK::MSG_ERROR, FUNC_ID);
			return EXIT_FAILURE;
		}
	}
	else {
		double temp, rad;
		try {
			temp = IBK::string2valChecked<double>(args.args()[11]); // K
			rad = IBK::string2valChecked<double>(args.args()[12]); // W/m2
		}
		catch (IBK::Exception  &ex) {
			ex.writeMsgStackToError();
			IBK::IBK_Message("Temperature or radiation invalid on command line!", IBK::MSG_ERROR, FUNC_ID);
			return EXIT_FAILURE;
		}
		// calculation for a single value
		try {
			double pvEnergy = pvtool.calcPVEnergy(temp, rad);
			// for scalar value variant, dump out result in english number format onto the console
			std::cout << pvEnergy << std::endl;
		}
		catch (IBK::Exception  &ex) {
			ex.writeMsgStackToError();
			IBK::IBK_Message("Error during energy calculation!", IBK::MSG_ERROR, FUNC_ID);
			return EXIT_FAILURE;
		}
	}

	return EXIT_SUCCESS;
}
