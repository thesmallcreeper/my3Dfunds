#pragma once

#pragma once

#pragma once

#include "Scene.h"
#include "Plane.h"
#include "Mat3.h"
#include "WaveVertexTextureEffect.h"

class VertexWaveScene : public Scene
{
public:
	typedef Pipeline<WaveVertexTextureEffect> Pipeline;
	typedef Pipeline::Vertex Vertex;
public:
	VertexWaveScene(Graphics& gfx)
		:
		itlist(Plane::GetSkinned<Vertex>(20)),
		zb(gfx.ScreenWidth, gfx.ScreenHeight),
		sb(gfx.ScreenWidth, gfx.ScreenHeight),
		pipeline(gfx, zb, sb),
		Scene("Test Plane Rippling VS")
	{
		pipeline.effect.ps.BindTexture(L"images\\sauron-bhole-100x100.png");
	}
	virtual void Update(Keyboard& kbd, Mouse& mouse, float dt) override
	{
		Vei2 mouseDelta = mouse.GetPos() - mouseLastPosition;
		mouseLastPosition = mouse.GetPos();

		if (kbd.KeyIsPressed('Q'))
		{
			theta_x = wrap_angle(theta_x + dTheta * dt);
		}
		if (kbd.KeyIsPressed('W'))
		{
			theta_y = wrap_angle(theta_y + dTheta * dt);
		}
		if (kbd.KeyIsPressed('E'))
		{
			theta_z = wrap_angle(theta_z + dTheta * dt);
		}
		if (kbd.KeyIsPressed('A'))
		{
			theta_x = wrap_angle(theta_x - dTheta * dt);
		}
		if (kbd.KeyIsPressed('S'))
		{
			theta_y = wrap_angle(theta_y - dTheta * dt);
		}
		if (kbd.KeyIsPressed('D'))
		{
			theta_z = wrap_angle(theta_z - dTheta * dt);
		}
		if (kbd.KeyIsPressed('R'))
		{
			offset_z += 2.0f * dt;
		}
		if (kbd.KeyIsPressed('F'))
		{
			offset_z -= 2.0f * dt;
		}
		time += dt;

		if (mouse.LeftIsPressed())
		{
			cameraP += PI * 0.0008f * mouseDelta.y;
			cameraH -= PI * 0.0008f * mouseDelta.x;

			if (cameraP < PI * 0.05f)
				cameraP = PI * 0.05f;
			if (cameraP > PI * 0.95f)
				cameraP = PI * 0.95f;
		}

		if (kbd.KeyIsPressed(VK_UP))
		{
			float speedOfTheKey = 2.0f;
			if (kbd.KeyIsPressed(VK_LEFT) || kbd.KeyIsPressed(VK_RIGHT))
				speedOfTheKey = sqrt(2.0f);

			positionZ += speedOfTheKey * sin(cameraP) * cos(cameraH) * dt;
			positionX += speedOfTheKey * sin(cameraP) * sin(cameraH) * dt;
			positionY += speedOfTheKey * cos(cameraP) * dt;
		}
		if (kbd.KeyIsPressed(VK_LEFT))
		{
			float speedOfTheKey = 2.0f;
			if (kbd.KeyIsPressed(VK_UP))
				speedOfTheKey = sqrt(2.0f);

			positionZ -= speedOfTheKey * sin(cameraH) * dt;
			positionX += speedOfTheKey * cos(cameraH) * dt;
		}
		if (kbd.KeyIsPressed(VK_RIGHT))
		{
			float speedOfTheKey = 2.0f;
			if (kbd.KeyIsPressed(VK_UP))
				speedOfTheKey = sqrt(2.0f);

			positionZ += speedOfTheKey * sin(cameraH) * dt;
			positionX -= speedOfTheKey * cos(cameraH) * dt;
		}
		if (kbd.KeyIsPressed(VK_DOWN))
		{
			float speedOfTheKey = 2.0f;
			if (kbd.KeyIsPressed(VK_LEFT) || kbd.KeyIsPressed(VK_RIGHT))
				speedOfTheKey = sqrt(2.0f);

			positionZ -= speedOfTheKey * sin(cameraP) * cos(cameraH) * dt;
			positionX -= speedOfTheKey * sin(cameraP) * sin(cameraH) * dt;
			positionY -= speedOfTheKey * cos(cameraP) * dt;
		}
	}
	virtual void Draw() override
	{
		pipeline.BeginFrame();
		// generate rotation matrix from euler angles
		// translation from offset
		const Mat3 rot =
			Mat3::RotationX(theta_x) *
			Mat3::RotationY(theta_y) *
			Mat3::RotationZ(theta_z);
		Vec3 cameraDir = { +sin(cameraP) * sin(cameraH),  +cos(cameraP)  , +sin(cameraP) * cos(cameraH) };
		// set pipeline transform
		pipeline.effect.vs.BindRotation(rot);
		pipeline.effect.vs.BindTranslation({ offset_x,offset_y,offset_z });
		pipeline.effect.vs.BindCameraPosition({ positionX,positionY,positionZ });
		pipeline.effect.vs.BindCameraRotation(Mat3::ChangeView(cameraDir, { 0.0f,1.0f,0.0f }));
		pipeline.effect.vs.SetTime(time);
		// render triangles
		pipeline.Draw(itlist);
	}
private:
	IndexedTriangleList<Vertex> itlist;
	Pipeline pipeline;
	WBuffer zb;
	StencilBuffer sb;

	static constexpr float dTheta = PI;
	float offset_x = +0.0f;
	float offset_y = +0.0f;
	float offset_z = -5.0f;

	float theta_x = +0.0f;
	float theta_y = +0.0f;
	float theta_z = +0.0f;

	float cameraP = +PI / 2.0f;
	float cameraH = PI;

	float positionX = +0.0f;
	float positionY = +0.0f;
	float positionZ = +0.0f;

	float time = 0.0f;

	Vei2 mouseLastPosition;
};