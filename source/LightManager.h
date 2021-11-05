#pragma once
#include <vector>

class Light;
class PointLight;
class DirectionalLight;

class LightManager final
{
public:
	static LightManager* GetInstance();

	const std::vector<Light*>& GetLightVector() const;
	void AddPointLight(PointLight* pointLight);
	void AddDirectionalLight(DirectionalLight* directionalLight);
	void ToggleLight(int index);

	static void Clear();

private:
	LightManager();
	~LightManager();

	void DeleteObjects();

	static LightManager* m_Instance;
	std::vector<Light*> m_LightVector;
};