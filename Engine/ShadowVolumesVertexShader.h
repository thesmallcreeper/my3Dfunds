#pragma once

template<class Vertex>
class ShadowVolumesVertexShader
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
	void BindLightSourcePosition(const Vec3& lightsourceposition_in)
	{
		lightsourceposition = lightsourceposition_in;
	}

	void operator()(std::vector<Vertex>& vertices_in, std::vector<size_t>& indices_in) const
	{
		std::transform(vertices_in.begin(), vertices_in.end(),
			vertices_in.begin(),
			[&](const auto& lambdain) -> Vertex {return { (lambdain.pos * rotation + translation - position) * camerarotation, lambdain }; });

		Vec3 lightsourceposition_use = (lightsourceposition - position) * camerarotation;

		// Create the new vertices vector
		std::vector<Vertex> volumes_new_points;
		volumes_new_points.reserve(vertices_in.size());

		std::transform(vertices_in.begin(), vertices_in.end(),
			volumes_new_points.begin(),
			[&](const auto& lambdain) -> Vertex
		{
			Vec3 direction(lambdain.pos.x - lightsourceposition_use.x, lambdain.pos.y - lightsourceposition_use.y, lambdain.pos.z - lightsourceposition_use.z);
			direction.Normalize();
			Vertex result(direction * 128 + lightsourceposition_use);
			return result;
		});

		std::vector<Output> vertices_out;
		vertices_out.reserve(vertices_in.size() * 2);

		for (size_t i = 0; i < vertices_in.size() * 2; i++)
		{
			if (i % 2 == 0)
				vertices_out.emplace_back(vertices_in[i / 2]);
			else
				vertices_out.emplace_back(volumes_new_points[i / 2]);
		}

		// Create vertices_codes
		unsigned __int8* vertices_codes = new unsigned __int8[vertices_in.size()];
		memset(vertices_codes, 0, vertices_in.size());

		bool* triangles_codes = new bool[indices_in.size() / 3];
		memset(triangles_codes, false, indices_in.size() / 3);

		for (size_t i = 0; i < indices_in.size() / 3; i++)
		{
			Vec3 faceNormal = ((vertices_in[indices_in[i * 3 + 1]].pos - vertices_in[indices_in[i * 3]].pos).GetNormalized() % (vertices_in[indices_in[i * 3 + 2]].pos - vertices_in[indices_in[i * 3]].pos).GetNormalized()).GetNormalized();
			Vec3 lightToFace = (lightsourceposition_use - vertices_in[indices_in[i * 3]].pos).GetNormalized();

			if (faceNormal * lightToFace >= 0.0f)
			{
				vertices_codes[indices_in[i * 3]] |= 0b00000001;
				vertices_codes[indices_in[i * 3 + 1]] |= 0b00000001;
				vertices_codes[indices_in[i * 3 + 2]] |= 0b00000001;
				triangles_codes[i] = true;
			}
			else
			{
				vertices_codes[indices_in[i * 3]] |= 0b00000010;
				vertices_codes[indices_in[i * 3 + 1]] |= 0b00000010;
				vertices_codes[indices_in[i * 3 + 2]] |= 0b00000010;
				triangles_codes[i] = false;
			}
		}

		std::vector<size_t> indices_out;
		for (size_t i = 0; i < indices_in.size(); i++)
		{
			if (vertices_codes[indices_in[i]] == 0b00000011 && triangles_codes[i / 3] == true)
			{
				switch (i % 3) {
				case 0:
				case 1:
					if (vertices_codes[indices_in[i + 1]] == 0b00000011)
					{
						indices_out.emplace_back(indices_in[i + 1] * 2);
						indices_out.emplace_back(indices_in[i] * 2);
						indices_out.emplace_back(indices_in[i] * 2 + 1);

						indices_out.emplace_back(indices_in[i + 1] * 2);
						indices_out.emplace_back(indices_in[i] * 2 + 1);
						indices_out.emplace_back(indices_in[i + 1] * 2 + 1);
					}
					break;
				case 2:
					if (vertices_codes[indices_in[i - 2]] == 0b00000011)
					{
						indices_out.emplace_back(indices_in[i - 2] * 2);
						indices_out.emplace_back(indices_in[i] * 2);
						indices_out.emplace_back(indices_in[i] * 2 + 1);

						indices_out.emplace_back(indices_in[i - 2] * 2);
						indices_out.emplace_back(indices_in[i] * 2 + 1);
						indices_out.emplace_back(indices_in[i - 2] * 2 + 1);
					}
					break;
				}
			}
		}

		vertices_in = std::move(vertices_out);
		indices_in = std::move(indices_out);

	}

private:
	Mat3 rotation;
	Mat3 camerarotation;
	Vec3 translation;
	Vec3 position;

	Vec3 lightsourceposition;
};
