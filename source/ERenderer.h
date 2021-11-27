/*=============================================================================*/
// Copyright 2017-2019 Elite Engine
// Authors: Matthieu Delaere
/*=============================================================================*/
// ERenderer.h: class that holds the surface to render to, does traverse the pixels 
// and traces the rays using a tracer
/*=============================================================================*/
#ifndef ELITE_RAYTRACING_RENDERER
#define	ELITE_RAYTRACING_RENDERER

#include <cstdint>
#include <vector>
#include "Ray.h"
#include "HitRecord.h"
#include "ERGBColor.h"
#include "Camera.h"
#include <mutex>


struct SDL_Window;
struct SDL_Surface;
class Camera;
class Light;
class RenderObject;

enum class RenderMode
{
	IrradianceOnly,
	BRDFOnly,
	All
};

namespace Elite
{
	class Renderer final
	{
	public:
		Renderer(SDL_Window* pWindow);
		~Renderer();

		Renderer(const Renderer&) = delete;
		Renderer(Renderer&&) noexcept = delete;
		Renderer& operator=(const Renderer&) = delete;
		Renderer& operator=(Renderer&&) noexcept = delete;

		void Render(const Camera* camera);
		bool SaveBackbufferToImage() const;
		void IncreaseBounces();
		void DecreaseBounces();
		void ToggleShadows();
		void ToggleRenderMode();

	private:

		void ResetBackbuffer();

		void SetupRay(Ray& ray, int row, int col, const CameraData& cameraData);

		void CalculateIrradiance(const std::vector<Light*>& lightVector, const std::vector<RenderObject*>& objectVector, Ray& shadowRay, const HitRecord& firstHit, const Ray& cameraRay, RGBColor& pixelColor) const;

		void RenderScreenPortion(const CameraData& cameraData, const std::vector<RenderObject*>& objectVector, const std::vector<Light*>& lightVector, int startRow, int jobRows, int threadId);

		Elite::RGBColor ShootRay(const Ray& ray, const std::vector<RenderObject*>& objects, const std::vector<Light*>& lights, float refractionIdx, int& bounce);

		SDL_Window* m_pWindow = nullptr;
		SDL_Surface* m_pFrontBuffer = nullptr;
		SDL_Surface* m_pBackBuffer = nullptr;
		uint32_t* m_pBackBufferPixels = nullptr;
		uint32_t m_Width = 0;
		uint32_t m_Height = 0;
		int m_MaxReflectionBounces = 0;
		bool m_HardShadowsEnabled;
		RenderMode m_RenderMode;

		std::vector<bool> m_FinishedJobs;
		std::mutex m_FinishedJobsMutex;

		std::vector<float> m_SecsPerJob;
		std::vector<int> m_HitsPerJob;
		std::vector<int> m_RowsPerJob;
	};
}

#endif