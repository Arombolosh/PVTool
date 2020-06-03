#include <iostream>
#include <limits>

#include <IBK_assert.h>
#include <IBK_messages.h>
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



int main(int argc, char* argv[])
{
	IBK::ArgParser args;

	args.m_appname = "PVEnergy";
	//option for values double, string, etc.
	args.addOption('p', "path", "Input path for d6o result files.", "","");
	//flags for bool
	args.addFlag('v', "version", "Displays current version");
	args.addFlag('e', "enable", "enables something", "true");

	args.parse(argc, argv);

	//help
	if(args.handleDefaultFlags(std::cout)){
		return EXIT_SUCCESS;
	}

	if(args.flagEnabled('v')){
		IBK::IBK_Message(IBK::FormatString("PVEnergy version: %1 \n").arg(LONG_VERSION));
		return EXIT_SUCCESS;
	}

	//32.24 9.27 39.75 9.76 0.00013 -0.0031 -0.41 60 298.15 monoSi 30 300

	//PCM_Material pcm;
	//pcm.createM6File(IBK::Path("c:/temp/"));

	/*
		pv_tool.exe vmp imp voc isc alpha beta gamma nSer refTemp Material T Rad
		pv_tool.exe vmp imp voc isc alpha beta gamma nSer refTemp Material -f path
			returns Energy

	*/

	if(argc<13){
		std::cout << "Not enough input parameter." << std::endl;
		helpDocumentation();
	}
	else {
		PVTOOL::Energy pvtool;
		PVTOOL::Energy::ManufactureData &manuData = pvtool.m_manuData;
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
		} catch (IBK::Exception  &ex) {
			std::cout << "PV-Data invalid!" << std::endl;
			std::cout << ex.what() << std::endl;
			return EXIT_FAILURE;
		}

		//material is not taken into account yet; Default monoSi
		if(std::strcmp(argv[10], "monoSi")==0){
			manuData.m_material = 0;
		}
		else {
			manuData.m_material = 0;
		}

		//finished read PV module data
		//read temperature and radiation data
		IBK::UnitVector tempVec, radVec, pvEnergyVec;
		try {
			IBK::Path path;
			if(std::strcmp(argv[11], "-f") == 0){
				path = IBK::Path(argv[12]);

				extractDataFromD6Results(path, tempVec, radVec);
			}
			else {
				try {
					tempVec.set(1,IBK::string2val<double>(argv[11]), "K");
					radVec.set(1,IBK::string2val<double>(argv[12]), "W/m2");
				} catch (IBK::Exception  &ex) {
					std::cout << "Temperature or radiation invalid!" << std::endl;
					std::cout << ex.what() << std::endl;
					ex.writeMsgStackToError();
					return EXIT_FAILURE;
				}
			}

		} catch (IBK::Exception &ex) {
			std::cout << ex.what() << std::endl;
		}

		tempVec.convert(IBK::Unit("K"));
		radVec.convert(IBK::Unit("W/m2"));
		pvEnergyVec.m_unit = IBK::Unit("W");

		//Calculation of physical PV data
		try {
			pvtool.calcPhysicalParameterFromManufactureData();
			pvtool.calcPVEnergy(tempVec.m_data, radVec.m_data, pvEnergyVec.m_data);

		} catch (IBK::Exception  &ex) {
			std::cout << "PV calculation did not succeed!" << std::endl;
			std::cout << ex.what() << std::endl;
			ex.writeMsgStackToError();
			return EXIT_FAILURE;
		}
		writeResultData(IBK::Path("c:/temp/"), pvEnergyVec);

		std::cout << "Program Exit" << std::endl;


		return EXIT_SUCCESS;
	}






	return EXIT_SUCCESS;



}
