#include "PCM_Material.h"

#include <MM_Material.h>




void PCM_Material::createM6File(const IBK::Path &path )
{
	readInputs();

	MM::Material mat, insulation;

	mat.m_identification.m_name.setString(m_name, "de");
	mat.m_identification.m_productId.setString("PVTool", "de");
	mat.m_identification.m_producer.setString("Rubitherm", "de");
	mat.m_identification.m_flags =
			MM::MaterialIdentification::AIR_TIGHT &
			MM::MaterialIdentification::WATER_TIGHT &
			MM::MaterialIdentification::VAPOR_TIGHT;
	mat.m_identification.m_cat = MM::MaterialCategory::Miscellaneous;

	mat.m_id = 1001001;

	IBK::Path filename(path / (m_name+"_"+IBK::val2string(mat.m_id)+".m6"));

	mat.m_paraTransport[MM::Material::MP_LAMBDA].set("LAMBDA",m_para[P_Conductivity].get_value(IBK::Unit("W/mK")), IBK::Unit("W/mK"));
	mat.m_paraStorage[MM::Material::MP_RHO].set("RHO",m_para[P_Density].get_value(IBK::Unit("kg/m3")),IBK::Unit("kg/m3"));
	//mat.m_paraStorage[MM::Material::MP_CE].set(m_para[P_SpecHeat].get_value(IBK::Unit("J/kgK")), IBK::Unit("J/kgK"));

	mat.m_thermalStorage.m_spline[MM::ThermalStorage::F_u_T] = m_linSpl;

	mat.write(filename);

	insulation.m_identification.m_name.setString("Insulation", "de");
	insulation.m_identification.m_productId.setString("PVTool", "de");
	insulation.m_identification.m_producer.setString("PVTool", "de");
	insulation.m_identification.m_flags =
			MM::MaterialIdentification::AIR_TIGHT &
			MM::MaterialIdentification::WATER_TIGHT &
			MM::MaterialIdentification::VAPOR_TIGHT;

	insulation.m_identification.m_cat = MM::MaterialCategory::Insulations;
	insulation.m_id = 1001002;
	insulation.m_paraTransport[MM::Material::MP_LAMBDA] = IBK::Parameter("LAMBDA", 0.035, IBK::Unit("W/mK"));
	insulation.m_paraStorage[MM::Material::MP_RHO] = IBK::Parameter("RHO", 35, IBK::Unit("kg/m3"));
	insulation.m_paraStorage[MM::Material::MP_CE] = IBK::Parameter("CE", 1500, IBK::Unit("J/kgK"));

	IBK::Path insuFilename (path / ("Insulation_" + IBK::val2string(insulation.m_id) + ".m6"));

	insulation.write(insuFilename);
}

void PCM_Material::readInputs()
{
	m_specCapacityCurve[IBK::Parameter("Temperature",16,IBK::Unit("C"))] = IBK::Parameter("Enthalpy", 4, IBK::Unit("kJ"));
	m_specCapacityCurve[IBK::Parameter("Temperature",17,IBK::Unit("C"))] = IBK::Parameter("Enthalpy", 3, IBK::Unit("kJ"));
	m_specCapacityCurve[IBK::Parameter("Temperature",18,IBK::Unit("C"))] = IBK::Parameter("Enthalpy", 3, IBK::Unit("kJ"));
	m_specCapacityCurve[IBK::Parameter("Temperature",19,IBK::Unit("C"))] = IBK::Parameter("Enthalpy", 4, IBK::Unit("kJ"));
	m_specCapacityCurve[IBK::Parameter("Temperature",20,IBK::Unit("C"))] = IBK::Parameter("Enthalpy", 5, IBK::Unit("kJ"));
	m_specCapacityCurve[IBK::Parameter("Temperature",21,IBK::Unit("C"))] = IBK::Parameter("Enthalpy", 6, IBK::Unit("kJ"));
	m_specCapacityCurve[IBK::Parameter("Temperature",22,IBK::Unit("C"))] = IBK::Parameter("Enthalpy", 12, IBK::Unit("kJ"));
	m_specCapacityCurve[IBK::Parameter("Temperature",23,IBK::Unit("C"))] = IBK::Parameter("Enthalpy", 22, IBK::Unit("kJ"));
	m_specCapacityCurve[IBK::Parameter("Temperature",24,IBK::Unit("C"))] = IBK::Parameter("Enthalpy", 27, IBK::Unit("kJ"));
	m_specCapacityCurve[IBK::Parameter("Temperature",25,IBK::Unit("C"))] = IBK::Parameter("Enthalpy", 50, IBK::Unit("kJ"));
	m_specCapacityCurve[IBK::Parameter("Temperature",26,IBK::Unit("C"))] = IBK::Parameter("Enthalpy", 25, IBK::Unit("kJ"));
	m_specCapacityCurve[IBK::Parameter("Temperature",27,IBK::Unit("C"))] = IBK::Parameter("Enthalpy", 7, IBK::Unit("kJ"));
	m_specCapacityCurve[IBK::Parameter("Temperature",28,IBK::Unit("C"))] = IBK::Parameter("Enthalpy", 4, IBK::Unit("kJ"));
	m_specCapacityCurve[IBK::Parameter("Temperature",29,IBK::Unit("C"))] = IBK::Parameter("Enthalpy", 4, IBK::Unit("kJ"));
	m_specCapacityCurve[IBK::Parameter("Temperature",30,IBK::Unit("C"))] = IBK::Parameter("Enthalpy", 4, IBK::Unit("kJ"));
	m_specCapacityCurve[IBK::Parameter("Temperature",31,IBK::Unit("C"))] = IBK::Parameter("Enthalpy", 5, IBK::Unit("kJ"));

	m_specCapacityCurve[IBK::Parameter("Temperature",-273.15,IBK::Unit("C"))] = IBK::Parameter("Enthalpy", 0, IBK::Unit("kJ"));
	m_specCapacityCurve[IBK::Parameter("Temperature",100,IBK::Unit("C"))] = IBK::Parameter("Enthalpy", 4, IBK::Unit("kJ"));

	std::vector<double> temp;
	std::vector<double> ent;

	for (auto &specObj : m_specCapacityCurve) {
		if(temp.size()==0){
			temp.push_back(specObj.first.get_value(IBK::Unit("K")));
			ent.push_back(specObj.second.get_value(IBK::Unit("J")));
		}
		else {
			bool isInsert = false;
			for(size_t i=0; i< temp.size(); ++i){
				double absTemp = specObj.first.get_value(IBK::Unit("K"));
				if(absTemp<temp[i]){
					temp.insert(temp.begin()+i, absTemp);
					ent.insert(ent.begin()+i, specObj.second.get_value(IBK::Unit("J")));
					isInsert = true;
					break;
				}
			}
			if(!isInsert){
				temp.push_back(specObj.first.get_value(IBK::Unit("K")));
				ent.push_back(specObj.second.get_value(IBK::Unit("J")));
			}
		}
	}

	//read name of pcm
	m_name = "SP26";

	//read parameters
	m_para[P_Density] = IBK::Parameter("Density", 1500, IBK::Unit("kg/m3"));
	m_para[P_Conductivity] = IBK::Parameter("Conductivity", 0.5, IBK::Unit("W/mK"));

	for (unsigned int i=1;i<temp.size();++i)
		ent[i] = m_para[P_Density].get_value(IBK::Unit("kg/m3")) * (temp[i] - temp[i-1]) * ent[i] + ent[i-1];

	m_linSpl.setValues(temp, ent);

}


