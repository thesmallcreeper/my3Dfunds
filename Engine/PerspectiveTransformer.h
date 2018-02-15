#pragma once
#include "Vec3.h"
#include "Mat3.h"
#include "ExtendedVertex.h"

class PerspectiveTransformer
{
public:
	PerspectiveTransformer(float l = -1.0f, float r = 1.0f, float b = -1.0f, float t = 1.0f, float n = -1.0f, float f = -5.0f) {
		persMat = { 2 * n / (r - l)	, 0					, -(r + l) / (r - l),
					0				, 2 * n / (t - b)	, -(t + b) / (t - b),
					0				, 0					,  (f + n) / (f - n)};

		persVec = { 0				, 0					, -2 * f * n / (f - n) };

	}

	template<class ExtVertex>
	void TransformMatrix(ExtVertex& ev) const
	{
		ev.w = ev.Vertex.pos.z;

		ev.Vertex.pos *= persMat;
		ev.Vertex.pos += persVec;


	}
	template<class ExtVertex>
	void TransformDivision(ExtVertex& ev) const
	{
		ev.w = 1.0f / ev.w;
		ev.Vertex *= ev.w;

	}
private:
	Mat3 persMat;
	Vec3 persVec;
};
