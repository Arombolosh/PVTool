#include "PVTool_Energy.h"

#include <IBK_Exception.h>
#include <IBK_messages.h>
#include <iostream>

namespace PVTOOL {


Energy::Energy() {
//	double m_egRef = module6par().bandgap();			//Materialabhängig ToDo Katja


}

void Energy::calcPhysicalParameterFromManufactureData(double eps) {
	FUNCID(Energy::calcPhysicalParameterFromManufactureData);

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

	if (err !=0) {
		throw IBK::Exception(IBK::FormatString("Error iteration did not converge. Please check your input parameter."), FUNC_ID);
	}

	//outputs
	m_pvData.m_a = m6par.a;
	m_pvData.m_rS = m6par.Rs;
	m_pvData.m_adj = m6par.Adj;
	m_pvData.m_iLRef = m6par.Il;
	m_pvData.m_iORef = m6par.Io;
	m_pvData.m_rShRef = m6par.Rsh;

#if 0
	std::cout << "iLRef:\t" << m_pvData.m_iLRef << std::endl;
	std::cout << "iORef:\t" << m_pvData.m_iORef << std::endl;
	std::cout << "rS:\t" << m_pvData.m_rS << std::endl;
	std::cout << "rShRef:\t" << m_pvData.m_rShRef << std::endl;
	std::cout << "a:\t" << m_pvData.m_a << std::endl;
	std::cout << "adj:\t" << m_pvData.m_adj << std::endl << std::endl;
#endif
}


double Energy::calcPVEnergy(double absTemp, double rad, double airMass) const {
	FUNCID(Energy::calcPVEnergy);

	//not enough radiation
	if (rad<=5){
		IBK::IBK_Message(IBK::FormatString("%1\t%2\t%3\n").arg(0).arg(0).arg(0),IBK::MSG_PROGRESS, FUNC_ID,IBK::VL_DEVELOPER);
		return 0;
	}

	//calculation of P_mpp
	double deltaT = (absTemp - m_cellTemperatureRef);
	double iL = rad / m_radiationRef * airMass / m_airMassRef
			* (m_pvData.m_iLRef+ m_manuData.m_alphaIsc * deltaT);

	double eg = m_egRef * (1 - 0.0002677 * deltaT);

	double rSh = m_pvData.m_rShRef* m_radiationRef / rad;

	double iO = m_pvData.m_iORef * std::pow((absTemp/m_cellTemperatureRef),3) *
					std::exp(1/m_sigma * (m_egRef / m_cellTemperatureRef - eg / absTemp));

	double pMax = 0;
	double current = 0;
	double volt = -0.01;
	double eps =1E-10;

	IBK::IBK_Message(IBK::FormatString("pMax (MPP) [W]\tvoltage [V]\tcurrent [A]\n"),IBK::MSG_PROGRESS,FUNC_ID,IBK::VL_DEVELOPER);


	double uMpp=volt;				//Save Voltage at Maximum Power
	double iMpp=current;			//Save Current at Maximum Power
	for (size_t j=0; j<int(2*m_manuData.m_voc / 0.01); ++j) {
		volt += 0.01;
		for(size_t i= 0; i<1000; ++i)
		{
			double i0 = current;
			current = iL - iO * (std::exp((volt + current * m_pvData.m_rS)/m_pvData.m_a)-1) - (volt + current * m_pvData.m_rS) / rSh;
			if(std::fabs(i0-current)<eps)
				break;
			if (current < 0 )
				break;
		}

		double P= volt * current; //Power at currently current and Voltage
		pMax = std::fmax(P,pMax); //Find Maximum

		if (pMax>P) {//std::abs(Pold - Pmax) < 0.0001  ) { //Wenn neues Maximum gefunden wurde

			IBK::IBK_Message(IBK::FormatString("pMax = %1 \tuMpp = %2 \tiMpp = 3\n").arg(pMax).arg(uMpp).arg(iMpp),IBK::MSG_PROGRESS,FUNC_ID,IBK::VL_DEVELOPER);

			break;
		}
		IBK::IBK_Message(IBK::FormatString("P = %1 \tvolt = %2 \tcurrent = 3\n").arg(P).arg(volt).arg(current),IBK::MSG_PROGRESS,FUNC_ID,IBK::VL_DEVELOPER);

		uMpp=volt;  //Save Voltage at Maximum Power
		iMpp=current;  //Save Current at Maximum Power
		//Pold=Pmax;
	}
	return uMpp * iMpp;
}


void Energy::calcPVEnergy(const std::vector<double> &absTemp, const std::vector<double> &rad, std::vector<double> &energyPV) const {
	const char * const FUNC_ID = "[Energy::calcPVEnergy]";

	if (absTemp.size() != rad.size())
		throw IBK::Exception(IBK::FormatString("Size of both Vectors are not equal"), FUNC_ID);

	//Energie berechnen für jeden Zeitpunkt der mit Temp. und Strahlung (und AirMass) gegeben ist.
	energyPV.clear();
	energyPV.reserve(absTemp.size());
	for (size_t k=0;k<rad.size(); ++k) {
		const double &radiation = rad[k];
		const double &cellTemperature = absTemp[k];

		double energy = calcPVEnergy(cellTemperature, radiation); // in W
		energyPV.push_back(energy);
	}
}

} // namespace PVTOOL
