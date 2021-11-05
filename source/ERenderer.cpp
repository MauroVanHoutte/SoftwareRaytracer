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


Elite::Renderer::Renderer(SDL_Window * pWindow)
	:m_HardShadowsEnabled{false}
	, m_RenderMode{RenderMode::All}
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

	m_FinishedThreads.resize(m_Height, false);
}

Elite::Renderer::~Renderer()
{
	//
}


void SetupRay(Ray& ray, int row, int col, uint32_t width, uint32_t height, const CameraData& cameraData);

void Elite::Renderer::Render(const Camera* camera)
{
	SDL_LockSurface(m_pBackBuffer);

	ResetBackbuffer();

	auto threadManager = ThreadManager::GetInstance();

	m_FinishedThreads.resize(threadManager->GetNrThreads(), false);
	std::fill(m_FinishedThreads.begin(), m_FinishedThreads.end(), false);

	const CameraData cameraData{ camera->GetCameraData() }; //caching this frames camera values
	const std::vector<RenderObject*> objectVector = ObjectManager::GetInstance()->GetObjectVector(); //caching objects
	const std::vector<Light*> lightVector = LightManager::GetInstance()->GetLightVector(); //caching lights
	//Loop over all the pixels
	
	size_t rowsPerThread = m_Height / threadManager->GetNrThreads();

	//Ray ray{ {}, {} }; //initialising ray that will be reused for every pixel
	//Ray shadowRay{ {}, {} };
	//HitRecord firstHit{ false, Elite::FPoint3{0.f, 0.f, 0.f}, ray.tMax, Elite::FVector3{0.f, 0.f, 0.f}, nullptr }; //initialising hitrecord that will be used for every pixel

	//for (uint32_t row = 0; row < m_Height; row++)
	//{
	//	for (uint32_t col = 0; col < m_Width; ++col)
	//	{
	//		SetupRay(ray, row, col, m_Width, m_Height, cameraData);
	//		firstHit.hit = false;
	//		firstHit.t = ray.tMax;

	//		Elite::RGBColor pixelColor{ 0.f, 0.f, 0.f };
	//		for (const RenderObject* object : objectVector) // checking if the ray intersects with any objects
	//		{
	//			object->Hit(ray, firstHit); // keeping the hitrecord of object closest to the ray origin
	//		}
	//		if (firstHit.hit)
	//		{
	//			CalculateIrradiance(lightVector, objectVector, shadowRay, firstHit, ray, pixelColor); //counting up Radiance of all lights
	//			int bounces{ 0 };
	//			while (bounces < m_MaxReflectionBounces)
	//			{
	//				++bounces;
	//				Ray reflectRay{};
	//				HitRecord reflectHit{ false, Elite::FPoint3{0.f, 0.f, 0.f}, reflectRay.tMax, Elite::FVector3{0.f, 0.f, 0.f}, nullptr };
	//				reflectRay.direction = Elite::Reflect(ray.direction, firstHit.normal);
	//				reflectRay.origin = firstHit.pIntersect;
	//				reflectRay.tMin = 0.01f;
	//				for (const RenderObject* object : objectVector)
	//				{
	//					object->Hit(reflectRay, reflectHit);
	//				}
	//				if (reflectHit.hit)
	//				{
	//					Elite::RGBColor reflectColor;
	//					CalculateIrradiance(lightVector, objectVector, shadowRay, reflectHit, reflectRay, reflectColor);
	//					pixelColor += reflectColor * 0.1f;
	//					firstHit = reflectHit;
	//				}
	//				else
	//					break;
	//			}
	//			pixelColor.MaxToOne();
	//			m_pBackBufferPixels[col + (row * m_Width)] = SDL_MapRGB(m_pBackBuffer->format,
	//				static_cast<uint8_t>(pixelColor.r * 255),
	//				static_cast<uint8_t>(pixelColor.g * 255),
	//				static_cast<uint8_t>(pixelColor.b * 255));
	//		}
	//	}
	//}
	
	for (uint32_t r = 0; r < m_Height; r += uint32_t(rowsPerThread))
	{
		threadManager->AddJob([&cameraData, &objectVector, &lightVector, r, rowsPerThread, this]() {

			Ray ray{ {}, {} }; //initialising ray that will be reused for every pixel
			Ray shadowRay{ {}, {} };
			HitRecord firstHit{ false, Elite::FPoint3{0.f, 0.f, 0.f}, ray.tMax, Elite::FVector3{0.f, 0.f, 0.f}, nullptr }; //initialising hitrecord that will be used for every pixel

			for (uint32_t row = r; row < r + rowsPerThread; row++)
			{
				for (uint32_t col = 0; col < m_Width; ++col)
				{
					SetupRay(ray, row, col, m_Width, m_Height, cameraData);
					firstHit.hit = false;
					firstHit.t = ray.tMax;

					Elite::RGBColor pixelColor{ 0.f, 0.f, 0.f };
					for (const RenderObject* object : objectVector) // checking if the ray intersects with any objects
					{
						object->Hit(ray, firstHit); // keeping the hitrecord of object closest to the ray origin
					}
					if (firstHit.hit)
					{
						CalculateIrradiance(lightVector, objectVector, shadowRay, firstHit, ray, pixelColor); //counting up Radiance of all lights
						int bounces{ 0 };
						while (bounces < m_MaxReflectionBounces)
						{
							++bounces;
							Ray reflectRay{};
							HitRecord reflectHit{ false, Elite::FPoint3{0.f, 0.f, 0.f}, reflectRay.tMax, Elite::FVector3{0.f, 0.f, 0.f}, nullptr };
							reflectRay.direction = Elite::Reflect(ray.direction, firstHit.normal);
							reflectRay.origin = firstHit.pIntersect;
							reflectRay.tMin = 0.01f;
							for (const RenderObject* object : objectVector)
							{
								object->Hit(reflectRay, reflectHit);
							}
							if (reflectHit.hit)
							{
								Elite::RGBColor reflectColor;
								CalculateIrradiance(lightVector, objectVector, shadowRay, reflectHit, reflectRay, reflectColor);
								pixelColor += reflectColor * 0.1f;
								firstHit = reflectHit;
							}
							else
								break;
						}
						pixelColor.MaxToOne();
						m_pBackBufferPixels[col + (row * m_Width)] = SDL_MapRGB(m_pBackBuffer->format,
							static_cast<uint8_t>(pixelColor.r * 255),
							static_cast<uint8_t>(pixelColor.g * 255),
							static_cast<uint8_t>(pixelColor.b * 255));
					}
				}
			}
			m_FinishedThreads[r / rowsPerThread] = true;
		});
	}

	while (true)
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(1));
		if (std::all_of(m_FinishedThreads.begin(), m_FinishedThreads.end(), [](bool b) {return b; }))
			break;
	}

	SDL_UnlockSurface(m_pBackBuffer);
	SDL_BlitSurface(m_pBackBuffer, 0, m_pFrontBuffer, 0);
	SDL_UpdateWindowSurface(m_pWindow);
}

void Elite::Renderer::UpdateRenderOptions()
{
	auto keyStates = SDL_GetKeyboardState(nullptr);
	if (keyStates[SDL_SCANCODE_L])
	{
		LightManager::GetInstance()->ToggleLight(0);
	}
	if (keyStates[SDL_SCANCODE_K])
	{
		LightManager::GetInstance()->ToggleLight(1);
	}
	if (keyStates[SDL_SCANCODE_J])
	{
		LightManager::GetInstance()->ToggleLight(2);
	}

	if (keyStates[SDL_SCANCODE_Z])
	{
		m_HardShadowsEnabled = !m_HardShadowsEnabled;
	}

	if (keyStates[SDL_SCANCODE_T])
	{
		if (m_RenderMode == RenderMode::All)
		{
			m_RenderMode = RenderMode::IrradianceOnly;
		} 
		else
			m_RenderMode = RenderMode(int(m_RenderMode) + 1);
	}
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

void Elite::Renderer::ResetBackbuffer()
{
	for (uint32_t r = 0; r < m_Height; r++)
	{
		for (uint32_t c = 0; c < m_Width; c++) //resetting color
		{
			m_pBackBufferPixels[c + (r * m_Width)] = SDL_MapRGB(m_pBackBuffer->format,
				static_cast<uint8_t>(135),
				static_cast<uint8_t>(206),
				static_cast<uint8_t>(235));
		}
	}
}

void SetupRay(Ray& ray, int row, int col, uint32_t width, uint32_t height, const CameraData& cameraData)
{
	float yScreenSpace{ 1 - (2 * (row + 0.5f) / float(height)) };
	float yCameraSpace{ yScreenSpace * cameraData.fov };

	float xScreenSpace{ (2 * (col + 0.5f) / float(width)) - 1 };
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
