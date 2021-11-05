#include "LambertMaterial.h"
#include "BRDF.h"

LambertMaterial::LambertMaterial(const Elite::RGBColor& diffuseColor, const Elite::RGBColor& diffuseReflectance)
    : m_Color{diffuseColor}
    , m_DiffuseReflectance{diffuseReflectance}
{
}

Elite::RGBColor LambertMaterial::Shade(const HitRecord& hitRecord, const Elite::FVector3& incoming, const Elite::FVector3& outgoing) const
{
    return BRDF::Lambert(m_DiffuseReflectance, m_Color);
}
