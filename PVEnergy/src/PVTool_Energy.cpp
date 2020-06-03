#include "PVTool_Energy.h"

#include <IBK_Exception.h>
#include <IBK_messages.h>
#include <iostream>

namespace PVTool {


void Energy::calcPhysicalParameterFromManufactureData(double eps)
{

	m_manuData.calcCoefficients();
	//input parameters
	module6par m6par(m_manuData.m_material,
					 m_manuData.m_vmp,
					 m_manuData.m_imp,
					 m_manuData.m_voc,
					 m_manuData.m_isc,
					 m_manuData.m_betaVoc,
					 m_manuData.m_alphaIsc,
					 m_manuData.m_gamma,
					 m_manuData.m_nSer,
					 m_manuData.m_refTemp);

	//solving
	int err = m6par.solve_with_sanity_and_heuristics<double>(300, eps);

	if(err !=0){
		throw IBK::Exception(IBK::FormatString("Error iteration did not converge. Please check your input parameter."), "calcPhysicalParameterFromManufactureData");
	}

	//outputs
	m_pvData.m_a = m6par.a;
	m_pvData.m_rS = m6par.Rs;
	m_pvData.m_adj = m6par.Adj;
	m_pvData.m_iLRef = m6par.Il;
	m_pvData.m_iORef = m6par.Io;
	m_pvData.m_rShRef = m6par.Rsh;

	std::cout << "iLRef:\t" << m_pvData.m_iLRef << std::endl;
	std::cout << "iORef:\t" << m_pvData.m_iORef << std::endl;
	std::cout << "rS:\t" << m_pvData.m_rS << std::endl;
	std::cout << "rShRef:\t" << m_pvData.m_rShRef << std::endl;
	std::cout << "a:\t" << m_pvData.m_a << std::endl;
	std::cout << "adj:\t" << m_pvData.m_adj << std::endl << std::endl;
}

void Energy::calcPVEnergy(const std::vector<double> &absTemp, const std::vector<double> &rad, std::vector<double> &energyPV) const {

	const char * const FUNC_ID = "[Energy::calcPVEnergy]";

	if(absTemp.size() != rad.size())
		throw IBK::Exception(IBK::FormatString("Size of both Vectors are not equal"), "calcPVEnergy");



	//Energie berechnen für jeden Zeitpunkt der mit Temp. und Strahlung (und AirMass) gegeben ist.
	//Vorgabe durch Delphin
	//Laden der d6o Dateien
	double airMass = 1.5;							//derzeit nicht dynamisch

//in constructor ToDo Katja
	//Bedingungen nach Reference Modell
	//jetzt grad STC
	double m_radiationRef = 1000;		//in W/m2
	double m_cellTemperatureRef = 25+273.15;	// in K
	double m_airMassRef = 1.5;			//AM: 1,5 (AM stands for Air Mass, the thickness of the atmosphere; at the equator, air mass = 1, in Europe approx. 1,5)

	//constants
	double m_sigma = 8.617333262E-5;					//Boltzmann Konstante in eV/K
	double m_egRef = module6par().bandgap();			//Materialabhängig ToDo Katja

	//Parameter von Simulation
	//zeitabhängig


	energyPV.clear();
	energyPV.reserve(absTemp.size());
	for (size_t k=0;k<rad.size(); ++k) {
		const double &radiation = rad[k];
		const double &cellTemperature = absTemp[k];

		//not enough radiation
		if(radiation<=5){
			energyPV.push_back(0);

			IBK::IBK_Message(IBK::FormatString("%1\t%2\t%3\n").arg(0).arg(0).arg(0),IBK::MSG_DEBUG, FUNC_ID,IBK::VL_DEVELOPER);

			continue;
		}

		//calculation of P_mpp
		double deltaT = (cellTemperature - m_cellTemperatureRef);
		double m_iL = radiation / m_radiationRef * airMass / m_airMassRef
				* (m_pvData.m_iLRef+ m_manuData.m_alphaIsc * deltaT);

		double m_eg = m_egRef * (1 - 0.0002677 * deltaT);

		double m_rSh = m_pvData.m_rShRef* m_radiationRef / radiation;

		double m_iO = m_pvData.m_iORef * std::pow((cellTemperature/m_cellTemperatureRef),3) *
						std::exp(1/m_sigma * (m_egRef / m_cellTemperatureRef - m_eg / cellTemperature));

		double pMax = 0;
		double m_i = 0;
		double volt = -0.01;
		double eps =1E-10;

        IBK::IBK_Message(IBK::FormatString("pMax (MPP) [W]\tvoltage [V]\tcurrent [A]\n"),IBK::MSG_DEBUG,FUNC_ID,IBK::VL_DEVELOPER);


		double m_uMpp=volt;				//Save Voltage at Maximum Power
		double m_iMpp=m_i;				//Save Current at Maximum Power
		for (size_t j=0;j<int(2*m_manuData.m_voc / 0.01); ++j) {
			volt += 0.01;
			for(size_t i= 0; i<1000; ++i)
			{
				double i0 = m_i;
				m_i = m_iL - m_iO * (std::exp((volt + m_i * m_pvData.m_rS)/m_pvData.m_a)-1) - (volt + m_i * m_pvData.m_rS) / m_rSh;
				if(std::fabs(i0-m_i)<eps)
					break;
				if (m_i < 0 )
					break;
			}

			double P= volt * m_i; //Power at currently current and Voltage
			pMax = std::fmax(P,pMax); //Find Maximum

			if (pMax>P) {//std::abs(Pold - Pmax) < 0.0001  ) { //Wenn neues Maximum gefunden wurde

                IBK::IBK_Message(IBK::FormatString("pMax\tm_uMpp\tm_iMpp\n"),IBK::MSG_DEBUG,FUNC_ID,IBK::VL_DEVELOPER);

				break;
			}
            IBK::IBK_Message(IBK::FormatString("P\tvolt\tm_i\n"),IBK::MSG_DEBUG,FUNC_ID,IBK::VL_DEVELOPER);

			m_uMpp=volt;  //Save Voltage at Maximum Power
			m_iMpp=m_i;  //Save Current at Maximum Power
			//Pold=Pmax;
		}
		energyPV.push_back(m_uMpp * m_iMpp);
	}
}
}
