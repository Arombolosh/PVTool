#ifndef PVTOOL_ENERGY_H
#define PVTOOL_ENERGY_H

#include <IBK_UnitVector.h>

#include <6par_solve.h>

namespace PVTOOL {

/*! Utility class to calculate */
class Energy {
public:
	/*! Manufacture dataset for PV module under STC.*/
	struct ManufactureData{

		ManufactureData(){}

		ManufactureData(double vmp, double imp, double voc, double isc, double alpha, double beta, double gamma, int nSer, std::string name):
			m_material(module6par::monoSi),
			m_vmp(vmp),
			m_imp(imp),
			m_voc(voc),
			m_isc(isc),
			m_alpha(alpha),
			m_beta(beta),
			m_gamma(gamma),
			m_nSer(nSer),
			m_refTemp(25+273.15),
			m_name(name)
		{}

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
		std::string		m_name;				///<Name of the PV panel

		/*! Calculates the coeff. alphaIsc and betaVoc */
		void calcCoefficients(){
			m_alphaIsc = m_alpha * m_isc*0.01;	//0.01 = change percent to unitless
			m_betaVoc = m_beta * m_voc*0.01;
		}

		void testValues(){
			m_material = module6par::monoSi;
			m_vmp = 32.24;
			m_imp = 9.27;
			m_voc = 39.75;
			m_isc = 9.76;
			m_alpha = 0.043;
			m_beta = -0.31;
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


	/*! C'tor. */
	Energy(){
		m_manuData = ManufactureData();
	}

	/*! Produces calculation parameters from manufacture dataset for pv-module.
		In case of errors, throws an IBK::Exception.
	*/
	void calcPhysicalParameterFromManufactureData( double eps = 1E-7);

	/*! Calculates the electrical energy in [W] for a given temperature and solar radiation load.
		This function is called from calcPVEnergy().

		\param absTol Absolute temperature [K]
		\param rad Imposed radiation on the PV module. [W/m2]

		\return Returns usable energy in [W].
	*/
	double calcPVEnergy(double absTemp, double rad, double airMass = 1.5) const;

	/*! Calculates the produced energy of the PV module for all timepoints
		\param absTol absolute temperature [K]
		\param rad imposed radiation on the PV module. [W/m2]
		\param energyPV Computed usable energy in [W].
	*/
	void calcPVEnergy(const std::vector<double> &absTemp, const std::vector<double> &rad, std::vector<double> &energyPV) const;

	ManufactureData				m_manuData;			///< manufacture dataset for pv module
	PhysicalDataPV				m_pvData;			///< pv calculation parameters for physical equation

private:
	//Bedingungen nach Reference Modell
	//jetzt grad STC

	double m_radiationRef		= 1000;		//in W/m2
	double m_cellTemperatureRef = 25+273.15;	// in K
	double m_airMassRef			= 1.5;			//AM: 1,5 (AM stands for Air Mass, the thickness of the atmosphere; at the equator, air mass = 1, in Europe approx. 1,5)

	//constants
	double m_sigma				= 8.617333262E-5;					//Boltzmann Konstante in eV/K
	double m_egRef = module6par().bandgap();			//Materialabhängig ToDo Katja

};


} // namespace PVTOOL

#endif // PVTOOL_ENERGY_H
