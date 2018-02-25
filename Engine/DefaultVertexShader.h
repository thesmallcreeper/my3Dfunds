#pragma once

#include "IndexedTriangleList.h"

template<class Vertex>
class DefaultVertexShader
{
public:
	typedef Vertex Output;
public:
	void BindRotation(const Mat3& rotation_in)
	{
		rotation = rotation_in;
	}
	void BindTranslation(const Vec3& translation_in)
	{
		translation = translation_in;
	}
	void BindCameraPosition(const Vec3& position_in)
	{
		position = position_in;
	}
	void BindCameraRotation(const Mat3& camerarotation_in)
	{
		camerarotation = camerarotation_in;
	}
	IndexedTriangleList<Output> operator()(std::vector<Vertex> vertices_in, std::vector<size_t> indices_in)
	{
	//	std::vector<Output> vertices_out;
	//	vertices_out.reserve(vertices_in.size());

		std::transform(vertices_in.begin(), vertices_in.end(),
		vertices_in.begin(),
		[&](const auto& lambdain) -> Vertex {return { (lambdain.pos * rotation + translation - position) * camerarotation, lambdain }; });

		IndexedTriangleList<Output> out(vertices_in, indices_in);
		return out;
	}

private:
	Mat3 rotation;
	Mat3 camerarotation;
	Vec3 translation;
	Vec3 position;
};