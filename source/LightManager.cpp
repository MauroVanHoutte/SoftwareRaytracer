#include "LightManager.h"
#include "Light.h"
#include "PointLight.h"
#include "DirectionalLight.h"

LightManager* LightManager::m_Instance = nullptr;

LightManager::LightManager()
{
};

LightManager* LightManager::GetInstance()
{
	if (m_Instance == nullptr)
	{
		m_Instance = new LightManager();
	}
	return m_Instance;
}

const std::vector<Light*>& LightManager::GetLightVector() const
{
	return m_LightVector;
}

void LightManager::AddPointLight(PointLight* pointLight)
{
	m_LightVector.push_back(pointLight);
}

void LightManager::AddDirectionalLight(DirectionalLight* directionalLight)
{
	m_LightVector.push_back(directionalLight);
}

void LightManager::ToggleLight(int index)
{
	if (index >= 0 && index < m_LightVector.size())
	{
		m_LightVector[index]->ToggleLight();
	}
}

void LightManager::Clear()
{
	if (m_Instance != nullptr)
	{
		m_Instance->DeleteObjects();
	}
	delete m_Instance;
}

LightManager::~LightManager()
{
}

void LightManager::DeleteObjects()
{
	for (Light* object : m_LightVector)
	{
		delete object;
	}
}