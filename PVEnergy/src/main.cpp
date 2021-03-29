#include <iostream>
#include <limits>
#include <iostream>
#include <fstream>

#include <IBK_assert.h>
#include <IBK_messages.h>
#include <IBK_MessageHandlerRegistry.h>
#include <IBK_FileReader.h>
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
	temp = readDataIO( path /"results/TMean.d6o");
	rad = readDataIO(path / "results/GlobalRadiation.d6o");
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


void writeResultData(const IBK::Path &path, const IBK::UnitVector &result, const std::string &filename){
	std::cout<< "write d6o files."  <<std::endl;
	DATAIO::DataIO data;
	std::vector<std::vector<double>> resDataIO(result.size());
	for(size_t i=0; i<result.size(); ++i){
		data.m_timepoints.push_back(i*3600);
		resDataIO[i].push_back(result[i]);
	}
	data.setData(data.m_timepoints, resDataIO);
	data.m_nums.push_back(1);
	data.m_type = DATAIO::DataIO::T_FIELD;
	data.m_filename = path / filename;
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
	args.addOption('o',"output","Output path for results.","","");

	//flags for bool
	args.addFlag('v', "version", "Displays current version");
	args.addFlag('s', "filename", "First input: file name for data with name.tsv and second input: file name for modul data with name2.txt.");
	//args.addFlag('f', "path", "Displays current version");

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
		int verbosityLevel = IBK::string2val<int>(args.option("verbosity-level"));
		IBK::MessageHandlerRegistry::instance().messageHandler()->setConsoleVerbosityLevel(3/*verbosityLevel*/);
	} catch (IBK::Exception & ex) {
		ex.writeMsgStackToError();
		IBK::IBK_Message("Invalid argument to 'verbosity-level'.", IBK::MSG_ERROR, FUNC_ID);
		return EXIT_FAILURE;
	}

	// Example command lines:
	// pv_tool.exe vmp imp voc isc alpha beta gamma nSer refTemp Material T Rad
	// pv_tool.exe vmp imp voc isc alpha beta gamma nSer refTemp Material -f path
	//
	// With temperature:	31.4 8.44 38.3 8.91 0.05 -0.30 -0.43 60 298.15 monoSi (273.15+30) 300
	// temperatures are all absolute values [K]
	// radiation in [W/m2]
	// temperature depend values in %/K
	// alpha=Temp Coeff of Isc (Isc[A]*alpha[%/K] = alpha[A/K])
	// beta= Temp coeff of Voc (Isc[V]*beta[%/K] = beta[V/K])
	// gamma=Temp coeff of Mpp
	// nSer number of cells in series
	// refTemp in K [STC..25°C...289.15K]

	// With file:			31.4 8.44 38.3 8.91 0.05 -0.30 -0.43 60 298.15 monoSi -f="c:/temp/test02/"

	// case filepath?
	bool withTSV = false;

	if(args.flagEnabled('s')){
		withTSV  = true;
		if(args.args().size() != 3){
			IBK::IBK_Message("Need two filenames. First filename -> data.tsv Second filename -> modulData.txt", IBK::MSG_ERROR);
			return EXIT_FAILURE;
		}
	}
	else if (args.hasOption('o') && args.hasOption('f')) {
		if ( args.args().size() != 11 ) {
			IBK::IBK_Message("Invalid command line, 11 positional arguments expected in addition to -f=<> and -o=<> option . Use --help.", IBK::MSG_ERROR);
			return EXIT_FAILURE;
		}
	} else 	if (args.hasOption('o')) {
		if ( args.args().size() != 11 ) {
			IBK::IBK_Message("Invalid command line, 11 positional arguments expected in addition to -o=<> option. Use --help.", IBK::MSG_ERROR);
			return EXIT_FAILURE;
		}
	} else if (args.hasOption('f')) {
		if ( args.args().size() != 11 ) {
			IBK::IBK_Message("Invalid command line, 11 positional arguments expected in addition to -f=<> option. Use --help.", IBK::MSG_ERROR);
			return EXIT_FAILURE;
		}
	} else {
		if (args.args().size() != 13 ) {
			IBK::IBK_Message("Invalid command line, 12 positional arguments expected. U --help.", IBK::MSG_ERROR);
			return EXIT_FAILURE;
		}
	}
	// Mind: args.args()[0] = path/to/PVEnergy.exe

	PVTOOL::Energy pvtool;
	PVTOOL::Energy::ManufactureData &manuData = pvtool.m_manuData;

	if(withTSV){
		//set filenames
		IBK::Path dataFilename(args.args()[1]);
		IBK::Path pvFilename(args.args()[2]);
		std::string test = dataFilename.absolutePath().c_str();

		//check filenames
		if(!dataFilename.exists()){
			IBK::IBK_Message(IBK::FormatString("Data file name: '%1'.").arg(dataFilename), IBK::MSG_ERROR);
			return EXIT_FAILURE;
		}
		if(!pvFilename.exists()){
			IBK::IBK_Message(IBK::FormatString("PV file name: '%1'.").arg(pvFilename), IBK::MSG_ERROR);
			return EXIT_FAILURE;
		}

		//read files
		IBK::FileReader dataFile(dataFilename);
		IBK::FileReader pvFile(pvFilename);

		std::vector<std::string> dataLines, pvLines;
		dataFile.readAll(dataFilename, dataLines, std::vector<std::string>{"\n"});

		if(dataLines.empty()){
			IBK::IBK_Message("Datasheet is empty. Can't find values.`", IBK::MSG_ERROR);
			return EXIT_FAILURE;
		}
		std::vector<std::string> allData(1, dataLines.front());
		std::vector<double> tempDblVec(dataLines.size()-1), radDblVec(dataLines.size()-1);
		for (unsigned int i=0; i < dataLines.size(); ++i){
			const std::string &line = dataLines[i];
			//skip empty lines and first line (header)
			if (line.empty() || i==0)
				continue;
			std::vector<std::string> tempData;
			tempData = IBK::explode(line, '\t');
			if( tempData.size()<3){
				//continue;//das muss dann wieder weg
				//TODO Mira Fehler abbruch
				IBK::IBK_Message("Missing values. If no values available, please assume zero.", IBK::MSG_ERROR);
				return EXIT_FAILURE;
			}
			try {
				tempDblVec[i-1] = IBK::string2val<double>(tempData[2]);
				radDblVec[i-1] = IBK::string2val<double>(tempData[1]);
			}  catch (IBK::Exception &ex) {
				ex.writeMsgStackToError();
				IBK::IBK_Message("Invalid Datatype.", IBK::MSG_ERROR, FUNC_ID);
				return EXIT_FAILURE;
			}



			//allData.push_back(line.front() + "\t");

		}
		IBK::UnitVector tempVec(tempDblVec.size(),IBK::Unit("C")), radVec(radDblVec.size(),IBK::Unit("W/m2")), energyVec;
		tempVec.m_data.swap(tempDblVec);
		radVec.m_data.swap(radDblVec);

		energyVec.m_unit = IBK::Unit("W");

		pvFile.readAll(pvFilename, pvLines, std::vector<std::string>{"\n"});

		//we must have 2 lines
		//second line contains values
		if(pvLines.size()<2){
			IBK::IBK_Message(IBK::FormatString("PV file data is invalid: '%1'.").arg(pvFilename), IBK::MSG_ERROR);
			return EXIT_FAILURE;
		}

		//eplode 2nd line in pv data
		std::vector<std::string> pv = IBK::explode(pvLines[1], '\t');
		if(pv.size() != 10 ){
			//Todo mira
		}

		try {
			manuData.m_vmp =	IBK::string2val<double>(pv[0]);
			manuData.m_imp =	IBK::string2val<double>(pv[1]);
			manuData.m_voc =	IBK::string2val<double>(pv[2]);
			manuData.m_isc =	IBK::string2val<double>(pv[3]);
			manuData.m_alpha =	IBK::string2val<double>(pv[4]);
			manuData.m_beta =	IBK::string2val<double>(pv[5]);
			manuData.m_gamma =	IBK::string2val<double>(pv[6]);
			manuData.m_nSer =	IBK::string2val<int>(pv[7]);
			manuData.m_refTemp =	IBK::string2val<double>(pv[8]);

			if (pv[9] == "monoSi")
				manuData.m_material = static_cast<int>(PVTOOL::Energy::ManufactureData::CellType::monoSi);
			else if (pv[9] == "CdTe")
				manuData.m_material = static_cast<int>(PVTOOL::Energy::ManufactureData::CellType::CdTe);
			else if (pv[9] == "CIS")
				manuData.m_material = static_cast<int>(PVTOOL::Energy::ManufactureData::CellType::CIS);
			else if (pv[9] == "CIGS")
				manuData.m_material = static_cast<int>(PVTOOL::Energy::ManufactureData::CellType::CIGS);
			else if (pv[9] == "multiSi")
				manuData.m_material = static_cast<int>(PVTOOL::Energy::ManufactureData::CellType::multiSi);
			else if (pv[9] == "Amorphous")
				manuData.m_material = static_cast<int>(PVTOOL::Energy::ManufactureData::CellType::Amorphous);
			else
				throw IBK::Exception(IBK::FormatString("PV material is not valid. Please use one of the following materials:\n "
													   "monoSi, CdTe, CIS, CIGS, multiSi, Amorphous"), FUNC_ID);


			// compute derived parameters, may throw an exception
			pvtool.calcPhysicalParameterFromManufactureData();
		}  catch (IBK::Exception  &ex) {
			ex.writeMsgStackToError();
			IBK::IBK_Message("PV data is missing or not valid. Please check with datasheet.", IBK::MSG_ERROR, FUNC_ID);
			return EXIT_FAILURE;
		}


		//calculate
		tempVec.convert(IBK::Unit("K"));

		pvtool.calcPVEnergy(tempVec.m_data, radVec.m_data, energyVec.m_data);

		//write output
		dataLines.front() += "\tPower PV [W]";
		for(unsigned int i=0; i<energyVec.size(); ++i){
			dataLines[i+1] += "\t" + IBK::val2string(energyVec[i]);
		}
		std::ofstream myfile;
		myfile.open (dataFilename.absolutePath().c_str());
		for(auto &d : dataLines)
			myfile << d + "\n";
		myfile.close();
		IBK::IBK_Message("All done.\n", IBK::MSG_PROGRESS, FUNC_ID);

	}
	else{

		//read PV module data
		try {
			manuData.m_vmp =	IBK::string2val<double>(argv[1]);
			manuData.m_imp =	IBK::string2val<double>(argv[2]);
			manuData.m_voc =	IBK::string2val<double>(argv[3]);
			manuData.m_isc =	IBK::string2val<double>(argv[4]);
			manuData.m_alpha =	IBK::string2val<double>(argv[5]);
			manuData.m_beta =	IBK::string2val<double>(argv[6]);
			manuData.m_gamma =	IBK::string2val<double>(argv[7]);
			manuData.m_nSer =	IBK::string2val<int>(argv[8]);
			manuData.m_refTemp =	IBK::string2val<double>(argv[9]);

			if (args.args()[10] == "monoSi")
				manuData.m_material = static_cast<int>(PVTOOL::Energy::ManufactureData::CellType::monoSi);
			else if (args.args()[10] == "CdTe")
				manuData.m_material = static_cast<int>(PVTOOL::Energy::ManufactureData::CellType::CdTe);
			else if (args.args()[10] == "CIS")
				manuData.m_material = static_cast<int>(PVTOOL::Energy::ManufactureData::CellType::CIS);
			else if (args.args()[10] == "CIGS")
				manuData.m_material = static_cast<int>(PVTOOL::Energy::ManufactureData::CellType::CIGS);
			else if (args.args()[10] == "multiSi")
				manuData.m_material = static_cast<int>(PVTOOL::Energy::ManufactureData::CellType::multiSi);
			else if (args.args()[10] == "Amorphous")
				manuData.m_material = static_cast<int>(PVTOOL::Energy::ManufactureData::CellType::Amorphous);
			else
				throw IBK::Exception(IBK::FormatString("PV material is not valid. Please use one of the following materials:\n "
													   "monoSi, CdTe, CIS, CIGS, multiSi, Amorphous"), FUNC_ID);


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

				std::string filename;

				if (args.hasOption('o')) {
					filename = args.option('o');
					if(filename.substr(filename.find_last_of(".") + 1) != "d6o")
						filename += "d6o";
				} else
					filename = "pvEnergy.d6o";

				IBK::Path cTemp("c:/temp/");

				if(!cTemp.exists())
				{
					throw IBK::Exception(IBK::FormatString("Directory: '%1' is missing. Please create directory.").arg(cTemp.str()), FUNC_ID);
					return EXIT_FAILURE;
				}

				writeResultData(cTemp, pvEnergyVec, filename);
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
				temp = IBK::string2val<double>(args.args()[11]); // K
				rad = IBK::string2val<double>(args.args()[12]); // W/m2
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
	}
	return EXIT_SUCCESS;
}
