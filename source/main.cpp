//External includes
#include "vld.h"
#include "SDL.h"
#include "SDL_surface.h"
#undef main

//Standard includes
#include <iostream>

//Project includes
#include "OBJReader.h"
#include <vector>
#include "ETimer.h"
#include "ERenderer.h"
#include "ObjectManager.h"
#include "EMath.h"
#include "EMathUtilities.h"
#include "Sphere.h"
#include "Plane.h"
#include "Triangle.h"
#include "TriangleMesh.h"
#include "Camera.h"
#include "LightManager.h"
#include "PointLight.h"
#include "LambertMaterial.h"
#include "DirectionalLight.h"
#include "LambertDPhongS.h"
#include "LambertDCook-TorranceS.h"
#include "ThreadManager.h"

//#define BunnyScene

void ShutDown(SDL_Window* pWindow)
{
	SDL_DestroyWindow(pWindow);
	SDL_Quit();
}

int main(int argc, char* args[])
{
	//Unreferenced parameters
	(void)argc;
	(void)args;

	//Create window + surfaces
	SDL_Init(SDL_INIT_VIDEO);

	const uint32_t width = 640;
	const uint32_t height = 480;
	SDL_Window* pWindow = SDL_CreateWindow(
		"RayTracer - Mauro Van Houtte",
		SDL_WINDOWPOS_UNDEFINED,
		SDL_WINDOWPOS_UNDEFINED,
		width, height, 0);

	if (!pWindow)
		return 1;

	//Initialize "framework"
	Elite::Timer* pTimer = new Elite::Timer();
	Elite::Renderer* pRenderer = new Elite::Renderer(pWindow);
	Camera* m_pCamera = new Camera({ 0.f, 2.f, 30.f }, {0.f,0.f,1.f}, (float(width) / float(height)));

#ifdef BunnyScene
	TriangleMesh* pBunny{ new TriangleMesh{ "teapot.obj", new LambertCookTorrance{ true, 0.6f, Elite::RGBColor{0.955f, 0.638f, 0.538f} }, {0.f, 0.f, 0.f}, {0.f, 0.f, 1.f}, CullingMode::None }};
	ObjectManager::GetInstance()->AddTriangleMesh(pBunny);
#else
	/*ObjectManager::GetInstance()->AddSphere(new Sphere{ Elite::FPoint3{-2.5f, 1.f, 0.f}, 1.f, new LambertCookTorrance{ false, 0.1f} });
	ObjectManager::GetInstance()->AddSphere(new Sphere{ Elite::FPoint3{0.f, 1.f, 0.f}, 1.f, new LambertCookTorrance{ false, 0.6f } });
	ObjectManager::GetInstance()->AddSphere(new Sphere{ Elite::FPoint3{2.5f, 1.f, 0.f}, 1.f, new LambertCookTorrance{ false, 1.f } });
	ObjectManager::GetInstance()->AddSphere(new Sphere{ Elite::FPoint3{-2.5f, 3.f, 0.f}, 1.f, new LambertCookTorrance{ true, 0.1f, Elite::RGBColor{0.955f, 0.638f, 0.538f} } });
	ObjectManager::GetInstance()->AddSphere(new Sphere{ Elite::FPoint3{0.f, 3.f, 0.f}, 1.f, new LambertCookTorrance{ true, 0.6f, Elite::RGBColor{0.955f, 0.638f, 0.538f} } });*/
	ObjectManager::GetInstance()->AddSphere(new Sphere{ Elite::FPoint3{2.5f, 4.f, 0.f}, 1.f, new LambertCookTorrance{ true, 1.f, Elite::RGBColor{0.955f, 0.638f, 0.538f} } });
	TriangleMesh* pTriangle0{ new TriangleMesh{ "tri1.obj", new LambertMaterial{Elite::RGBColor{0.6f, 0.6f, 0.6f}, Elite::RGBColor{1.f,1.f,1.f}}, {3.f, 5.f, 0.f}, {0.f, 0.f, 1.f}, CullingMode::None } };
	TriangleMesh* pTriangle1{ new TriangleMesh{ "tri1.obj", new LambertMaterial{Elite::RGBColor{0.6f, 0.6f, 0.6f}, Elite::RGBColor{1.f,1.f,1.f}}, {0.f, 5.f, 0.f}, {0.f, 0.f, 1.f}, CullingMode::None } };
	TriangleMesh* pTriangle2{ new TriangleMesh{ "tri1.obj", new LambertMaterial{Elite::RGBColor{0.6f, 0.6f, 0.6f}, Elite::RGBColor{1.f,1.f,1.f}}, {-3.f, 5.f, 0.f}, {0.f, 0.f, 1.f}, CullingMode::None } };
	ObjectManager::GetInstance()->AddTriangleMesh(pTriangle0);
	ObjectManager::GetInstance()->AddTriangleMesh(pTriangle1);
	ObjectManager::GetInstance()->AddTriangleMesh(pTriangle2);

#endif // 
	ObjectManager::GetInstance()->AddPlane(new Plane{ Elite::FPoint3{0.f, 0.f, 0.f}, Elite::FVector3{0.f, 1.f, 0.f}, new LambertMaterial{Elite::RGBColor{0.6f, 0.6f, 0.6f}, Elite::RGBColor{1.f,1.f,1.f}} });
	ObjectManager::GetInstance()->AddPlane(new Plane{ Elite::FPoint3{0.f, 0.f, -5.f}, Elite::FVector3{0.f, 0.f, 1.f}, new LambertMaterial{Elite::RGBColor{0.6f, 0.6f, 0.6f}, Elite::RGBColor{1.f,1.f,1.f}} });
	ObjectManager::GetInstance()->AddPlane(new Plane{ Elite::FPoint3{-5.f, 0.f, 0.f}, Elite::FVector3{1.f, 0.f, 0.f}, new LambertMaterial{Elite::RGBColor{0.6f, 0.6f, 0.6f}, Elite::RGBColor{1.f,1.f,1.f}} });
	ObjectManager::GetInstance()->AddPlane(new Plane{ Elite::FPoint3{5.f, 0.f, 0.f}, Elite::FVector3{-1.f, 0.f, 0.f}, new LambertMaterial{Elite::RGBColor{0.6f, 0.6f, 0.6f}, Elite::RGBColor{1.f,1.f,1.f}} });

	LightManager::GetInstance()->AddPointLight(new PointLight{ Elite::FPoint3{3.f, 7.f, 5.f}, Elite::RGBColor{1.f, 0.5f, 1.f}, 150.f });
	LightManager::GetInstance()->AddPointLight(new PointLight{ Elite::FPoint3{-3.f, 3.f, 4.f}, Elite::RGBColor{0.6f, 0.6f, 1.f}, 45.f });
	LightManager::GetInstance()->AddDirectionalLight(new DirectionalLight{ {0.f, -1.f, -1.f}, {1.f,1.f,1.f}, 1.f });

	//Start loop
	pTimer->Start();
	float printTimer = 0.f;
	bool isLooping = true;
	bool takeScreenshot = false;
	while (isLooping)
	{
		//--------- Get input events ---------
		SDL_Event e;
		while (SDL_PollEvent(&e))
		{
			switch (e.type)
			{
			case SDL_QUIT:
				isLooping = false;
				break;
			case SDL_KEYUP:
				if(e.key.keysym.scancode == SDL_SCANCODE_X)
					takeScreenshot = true;
				break;
			case SDL_MOUSEWHEEL:
				if (e.wheel.y > 0)
				{
					m_pCamera->MoveSpeedUp();
				}
				if (e.wheel.y < 0)
				{
					m_pCamera->MoveSpeedDown();
				}
				break;
			case SDL_KEYDOWN:
				if (e.key.keysym.scancode == SDL_SCANCODE_1)
				{
					m_pCamera->FOVAngleUp();
				}
				if (e.key.keysym.scancode == SDL_SCANCODE_2)
				{
					m_pCamera->FOVAngleDown();
				}
				if (e.key.keysym.scancode == SDL_SCANCODE_KP_8)
				{
					pRenderer->IncreaseBounces();
				}
				if (e.key.keysym.scancode == SDL_SCANCODE_KP_2)
				{
					pRenderer->DecreaseBounces();
				}
				if (e.key.keysym.scancode == SDL_SCANCODE_T)
				{
					pRenderer->ToggleRenderMode();
				}
				if (e.key.keysym.scancode == SDL_SCANCODE_Z)
				{
					pRenderer->ToggleShadows();
				}
			}
		}

		//--------- Render ---------
		m_pCamera->Update(pTimer->GetElapsed());
		pRenderer->Render(m_pCamera);
	
#ifndef BunnyScene
		pTriangle0->Rotate(pTimer->GetElapsed());
		pTriangle1->Rotate(pTimer->GetElapsed());
		pTriangle2->Rotate(pTimer->GetElapsed());
#endif // !BunnyScene


		//--------- Timer ---------
		pTimer->Update();
		printTimer += pTimer->GetElapsed();
		if (printTimer >= 1.f)
		{
			printTimer = 0.f;
			std::cout << "FPS: " << pTimer->GetFPS() << std::endl;
		}

		//Save screenshot after full render
		if (takeScreenshot)
		{
			if (!pRenderer->SaveBackbufferToImage())
				std::cout << "Screenshot saved!" << std::endl;
			else
				std::cout << "Something went wrong. Screenshot not saved!" << std::endl;
			takeScreenshot = false;
		}
	}
	pTimer->Stop();

	//Shutdown "framework"
	ThreadManager::GetInstance()->Destroy();
	ObjectManager::Clear();
	LightManager::Clear();
	OBJReader::Delete();
	delete pRenderer;
	delete pTimer;
	delete m_pCamera;

	ShutDown(pWindow);
	return 0;
}
