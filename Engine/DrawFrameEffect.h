#pragma once

#include <cmath>
#include "Pipeline.h"
#include "DefaultVertexShader.h"
#include "VertexTypes.h"

// basic texture effect
class DrawFrameEffect
{
public:
	typedef DefaultVertex Vertex;
	typedef DefaultVertexWithTC VertexWithTC;

public:
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
			Color colorTex = pTex->GetPixel(
								std::min((unsigned int)(in.t.x * tex_width + 0.5f), tex_xclamp),
								std::min((unsigned int)(in.t.y * tex_height + 0.5f), tex_yclamp)
							);
			if (stencil.get())
				return colorTex;
			else
				return colorTex / 3.f;
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