#pragma once

#include <cmath>
#include "Pipeline.h"
#include "DefaultVertexShader.h"

// basic texture effect
class TextureEffectWithGS
{
public:
	// the vertex type that will be input into the pipeline
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
			pos(pos)
		{}
		Vertex& operator+=(const Vertex& rhs)
		{
			pos += rhs.pos;
			return *this;
		}
		Vertex operator+(const Vertex& rhs) const
		{
			return Vertex(*this) += rhs;
		}
		Vertex& operator-=(const Vertex& rhs)
		{
			pos -= rhs.pos;
			return *this;
		}
		Vertex operator-(const Vertex& rhs) const
		{
			return Vertex(*this) -= rhs;
		}
		Vertex& operator*=(float rhs)
		{
			pos *= rhs;
			return *this;
		}
		Vertex operator*(float rhs) const
		{
			return Vertex(*this) *= rhs;
		}
		Vertex& operator/=(float rhs)
		{
			pos /= rhs;
			return *this;
		}
		Vertex operator/(float rhs) const
		{
			return Vertex(*this) /= rhs;
		}
	public:
		Vec3 pos;
	};

	class VertexWithTC
	{
	public:
		VertexWithTC() = default;
		VertexWithTC(const Vec3& pos)
			:
			pos(pos)
		{}
		VertexWithTC(const Vec3& pos, const VertexWithTC& src)
			:
			t(src.t),
			pos(pos)
		{}
		VertexWithTC(const Vec3& pos, const Vec2& t)
			:
			t(t),
			pos(pos)
		{}
		VertexWithTC& operator+=(const VertexWithTC& rhs)
		{
			pos += rhs.pos;
			t += rhs.t;
			return *this;
		}
		VertexWithTC operator+(const VertexWithTC& rhs) const
		{
			return VertexWithTC(*this) += rhs;
		}
		VertexWithTC& operator-=(const VertexWithTC& rhs)
		{
			pos -= rhs.pos;
			t -= rhs.t;
			return *this;
		}
		VertexWithTC operator-(const VertexWithTC& rhs) const
		{
			return VertexWithTC(*this) -= rhs;
		}
		VertexWithTC& operator*=(float rhs)
		{
			pos *= rhs;
			t *= rhs;
			return *this;
		}
		VertexWithTC operator*(float rhs) const
		{
			return VertexWithTC(*this) *= rhs;
		}
		VertexWithTC& operator/=(float rhs)
		{
			pos /= rhs;
			t /= rhs;
			return *this;
		}
		VertexWithTC operator/(float rhs) const
		{
			return VertexWithTC(*this) /= rhs;
		}
	public:
		Vec3 pos;
		Vec2 t;
	};

	// default vs rotates and translates vertices
	// does not touch attributes
	typedef DefaultVertexShader<Vertex> VertexShader;

	// custom gs gives every thriangles its texture coordinates

	class GeometryShader
	{
	public:
		void BindShader(const std::vector<Vec2>& tc_in, const std::vector<size_t>& uvMapping_in)
		{
			tc = tc_in;
			uvMapping = uvMapping_in;
		}
	
	public:
		typedef VertexWithTC Output;
	
	public:
		template<class Vertex>
		Triangle<Output> operator()(const Vertex& in0, const Vertex& in1, const Vertex& in2, size_t triangle_index)
		{
			VertexWithTC out0(in0.pos, tc[uvMapping[triangle_index * 3]]);
			VertexWithTC out1(in1.pos, tc[uvMapping[triangle_index * 3 + 1]]);
			VertexWithTC out2(in2.pos, tc[uvMapping[triangle_index * 3 + 2]]);
			return{ out0, out1, out2 };
		}

	private:
		std::vector<Vec2> tc;
		std::vector<size_t> uvMapping;
	};


	// invoked for each pixel of a triangle
	// takes an input of attributes that are the
	// result of interpolating vertex attributes
	// and outputs a color
	class PixelShader
	{
	public:
		template<class Input>
		Color operator()(const Input& in, StencilBufferPtr& stencil) const
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