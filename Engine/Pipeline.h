#pragma once

#include "ChiliWin.h"
#include "Graphics.h"
#include "Triangle.h"
#include "IndexedTriangleList.h"
#include "PubeScreenTransformer.h"
#include "PerspectiveTransformer.h"
#include "Mat3.h"
#include "ExtendedVertex.h"
#include "ZBuffer.h"
#include "ClippingToolkit.h"
#include <algorithm>

#include <sstream>

// triangle drawing pipeline with programable
// pixel shading stage
template<class Effect>
class Pipeline
{
public:
	// vertex type used for geometry and throughout pipeline
	typedef typename Effect::Vertex Vertex;
	typedef typename Effect::VertexShader::Output VSOut;
public:
	Pipeline(Graphics& gfx)
		:
		gfx(gfx),
		zb(gfx.ScreenWidth, gfx.ScreenHeight),
		perspt(-1.155f , 1.155f , -0.65f , 0.65f , -1.0f , -40.0f)

	{}
	void Draw( IndexedTriangleList<Vertex>& triList )
	{
		ProcessVertices( triList.vertices,triList.indices );
	}

	// needed to reset the z-buffer after each frame
	void BeginFrame()
	{
		zb.Clear();
	}
private:
	// vertex processing function
	// transforms vertices using vs and then passes vtx & idx lists to triangle assembler
	void ProcessVertices( const std::vector<Vertex>& vertices,const std::vector<size_t>& indices )
	{
		OutputDebugStringA("New Frame\n");

		// create vertex vector for vs output
		std::vector<VSOut> verticesOut(vertices.size());

		// transform vertices with vs
		std::transform(vertices.begin(), vertices.end(),
			verticesOut.begin(),
			effect.vs);

		// assemble triangles from stream of indices and vertices
		AssembleTriangles( verticesOut,indices );
	}
	// triangle assembly function
	// assembles indexed vertex stream into triangles and passes them to post process
	// culls (does not send) back facing triangles
	void AssembleTriangles(const std::vector<VSOut>& vertices, const std::vector<size_t>& indices)
	{
		// assemble triangles in the stream and process
		for( size_t i = 0,end = indices.size() / 3;
			 i < end; i++ )
		{
			// determine triangle vertices via indexing
			const auto& v0 = vertices[indices[i * 3]];
			const auto& v1 = vertices[indices[i * 3 + 1]];
			const auto& v2 = vertices[indices[i * 3 + 2]];
			// cull backfacing triangles with cross product (%) shenanigans and check if there are at least partially in front of the viewport
			if( (v1.pos - v0.pos) % (v2.pos - v0.pos) * v0.pos <= 0.0f && (v0.pos.z < -1.05f || v1.pos.z < -1.05f || v2.pos.z < -1.05f))
			{
				// process 3 vertices into a triangle
				ProcessTriangle( v0,v1,v2 );
			}
		}
	}
	// triangle processing function
	// takes 3 vertices to generate triangle
	// sends generated triangle to post-processing
	void ProcessTriangle(const VSOut& v0, const VSOut& v1, const VSOut& v2)
	{
		// store v0, v1, v2 at extented vertex that carries 1/z
		ExtVertex<Vertex> ev0 (v0);
		ExtVertex<Vertex> ev1 (v1);
		ExtVertex<Vertex> ev2 (v2);

		// move ev0, ev1, ev2 at the canonical space
		perspt.Transform(ev0);
		perspt.Transform(ev1);
		perspt.Transform(ev2);

		// find where ev0, ev1, ev2 points are now located (inside, on top?..)
		OutCode ev0C = ClippingOutCode(ev0.Vertex.pos);
		OutCode ev1C = ClippingOutCode(ev1.Vertex.pos);
		OutCode ev2C = ClippingOutCode(ev2.Vertex.pos);
		// triangleOutCode useful to find out the space that the triangle is
		// vertexsCommonSpace useful to find out if all the triangle is beyond a surface
		OutCode triangleOutCode = ev0C | ev1C | ev2C;
		OutCode vertexsCommonSpace = ev0C & ev1C & ev2C;

		if (!triangleOutCode) {
			ev0.iZtoVertexZ();
			ev1.iZtoVertexZ();
			ev2.iZtoVertexZ();

			PostProcessTriangleVertices(Triangle<VSOut>{ ev0.Vertex, ev1.Vertex, ev2.Vertex });
		}
		else if (!vertexsCommonSpace) {
			// Sutherland–Hodgman algorithm optimized to avoid checking surfaces that dont cut the triangle

			std::vector<ExtVertex<Vertex>> input;
			std::vector<ExtVertex<Vertex>> output;

			output.emplace_back(ev0);
			output.emplace_back(ev1);
			output.emplace_back(ev2);

			if (triangleOutCode & NEARC) {
				input = output;
				output.clear();
				ExtVertex<Vertex> ePrev = input.back();
				for (const auto& eThis : input)
				{
					if (!(ClippingOutCode(eThis.Vertex.pos) & NEARC))
					{
						if (ClippingOutCode(ePrev.Vertex.pos) & NEARC)
							output.push_back(ComputeIntersectionNEAR<Vertex>(eThis, ePrev));
						output.push_back(eThis);
					}
					else if (!(ClippingOutCode(ePrev.Vertex.pos) & NEARC))
						output.push_back(ComputeIntersectionNEAR<Vertex>(ePrev, eThis));
					ePrev = eThis;
				}
			}
			if (triangleOutCode & FARC) {
				input = output;
				output.clear();
				ExtVertex<Vertex> ePrev = input.back();
				for (const auto& eThis : input)
				{
					if (!(ClippingOutCode(eThis.Vertex.pos) & FARC))
					{
						if (ClippingOutCode(ePrev.Vertex.pos) & FARC)
							output.push_back(ComputeIntersectionFAR<Vertex>(eThis, ePrev));
						output.push_back(eThis);
					}
					else if (!(ClippingOutCode(ePrev.Vertex.pos) & FARC))
						output.push_back(ComputeIntersectionFAR<Vertex>(ePrev, eThis));
					ePrev = eThis;
				}
			}

			if (triangleOutCode & LEFTC) {
				input = output;
				output.clear();
				ExtVertex<Vertex> ePrev = input.back();
				for (const auto& eThis : input)
				{
					if (!(ClippingOutCode(eThis.Vertex.pos) & LEFTC))
					{
						if (ClippingOutCode(ePrev.Vertex.pos) & LEFTC)
							output.push_back(ComputeIntersectionLEFT<Vertex>(eThis, ePrev));
						output.push_back(eThis);
					}
					else if (!(ClippingOutCode(ePrev.Vertex.pos) & LEFTC))
						output.push_back(ComputeIntersectionLEFT<Vertex>(ePrev, eThis));
					ePrev = eThis;
				}
			}
			if (triangleOutCode & RIGHTC) {
				input = output;
				output.clear();
				ExtVertex<Vertex> ePrev = input.back();
				for (const auto& eThis : input)
				{
					if (!(ClippingOutCode(eThis.Vertex.pos) & RIGHTC))
					{
						if (ClippingOutCode(ePrev.Vertex.pos) & RIGHTC)
							output.push_back(ComputeIntersectionRIGHT<Vertex>(eThis, ePrev));
						output.push_back(eThis);
					}
					else if (!(ClippingOutCode(ePrev.Vertex.pos) & RIGHTC))
						output.push_back(ComputeIntersectionRIGHT<Vertex>(ePrev, eThis));
					ePrev = eThis;
				}
			}

			if (triangleOutCode & BOTTOMC) {
				input = output;
				output.clear();
				ExtVertex<Vertex> ePrev = input.back();
				for (const auto& eThis : input)
				{
					if (!(ClippingOutCode(eThis.Vertex.pos) & BOTTOMC))
					{
						if (ClippingOutCode(ePrev.Vertex.pos) & BOTTOMC)
							output.push_back(ComputeIntersectionBOTTOM<Vertex>(eThis, ePrev));
						output.push_back(eThis);
					}
					else if (!(ClippingOutCode(ePrev.Vertex.pos) & BOTTOMC))
						output.push_back(ComputeIntersectionBOTTOM<Vertex>(ePrev, eThis));
					ePrev = eThis;
				}
			}
			if (triangleOutCode & TOPC) {
				input = output;
				output.clear();
				ExtVertex<Vertex> ePrev = input.back();
				for (const auto& eThis : input)
				{
					if (!(ClippingOutCode(eThis.Vertex.pos) & TOPC))
					{
						if (ClippingOutCode(ePrev.Vertex.pos) & TOPC)
							output.push_back(ComputeIntersectionTOP<Vertex>(eThis, ePrev));
						output.push_back(eThis);
					}
					else if (!(ClippingOutCode(ePrev.Vertex.pos) & TOPC))
						output.push_back(ComputeIntersectionTOP<Vertex>(ePrev, eThis));
					ePrev = eThis;
				}
			}

			OutputDebugStringA("New cutted triangle\n");

			for (int i = 0, end = (int)(output.size() - 2); i < end; i++)
			{
				std::stringstream ss;

				ss	<<	"Point 0  : X:" << output[0].Vertex.pos.x << " Y:" << output[0].Vertex.pos.y << " Z:" << output[0].Vertex.pos.z << std::endl 
					<<	"Point i+1: X:" << output[i + 1].Vertex.pos.x << " Y:" << output[i + 1].Vertex.pos.y << " Z:" << output[i + 1].Vertex.pos.z << std::endl
					<<	"Point i+2: X:" << output[i + 2].Vertex.pos.x << " Y:" << output[i + 2].Vertex.pos.y << " Z:" << output[i + 2].Vertex.pos.z << std::endl << " -- " << std::endl;

				OutputDebugStringA(ss.str().c_str());
			}
			// at this point at output vector there is a list of the triangles that the main triangle broke down


			// prepare Vertexs of ExtVertexs to be send to the next function
			for (auto& eThis : output)
				eThis.iZtoVertexZ();

			// send all the triangles that created to render
			for (int i = 0, end = (int)(output.size() - 2); i < end; i++)
				PostProcessTriangleVertices(Triangle<VSOut>{ output[0].Vertex, output[i + 1].Vertex, output[i + 2].Vertex });

		}
	}
	// vertex post-processing function
	// perform perspective and viewport transformations
	void PostProcessTriangleVertices( Triangle<Vertex>& triangle )
	{

		// perspective divide and screen transform for all 3 vertices
		pst.Transform( triangle.v0 );
		pst.Transform( triangle.v1 );
		pst.Transform( triangle.v2 );

		// draw the triangle
		DrawTriangle( triangle );
	}
	// === triangle rasterization functions ===
	//   it0, it1, etc. stand for interpolants
	//   (values which are interpolated across a triangle in screen space)
	//
	// entry point for tri rasterization
	// sorts vertices, determines case, splits to flat tris, dispatches to flat tri funcs
	void DrawTriangle( const Triangle<VSOut>& triangle)
	{
		// using pointers so we can swap (for sorting purposes)
		const VSOut* pv0 = &triangle.v0;
		const VSOut* pv1 = &triangle.v1;
		const VSOut* pv2 = &triangle.v2;

		// sorting vertices by y
		if( pv1->pos.y < pv0->pos.y ) std::swap( pv0,pv1 );
		if( pv2->pos.y < pv1->pos.y ) std::swap( pv1,pv2 );
		if( pv1->pos.y < pv0->pos.y ) std::swap( pv0,pv1 );

		if( pv0->pos.y == pv1->pos.y ) // natural flat top
		{
			// sorting top vertices by x
			if( pv1->pos.x < pv0->pos.x ) std::swap( pv0,pv1 );

			DrawFlatTopTriangle( *pv0,*pv1,*pv2 );
		}
		else if( pv1->pos.y == pv2->pos.y ) // natural flat bottom
		{
			// sorting bottom vertices by x
			if( pv2->pos.x < pv1->pos.x ) std::swap( pv1,pv2 );

			DrawFlatBottomTriangle( *pv0,*pv1,*pv2 );
		}
		else // general triangle
		{
			// find splitting vertex interpolant
			const float alphaSplit =
				(pv1->pos.y - pv0->pos.y) /
				(pv2->pos.y - pv0->pos.y);
			const auto vi = interpolate( *pv0,*pv2,alphaSplit );

			if( pv1->pos.x < vi.pos.x ) // major right
			{
				DrawFlatBottomTriangle( *pv0,*pv1,vi );
				DrawFlatTopTriangle( *pv1,vi,*pv2 );
			}
			else // major left
			{
				DrawFlatBottomTriangle( *pv0,vi,*pv1 );
				DrawFlatTopTriangle( vi,*pv1,*pv2 );
			}
		}
	}
	// does flat *TOP* tri-specific calculations and calls DrawFlatTriangle
	void DrawFlatTopTriangle( const VSOut& it0,
							  const VSOut& it1,
							  const VSOut& it2 )
	{
		// calulcate dVertex / dy
		// change in interpolant for every 1 change in y
		const float delta_y = it2.pos.y - it0.pos.y;
		const auto dit0 = (it2 - it0) / delta_y;
		const auto dit1 = (it2 - it1) / delta_y;

		// create right edge interpolant
		auto itEdge1 = it1;

		// call the flat triangle render routine
		DrawFlatTriangle( it0,it1,it2,dit0,dit1,itEdge1 );
	}
	// does flat *BOTTOM* tri-specific calculations and calls DrawFlatTriangle
	void DrawFlatBottomTriangle( const VSOut& it0,
								 const VSOut& it1,
								 const VSOut& it2 )
	{
		// calulcate dVertex / dy
		// change in interpolant for every 1 change in y
		const float delta_y = it2.pos.y - it0.pos.y;
		const auto dit0 = (it1 - it0) / delta_y;
		const auto dit1 = (it2 - it0) / delta_y;

		// create right edge interpolant
		auto itEdge1 = it0;

		// call the flat triangle render routine
		DrawFlatTriangle( it0,it1,it2,dit0,dit1,itEdge1 );
	}
	// does processing common to both flat top and flat bottom tris
	// scan over triangle in screen space, interpolate attributes,
	// depth cull, invoke ps and write pixel to screen
	void DrawFlatTriangle( const VSOut& it0,
						   const VSOut& it1,
						   const VSOut& it2,
						   const VSOut& dv0,
						   const VSOut& dv1,
						   VSOut itEdge1 )
	{
		// create edge interpolant for left edge (always v0)
		auto itEdge0 = it0;

		// calculate start and end scanlines
		const int yStart = (int)ceil( it0.pos.y - 0.5f );
		const int yEnd = (int)ceil( it2.pos.y - 0.5f ); // the scanline AFTER the last line drawn

		// do interpolant prestep
		itEdge0 += dv0 * (float( yStart ) + 0.5f - it0.pos.y);
		itEdge1 += dv1 * (float( yStart ) + 0.5f - it0.pos.y);

		for( int y = yStart; y < yEnd; y++,itEdge0 += dv0,itEdge1 += dv1 )
		{
			// calculate start and end pixels
			const int xStart = (int)ceil( itEdge0.pos.x - 0.5f );
			const int xEnd = (int)ceil( itEdge1.pos.x - 0.5f ); // the pixel AFTER the last pixel drawn

			// create scanline interpolant startpoint
			// (some waste for interpolating x,y,z, but makes life easier not having
			//  to split them off, and z will be needed in the future anyways...)
			auto iLine = itEdge0;

			// calculate delta scanline interpolant / dx
			const float dx = itEdge1.pos.x - itEdge0.pos.x;
			const auto diLine = (itEdge1 - iLine) / dx;

			// prestep scanline interpolant
			iLine += diLine * (float( xStart ) + 0.5f - itEdge0.pos.x);

			for( int x = xStart; x < xEnd; x++,iLine += diLine )
			{
				// recover interpolated z from interpolated 1/z
				const float z = 1.0f / iLine.pos.z;
				// do z rejection / update of z buffer
				// skip shading step if z rejected (early z)
				if( zb.TestAndSet( x,y,z ) )
				{
					// recover interpolated attributes
					// (wasted effort in multiplying pos (x,y,z) here, but
					//  not a huge deal, not worth the code complication to fix)
					const auto attr = iLine * z;
					// invoke pixel shader with interpolated vertex attributes
					// and use result to set the pixel color on the screen
					if (attr.t.x >= 0.0f && attr.t.x <= 1.0f && attr.t.y >= 0.0f && attr.t.y <= 1.0f)
						gfx.PutPixel(x, y, effect.ps(attr));
				}
			}
		}
	}
public:
	Effect effect;
private:
	Graphics& gfx;
	PubeScreenTransformer pst;
	PerspectiveTransformer perspt;
	ZBuffer zb;
	Mat3 rotation;
	Vec3 translation;
	Mat3 orientation = Mat3::Identity();
	Vec3 position;
};