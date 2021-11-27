#include "Camera.h"
#include <SDL.h>
#include <iostream>

Camera::Camera(Elite::FPoint3 pos, Elite::FVector3 forward, float aspectRatio)
{
	m_CameraData.pos = pos;
	m_CameraData.forward = Elite::GetNormalized(forward);
	m_CameraData.aspectRatio = aspectRatio;
	RecalculateONB();
}

void Camera::Update(float elapsedSec)
{
	CalculateFOV(m_FOVAngle);
	HandleMouseMovement();
	HandleKeyboardMovement(elapsedSec);
}

void Camera::CalculateFOV(float angle)
{
	m_CameraData.fov = tanf(angle / 2);
}

void Camera::HandleMouseMovement()
{
	Elite::IPoint2 currentMousePos;
	auto mouseState = SDL_GetRelativeMouseState(&currentMousePos.x, &currentMousePos.y);
	float displacementX{ float(currentMousePos.x) }; //mouse movement for current frame
	float displacementY{ float(currentMousePos.y) }; 
	float angleScalingFactor{ 10.f };
	bool moved{ false };

	Elite::FVector3 up{m_CameraData.ONB[1]};

	if ((mouseState & SDL_BUTTON(SDL_BUTTON_LEFT)) && (mouseState & SDL_BUTTON(SDL_BUTTON_RIGHT)))
	{
		m_CameraData.pos -= up * (displacementY); //move along the camera's up vector 
		moved = true;
	} else 
	if (mouseState == SDL_BUTTON(SDL_BUTTON_LEFT))
	{
		m_CameraData.forward = Elite::GetNormalized(Elite::MakeRotationY(-Elite::ToRadians(displacementX/angleScalingFactor)) * m_CameraData.forward); //rotate around m_Forward's y component
		m_CameraData.pos += m_CameraData.forward * displacementY; //move along camera's forward vector
		moved = true;
	} else
	if (mouseState == SDL_BUTTON(SDL_BUTTON_RIGHT))
	{
		Elite::FVector3 right{ m_CameraData.ONB[0] };

		m_CameraData.forward = Elite::GetNormalized(Elite::MakeRotation(Elite::ToRadians(-displacementY / angleScalingFactor), m_CameraData.ONB[0].xyz) * m_CameraData.forward);		
		
		m_CameraData.forward = Elite::GetNormalized(Elite::MakeRotation(Elite::ToRadians(-displacementX / angleScalingFactor), m_CameraData.ONB[1].xyz) * m_CameraData.forward); //rotate around m_Forward's y component
		
		moved = true;
	}

	if (moved)
	{
		RecalculateONB();
	}
}

void Camera::HandleKeyboardMovement(float elapsedSec)
{
	bool moved{ false };
	auto keyStates = SDL_GetKeyboardState(nullptr);
	if (keyStates[SDL_SCANCODE_W])
	{
		m_CameraData.pos -= m_CameraData.forward * m_MoveSpeed * elapsedSec; 
		moved = true;
	}
	if (keyStates[SDL_SCANCODE_S])
	{
		m_CameraData.pos += m_CameraData.forward * m_MoveSpeed * elapsedSec;
		moved = true;
	}
	if (keyStates[SDL_SCANCODE_A])
	{
		Elite::FVector3 right;
		right.x = m_CameraData.ONB[0].x;
		right.y = m_CameraData.ONB[0].y;
		right.z = m_CameraData.ONB[0].z;
		m_CameraData.pos -= right * m_MoveSpeed * elapsedSec;
		moved = true;
	}
	if (keyStates[SDL_SCANCODE_D])
	{
		Elite::FVector3 right;
		right.x = m_CameraData.ONB[0].x;
		right.y = m_CameraData.ONB[0].y;
		right.z = m_CameraData.ONB[0].z;
		m_CameraData.pos += right * m_MoveSpeed * elapsedSec;
		moved = true;
	}
	if (moved)
	{
		RecalculateONB();
	}
}

float Camera::GetFOV() const 
{
	return m_CameraData.fov;
}

float Camera::GetAspectRatio() const 
{
	return m_CameraData.aspectRatio;
}

const Elite::FPoint3& Camera::GetPos() const
{
	return m_CameraData.pos;
}

const Elite::FVector3& Camera::GetForward() const
{
	return m_CameraData.forward;
}

const Elite::FMatrix4& Camera::GetONB() const
{
	return m_CameraData.ONB;
}

const CameraData& Camera::GetCameraData() const
{
	return m_CameraData;
}

void Camera::RecalculateONB()
{
	Elite::FVector3 right{ Elite::Cross({0.f,1.f,0.f}, m_CameraData.forward) };
	Elite::FVector3 up{ Elite::Cross(m_CameraData.forward, right)};

	m_CameraData.ONB[0] = { right, 0 };
	m_CameraData.ONB[1] = { up, 0 };
	m_CameraData.ONB[2] = { m_CameraData.forward, 0 };
	m_CameraData.ONB[3] = { Elite::FVector3{m_CameraData.pos}, 1 };
}

void Camera::MoveSpeedUp()
{
	if (m_MoveSpeed < 1000.f)
	{
		m_MoveSpeed += 20.f;
	}
}

void Camera::MoveSpeedDown()
{
	if (m_MoveSpeed > 30.f)
	{
		m_MoveSpeed -= 20.f;
	}
}

void Camera::FOVAngleUp()
{
	m_FOVAngle += Elite::ToRadians(5.f);
}

void Camera::FOVAngleDown()
{
	m_FOVAngle -= Elite::ToRadians(5.f);
}
