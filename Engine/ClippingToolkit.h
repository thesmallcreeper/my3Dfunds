#pragma once
#include "Vec3.h"
#include "Mat3.h"
#include "ExtendedVertex.h"

typedef int OutCode;

const int INSIDEC = 0;		// 000000
const int LEFTC = 1;		// 000001
const int RIGHTC = 2;		// 000010
const int BOTTOMC = 4;		// 000100
const int TOPC = 8;			// 001000
const int NEARC = 16;		// 010000
const int FARC = 32;		// 100000

OutCode ClippingOutCode(const Vec3& point)
{
	OutCode code = 0;

	code = INSIDEC;					// initialised as being inside of [[clip window]]

	if (point.x < -1)				// to the left of clip window
		code |= LEFTC;
	else if (point.x > 1)			// to the right of clip window
		code |= RIGHTC;
	if (point.y < -1)				// below the clip window
		code |= BOTTOMC;
	else if (point.y > 1)			// above the clip window
		code |= TOPC;
	if (point.z < -1)				//far from the clip window
		code |= NEARC;
	else if (point.z > 1)			// near the clip window
		code |= FARC;

	return code;
}

// the following functions find where a line get cutted by a specific surface

template <typename T>
ExtVertex<T> ComputeIntersectionLEFT(const ExtVertex<T> eA, const ExtVertex<T> eB) {
	ExtVertex<T> eOut ;

	const float t = (-1.0f - eA.Vertex.pos.x) / (eB.Vertex.pos.x - eA.Vertex.pos.x);

	eOut.Vertex = eA.Vertex + (eB.Vertex - eA.Vertex) * t;
	eOut.Vertex.pos.x = -1.0f;

	eOut.iZ = eA.iZ + t * (eB.iZ - eA.iZ);

	return eOut;
}

template <typename T>
ExtVertex<T> ComputeIntersectionRIGHT(const ExtVertex<T> eA, const ExtVertex<T> eB) {
	ExtVertex<T> eOut ;

	const float t = ( 1.0f - eA.Vertex.pos.x) / (eB.Vertex.pos.x - eA.Vertex.pos.x);

	eOut.Vertex = eA.Vertex + (eB.Vertex - eA.Vertex) * t;
	eOut.Vertex.pos.x = 1.0f;

	eOut.iZ = eA.iZ + t * (eB.iZ - eA.iZ);

	return eOut;
}

template <typename T>
ExtVertex<T> ComputeIntersectionBOTTOM(const ExtVertex<T> eA, const ExtVertex<T> eB) {
	ExtVertex<T> eOut;

	const float t = (-1.0f - eA.Vertex.pos.y) / (eB.Vertex.pos.y - eA.Vertex.pos.y);

	eOut.Vertex = eA.Vertex + (eB.Vertex - eA.Vertex) * t;
	eOut.Vertex.pos.y = -1.0f;

	eOut.iZ = eA.iZ + t * (eB.iZ - eA.iZ);

	return eOut;
}

template <typename T>
ExtVertex<T> ComputeIntersectionTOP(const ExtVertex<T> eA, const ExtVertex<T> eB) {
	ExtVertex<T> eOut;

	const float t = (1.0f - eA.Vertex.pos.y) / (eB.Vertex.pos.y - eA.Vertex.pos.y);

	eOut.Vertex = eA.Vertex + (eB.Vertex - eA.Vertex) * t;
	eOut.Vertex.pos.y =  1.0f;

	eOut.iZ = eA.iZ + t * (eB.iZ - eA.iZ);

	return eOut;
}

template <typename T>
ExtVertex<T> ComputeIntersectionNEAR(const ExtVertex<T> eA, const ExtVertex<T> eB) {
	ExtVertex<T> eOut;

	const float t = (-1.0f - eA.Vertex.pos.z) / (eB.Vertex.pos.z - eA.Vertex.pos.z);

	eOut.Vertex = eA.Vertex + (eB.Vertex - eA.Vertex) * t;
	eOut.Vertex.pos.z = -1.0f;

	eOut.iZ = eA.iZ + t * (eB.iZ - eA.iZ);

	return eOut;
}

template <typename T>
ExtVertex<T> ComputeIntersectionFAR(const ExtVertex<T> eA, const ExtVertex<T> eB) {
	ExtVertex<T> eOut;

	const float t = ( 1.0f - eA.Vertex.pos.z) / (eB.Vertex.pos.z - eA.Vertex.pos.z);
 
	eOut.Vertex = eA.Vertex + (eB.Vertex - eA.Vertex) * t;
	eOut.Vertex.pos.z =  1.0f;

	eOut.iZ = eA.iZ + t * (eB.iZ - eA.iZ);

	return eOut;
}