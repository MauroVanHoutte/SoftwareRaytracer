#include "LambertDPhongS.h"
#include "BRDF.h"

LambertPhong::LambertPhong(const Elite::RGBColor& diffuseReflectance, const Elite::RGBColor& diffuseColor, float specularReflectance, float phongExponent)
    : m_DiffuseReflectance{diffuseReflectance}
    , m_DiffuseColor{diffuseColor}
    , m_SpecularReflectance{specularReflectance}
    , m_PhongExponent{phongExponent}
{
}

Elite::RGBColor LambertPhong::Shade(const HitRecord& hitRecord, const Elite::FVector3& incoming, const Elite::FVector3& outgoing) const
{
    return BRDF::Lambert(m_DiffuseReflectance, m_DiffuseColor) + BRDF::Phong(m_SpecularReflectance, m_PhongExponent, incoming, outgoing, hitRecord.normal);
}
