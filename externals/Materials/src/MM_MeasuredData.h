#ifndef MM_MeasuredDataH
#define MM_MeasuredDataH

#include <IBK_UnitVector.h>
#include <IBK_NotificationHandler.h>

#include <map>
#include <string>

#include "MM_Types.h"

namespace MM {

/*!	This container stores the experiment data created in the IBK laboratory.

	These fields becomes most relevant for material creator
	projects and may need improvment when a new material creator becomes
	implemented.
*/
class MeasuredData {
public:

	// ***KEYWORDLIST-START***

	/*! Various top-level section headers within Measured data */
	enum Sections {
		S_GEOMETRY,		// Keyword: GEOMETRY
		S_EXPERIMENTS,	// Keyword: EXPERIMENTS
		NUM_Sections
	};

	/*!	This enumeration describes value indices for Water Retention values
		in the summary page of a material excel file. It works also
		for Sorption Isotherm tables as well as adsorption and
		desorption processes. For Sorption Isotherms the user must
		recalculate phi to pc, while reading the excel file template.
	*/
	enum WaterRetentionValues {
		WR_NON_LOG_PC,		// Keyword: NON_LOG_PC [Pa]
		WR_TEMPERATURE,		// Keyword: TEMPERATURE [K]
		WR_THETA_L,			// Keyword: THETA_L [m3/m3]
		WR_STD_DEVIATION,	// Keyword: STD_DEVIATION [-]
		WR_MINIMUM,			// Keyword: MINIMUM [m3/m3]
		WR_MAXIMUM,			// Keyword: MAXIMUM [m3/m3]
		NUM_WaterRetentionValues
	};

	/*!	This enumeration describes values in the water vapour permeability table in a summary page of a
		material excel file. (DRY_CUP, WET_CUP, MEAN_CUP)
	*/
	enum WaterVapourPermeabilityValues {
		VP_PHI_INSIDE,		// Keyword: PHI_INSIDE [%]
		VP_PHI_OUTSIDE,		// Keyword: PHI_OUTSIDE [%]
		VP_MEW,				// Keyword: MEW [-]
		VP_STD_DEVIATION,	// Keyword: STD_DEVIATION [-]
		VP_MINIMUM,			// Keyword: MINIMUM [-]
		VP_MAXIMUM,			// Keyword: MAXIMUM [-]
		NUM_WaterVapourPermeabilityValues
	};

	/*! Types used to describe statistic variation of liquid transport coefficients. */
	enum LiquidWaterConductivityValues {
		LC_STD_DEVIATION,	// Keyword: STD_DEVIATION [-]
		LC_MINIMUM,			// Keyword: MINIMUM [s]
		LC_MAXIMUM,			// Keyword: MAXIMUM [s]
		NUM_LiquidWaterConductivityValues
	};

	/*! Types used to parametrize absorption (imbibition) experiments. */
	enum WaterUptakeValues {
		WU_MASS,					// Keyword: MASS [Kg] 'Mass',
		NUM_WaterUptakeValues
	};

	/*! Types used to parametrize drying experiment Type 0. */
	enum DryingValues0 {
		D_WEIGHT,				// Keyword: WEIGHT [m] ''
		D_WIDTH_0,				// Keyword: WIDTH_0 [m] ''
		D_LENGTH_0,				// Keyword: LENGTH_0 [m] ''
		D_HEIGHT_0,				// Keyword: HEIGHT_0 [m] ''
		D_WIDTH_1,				// Keyword: WIDTH_1 [m] ''
		D_LENGTH_1,				// Keyword: LENGTH_1 [m] ''
		D_HEIGHT_1,				// Keyword: HEIGHT_1 [m] ''
		NUM_DryingValues0
	};

	/*! Types used to parametrize drying experiment Type 1. */
	enum DryingValues1 {
		D_BC_TEMPERATURE_0,		// Keyword: BC_TEMPERATURE_0 [K] 'Boundary Temperature'
		D_BC_RH_0,				// Keyword: BC_RH_0 [%] 'Boundary Relative Humidity'
		D_BC_TEMPERATURE_1,		// Keyword: BC_TEMPERATURE_1 [K] 'Boundary Temperature'
		D_BC_RH_1,				// Keyword: BC_RH_1 [%] 'Boundary  Relative Humidity'
		D_SAMPLE_TEMP_0,			// Keyword: SAMPLE_TEMPERATURE_0 [K] 'Sample Temperature'
		D_SAMPLE_TEMP_1,			// Keyword: SAMPLE_TEMPERATURE_1 [K] 'Sample Temperature'
		D_SAMPLE_TEMP_2,			// Keyword: SAMPLE_TEMPERATURE_2 [K] 'Sample Temperature'
		D_SAMPLE_TEMP_3,			// Keyword: SAMPLE_TEMPERATURE_3 [K] 'Sample Temperature'
		D_SAMPLE_TEMP_4,			// Keyword: SAMPLE_TEMPERATURE_4 [K] 'Sample Temperature'
		D_SAMPLE_TEMP_5,			// Keyword: SAMPLE_TEMPERATURE_5 [K] 'Sample Temperature'
		NUM_DryingValues1
	};

	/*! Types used to describte geometry of an IBK specimen. */
	enum GeometryKeys {
		SPECIMEN,		// Keyword: SPECIMEN
		CYLINDRIC,		// Keyword: CYLINDRIC
		WIDTH_0,		// Keyword: WIDTH_0
		WIDTH_1,		// Keyword: WIDTH_1
		WIDTH_2,		// Keyword: WIDTH_2
		WIDTH_3,		// Keyword: WIDTH_3
		LENGTH_0,		// Keyword: LENGTH_0
		LENGTH_1,		// Keyword: LENGTH_1
		LENGTH_2,		// Keyword: LENGTH_2
		LENGTH_3,		// Keyword: LENGTH_3
		HEIGHT_0,		// Keyword: HEIGHT_0
		HEIGHT_1,		// Keyword: HEIGHT_1
		HEIGHT_2,		// Keyword: HEIGHT_2
		HEIGHT_3,		// Keyword: HEIGHT_3
		NUM_GeometryKeys
	};

	/*!	Types  for all values in an experimental section. */
	enum ExperimentKeys {
		EK_SPECIMEN,			// Keyword: SPECIMEN
		EK_EXPERIMENT,			// Keyword: EXPERIMENT
		EK_X_VALUES,			// Keyword: X_VALUES
		EK_NON_LOG_PC,			// Keyword: NON_LOG_PC [Pa]
		EK_TEMPERATURE,			// Keyword: TEMPERATURE [K]
		EK_THETA_L,				// Keyword: THETA_L [m3/m3]
		EK_PHI_INSIDE,			// Keyword: PHI_INSIDE [%]
		EK_PHI_OUTSIDE,			// Keyword: PHI_OUTSIDE [%]
		EK_MEW,					// Keyword: MEW [-]
		EK_MINIMUM,				// Keyword: MINIMUM [-]
		EK_MAXIMUM,				// Keyword: MAXIMUM [-]
		EK_STD_DEVIATION,		// Keyword: STD_DEVIATION [-]
		EK_DATE_SIZE,			// Kexword: DATE_SIZE [s]
		EK_WEIGHT,				// Keyword: WEIGHT [m]
		EK_WIDTH_0,				// Keyword: WIDTH_0 [m]
		EK_LENGTH_0,			// Keyword: LENGTH_0 [m]
		EK_HEIGHT_0,			// Keyword: HEIGHT_0 [m]
		EK_WIDTH_1,				// Keyword: WIDTH_1 [m]
		EK_LENGTH_1,			// Keyword: LENGTH_1 [m]
		EK_HEIGHT_1,			// Keyword: HEIGHT_1 [m]
		EK_BC_DATE,				// Keyword: DATE_BC [s]
		EK_BC_TEMPERATURE_0,	// Keyword: BC_TEMPERATURE_0 [K]
		EK_BC_RH_0,				// Keyword: BC_RH_0 [%]
		EK_BC_TEMPERATURE_1,	// Keyword: BC_TEMPERATURE_1 [K]
		EK_BC_RH_1,				// Keyword: BC_RH_1 [%]
		EK_SAMPLE_TEMP_0,		// Keyword: SAMPLE_TEMPERATURE_0 [K]
		EK_SAMPLE_TEMP_1,		// Keyword: SAMPLE_TEMPERATURE_1 [K]
		EK_SAMPLE_TEMP_2,		// Keyword: SAMPLE_TEMPERATURE_2 [K]
		EK_SAMPLE_TEMP_3,		// Keyword: SAMPLE_TEMPERATURE_3 [K]
		EK_SAMPLE_TEMP_4,		// Keyword: SAMPLE_TEMPERATURE_4 [K]
		EK_SAMPLE_TEMP_5,		// Keyword: SAMPLE_TEMPERATURE_5 [K]
		NUM_ExperimentKeys
	};


	/*!	Complete list of experiments executed at the IBK. Drying
		consists of two tables entries (Type 0 and Type 1) since size of a specimen
		might change during experiments.
	*/
	enum Experiments {
		WET_CUP					= 0x00000001,		// Keyword: WET_CUP 'K_l(Thetal)'
		MED_CUP					= 0x00000002,		// Keyword: MED_CUP 'K_l(Thetal)'
		DRY_CUP					= 0x00000004,		// Keyword: DRY_CUP 'K_l(Thetal)'
		WATER_RETENTION			= 0x00000008,		// Keyword: WATER_RETENTION ''
		SORPTION_ISOTHERM		= 0x00000010,		// Keyword: SORPTION_ISOTHERM ''
		WATER_UPTAKE			= 0x00000020,		// Keyword: WATER_UPTAKE ''
		DRYING_0				= 0x00000040,		// Keyword: DRYING_0 ''
		DRYING_1				= 0x00000080,		// Keyword: DRYING_1 ''
		ExperimentsMask			= 0x00FFFFFF,
		NUM_Experiments
	};
	// ***KEYWORDLIST-END***


	/*!
		ExperimentFlags are used in-conjunction with the Experiments enumeration. Thus
		the maximum experiment flag can be 0x00FFFFFF.
	*/
	enum ExperimentFlags{
		DIRECTION_U = 0x01000000,				// Keyword: DIRECTION_U 'material direction u'
		DIRECTION_V = 0x02000000,				// Keyword: DIRECTION_V 'material direction v'
		DIRECTION_W = 0x04000000,				// Keyword: DIRECTION_W 'material direction w'
		MATERIAL_DIRECTION_MASK = 0x07000000,
		DESORPTION = 0x40000000,				// Keyword: DESORPTION 'adsorption flag'
		ADSORPTION = 0x80000000,				// Keyword: ADSORPTION 'desorption flag'
		DIRECTION_MASK = 0xC0000000
	};


	/*!
		Incorporates a measured data set assigned to a sample.
		The number of y Values depends on executed experiment.
	*/
	struct DataSample {
		void clear() { m_assignedExperiment = 0; m_xValue.clear(); m_yValue.clear(); }
		unsigned int							m_assignedExperiment;	///< the experiment to which this data set is assigned
		IBK::UnitVector							m_xValue;				///< the independent value
		std::vector< IBK::UnitVector >			m_yValue;				///< the dependent values must have same length as x values
	};

	/*!
		Defines the geometry of a data sample.
		\todo Naming convention!
	*/
	struct GeometryData {
		bool	isCylindric;
		double	width[4];
		double	length[4];
		double	height[4];
	};


	/*! Returns a const reference to the additional data mapping. */
	const std::map< std::string, std::vector< DataSample > >  &data() const { return m_data; }

	/*! Sets the additional data mapping. */
	void setData(const std::map< std::string, std::vector< DataSample > >& d) { m_data = d; }

	/*!
		Generic storage member for other data (e.g. experimental values).
		Stores all experiments according to a specimen identifier.
		A speciment identifier contains a user defined text (no umlauts, no äöü, no "_",
		no non-printable characters)
		followed by "_" followed by sample ID (string, all specimen share the same sample ID),
		followed by "_" followed by specimen ID (unsigned int);

	*/
	std::map< std::string, std::vector< DataSample > >	m_data;

	/*!
		Generic storage member for geometry proportions of a speciment.
		Stores all geometry information according to a specimen identifier.
		A specimen identifier contains a user defined text (no umlauts, no äöü, no "_",
		no non-printable characters)
		followed by "_" followed by sample ID (string, all speciment share the same sample ID),
		followed by "_" followed by speciment ID (unsigned int);

	*/
	std::map< std::string, GeometryData >	m_geometry;


	/*! ctor */
	MeasuredData();

	/*! Reads moisture transport data from the string 'data'. */
	void read(const std::string& data);

	/*! Reads moisture transport data from the string 'data'. */
	void read(const std::vector<std::string>& data);


	/*! Writes the moisture transport data into the stream 'out' using
		'indent' spaces for indentation.
		Writes in UTF8 encoding.
	*/
	void write(std::ostream& out, unsigned int indent=0, MaterialDirection direction=U_DIRECTION, bool writeEmptyKeywords=false ) const;


	/*! Writes a parameter into the output stream.
		It uses the binary representation.
		\param out	The output stream.
	*/
	void writeBinary(std::ostream& out) const;


	/*! Reads a parameter from the stream 'in'.
		It uses the binary representation.
		\param in	The input stream.
	*/
	void readBinary(std::istream& in);


	/*! Clears the data.*/
	void clear();


private:
	void readGeometry( const std::string & data );


};

/*! \file MM_MeasuredData.h
	\brief Contains the class interface for measured data access functions.
*/


} // namespace MM

#endif // MM_MeasuredDataH
