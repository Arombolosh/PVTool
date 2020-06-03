#ifndef MM_MaterialH
#define MM_MaterialH

#include <string>
#include <iomanip>
#include <iosfwd>
#include <vector>
#include <fstream>
#include <sstream>
#include <iostream>
#include <ctime>

#include <IBK_UnitList.h>
#include <IBK_Unit.h>
#include <IBK_rectangle.h>
#include <IBK_Color.h>
#include <IBK_messages.h>
#include <IBK_StringUtils.h>
#include <IBK_assert.h>
#include <IBK_math.h>
#include <IBK_MultiLanguageString.h>
#include <IBK_Parameter.h>
#include <IBK_Exception.h>
#include <IBK_InputOutput.h> // also needed for MSVC to define uint32_t
#include <IBK_Path.h>

#include "MM_Types.h"
#include "MM_Identification.h"
#include "MM_AirTransport.h"
#include "MM_ThermalStorage.h"
#include "MM_ThermalTransport.h"
#include "MM_MoistureStorage.h"
#include "MM_MoistureTransport.h"
#include "MM_MeasuredData.h"
#include "MM_VaporContactResistanceModel.h"


/*! \brief The namespace MM holds all classes and functions related to reading/writing
	and managing material data.

	The main page (\ref index) provides some introductory information and pointers on how
	to read the API documentation.
*/
namespace MM {


/*! Material container class.

	Materials can be stored in files. Use read() to read a material file. Use write() to write
	a material file. Material files can be written in binary or ASCII mode. When reading
	a file, the read() function detects automatically (based on the magic header) which format
	the file is in and calls the appropriate binary/ASCII read function. When writing you need
	to specify filename and binary/ASCII mode via an optional flag.

	When writing material data within another context use the alternative toString() and
	fromString() functions.

	See the main page (\ref index) about some general concepts of the library and the
	implementation of the Material class.
*/
class Material {
public:

	// ***KEYWORDLIST-START***

	/*! Enum type with all possible material sections.*/
	enum Sections {
		IDENTIFICATION,					// Keyword: IDENTIFICATION
		STORAGE_BASE_PARAMETERS,		// Keyword: STORAGE_BASE_PARAMETERS
		TRANSPORT_BASE_PARAMETERS,		// Keyword: TRANSPORT_BASE_PARAMETERS
		MECHANICAL_BASE_PARAMETERS,		// Keyword: MECHANICAL_BASE_PARAMETERS
		THERMAL_STORAGE,				// Keyword: THERMAL_STORAGE
		MOISTURE_STORAGE,				// Keyword: MOISTURE_STORAGE
		MOISTURE_TRANSPORT,				// Keyword: MOISTURE_TRANSPORT
		HEAT_TRANSPORT,					// Keyword: HEAT_TRANSPORT
		AIR_TRANSPORT,					// Keyword: AIR_TRANSPORT
		MOISTURE_TRANSPORT_V,			// Keyword: MOISTURE_TRANSPORT_V
		HEAT_TRANSPORT_V,				// Keyword: HEAT_TRANSPORT_V
		AIR_TRANSPORT_V,				// Keyword: AIR_TRANSPORT_V
		MOISTURE_TRANSPORT_W,			// Keyword: MOISTURE_TRANSPORT_W
		HEAT_TRANSPORT_W,				// Keyword: HEAT_TRANSPORT_W
		AIR_TRANSPORT_W,				// Keyword: AIR_TRANSPORT_W
		MEASURED_DATA,					// Keyword: MEASURED_DATA
		VAPOR_CONTACT_RESISTANCE,		// Keyword: VAPOR_CONTACT_RESISTANCE
		NUM_Sections
	};

	/*! Enum that describes pre V6 data format */
	enum SectionsPreV6 {
		/*! MATERIAL is just a dummy section header to parse 5.6 and 5.7 embedded materials with the same code. */
		MATERIAL,			// Keyword: MATERIAL
		ID,					// Keyword: IDENTIFICATION
		GENERAL,			// Keyword: GENERAL
		MOISTSTORAGE,		// Keyword: MRC
		MOISTTRANS,			// Keyword: MOISTTRANS
		HEATTRANS,			// Keyword: HEATTRANS
		AIRTRANS,			// Keyword: AIRTRANS
		PARAMETER,			// Keyword: PARAMETER
		PCMMATERIAL,		// Keyword: PCMMATERIAL
		NUM_SectionsPreV6
	};

	/*! Enum type with all possible basic parameters related to storage.*/
	enum ParaStorage {
		MP_RHO,				// Keyword: RHO					[kg/m3]			'Bulk density of dry material'
		MP_CE,				// Keyword: CE			CET		[J/kgK]			'Specific heat capacity of dry material'
		MP_THETA_POR,		// Keyword: THETA_POR	OPOR	[m3/m3]			'Open porosity'
		MP_THETA_EFF,		// Keyword: THETA_EFF	OEFF	[m3/m3]			'Effective saturation (long term process)'
		// the keywords below are additional parameters
		MP_THETA_CAP,		// Keyword: THETA_CAP	OCAP	[m3/m3]			'Capillary saturation content (short term process)'
		MP_THETA_LIMHYG,	// Keyword: THETA_LIM_HYG		[m3/m3]			'Maxiumum moisture content allowed (material must not get wet)'
		MP_THETA_80,		// Keyword: THETA_80	O80		[m3/m3]			'Hygroscopic sorption value at RH=80%'
		NUM_ParaStorage
	};

	/*! Enum type with all possible basic parameters related to transport.*/
	enum ParaTransport {
		MP_LAMBDA,			// Keyword: LAMBDA			[W/mK]			'Thermal conductivity (dry)'
		MP_AW,				// Keyword: AW				[kg/m2s05]		'Water uptake coefficient'
		MP_MEW,				// Keyword: MEW				[-]				'Water vapor diffusion resistance factor'
		MP_SD,				// Keyword: SD				[m]				'Diffusiun equivalent air layer thickness'
		MP_KLEFF,			// Keyword: KLEFF	KLSAT	[s]				'Liquid water conductivity at effective saturation'
		MP_DLEFF,			// Keyword: DLEFF			[m2/s]			'Liquid water diffusivity at effective saturation'
		MP_KG,				// Keyword: KG		KGDRY	[s]				'Air permeability'
		MP_LAMBDA_V,		// Keyword: LAMBDA_V		[W/mK]			'Thermal conductivity (dry) in material direction v'
		MP_AW_V,			// Keyword: AW_V			[kg/m2s05]		'Water uptake coefficient in material direction v'
		MP_MEW_V,			// Keyword: MEW_V			[-]				'Water vapor diffusion resistance factor in material direction v'
		MP_SD_V,			// Keyword: SD_V			[m]				'Diffusiun equivalent air layer thickness in material direction v'
		MP_KLEFF_V,			// Keyword: KLEFF_V			[s]				'Liquid water conductivity at effective saturation in material direction v'
		MP_DLEFF_V,			// Keyword: DLEFF_V			[m2/s]			'Liquid water diffusivity at effective saturation in material direction v'
		MP_KG_V,			// Keyword: KG_V			[s]				'Air permeability in material direction v'
		MP_LAMBDA_W,		// Keyword: LAMBDA_W		[W/mK]			'Thermal conductivity (dry) in material direction w'
		MP_AW_W,			// Keyword: AW_W			[kg/m2s05]		'Water uptake coefficient in material direction w'
		MP_MEW_W,			// Keyword: MEW_W			[-]				'Water vapor diffusion resistance factor in material direction w'
		MP_SD_W,			// Keyword: SD_W			[m]				'Diffusiun equivalent air layer thickness in material direction w'
		MP_KLEFF_W,			// Keyword: KLEFF_W			[s]				'Liquid water conductivity at effective saturation in material direction w'
		MP_DLEFF_W,			// Keyword: DLEFF_W			[m2/s]			'Liquid water diffusivity at effective saturation in material direction w'
		MP_KG_W,			// Keyword: KG_W			[s]				'Air permeability in material direction w'
		MP_LAMBDA_DESIGN,	// Keyword: LAMBDA_DESIGN	[W/mK]			'Design value of thermal conductivity'
		NUM_ParaTransport
	};


	/*! Enum type with all possible basic mechanical parameters.
		\todo Add full mechanical property model later.
	*/
	enum ParaMechanical {
		MP_ALPHA,			// Keyword: ALPHA			[mm/mK]		'Thermal expansion coefficient'
		MP_BETA_THETA_L,	// Keyword: BETA_THETA_L	[m3m/m3m]	'Hygric expansion coefficient (volumetric moisture content)'
		MP_BETA_THETA_L2,	// Keyword: BETA_THETA_L2	[m3/m3]		'Fiber saturation point (wood)'
		MP_ALPHA_V,			// Keyword: ALPHA_V			[mm/mK]		'Thermal expansion coefficient in material direction v'
		MP_BETA_THETA_L_V,	// Keyword: BETA_THETA_L_V	[m3m/m3m]	'Hygric expansion coefficient (volumetric moisture content) in material direction v'
		MP_BETA_THETA_L2_V,	// Keyword: BETA_THETA_L2_V	[m3/m3]		'Fiber saturation point (wood) in material direction v'
		MP_ALPHA_W,			// Keyword: ALPHA_W			[mm/mK]		'Thermal expansion coefficient in material direction w'
		MP_BETA_THETA_L_W,	// Keyword: BETA_THETA_L_W	[m3m/m3m]	'Hygric expansion coefficient (volumetric moisture content) in material direction w'
		MP_BETA_THETA_L2_W,	// Keyword: BETA_THETA_L2_W	[m3/m3]		'Fiber saturation point (wood) in material direction w'
		NUM_ParaMechanical
	};
	// ***KEYWORDLIST-END***


	/*! Enumeration values that are used to identify parametrization suitable for
		calculation of various physical effects.
		The function capabilities() can be used to query calculation capabilities.
	*/
	enum TransportType {
		TT_None			= 0x0000,
		TT_Thermal		= 0x0001,
		TT_Vapour		= 0x0002,
		TT_Liquid		= 0x0004,
		TT_MoistureMask	= TT_Vapour | TT_Liquid,
		TT_Airflow		= 0x0008,
		TT_Salt			= 0x0010,
		TT_VOC			= 0x0020
	};



	// *** PUBLIC MEMBER FUNCTIONS ***

	/*! Default constructor.*/
	Material() { clear(); }

	/*! Virtual destructor, so that derived classes may contain member variables. */
	virtual ~Material()  {}

	/*! Resets all variables such that the class
		corresponds to a newly created object.
	*/
	void clear();

	/*! Function for reading a material from file.
		This function calls internally either readASCII() or readBinary(), depending on
		the magic header in the file.
		\param fname	The filename of the file to be read.
	*/
	void read(const IBK::Path & fname);

	void readOld(const IBK::Path & fname);

	/*! Function for writing a material to file.
		\param fname	The filename of the file to be written.
		\param binary	If true, write material file in binary mode.
		\param writeEmptyKeywords	If true, all keywords are written, even if the corresponding parameters are empty.
	*/
	void write(const IBK::Path & fname, bool binary = false, bool writeEmptyKeywords = false) const;

	/*! Populates material object from data string. The data is expected to begin
		right after the [MATERIAL] section header and must not contain magic numbers.
		This function can be used to counterpart to toString().
		This function is a convenience function and simply uses readASCII() with a temporary stringstream.
		\note This function is primarily useful for Delphin 5.x code (embedded material data in project file)
			  and for drag-and-drop implementations.
		\param data String that contains the material data.
		\param preV6 If true, the function expects the string to hold Delphin 5 material data.
	*/
	void fromString(const std::string & data, bool preV6);

	/*! Writes material data to string.
		The string will begin with the [MATERIAL] section header and does not contain a magic header.
		The function always writes in current material file format.
		This function can be used to counterpart to fromString().
		\note This function is primarily useful for Delphin 5.x code (embedded material data in project file)
			  and for drag-and-drop implementations.
		\param indent The number of indentation characters to be used
		\param writeEmptyKeywords	If true, all keywords are written, even if the corresponding parameters are empty.
	*/
	std::string toString(unsigned int indent=0, bool writeEmptyKeywords=false) const;

	/*! Wrapper function for read() (required for IBK::Database).
		\param fname File path.
	*/
	void readXML (const IBK::Path & fname);

	/*! Wrapper function for write() (required for IBK::Database).
		\param fname File path.
	*/
	void writeXML(const IBK::Path & fname) const;


	/*! Major version number as 32bit integer.
		This value is set in the read() function.
	*/
	unsigned int majorVersionNumber() const { return m_majorFileVersion; }

	/*! Minor version number as 32bit integer.
		This value is set in the read() function.
	*/
	unsigned int minorVersionNumber() const { return m_minorFileVersion; }

	/*! Returns unique ID of this material. */
	unsigned int id() const { return m_id; }

	/*! Sets unique ID of this material. */
	void setId(unsigned int id) { m_id = id; }

	/*! Returns current filename of material (may include placeholders). */
	const IBK::Path & filename() const { return m_filename; }

	/*! Sets current filename (may include placeholders). */
	void setFilename(const IBK::Path & fname) { m_filename = fname; }



	/*! Prepares a material for calculation following a features flag field.
		The function will initialize the material and all its storage and
		transport functions for calculation.
		The function will write progress and warning messages of different
		detail using IBK::IBK_Message().
		If a critical error (missing parameters etc.) is encountered, an
		IBK::Exception is thrown.
		\note This function is called automatically from the read functions.
		\warning When changing content of the material object by accessing
				 its public members remember to call prepareCalculation() first
				 before using any of the calculation functions!
	*/
	void prepareCalculation();

	/*! Determines and returns calculation capabilities, that is parametrization stored
		in the material object that permits calculation of various physical processes.
		\warning This function requires an initialized material object. That means, that
				 a call to prepare calculation was completed successfully.
		\return Returns an or'ed combination of MM::Material::TransportType.
		This version only returns the internal capability object;
	*/
	int capabilities() const;

	/*! Determines and returns calculation capabilities, that is parametrization stored
		in the material object that permits calculation of various physical processes.
		\warning This function requires an initialized material object. That means, that
				 a call to prepare calculation was completed successfully.
		\return Returns an or'ed combination of MM::Material::TransportType.
		This version checks the capability and returns the result without changing the internal capability object.
	*/
	int getCapabilities() const;

	/*! Friend declaration for global operator==.*/
	friend bool operator==(const Material& lhs, const Material& rhs);


	// *** PUBLIC MEMBER VARIABLES ***

	/*! Filename of the material file, may contain placeholders.
		Filename is mutable so it can be set in the constant write() function.
	*/
	mutable IBK::Path			m_filename;

	/*! The material data format type (1D = isotropic, 2D or 3D anisotropic).
		This value is not stored in the file but generated when the data is read.
	*/
	MaterialFormat				m_type;

	/*! ID name of the material (0 = invalid material). */
	unsigned int				m_id;

	/*! Used in Delphin 5 solver, marker flag for assignments. */
	bool						m_used;

	/*! Contains the material identification meta data. */
	MaterialIdentification		m_identification;

	/*! Flag that safes the read state of file, it is false by default and true if an binary format was read. It doesn't trigger binary writing. */
	bool						m_isBinary;

	// *** Parameters ***

	/*! The basic storage material parameters. */
	IBK::Parameter				m_paraStorage[NUM_ParaStorage];
	/*! The basic transport material parameters. */
	IBK::Parameter				m_paraTransport[NUM_ParaTransport];
	/*! The basic mechanical material parameters. */
	IBK::Parameter				m_paraMechanical[NUM_ParaMechanical];

	// *** Isotropic Functions ***

	/*! Thermal storage. */
	ThermalStorage				m_thermalStorage;
	/*! Moisture storage functions. */
	MoistureStorage				m_moistureStorage;

	// *** Anisotropic Functions ***

	/*! Moisture transport functions. */
	MoistureTransport			m_moistureTransport[NUM_MaterialDirection];
	/*! Thermal conductivity functions. */
	ThermalTransport			m_thermalTransport[NUM_MaterialDirection];
	/*! Air transport functions. */
	AirTransport				m_airTransport[NUM_MaterialDirection];


	/*! Object that keeps all measured data. */
	MeasuredData				m_measuredData;

	/*! Object that keeps vapor contact resistance information. */
	VaporContactResistanceModel	m_vaporContactResistance;


	/*! Set this static variable to false to allow inconsistent material
		properties, such as non-monotonic material functions.
		During init for a simulation and the preceeding call to prepareCalculation()
		this flag should be set to true (the default).
	*/
	static bool					m_useStrictConsistencyChecking;

private:
	/*! Read function for reading material data from input stream in ASCII format.
		The function will throw an IBK::Exception if reading fails.
		\param in Input file stream (opened in ASCII mode) is expected to be positioned
				  after the magic header and [MATERIAL] section header.
		\param preV6 If true, the function expects the input stream to hold Delphin 5 material data.

	*/
	void readASCII(std::istream& in, bool preV6);

	/*! Read function for reading material data from input data vector in ASCII format.
		The function will throw an IBK::Exception if reading fails.
		\param data Input data vector should contain only material data (no magic header).
		\param preV6 If true, the function expects the input stream to hold Delphin 5 material data.

	*/
	void readASCII(const std::vector<std::string>& data, bool preV6);

	/*! Write function for writing material data into an output stream in ASCII format.
		This function always writes in the current file version.
		The data is written beginning after the magic header (or [MATERIAL] section header).
		Magic headers are not being written with this function.
		\param out Output file stream should be opened in ASCII mode.
		\param indent Number of indentation characters to use for top-level indentation.
		\warning Since this function expects the file stream to be writable and opened,
				 the only errors that have to be expected result from bad data in the material
				 classes. This is considered to be programming errors and results in an
				 IBK::Exception to be thrown.
				 If the function returns normally, a bad stream state may still indicate
				 a write problem and should therefore be evaluated by the calling application.
	*/
	void writeASCII(std::ostream & out, unsigned int indent, bool writeEmptyKeywords) const;

	/*! Read function for reading material data in V6 or newer format (binary mode).
		The function will throw an IBK::Exception if reading fails.
		\param in Input file stream (opened in binary mode) is expected to be positioned
				  right after the magic header.
	*/
	void readBinary(std::istream& in);

	/*! Write function for writing material data in V6 or newer format (binary mode).
		\param out Output file stream (opened in binary mode).
		\warning Since this function expects the file stream to be writable and opened,
				 the only errors that have to be expected result from bad data in the material
				 classes. This is considered to be programming errors and results in an
				 IBK::Exception to be thrown.
				 If the function returns normally, a bad stream state may still indicate
				 a write problem and should therefore be evaluated by the calling application.
	*/
	void writeBinary(std::ostream& out) const;

	/*! Tries to read a basic parameter from the given line.
		\param line Line string.
		It throws an exception if no parameter can be read.
		In case of an unknown keyword only a message will occure.
	*/
	void readASCIIParameterFromLine(const std::string & line);

	/*! Reads all basic parameters.
		\param data Holds data section (without section headers) of all three base parameter blocks.
	*/
	void readASCIIParameters(const std::string & data);

	/*! Reads all basic parameters from a vector of lines.
		\param data Holds data section (without section headers) of all three base parameter blocks.
	*/
	void readASCIIParameters(const std::vector<std::string> & data);

	/*! Writes parameter data block.
		\param out Output file stream (ASCII mode).
		\param indent Number of indentation characters to use.
		\param writeEmptyKeywords If true, all parameter keywords are written, even if the parameters
								  themselves are empty.
	*/
	void writeASCIIParameters(std::ostream & out, unsigned int indent, bool writeEmptyKeywords) const;


	// *** PRIVATE MEMBER VARIABLES ***

	/*! Major file version, for old DELPHIN 5 format this is 5, for new format 6 and higher. */
	unsigned int						m_majorFileVersion;
	/*! Minor file version, only defined for Version 6 or newer files. */
	unsigned int						m_minorFileVersion;

	/*! Bitfield holding capability flags, updated in prepareCalculation().
		Can be queried with capabilities().
	*/
	int									m_capabilities;
};

/*!	Global operator== for comparison of two materials.
	If IDs are equal paramters must be equal!
*/
inline bool operator==(const Material& lhs, const Material& rhs) {
	return lhs.id() == rhs.id();
}

/*! Global operator== for comparison of a materials and a material id. */
inline bool operator==(const Material& lhs, unsigned int id) {
	return lhs.id() == id;
}



} // namespace MM


/*! \mainpage Material Library Overview
This page will hold an introductory description of a material library API. The material library
manages material files. Its central class is MM::Material which serves as container for
material data and represents the content of a single material file.

\section intro_sec Introduction
The API is centered around the class MM::Material. Users of the library will typically instantiate
objects of type MM::Material and use the provided member functions. Reading the API of the Material
class will give a good idea of what is possible with the library. Most other classes are used
as members of MM::Material.

MM_Utils.h provides a number of free functions and functors with use in/with standard C++ algorithms and
containers.

MM_Constants.h holds constants defined for the material library, for example the current version.

\section errors Error handling concepts
This library handles errors and warnings via exceptions and messages. Whenever an error occurs,
be it expected (user data) or through API use, we throw IBK::Exceptions. This exception may be
caught internally, in which case it is rethrown again with an outer context added to the message.
This typically looks like:
\code
// FUNC_ID defines the function and its class/namespace contexts
const char * const FUNC_ID = "Material::someFunction()";
try {
	// some code that may throw
}
catch (IBK::Exception & ex) {
	// rethrow with additional scope information, but pass ex as first argument!
	throw IBK::Exception(ex, "Some detailed info about this context", FUNC_ID);
}
\endcode
If warnings are encountered, for example when deprecated/unknown keywords are read, an
IBK::IBK_Message() is emitted with the IBK::MSG_WARNING type. If a function processes data
and writes progress messages, it will use IBK::IBK_Message() and write messages of type
IBK::IBK_PROGRESS with verbosity level 2 or 3. The internal functions in the material library
must not use verbosity level 1 or below for notification/progress messages.

Time-critical functions will normally not explicitly check for parameters, but instead expect a
successfull initialization via prepareCalculation(). Still, IBK_ASSERT() assertions may be used to
signal during debugging that invalid conditions are reached.

\section io_sec Read/Write Functionality
Material objects can be read from file using Material::read(). Note that Material::readXML() is just
a wrapper function around read(), since material files are currently plain ASCII text format and not
in XML format. The read() function determines whether the file is in Version 6 format or Version 5
format and calls appropriate read functions.

When material data is available as string (for example, material data that was embedded in a
Delphin 5 project file) you can use Material::readFromString() to populate the Material class.
When reading/writing material data with readFromString() and toString(), the material section
header [MATERIAL] (used in Delphin 5) is not part of the data. Input data must not contain
this header and output data will not contain this header.

The material library always writes in the most recent format. The function Material::write() can
be used to write both ASCII and binary representations, depending on the argument passed to the
function.

\subsection filetype_sec File formats
Describes supported file formats:
- mat - Delphin 5 material files (Version 5.x)
- m6 - Delphin 6.x material files (ASCII format)
- m6b - Delphin 6.x material files (binary format)

Only version 6 material files can be stored in binary mode.

\subsection anisotropicFiles Isotropic and Anisotropic material files
With version 6 of the material files, anisotropic data can be stored. Anisotropic materials
can have different transport parameters for up to 3 material directions: U, V and W.

We distinguish 3 parametrization types:
1. MF_1D isotropic data (transport properties are the same in all directions),
2. MF_2D anisotropic material: transport properties given for two material directions,
3. MF_3D anisotropic material: transport properties given for all three material directions.

In the material data file, the section header for transport properties of anisotropic materials
are appended with _V and _W, for the V and W transport directions, respectively.

For example:
<pre>
; properties for U direction
[MOISTURE_TRANSPORT]
  ...

; properties for V direction
[MOISTURE_TRANSPORT_V]
  ...

; properties for W direction
[MOISTURE_TRANSPORT_W]
  ...
</pre>

When specifying 2D anisotropic materials, it is required that
only the _V direction is used.


\subsection io_consisteny Consistency Requirements on Material Files
A material file can only be read without throwing an exception if all parameters are given in
syntactically correct format. That includes:
- parameters (IBK::Parameter) must have value and valid IBK unit
- data tables/linear splines (IBK::LinearSpline) must have a correct name, the same number of
  x and y-values and strict monotony regarding the x values. Also, at least 2 data points must
  be stored for a linear spline
- section headers must be correctly named

In the case that invalid/unsupported keywords are found, the library just emits a warning and ignores the
parameter/spline data.



\section calculation_sec Calculation Functionality
The material library contains calculation functionality. Before using any of the calculation
functions, the material object and its members must first be initialized for calculation.
A valid and properly initialized material will therefore require a successful run of
MM::Material::prepareCalculation().

Whenever a material is read from file/string the prepareCalculation() function will be called,
automatically. This is done from within readASCII().

\warning When the material data is modified by changing any of the public members, a call to
MM::Material::prepareCalculation() is mandatory before using calculation functionality.


\subsection matFlags Material Flags
Material flags are used to mark certain physical (transport) properties as not available. For example,
steel as material won't have moisture storage and transport characteristics. Whenever one of the
material flags (AIT_TIGHT, WATER_TIGHT, VAPOR_TIGHT) is set, the corresponding parametrization is
ignored during initialization. You must not call any of the calculation functions for a material
property that is disabled through a flag.

\sa MM::MaterialIdentification::Flags


\subsection matCaps Material Calculation Capabilities
Based on the parametrization stored in a material file, different physical properties/material functions
can be evaluated. Since parametrization for all material functions is not available for all materials,
some materials will omit some data. In these cases, the corresponding material functions cannot be
called.

The set of functionality that is provided through available parametrization can be queried with
the function MM::Material::capabilities().

\note In the implementation, each storage and transport class defines one or more
parametrization types. If these are set to NUM_<XXX>Types (an enum value defined locally in each class),
the corresponding parametrization is not available.


\subsection limitChecks Consistency/Limit Checks
A simple rule applies to parametrization checks:
\quotation If parameters are given, they must be correct.

So, if parameters are missing, simply the corresponding capability will be disabled. However, if
one or more parameters for a certain model are given (for the simplest models these can be just base
parameters), they must be given completely and correctly.

Another part of the initialization for calculation routines is a check for function limits. For example,
functions that are defined with respect to moisture contents must enclose the whole moisture content
range.

Currently, the checks are very restrictive, meaning that materials must fulfil the requirements
very accurately (up to a tolerance of 1e-3).


\subsection calculation_init Initialization Functionality
When initializing a material for calculation, the following steps are performed:
- denormalization of all material functions (when Delphin 5 material file was read),
- detecting available parametrization
- caching of variables from Material in local classes,
- auto-generating auxiliary functions that speed up computation,
- performing consistency and accuracy checks.

For these operations each class defines the following functions: prepareCalculation(), denormalize(),
and checkLimits().


\subsection calculation_initOrder Initialization Order
The initialization processes the individual storage classes in the following order:
- ThermalStorage
- MoistureStorage
- AirTransport
- ThermalTransport
- MoistureTransport

This order is required, because MoistureTransport requires, for example, a valid and functional
MoistureStorage class with calculation capabilities.


\subsection models Model Support
While all functions can be specified as linear splines, in some cases it is possible to provide
the parametrization for analytical models, as well. Two restrictions apply:
- for each function type, for example Theta_l(pC) only a single model parametrization may
  be included in a material file
- if model parametrization is given alongside linear spline function data, the generated data must
  match the model since it is expected that the linear spline was generated from the model



*/

/*! \file MM_Material.h
	\brief Contains the declaration of the class Material and the MaterialFinder function object.
*/

#endif // MM_MaterialH

