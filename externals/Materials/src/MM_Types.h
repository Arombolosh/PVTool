#ifndef MM_TypesH
#define MM_TypesH

#include <functional>
#include <string>
#include <set>


namespace MM {

/*! Coordinate direction for anisotropic material properties. */
enum MaterialDirection {
	/*! U direction, also used as default for isotropic materials. */
	U_DIRECTION,
	/*! V direction. */
	V_DIRECTION,
	/*! W direction. */
	W_DIRECTION,
	/*! Invalid value for materialDirection_t */
	NUM_MaterialDirection
};

/*! Return value of the Material::determineMaterialFormat() function. */
enum MaterialFormat {
	/*! Isotropic material file format. */
	MF_1D,
	/*! Two-dimensional anisotropic material. */
	MF_2D,
	/*! Three-dimensional anisotropic material.  */
	MF_3D,
	/*! Invalid value for materialFormat_t. */
	NUM_MaterialFormat
};

/*! All material databases defined by the IBK. */
enum MaterialDBType {
	All,
	Standard,
	Demo,
	Feuchteatlas,
	Haacke,
	Knauf,
	IQLatorDemo,
	IQLatorPro,
	IQLatorStandardBasic,
	IQLatorStandardBuildingBoards,
	IQLatorStandardConcrete,
	IQLatorStandardInsulation,
	IQLatorStandardFoils,
	IQLatorStandardHaacke,
	IQLatorStandardWoodenInsulationPanels,
	IQLatorStandardMasonry,
	IQLatorStandardMineralInsulationPanels,
	IQLatorStandardNaturalStone,
	IQLatorStandardNaturalMaterials,
	IQLatorStandardPlasterLight,
	IQLatorStandardPlasterHeavy,
	IQLatorStandardMiscellaneous,
	IQLatorStandardBrick,
	NUM_MaterialDBType
};


/*! Types of transport parametrizations. */
enum TransportParametrization {
	TP_None,
	TP_Thermal,
	TP_Vapour,
	TP_Liquid,
	TP_Air,
	TP_Salt,
	TP_VOC
};

enum TargetClasses{
	TC_MoistureStorage = 100,
	TC_MoistureTransport = 200,
	TC_ThermalTransport = 300,
	TC_AirTransport = 400
};

enum UsedParameterForDistance {
	UPFD_DELPHINData		= 0x01,
	UPFD_Rho				= 0x02,
	UPFD_Lambda				= 0x04,
	UPFD_Mew				= 0x08,
	UPFD_Aw					= 0x10,
	UPFD_Theta80			= 0x20,
	UPFD_ThetaPor			= 0x40,
	UPFD_ThetaEff			= 0x80,
	UPFD_AllWithDELPHIN		= 0xff,
	UPFD_AllWithoutDELPHIN	= 0xfe,
};

struct BaseParameter {
	BaseParameter() :
		rho(-1),
		lambda(-1),
		mew(-1),
		aw(-1),
		theta80(-1),
		thetaPor(-1),
		thetaEff(-1)
	{}

	double rho;
	double lambda;
	double mew;
	double aw;
	double theta80;
	double thetaPor;
	double thetaEff;
};


// ***KEYWORDLIST-END***



} // MM

/*! \file MM_Types.h
	\brief Contains main types and additional enums.
*/



#endif // MM_TypesH
