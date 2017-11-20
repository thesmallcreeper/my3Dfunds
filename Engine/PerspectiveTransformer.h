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
	float Transform(ExtVertex& ev) const
	{
		float zInv;

		//Avoiding the division with 0, sth that might f up the iZ
		if(fabs(ev.Vertex.pos.z) > 0.0001)
			zInv = 1.0f / ev.Vertex.pos.z;
		else {
			if (ev.Vertex.pos.z > 0)
				zInv = 1 / 0.0001;
			else
				zInv = -1/ 0.0001;
		}

		ev.Vertex.pos *= persMat;
		ev.Vertex.pos += persVec;

		ev.Vertex *= zInv;

		ev.iZ = zInv;

		return zInv;
	}
private:
	Mat3 persMat;
	Vec3 persVec;
};
