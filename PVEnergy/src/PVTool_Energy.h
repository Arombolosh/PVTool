#ifndef PVTOOL_ENERGY_H
#define PVTOOL_ENERGY_H

#include <IBK_UnitVector.h>
#include <6par_solve.h>

namespace PVTool {


class Energy
{
public:
	/*! Manufacture dataset for PV module under STC.*/
	struct ManufactureData{

		int				m_material;			///<Material der Zellen				[-]
		double			m_vmp;				///<Spannung im Max Power Point		[V]
		double			m_imp;				///<Strom im Max Power Point		[A]
		double			m_voc;				///<Leerlaufspannung				[V]
		double			m_isc;				///<Kurzschlussstrom				[A]
		double			m_alpha;			///<Temperaturkoeff. Strom			[%/K]
		double			m_beta;				///<Temperaturkoeff. Spannung		[%/K]
		double			m_alphaIsc;			///<Temperaturkoeff. Strom			[A/K]
		double			m_betaVoc;			///<Temperaturkoeff. Spannung		[V/K]
		double			m_gamma;			///<Temperaturkoeff. Leistung		[%/K]
		int				m_nSer;				///<Zellen pro Modul.				[-]
		double			m_refTemp;			///<Referencetemperature			[K]

		/*! Calculates the coeff. alphaIsc and betaVoc */
		void calcCoefficients(){
			m_alphaIsc = m_alpha * m_isc;
			m_betaVoc = m_beta * m_voc;
		}

		void testValues(){
			m_material = module6par::monoSi;
			m_vmp = 32.24;
			m_imp = 9.27;
			m_voc = 39.75;
			m_isc = 9.76;
			m_alpha = 0.00043;
			m_beta = -0.0031;
			m_gamma = -0.41;
			m_nSer = 60;
			m_refTemp = 25+273.15;

			calcCoefficients();
		}
	};

	struct PhysicalDataPV{
		double m_iLRef;			///<light current in A
		double m_iORef;			///<diode reverse saturation current in A
		double m_rS;			///<series resistance in ohm
		double m_rShRef;		///<shunt resistance in ohm
		double m_a;				///<modified ideality factor
		double m_adj;			///<adjustment parameter
	};


	/*! Produces calculation parameters from manufacture dataset for pv-module. Returns the errors from the SAM-Lib.
		0 -> no errors. */
	void calcPhysicalParameterFromManufactureData( double eps = 1E-7);

	/*! Calculates the produced energy of the PV module for all timepoints
	 *	inputs:
	 *		absolute temperature [K]
	 *		radiation imposed on the PV module. [W/m2]*/
	void calcPVEnergy(const std::vector<double> &absTemp, const std::vector<double> &rad, std::vector<double> &energyPV) const;

	ManufactureData				m_manuData;			///< manufacture dataset for pv module
	PhysicalDataPV				m_pvData;			///< pv calculation parameters for physical equation
private:

};
}

#endif // PVTOOL_ENERGY_H
