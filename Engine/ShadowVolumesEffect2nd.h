#pragma once

#include <cmath>
#include "Pipeline.h"
#include "VertexTypes.h"

// basic texture effect
class ShadowVolumesEffect2nd
{
public:
	typedef DefaultVertex Vertex;

public:
	// custom vs in order to create triangles that will allow the creation of shadow volumes
	class VertexShader
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

			std::vector<Vertex> volumes_new_points;
			volumes_new_points.reserve(vertices_in.size());

			std::transform(vertices_in.begin(), vertices_in.end(),
				volumes_new_points.begin(),
				[&](const auto& lambdain) -> Vertex
			{
				Vec3 direction(lambdain.pos.x - lightsourceposition_use.x, lambdain.pos.y - lightsourceposition_use.y, lambdain.pos.z - lightsourceposition_use.z);
				direction.Normalize();
				Vertex result(direction * 128 + lambdain.pos);
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

			std::vector<size_t> indices_out;
			indices_out.reserve(indices_in.size() * 6);

			for (size_t i = 0; i < indices_in.size(); i++)
			{
				switch (i % 3) {
				case 0:
				case 1:
					indices_out.emplace_back(indices_in[i + 1] * 2);
					indices_out.emplace_back(indices_in[i] * 2);
					indices_out.emplace_back(indices_in[i] * 2 + 1);

					indices_out.emplace_back(indices_in[i] * 2 + 1);
					indices_out.emplace_back(indices_in[i + 1] * 2);
					indices_out.emplace_back(indices_in[i + 1] * 2 + 1);

					break;

				case 2:
					indices_out.emplace_back(indices_in[i - 2] * 2);
					indices_out.emplace_back(indices_in[i] * 2);
					indices_out.emplace_back(indices_in[i] * 2 + 1);

					indices_out.emplace_back(indices_in[i] * 2 + 1);
					indices_out.emplace_back(indices_in[i - 2] * 2);
					indices_out.emplace_back(indices_in[i - 2] * 2 + 1);

					break;
				}
			}


			indices_in = std::move(indices_out);
			vertices_in = std::move(vertices_out);
		}

	private:
		Mat3 rotation;
		Mat3 camerarotation;
		Vec3 translation;
		Vec3 position;

		Vec3 lightsourceposition;
	};


	// custom gs gives every thriangles its texture coordinates
	class GeometryShader
	{
	public:
		typedef Vertex Output;

	public:
		template<class Vertex>
		Triangle<Output> operator()(const Vertex& in0, const Vertex& in1, const Vertex& in2, size_t triangle_index)
		{
			return{ in0, in1, in2 };
		}
	};

	// invoked for each pixel of a triangle
	// takes an input of attributes that are the
	// result of interpolating vertex attributes
	// and outputs a color
	class PixelShader
	{
	public:
		template<class Input>
		Color operator()(const Input& in, StencilBufferPtr& stencil) const
		{
			stencil.decrease();
			return Colors::Red;
		}
	};
public:
	VertexShader vs;
	GeometryShader gs;
	PixelShader ps;
};

