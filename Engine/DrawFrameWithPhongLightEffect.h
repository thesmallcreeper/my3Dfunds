#pragma once

#include <cmath>
#include "Pipeline.h"
#include "VertexTypes.h"

// basic texture effect
class DrawFrameWithPhongLight
{
public:
	typedef DefaultVertex Vertex;
	typedef DefaultVertexWithPhong VertexWithPhong;
	typedef DefaultVertexWithPhongAndTC VertexWithPhongAndTC;

public:
	class VertexShader
	{
	public:
		typedef VertexWithPhong Output;
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


		IndexedTriangleList<Output> operator()(std::vector<Vertex> vertices_in, std::vector<size_t> indices_in)
		{
			std::transform(vertices_in.begin(), vertices_in.end(),
				vertices_in.begin(),
				[&](const auto& lambdain) -> Vertex {return { (lambdain.pos * rotation + translation - position) * camerarotation, lambdain }; });

			Vec3 lightsourceposition_use = (lightsourceposition - position) * camerarotation;

			// Calculate average normal
			Vec3* vertices_normals = new Vec3[vertices_in.size()];
			for (size_t i = 0; i < vertices_in.size(); i++)
			{
				vertices_normals[i] = { 0.f, 0.f, 0.f };
			}
			for (size_t i = 0; i < indices_in.size() / 3; i++)
			{
				Vec3 faceNormal = ((vertices_in[indices_in[i * 3 + 1]].pos - vertices_in[indices_in[i * 3]].pos) % (vertices_in[indices_in[i * 3 + 2]].pos - vertices_in[indices_in[i * 3]].pos)).GetNormalized();
				vertices_normals[indices_in[i * 3]] += faceNormal;
				vertices_normals[indices_in[i * 3 + 1]] += faceNormal;
				vertices_normals[indices_in[i * 3 + 2]] += faceNormal;
			}

			// Create the VertexWithPhong indexed triangle
			std::vector<VertexWithPhong> vertices_out;
			vertices_out.reserve(vertices_in.size());

			for (size_t i = 0; i < vertices_in.size(); i++)
			{
				VertexWithPhong toPushBack(vertices_in[i].pos, lightsourceposition_use - vertices_in[i].pos, -vertices_in[i].pos, vertices_normals[i].GetNormalized());
				vertices_out.push_back(toPushBack);
			}

			IndexedTriangleList<Output> out(vertices_out, indices_in);
			return out;
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
		void BindShader(const std::vector<Vec2>& tc_in, const std::vector<size_t>& uvMapping_in)
		{
			tc = tc_in;
			uvMapping = uvMapping_in;
		}

	public:
		typedef VertexWithPhongAndTC Output;

	public:
		template<class Vertex>
		Triangle<Output> operator()(const Vertex& in0, const Vertex& in1, const Vertex& in2, size_t triangle_index)
		{
			VertexWithPhongAndTC out0(in0.pos, in0.tolightsrc, in0.tocamera, in0.normal, tc[uvMapping[triangle_index * 3]]);
			VertexWithPhongAndTC out1(in1.pos, in1.tolightsrc, in1.tocamera, in1.normal, tc[uvMapping[triangle_index * 3 + 1]]);
			VertexWithPhongAndTC out2(in2.pos, in2.tolightsrc, in2.tocamera, in2.normal, tc[uvMapping[triangle_index * 3 + 2]]);
			return{ out0, out1, out2 };
		}

	private:
		std::vector<Vec2> tc;
		std::vector<size_t> uvMapping;
	};


	// invoked for each pixel of a triangle
	// takes an input of attributes that are the
	// result of interpolating vertex attributes
	// and outputs a color
	class PixelShader
	{
	public:
		void BindLightSourcePosition(const Vec3& lightsourceposition_in)
		{
			lightsourceposition = lightsourceposition_in;
		}
		void BindLightSourceColor(const Color& lightsourcecolor_in)
		{
			lightsourcecolor = lightsourcecolor_in;
		}
		void BindLightSourceDesnity(const float& lightsourcedensity_in)
		{
			lightsourcedensity = lightsourcedensity_in;
		}
		void BindAmbientLight(const float& ambientlight_in)
		{
			ambientlight = ambientlight_in;
		}
		void BindShininess(const int& shininess_in)
		{
			shininess = shininess_in + 2;
		}
		void BindSpecularWeight(const float& specularweight_in)
		{
			specularweight = specularweight_in;
		}

		void BindTexture(const std::wstring& filename)
		{
			pTex = std::make_unique<Surface>(Surface::FromFile(filename));
			tex_width = float(pTex->GetWidth());
			tex_height = float(pTex->GetHeight());
			tex_xclamp = (pTex->GetWidth() - 1);
			tex_yclamp = (pTex->GetHeight() - 1);
		}

		template<class Input>
		Color operator()(const Input& in, StencilBufferPtr& stencil) const
		{
			Color colorTex = pTex->GetPixel(
				std::min((unsigned int)(in.t.x * tex_width + 0.5f), tex_xclamp),
				std::min((unsigned int)(in.t.y * tex_height + 0.5f), tex_yclamp)
			);

			float light(ambientlight);

			if (stencil.get())
			{
				Vec3 normal(in.normal.GetNormalized());
				Vec3 tolightNormal(in.tolightsrc.GetNormalized());
				Vec3 tocameraNormal(in.tocamera.GetNormalized());
				
				float Idiff = std::max(normal * tolightNormal, 0.f);
				float Ispec = powToPowOf2((tolightNormal + tocameraNormal).GetNormalized() * normal, shininess);
				
				float d = 1.f / (in.tolightsrc.LenSq());

				light += (Idiff + specularweight * Ispec) * d * lightsourcedensity;
			}

			light = std::min(light, 1.f);

			return colorTex * (unsigned __int8)(255 * light);

		}
	private:
		std::unique_ptr<Surface> pTex;
		float tex_width;
		float tex_height;
		unsigned int tex_xclamp;
		unsigned int tex_yclamp;

		Vec3 lightsourceposition;
		Color lightsourcecolor;
		float lightsourcedensity;

		int shininess;
		float specularweight;
		float ambientlight;
	};
public:
	VertexShader vs;
	GeometryShader gs;
	PixelShader ps;
};
