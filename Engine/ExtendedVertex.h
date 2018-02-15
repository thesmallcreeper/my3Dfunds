#pragma once
template<class V>
class ExtVertex
{
public:
	V Vertex;
	float w;

	ExtVertex() = default;

	ExtVertex(V VertexIn)
	{
		Vertex = VertexIn;
		w = 1.0f;
	}

	template<class V>
	ExtVertex(ExtVertex<V> extVertexIn)
	{
		Vertex = extVertexIn.Vertex;
		w = extVertexIn.w;
	}

	ExtVertex(V VertexIn, float wIn)
	{
		Vertex = VertexIn;
		w = wIn;
	}

	ExtVertex&	operator=(const ExtVertex &rhs)
	{
		Vertex = rhs.Vertex;
		w = rhs.w;
		return *this;
	}

	void operator *= (const float ratio) 
	{
		Vertex *= ratio;
		w *= ratio;
	}

	void WtoVertexZ ()
	{
		Vertex.pos.z = w;
	}

};