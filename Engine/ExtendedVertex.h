#pragma once
template<class V>
class ExtVertex
{
public:
	V Vertex;
	float iZ;

	ExtVertex() = default;

	ExtVertex(V VertexIn) {
		Vertex = VertexIn;
		iZ = 1.0f;
	}

	ExtVertex(V VertexIn, float iZIn) {
		Vertex = VertexIn;
		iZ = iZin;
	}

	void operator *= (const float ratio) {
		Vertex *= ratio;
		iZ *= ratio;
	}

	void iZtoVertexZ (){
		Vertex.pos.z = iZ;
	}
};