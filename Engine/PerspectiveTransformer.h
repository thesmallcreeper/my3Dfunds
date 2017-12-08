#pragma once
#include "Vec3.h"
#include "Mat3.h"
#include "ExtendedVertex.h"

#include <sstream>

class PerspectiveTransformer
{
public:
	PerspectiveTransformer(float l = -1.0f, float r = 1.0f, float b = -1.0f, float t = 1.0f, float n = -1.0f, float f = -5.0f) {
		persMat = { 2 * n / (r - l)	, 0					, -(r + l) / (r - l),
					0				, 2 * n / (t - b)	, -(t + b) / (t - b),
					0				, 0					,  (f + n) / (f - n)};

		persVec = { 0				, 0					, -2 * f * n / (f - n) };

		std::stringstream ss;

		ss	<< "PersMat\n"
			<< persMat.elements[0][0] << " " << persMat.elements[1][0] << " " << persMat.elements[2][0] << std::endl
			<< persMat.elements[0][1] << " " << persMat.elements[1][1] << " " << persMat.elements[2][1] << std::endl
			<< persMat.elements[0][2] << " " << persMat.elements[1][2] << " " << persMat.elements[2][2] << std::endl;
		ss	<< "PersVec\n"
			<< persVec.x << " " << persVec.y << " " << persVec.z << std::endl;

		OutputDebugStringA( ss.str().c_str() );
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
		ev.iZ = 1.0f / ev.w;
		ev.Vertex *= ev.iZ;

	}
private:
	Mat3 persMat;
	Vec3 persVec;
};
