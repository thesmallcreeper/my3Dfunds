#pragma once

#include <cmath>
#include <limits>
#include "Pipeline.h"
#include "ShadowVolumesVertexShader.h"
#include "VertexTypes.h"

// basic texture effect
class ShadowVolumesEffect1st
{
public:
	typedef DefaultVertex Vertex;

public:
	// use custom vs to create Vertex shaders
	typedef ShadowVolumesVertexShader<Vertex> VertexShader;

	// custom gs gives every thriangles its texture coordinates
	class GeometryShader
	{
	public:
		typedef Vertex Output;

	public:
		template<class Vertex>
		Triangle<Output> operator()(const Vertex& in0, const Vertex& in1, const Vertex& in2, size_t triangle_index)
		{
			return{ in0, in1, in2 };
		}
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
			stencil.increase();
			return Colors::Yellow;
		}
	};
public:
	VertexShader vs;
	GeometryShader gs;
	PixelShader ps;
};
