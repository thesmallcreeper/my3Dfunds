#pragma once
template<class V>
class ExtVertex
{
public:
	V Vertex;
	float iZ;

	float& x;
	float& y;
	float& z;
	float& w;

	ExtVertex() : x(Vertex.pos.x), y(Vertex.pos.y), z(Vertex.pos.z), w(iZ) {}

	ExtVertex(V VertexIn) : x(Vertex.pos.x), y(Vertex.pos.y), z(Vertex.pos.z), w(iZ)
	{
		Vertex = VertexIn;
		iZ = 1.0f;
	}

	template<class V>
	ExtVertex(ExtVertex<V> extVertexIn) : x(Vertex.pos.x), y(Vertex.pos.y), z(Vertex.pos.z), w(iZ)
	{
		Vertex = extVertexIn.Vertex;
		iZ = extVertexIn.iZ;
	}

	ExtVertex(V VertexIn, float iZIn) : x(Vertex.pos.x), y(Vertex.pos.y), z(Vertex.pos.z), w(iZ)
	{
		Vertex = VertexIn;
		iZ = iZIn;
	}

	void operator *= (const float ratio) {
		Vertex *= ratio;
		iZ *= ratio;
	}

	void iZtoVertexZ (){
		Vertex.pos.z = iZ;
	}

	ExtVertex&	operator=(const ExtVertex &rhs)
	{
		Vertex = rhs.Vertex;
		iZ = rhs.iZ;
		return *this;
	}
	
};