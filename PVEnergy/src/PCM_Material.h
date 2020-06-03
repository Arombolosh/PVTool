#ifndef PCM_MATERIAL_H
#define PCM_MATERIAL_H

#include <IBK_Parameter.h>
#include <IBK_LinearSpline.h>

class PCM_Material
{
public:

	enum Parameters{
		P_Density,							///< density of material (mean value)
		P_SpecHeat,							///< specific heat capacity
		P_Conductivity,						///< conductivity of pcm material
		P_MeltingPoint,						///< melting start Point
		NUM_P
	};


	void createM6File(const IBK::Path &path);

	void readInputs();

	IBK::Parameter										m_para[NUM_P];
	std::map<IBK::Parameter, IBK::Parameter>			m_specCapacityCurve;	///<temperature to enthalpy
	std::string											m_name;
	IBK::LinearSpline m_linSpl;

};

#endif // PCM_MATERIAL_H
