#pragma once
#include "ERGBColor.h"
#include "EMath.h"
#include "EMathUtilities.h"

namespace BRDF
{
	Elite::RGBColor Lambert(const Elite::RGBColor& diffuseReflectance, const Elite::RGBColor& diffuseColor);
	Elite::RGBColor Phong(float specularReflectance, float phongExponent, const Elite::FVector3& lightDirection, const Elite::FVector3& viewDirection, const Elite::FVector3& hitNormal);
	Elite::RGBColor CookTorrance(const Elite::FVector3& hitNormal, const Elite::FVector3& lightDirection, const Elite::FVector3& halfVector, float roughnessSquared, const Elite::FVector3& viewDirection, const Elite::RGBColor& albedo, Elite::RGBColor& fresnel);
}