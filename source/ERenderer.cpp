//External includes
#include "SDL.h"
#include "SDL_surface.h"

//Project includes
#include "ERenderer.h"
#include "ERGBColor.h"
#include "ObjectManager.h"
#include "Ray.h"
#include "Sphere.h"
#include "Plane.h"
#include "LightManager.h"
#include "Light.h"
#include "Material.h"
#include "LambertMaterial.h"
#include "DirectionalLight.h"
#include "PointLight.h"
#include "ThreadManager.h"
#include <iostream>
#include <numeric>


Elite::Renderer::Renderer(SDL_Window * pWindow)
	:m_HardShadowsEnabled{false}
	, m_RenderMode{RenderMode::All}
	, m_ClearColor{0.529f, 0.808f, 0.922f}
{
	//Initialize
	m_pWindow = pWindow;
	m_pFrontBuffer = SDL_GetWindowSurface(pWindow);
	int width, height = 0;
	SDL_GetWindowSize(pWindow, &width, &height);
	m_Width = static_cast<uint32_t>(width);
	m_Height = static_cast<uint32_t>(height);
	m_pBackBuffer = SDL_CreateRGBSurface(0, m_Width, m_Height, 32, 0, 0, 0, 0);
	m_pBackBufferPixels = (uint32_t*)m_pBackBuffer->pixels;

	uint32_t nrThreads = ThreadManager::GetInstance()->GetNrThreads();
	m_FinishedJobs.resize(nrThreads, false);
	m_HitsPerJob.resize(nrThreads, 0);
	m_RowsPerJob.resize(nrThreads, m_Height/nrThreads);
	m_SecsPerJob.resize(nrThreads, 0);
}

Elite::Renderer::~Renderer()
{
}

void Elite::Renderer::Render(const Camera* camera)
{
	SDL_LockSurface(m_pBackBuffer);

	ResetBackbuffer();

	auto threadManager = ThreadManager::GetInstance();

	std::fill(m_FinishedJobs.begin(), m_FinishedJobs.end(), false);
	std::fill(m_HitsPerJob.begin(), m_HitsPerJob.end(), 0);

	const CameraData cameraData{ camera->GetCameraData() }; //caching this frames camera values
	const std::vector<RenderObject*> objectVector = ObjectManager::GetInstance()->GetObjectVector(); //caching objects
	const std::vector<Light*> lightVector = LightManager::GetInstance()->GetLightVector(); //caching lights
	//Loop over all the pixels
	
	int startRow = 0;
	for (uint32_t t = 0; t < threadManager->GetNrThreads(); t ++)
	{
		int jobRows = m_RowsPerJob[t];
		threadManager->AddJob(std::bind(&Elite::Renderer::RenderScreenPortion, this, cameraData, objectVector, lightVector, startRow, jobRows, t));
		startRow += m_RowsPerJob[t];
	}

	while(true)
	{
		std::this_thread::sleep_for(std::chrono::microseconds(5));
		if (std::all_of(m_FinishedJobs.begin(), m_FinishedJobs.end(), [](bool b) {return b; }))
			break;
	}

	SDL_UnlockSurface(m_pBackBuffer);
	SDL_BlitSurface(m_pBackBuffer, 0, m_pFrontBuffer, 0);
	SDL_UpdateWindowSurface(m_pWindow);
}

bool Elite::Renderer::SaveBackbufferToImage() const
{
	return SDL_SaveBMP(m_pBackBuffer, "BackbufferRender.bmp");
}

void Elite::Renderer::IncreaseBounces()
{
	++m_MaxReflectionBounces;
}

void Elite::Renderer::DecreaseBounces()
{
	if (m_MaxReflectionBounces == 0)
	{
		return;
	}
	--m_MaxReflectionBounces;
}

void Elite::Renderer::ToggleShadows()
{
	m_HardShadowsEnabled = !m_HardShadowsEnabled;
}

void Elite::Renderer::ToggleRenderMode()
{
	if (m_RenderMode == RenderMode::All)
	{
		m_RenderMode = RenderMode::IrradianceOnly;
	}
	else
		m_RenderMode = RenderMode(int(m_RenderMode) + 1);
}

void Elite::Renderer::ResetBackbuffer()
{
	for (uint32_t r = 0; r < m_Height; r++)
	{
		for (uint32_t c = 0; c < m_Width; c++) //resetting color
		{
			m_pBackBufferPixels[c + (r * m_Width)] = SDL_MapRGB(m_pBackBuffer->format,
				static_cast<uint8_t>(m_ClearColor.r)*255,
				static_cast<uint8_t>(m_ClearColor.g)*255,
				static_cast<uint8_t>(m_ClearColor.b)*255);
		}
	}
}

void Elite::Renderer::SetupRay(Ray& ray, int row, int col, const CameraData& cameraData)
{
	float yScreenSpace{ 1 - (2 * (row + 0.5f) / float(m_Height)) };
	float yCameraSpace{ yScreenSpace * cameraData.fov };

	float xScreenSpace{ (2 * (col + 0.5f) / float(m_Width)) - 1 };
	float xCameraSpace{ xScreenSpace * cameraData.fov * cameraData.aspectRatio };

	ray.origin.x = xCameraSpace; //ray coordinates in cameraSpace
	ray.origin.y = yCameraSpace;
	ray.origin.z = -1;

	Elite::FPoint4 rayOriginWorld{ cameraData.ONB * Elite::FPoint4{ ray.origin.x, ray.origin.y,ray.origin.z, 1 } }; //converting to worldSpace

	ray.origin.x = rayOriginWorld.x;
	ray.origin.y = rayOriginWorld.y;
	ray.origin.z = rayOriginWorld.z;

	ray.direction = Elite::GetNormalized(ray.origin - cameraData.pos); //direction from camerapos to ray
}

void Elite::Renderer::CalculateIrradiance(const std::vector<Light*>& lightVector, const std::vector<RenderObject*>& objectVector, Ray& shadowRay, const HitRecord& firstHit, const Ray& cameraRay, RGBColor& pixelColor) const
{
	for (const Light* light : lightVector) // taking contribution of every light
	{
		if (light->GetLightStatus())
		{
			shadowRay.origin = firstHit.pIntersect;
			shadowRay.direction = light->GetDirection(firstHit.pIntersect, 1.f); // ray to check if light will reach the object
			shadowRay.tMin = 0.001f; //preventing self collision
			bool shadowHit{ false };
			shadowRay.tMax = FLT_MAX;
			if (light->GetLightType() == LightType::PointLightType)
				shadowRay.tMax = Elite::Normalize(shadowRay.direction);
			if (Elite::Sign(Elite::Dot(-shadowRay.direction, firstHit.normal)) != Elite::Sign(Elite::Dot(cameraRay.direction, firstHit.normal))) //preventing lights from lighting the back of planes and triangles
			{
				shadowHit = true; 
			}
			else
			{
				if (m_HardShadowsEnabled && m_RenderMode != RenderMode::BRDFOnly)
				{
					for (const RenderObject* object : objectVector)
					{
						if (object->Hit(shadowRay))
						{
							shadowHit = true;
						}
					}
				}
			}

			if (!shadowHit)
			{
				float LambertCosine{ Elite::Dot(firstHit.normal, light->GetDirection(firstHit.pIntersect, shadowRay.tMax)) };
				Elite::RGBColor specular;
				if (LambertCosine < 0)
					LambertCosine = 0;
				switch (m_RenderMode)
				{
				case RenderMode::IrradianceOnly:
					pixelColor += light->CalculateRadiance(firstHit) * LambertCosine; //only rendering Radiance, no materials
					break;
				case RenderMode::BRDFOnly:
					pixelColor += firstHit.pMaterial->Shade(firstHit, shadowRay.direction, -cameraRay.direction) * LambertCosine; //only rendering brdf, no Radiance
					break;
				case RenderMode::All:
					pixelColor += light->CalculateRadiance(firstHit) * firstHit.pMaterial->Shade(firstHit, shadowRay.direction, -cameraRay.direction) * LambertCosine;//counting up radiance of all lights on surface and adjusting color based on material
					break;
				}
			}
		}
	}
}

void Elite::Renderer::RenderScreenPortion(const CameraData& cameraData, const std::vector<RenderObject*>& objectVector, const std::vector<Light*>& lightVector, int startRow, int jobRows, int threadId)
{
	auto startPoint = std::chrono::high_resolution_clock::now();

	Ray ray{ {}, {} }; //initialising ray that will be reused for every pixel
	Ray shadowRay{ {}, {} };
	HitRecord firstHit{ false, Elite::FPoint3{0.f, 0.f, 0.f}, ray.tMax, Elite::FVector3{0.f, 0.f, 0.f}, nullptr }; //initialising hitrecord that will be used for every pixel

	for (int row = startRow; row < startRow + jobRows; row++)
	{
		for (uint32_t col = 0; col < m_Width; ++col)
		{
			SetupRay(ray, row, col, cameraData);
			firstHit.hit = false;
			firstHit.t = ray.tMax;
			Elite::RGBColor pixelColor{ 0.f, 0.f, 0.f };
			
			pixelColor = ShootRay(ray, objectVector, lightVector, 1, 0);

			m_pBackBufferPixels[col + (row * m_Width)] = SDL_MapRGB(m_pBackBuffer->format,
						static_cast<uint8_t>(pixelColor.r * 255),
						static_cast<uint8_t>(pixelColor.g * 255),
						static_cast<uint8_t>(pixelColor.b * 255));

			//for (const RenderObject* object : objectVector) // checking if the ray intersects with any objects
			//{
			//	object->Hit(ray, firstHit); // keeping the hitrecord of object closest to the ray origin
			//}
			//if (firstHit.hit)
			//{
			//	CalculateIrradiance(lightVector, objectVector, shadowRay, firstHit, ray, pixelColor); //counting up Radiance of all lights
			//	int bounces{ 0 };
			//	while (bounces < m_MaxReflectionBounces)
			//	{
			//		++bounces;
			//		Ray reflectRay{};
			//		HitRecord reflectHit{ false, Elite::FPoint3{0.f, 0.f, 0.f}, reflectRay.tMax, Elite::FVector3{0.f, 0.f, 0.f}, nullptr };
			//		reflectRay.direction = Elite::Reflect(ray.direction, firstHit.normal);
			//		reflectRay.origin = firstHit.pIntersect;
			//		reflectRay.tMin = 0.01f;
			//		for (const RenderObject* object : objectVector)
			//		{
			//			object->Hit(reflectRay, reflectHit);
			//		}
			//		if (reflectHit.hit)
			//		{
			//			Elite::RGBColor reflectColor;
			//			CalculateIrradiance(lightVector, objectVector, shadowRay, reflectHit, reflectRay, reflectColor);
			//			pixelColor += reflectColor * 0.1f; //arbitrary 0.1 here, need to find a formula that decides the value
			//			firstHit = reflectHit;
			//		}
			//		else
			//			break;
			//	}
			//	pixelColor.MaxToOne();

			//	m_pBackBufferPixels[col + (row * m_Width)] = SDL_MapRGB(m_pBackBuffer->format,
			//		static_cast<uint8_t>(pixelColor.r * 255),
			//		static_cast<uint8_t>(pixelColor.g * 255),
			//		static_cast<uint8_t>(pixelColor.b * 255));
			//}
		}
	}

	auto endPoint = std::chrono::high_resolution_clock::now();
	m_SecsPerJob[threadId] = std::chrono::duration<float>(endPoint - startPoint).count();

	std::unique_lock<std::mutex> lock(m_FinishedJobsMutex);
	m_FinishedJobs[threadId] = true;
}

Elite::RGBColor Elite::Renderer::ShootRay(const Ray& ray, const std::vector<RenderObject*>& objects, const std::vector<Light*>& lights, float refractionIdx, int bounce)
{
	Ray shadowRay{ {}, {} };
	HitRecord firstHit{ false, Elite::FPoint3{0.f, 0.f, 0.f}, ray.tMax, Elite::FVector3{0.f, 0.f, 0.f}, nullptr };

	for (const RenderObject* object : objects) // checking if the ray intersects with any objects
	{
		object->Hit(ray, firstHit); // keeping the hitrecord of object closest to the ray origin
	}

	if (!firstHit.hit)
		return m_ClearColor;

	Elite::RGBColor color;
	CalculateIrradiance(lights, objects, shadowRay, firstHit, ray, color);

	if (firstHit.pMaterial->m_Reflects && bounce < m_MaxReflectionBounces)
	{

		Elite::FVector3 reflectDireciton = Elite::Reflect(ray.direction, firstHit.normal);
		
		Ray reflectRay = Ray(firstHit.pIntersect, reflectDireciton);

		auto reflectColor = color + ShootRay(reflectRay, objects, lights, refractionIdx, bounce + 1) * 0.5f;
		Elite::RGBColor transmittedColor;
		if (firstHit.pMaterial->m_Refracts)
		{
			float reflectanceRatio = Fresnel(ray.direction, firstHit.normal, refractionIdx, firstHit.pMaterial->m_RefractionIndex);

			Elite::FVector3 refractDirection = Refract(ray.direction, firstHit.normal, refractionIdx, firstHit.pMaterial->m_RefractionIndex);
			Ray refractRay = Ray(firstHit.pIntersect, refractDirection);
			transmittedColor = ShootRay(refractRay, objects, lights, firstHit.pMaterial->m_RefractionIndex, bounce + 1);
			Elite::RGBColor color{ reflectColor * reflectanceRatio + transmittedColor * (1 - reflectanceRatio) };
			color.MaxToOne();
			return color;
		}

		reflectColor.MaxToOne();
		return reflectColor;
	}

	
	color.MaxToOne();
	return color;
}

float Elite::Renderer::Fresnel(const Elite::FVector3& incomingDirection, const Elite::FVector3& surfaceNormal, const float refractionIdxBefore, const float refractionIdxAfter)
{
	float cosIncoming = Elite::Clamp(Elite::Dot(incomingDirection, surfaceNormal), -1.f, 1.f);
	// Compute sin using Snells law
	float sinOutgoing = (refractionIdxAfter / refractionIdxBefore) * sqrtf(std::max(0.f, 1 - cosIncoming * cosIncoming));

	if (sinOutgoing > 1)
	{
		return 1; // total reflection 
	}
	else
	{
		float cosOutgoing = sqrtf(std::max(0.f, 1 - sinOutgoing * sinOutgoing));
		cosOutgoing = fabsf(cosOutgoing);
		float fresnelEq1 = ((refractionIdxAfter * cosIncoming) - (refractionIdxBefore * cosOutgoing)) / ((refractionIdxAfter * cosIncoming) + (refractionIdxBefore * cosOutgoing));
		float fresnelEq2 = ((refractionIdxBefore * cosIncoming) - (refractionIdxAfter * cosOutgoing)) / ((refractionIdxBefore * cosIncoming) + (refractionIdxAfter * cosOutgoing));
		return (fresnelEq1 * fresnelEq1 + fresnelEq2 * fresnelEq2) / 2; // transmittance is 1 - fresnelRatio
	}
}

Elite::FVector3 Elite::Renderer::Refract(const Elite::FVector3& incomingDirection, const Elite::FVector3& surfaceNormal, const float refractionIdxBefore, const float refractionIdxAfter)
{
	float cosIncoming = Elite::Clamp(Elite::Dot(incomingDirection, surfaceNormal), -1.f, 1.f);

	Elite::FVector3 refracted = surfaceNormal;

	if (cosIncoming)
		cosIncoming = -cosIncoming;
	else
		refracted = -refracted;

	float refractionIdxRatio = refractionIdxBefore / refractionIdxAfter;

	float k = 1 - refractionIdxRatio * refractionIdxRatio * (1 - cosIncoming * cosIncoming);

	return refractionIdxRatio * incomingDirection + (refractionIdxRatio * cosIncoming - sqrtf(k)) * refracted;
}
