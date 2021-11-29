#include "LambertDCook-TorranceS.h"
#include "BRDF.h"

LambertCookTorrance::LambertCookTorrance(bool metalness, float roughness, const Elite::RGBColor& albedo)
	: m_Roughness{roughness}
	, m_Albedo{albedo}
{
	m_Metal = metalness;
	m_Reflects = metalness;
}

Elite::RGBColor LambertCookTorrance::Shade(const HitRecord& hitRecord, const Elite::FVector3& incoming, const Elite::FVector3& outgoing) const
{
	Elite::FVector3 halfVector{ (incoming + outgoing) / Elite::Magnitude((incoming + outgoing)) };
	Elite::RGBColor fresnel;
	Elite::RGBColor specularReflection{ BRDF::CookTorrance(hitRecord.normal, incoming, halfVector, m_Roughness * m_Roughness, outgoing, m_Albedo, fresnel) };
	Elite::RGBColor diffuseReflectance{};
	if (m_Metal)
		diffuseReflectance = {};
	else
		diffuseReflectance = Elite::RGBColor{ 1, 1, 1 } - fresnel;

	return diffuseReflectance * BRDF::Lambert(diffuseReflectance, Elite::RGBColor{1.f, 1.f, 1.f}) + fresnel * specularReflection;
}
