#include "PVTool_Energy.h"

#include <IBK_Exception.h>
#include <IBK_messages.h>
#include <iostream>
#include <algorithm>
#include <fstream>


namespace PVTOOL {




void Energy::calcPhysicalParameterFromManufactureData(double eps) {
	FUNCID(Energy::calcPhysicalParameterFromManufactureData);

	m_manuData.calcCoefficients();
	//input parameters
	module6par m6par(m_manuData.m_material/*module6par::monoSi*/,
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

	IBK::IBK_Message(IBK::FormatString("pMax (MPP) [W]\tvoltage [V]\tcurrent [A]\n"),IBK::MSG_PROGRESS,FUNC_ID,IBK::VL_DEVELOPER);
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

	double uMpp=volt;				//Save Voltage at Maximum Power
	double iMpp=current;			//Save Current at Maximum Power
	double stepInc = 0.1;

	//Vorberechnung

	//Achtung neue Auswertung
	if(false){
		struct pvVals{
			pvVals(double i, double u) :
				i(i),
				u(u),
				p(i*u)
			{}

			double i, u, p;
		};

		std::vector<pvVals>				refValues;
		std::vector<pvVals>				tempBasedValues;

		volt = 0;
		while(true){
			for(size_t i= 0; i<1000; ++i)
			{
				double i0 = current;
				//		double tempVal1 = volt + current * m_pvData.m_rS;
				//		double tempVal2 = (std::exp((volt + current * m_pvData.m_rS)/m_pvData.m_a)-1);
				//		double tempVal3 = (volt + current * m_pvData.m_rS) / rSh;
				//		double tempVal4 = iO * (std::exp((volt + current * m_pvData.m_rS)/m_pvData.m_a)-1);
				current = iL - iO * (std::exp((volt + current * m_pvData.m_rS)/m_pvData.m_a)-1) - (volt + current * m_pvData.m_rS) / rSh;
				if(std::fabs(i0-current)<eps)
					break;
				if (current < 0 ){
					//currentIsNegative = true;
					break;
				}
			}
			if(current <0)
				break;
			tempBasedValues.push_back(pvVals(current, volt));

			volt +=0.1;
		}

		{
			double iL = m_pvData.m_iLRef;
			double eg = m_egRef;
			double rSh = m_pvData.m_rShRef;
			double iO = m_pvData.m_iORef;

			volt = 0;
			while(true){
				for(size_t i= 0; i<1000; ++i)
				{
					double i0 = current;
					current = iL - iO * (std::exp((volt + current * m_pvData.m_rS)/m_pvData.m_a)-1) - (volt + current * m_pvData.m_rS) / rSh;
					if(std::fabs(i0-current)<eps)
						break;
					if (current < 0 ){
						//currentIsNegative = true;
						break;
					}
				}
				if(current <0)
					break;
				refValues.push_back(pvVals(current, volt));

				volt +=0.1;
			}
		}
		std::ofstream out;
		out.open("c:/temp/pvPostProc.csv");

		out << "Time [min]\tIref [A]\tUref [V]\tPref [W]\tI [A]\tU [V]\tP [W]" << std::endl;
		unsigned int sizeRef, size2;
		sizeRef = refValues.size();
		size2 = tempBasedValues.size();
		unsigned int time =0;
		for(unsigned int i=0; i<std::max<unsigned int>(sizeRef, size2); ++i){
			out << time++ << "\t";
			if(i < sizeRef){
				out << refValues[i].i << "\t";
				out << refValues[i].u << "\t";
				out << refValues[i].p << "\t";
			}
			else
				out << "0\t0\t0\t";
			if(i<size2){
				out << tempBasedValues[i].i << "\t";
				out << tempBasedValues[i].u << "\t";
				out << tempBasedValues[i].p << "\t";
			}
			else
				out << "0\t0\t0\t";
			out << std::endl;
		}
	}


	// calculate current and power at MPP --> powerA
	volt = m_manuData.m_vmp;
	bool currentIsNegative = false;
	for(size_t i= 0; i<1000; ++i)
	{
		double i0 = current;
//		double tempVal1 = volt + current * m_pvData.m_rS;
//		double tempVal2 = (std::exp((volt + current * m_pvData.m_rS)/m_pvData.m_a)-1);
//		double tempVal3 = (volt + current * m_pvData.m_rS) / rSh;
//		double tempVal4 = iO * (std::exp((volt + current * m_pvData.m_rS)/m_pvData.m_a)-1);
		current = iL - iO * (std::exp((volt + current * m_pvData.m_rS)/m_pvData.m_a)-1) - (volt + current * m_pvData.m_rS) / rSh;
		if(std::fabs(i0-current)<eps)
			break;
		if (current < 0 ){
			currentIsNegative = true;
			break;
		}
	}

	if(!currentIsNegative){
		double powerA = volt * current;
		//calculate current and power right from MPP --> powerB
		volt = m_manuData.m_vmp + stepInc;
		//current = m_manuData.m_imp;
		for(size_t i= 0; i<1000; ++i)
		{
			double i0 = current;
			double tempVal1 = volt + current * m_pvData.m_rS;
			double tempVal2 = (std::exp((volt + current * m_pvData.m_rS)/m_pvData.m_a)-1);
			double tempVal3 = (volt + current * m_pvData.m_rS) / rSh;
			double tempVal4 = iO * (std::exp((volt + current * m_pvData.m_rS)/m_pvData.m_a)-1);
			current = iL - iO * (std::exp((volt + current * m_pvData.m_rS)/m_pvData.m_a)-1) - (volt + current * m_pvData.m_rS) / rSh;
			if(std::fabs(i0-current)<eps)
				break;
			if (current < 0 ){
				break;
			}

		}
		double powerB = volt * current;

		//maximum is left of the position von m_manuData.m_voc
		if(powerA > powerB)
			stepInc *= -1;
		volt = m_manuData.m_vmp;
	}
	else
		volt = 0;

	for (size_t j=0; j<int(2*m_manuData.m_voc / stepInc); ++j) {
		volt += stepInc;
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

			//IBK::IBK_Message(IBK::FormatString("pMax = %1 \tuMpp = %2 \tiMpp = %3\n").arg(pMax).arg(uMpp).arg(iMpp),IBK::MSG_PROGRESS,FUNC_ID,IBK::VL_DETAILED);

			break;
		}
		//IBK::IBK_Message(IBK::FormatString("P = %1 \tvolt = %2 \tcurrent = %3\n").arg(P).arg(volt).arg(current),IBK::MSG_PROGRESS,FUNC_ID,IBK::VL_DEVELOPER);

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
		if(k>8750){
			double x=0;
		}

		double energy = calcPVEnergy(cellTemperature, radiation); // in W
		energyPV.push_back(energy);
	}
}

} // namespace PVTOOL
