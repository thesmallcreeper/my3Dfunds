#pragma once

#include "Pipeline.h"
#include "DefaultGeometryShader.h"

class WaveVertexTextureEffect
{
public:
	class Vertex
	{
	public:
		Vertex() = default;
		Vertex(const Vec3& pos)
			:
			pos(pos)
		{}
		Vertex(const Vec3& pos, const Vertex& src)
			:
			t(src.t),
			pos(pos)
		{}
		Vertex(const Vec3& pos, const Vec2& t)
			:
			t(t),
			pos(pos)
		{}
		Vertex& operator+=(const Vertex& rhs)
		{
			pos += rhs.pos;
			t += rhs.t;
			return *this;
		}
		Vertex operator+(const Vertex& rhs) const
		{
			return Vertex(*this) += rhs;
		}
		Vertex& operator-=(const Vertex& rhs)
		{
			pos -= rhs.pos;
			t -= rhs.t;
			return *this;
		}
		Vertex operator-(const Vertex& rhs) const
		{
			return Vertex(*this) -= rhs;
		}
		Vertex& operator*=(float rhs)
		{
			pos *= rhs;
			t *= rhs;
			return *this;
		}
		Vertex operator*(float rhs) const
		{
			return Vertex(*this) *= rhs;
		}
		Vertex& operator/=(float rhs)
		{
			pos /= rhs;
			t /= rhs;
			return *this;
		}
		Vertex operator/(float rhs) const
		{
			return Vertex(*this) /= rhs;
		}
	public:
		Vec3 pos;
		Vec2 t;
	};

	// perturbes vertices in y axis in sin wave based on
	// x position and time
	class VertexShader
	{
	public:
		typedef Vertex Output;
	public:
		void BindRotation(const Mat3& rotation_in)
		{
			rotation = rotation_in;
		}
		void BindTranslation(const Vec3& translation_in)
		{
			translation = translation_in;
		}
		void BindCameraPosition(const Vec3& position_in)
		{
			position = position_in;
		}
		void BindCameraRotation(const Mat3& camerarotation_in)
		{
			camerarotation = camerarotation_in;
		}

		void operator()(std::vector<Vertex>& vertices, std::vector<size_t>& indices) const
		{
			std::transform(vertices.begin(), vertices.end(),
				vertices.begin(),
				[&](const auto& lambdain) -> Output	{	Vec3 pos = (lambdain.pos * rotation + translation - position);
														pos.y += amplitude * std::sin(time * freqScroll + pos.x * freqWave);
														return{ pos  * camerarotation,lambdain.t }; });
		}

		void SetTime(float t)
		{
			time = t;
		}
	private:
		Mat3 rotation;
		Mat3 camerarotation;
		Vec3 translation;
		Vec3 position;
		float time = 0.0f;
		float freqWave = 10.0f;
		float freqScroll = 5.0f;
		float amplitude = 0.05f;
	};

	// default gs passes vertices through and outputs triangle
	typedef DefaultGeometryShader<VertexShader::Output> GeometryShader;

	// texture clamped ps
	class PixelShader
	{
	public:
		template<class Input>
		Color operator()(const Input& in, const StencilBufferPtr& stencil) const
		{
			return pTex->GetPixel(
				std::min((unsigned int)(in.t.x * tex_width + 0.5f), tex_xclamp),
				std::min((unsigned int)(in.t.y * tex_height + 0.5f), tex_yclamp)
			);
		}
		void BindTexture(const std::wstring& filename)
		{
			pTex = std::make_unique<Surface>(Surface::FromFile(filename));
			tex_width = float(pTex->GetWidth());
			tex_height = float(pTex->GetHeight());
			tex_xclamp = (pTex->GetWidth() - 1);
			tex_yclamp = (pTex->GetHeight() - 1);
		}
	private:
		std::unique_ptr<Surface> pTex;
		float tex_width;
		float tex_height;
		unsigned int tex_xclamp;
		unsigned int tex_yclamp;
	};
public:
	VertexShader vs;
	GeometryShader gs;
	PixelShader ps;
};
