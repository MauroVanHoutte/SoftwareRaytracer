#include "BRDF.h"
#include <SDL_stdinc.h>

Elite::RGBColor BRDF::Lambert(const Elite::RGBColor& diffuseReflectance, const Elite::RGBColor& diffuseColor)
{
	return diffuseColor*diffuseReflectance/float(M_PI);
}

Elite::RGBColor BRDF::Phong(float specularReflectance, float phongExponent, const Elite::FVector3& lightDirection, const Elite::FVector3& viewDirection, const Elite::FVector3& hitNormal)
{
	Elite::FVector3 reflect{ lightDirection - (2 * (Elite::Dot(hitNormal, lightDirection)*hitNormal)) };
	float specularReflection{ specularReflectance * pow(Elite::Dot(reflect, viewDirection), phongExponent) };
	return Elite::RGBColor{specularReflection, specularReflection, specularReflection};
}

Elite::RGBColor BRDF::CookTorrance(const Elite::FVector3& hitNormal, const Elite::FVector3& lightDirection, const Elite::FVector3& halfVector, float roughnessSquared, const Elite::FVector3& viewDirection, const Elite::RGBColor& albedo, Elite::RGBColor& fresnel)
{
	float dotHalfVectorViewDirection{ Elite::Dot(halfVector, viewDirection) };
	float dotHitNormalHalfVector{ Elite::Dot(hitNormal, halfVector) };
	float dotHitNormalViewDirection{ Elite::Dot(hitNormal, viewDirection) };
	float dotHitNormalLightDirection{ Elite::Dot(hitNormal, lightDirection) };
	float normalDistribution{ roughnessSquared * roughnessSquared / (float(M_PI) * pow((pow(dotHitNormalHalfVector,2) * (roughnessSquared * roughnessSquared - 1) + 1),2)) };
	fresnel = albedo + (Elite::RGBColor{1,1,1} - albedo) * (1 - (dotHalfVectorViewDirection* dotHalfVectorViewDirection* dotHalfVectorViewDirection* dotHalfVectorViewDirection* dotHalfVectorViewDirection));
	float k{ pow((roughnessSquared + 1),2) * .125f };
	float geometryView{ dotHitNormalViewDirection / (dotHitNormalViewDirection * (1 - k) + k) };
	float geometryLight{ dotHitNormalLightDirection / (dotHitNormalLightDirection * (1 - k) + k) };
	return (fresnel * normalDistribution * geometryView * geometryLight) / (4 * dotHalfVectorViewDirection * dotHitNormalLightDirection);
}
