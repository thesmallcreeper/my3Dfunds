#pragma once

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
	void operator()(std::vector<Vertex>& vertices, std::vector<size_t>& indices) const
	{

			std::transform(vertices.begin(), vertices.end(),
			vertices.begin(),
			[&](const auto& lambdain) -> Output {return { (lambdain.pos * rotation + translation - position) * camerarotation, lambdain }; });

	}

private:
	Mat3 rotation;
	Mat3 camerarotation;
	Vec3 translation;
	Vec3 position;
};