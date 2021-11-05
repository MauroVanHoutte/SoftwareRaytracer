#include "Light.h"

void Light::ToggleLight()
{
	m_IsEnabled = !m_IsEnabled;
}

bool Light::GetLightStatus() const
{
	return m_IsEnabled;
}
