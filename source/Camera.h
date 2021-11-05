#pragma once
#include <SDL_keycode.h>
#include <SDL_stdinc.h>
#include "EMath.h"
#include "EMathUtilities.h"

struct CameraData
{
	float fov;
	float aspectRatio;
	Elite::FPoint3 pos;
	Elite::FVector3 forward;
	Elite::FMatrix4 ONB;
};

class Camera
{
public:
	Camera(Elite::FPoint3 pos, Elite::FVector3 forward, float AspectRatio);
	~Camera() = default;

	void Update(float elpasedSec);

	float GetFOV() const;
	float GetAspectRatio() const;
	const Elite::FPoint3& GetPos() const;
	const Elite::FVector3& GetForward() const;
	const Elite::FMatrix4& GetONB() const;

	const CameraData& GetCameraData() const;

	void RecalculateONB();
	void MoveSpeedUp();
	void MoveSpeedDown();
	void FOVAngleUp();
	void FOVAngleDown();

private:

	CameraData m_CameraData;

	float m_FOVAngle = float(M_PI / 4.f);
	float m_S = 1.f;

	float m_MoveSpeed = 50.f;

	void CalculateFOV(float angle);
	void HandleMouseMovement();
	void HandleKeyboardMovement(float elapsedSec);
};

