#pragma once

#include "Scene.h"
#include "AddObjFileModelWithGS.h"
#include "Mat3.h"
#include "Pipeline.h"
#include "DrawFrameWithPhongLightEffect.h"
#include "ShadowVolumesEffect1st.h"
#include "ShadowVolumesEffect2nd.h"
#include "WBufferCreationEffect.h"

// scene demonstrating skinned model
class ShadowVolumesWithLightingScene : public Scene
{
public:
	typedef Pipeline<WBufferCreationEffect> PipelineWB;
	typedef Pipeline<ShadowVolumesEffect1st> PipelineSV1;
	typedef Pipeline<ShadowVolumesEffect2nd> PipelineSV2;
	typedef Pipeline<DrawFrameWithPhongLight> PipelineDF;
	typedef DefaultVertex Vertex;
public:
	ShadowVolumesWithLightingScene(Graphics& gfx, const std::wstring& odjfilename, const std::wstring& imagefilename, const float scale)
		:
		itlistWithTextures(AddObjFileModelWithGS::GetSkinnedFromObjFileWithGS<Vertex>(scale, odjfilename)),
		zb(gfx.ScreenWidth, gfx.ScreenHeight),
		sb(gfx.ScreenWidth, gfx.ScreenHeight),
		pipelinewb(gfx, zb, sb),
		pipelinesv1(gfx, zb, sb),
		pipelinesv2(gfx, zb, sb),
		pipelinedf(gfx, zb, sb),
		Scene("Textured Cube skinned using texture: " + std::string(imagefilename.begin(), imagefilename.end()))
	{
		pipelinedf.effect.ps.BindTexture(imagefilename);
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
			offset_x += 2.0f * dt;
		}
		if (kbd.KeyIsPressed('F'))
		{
			offset_x -= 2.0f * dt;
		}
		if (kbd.KeyIsPressed('T'))
		{
			offset_y += 2.0f * dt;
		}
		if (kbd.KeyIsPressed('G'))
		{
			offset_y -= 2.0f * dt;
		}
		if (kbd.KeyIsPressed('Y'))
		{
			offset_z += 2.0f * dt;
		}
		if (kbd.KeyIsPressed('H'))
		{
			offset_z -= 2.0f * dt;
		}

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
		pipelinewb.BeginFrame();
		// generate rotation matrix from euler angles
		// translation from offset
		const Mat3 rot =
			Mat3::RotationX(theta_x) *
			Mat3::RotationY(theta_y) *
			Mat3::RotationZ(theta_z);
		Vec3 cameraDir = { +sin(cameraP) * sin(cameraH),  +cos(cameraP)  , +sin(cameraP) * cos(cameraH) };
		// set pipeline transform for pipelineZB
		pipelinewb.effect.vs.BindRotation(rot);
		pipelinewb.effect.vs.BindTranslation({ offset_x,offset_y,offset_z });
		pipelinewb.effect.vs.BindCameraPosition({ positionX,positionY,positionZ });
		pipelinewb.effect.vs.BindCameraRotation(Mat3::ChangeView(cameraDir, { 0.0f,1.0f,0.0f }));
		// set pipeline transform and lightsource for pipelineSV1
		pipelinesv1.effect.vs.BindRotation(rot);
		pipelinesv1.effect.vs.BindTranslation({ offset_x,offset_y,offset_z });
		pipelinesv1.effect.vs.BindCameraPosition({ positionX,positionY,positionZ });
		pipelinesv1.effect.vs.BindCameraRotation(Mat3::ChangeView(cameraDir, { 0.0f,1.0f,0.0f }));
		pipelinesv1.effect.vs.BindLightSourcePosition({ 0.0f,10.0f,0.0f });
		// set pipeline transform and lightsource for pipelineSV2
		pipelinesv2.effect.vs.BindRotation(rot);
		pipelinesv2.effect.vs.BindTranslation({ offset_x,offset_y,offset_z });
		pipelinesv2.effect.vs.BindCameraPosition({ positionX,positionY,positionZ });
		pipelinesv2.effect.vs.BindCameraRotation(Mat3::ChangeView(cameraDir, { 0.0f,1.0f,0.0f }));
		pipelinesv2.effect.vs.BindLightSourcePosition({ 0.0f,10.0f,0.0f });
		// set pipeline transform and lightsource for pipelineDF
		pipelinedf.effect.vs.BindRotation(rot);
		pipelinedf.effect.vs.BindTranslation({ offset_x,offset_y,offset_z });
		pipelinedf.effect.vs.BindCameraPosition({ positionX,positionY,positionZ });
		pipelinedf.effect.vs.BindCameraRotation(Mat3::ChangeView(cameraDir, { 0.0f,1.0f,0.0f }));
		pipelinedf.effect.vs.BindLightSourcePosition({ 0.0f,10.0f,0.0f });
		// set geometry shader for pipelineDF
		pipelinedf.effect.gs.BindShader(itlistWithTextures.tc, itlistWithTextures.uvMapping);
		// set pixel shade for pipelineDF
		pipelinedf.effect.ps.BindLightSourcePosition({ 0.0f,10.0f,0.0f });
		pipelinedf.effect.ps.BindLightSourceDesnity(120.f);
		pipelinedf.effect.ps.BindAmbientLight(0.275f);
		// render triangles
		pipelinewb.switchZBufferSet(true);
		pipelinewb.switchZBufferEqualTest(false);
		pipelinewb.switchTurnFacing(false);
		pipelinewb.switchWriteOnGFX(false);
		pipelinewb.Draw(itlistWithTextures.itlist);

		pipelinesv1.switchZBufferSet(false);
		pipelinesv1.switchZBufferEqualTest(true);
		pipelinesv1.switchTurnFacing(false);
		pipelinesv1.switchWriteOnGFX(false);				// true for debugging
		pipelinesv1.Draw(itlistWithTextures.itlist);

		pipelinesv2.switchZBufferSet(false);
		pipelinesv2.switchZBufferEqualTest(true);
		pipelinesv2.switchTurnFacing(true);
		pipelinesv2.switchWriteOnGFX(false);				// true for debugging
		pipelinesv2.Draw(itlistWithTextures.itlist);

		pipelinedf.switchZBufferSet(false);
		pipelinedf.switchZBufferEqualTest(true);
		pipelinedf.switchTurnFacing(false);
		pipelinedf.switchWriteOnGFX(true);
		pipelinedf.Draw(itlistWithTextures.itlist);
	}
private:
	IndexedTriangleListWithTC<Vertex> itlistWithTextures;
	PipelineWB pipelinewb;
	PipelineSV1 pipelinesv1;
	PipelineSV2 pipelinesv2;
	PipelineDF pipelinedf;

	WBuffer zb;
	StencilBuffer sb;

	static constexpr float dTheta = PI;
	float offset_x = +0.0f;
	float offset_y = +0.0f;
	float offset_z = -10.0f;

	float theta_x = +0.0f;
	float theta_y = +0.0f;
	float theta_z = +0.0f;

	float cameraP = +PI / 2.0f;
	float cameraH = PI;

	float positionX = +0.0f;
	float positionY = +0.0f;
	float positionZ = +0.0f;

	Vei2 mouseLastPosition;
};


