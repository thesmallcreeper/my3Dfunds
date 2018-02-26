#pragma once

#include <algorithm>
#include "ppl.h"
#include "concrt.h"

#include "ChiliWin.h"
#include "Graphics.h"
#include "Triangle.h"
#include "IndexedTriangleList.h"
#include "PubeScreenTransformer.h"
#include "PerspectiveTransformer.h"
#include "Mat3.h"
#include "ExtendedVertex.h"
#include "WBuffer.h"
#include "StencilBuffer.h"
#include "ClippingToolkit.h"

// triangle drawing pipeline with programable
// pixel shading stage
template<class Effect>
class Pipeline
{
public:
	// vertex type used for geometry and throughout pipeline
	typedef typename Effect::Vertex Vertex;
	typedef typename Effect::VertexShader::Output VSOut;
	typedef typename Effect::GeometryShader::Output GSOut;
public:
	Pipeline(Graphics& gfx, WBuffer& zb, StencilBuffer& sb)
		:
		gfx(gfx),
		zb(zb),
		sb(sb),
		perspt(-1.155f, 1.155f, -0.65f, 0.65f, -1.0f, -32.0f),
		writeongfx(true),
		turnfacing(false)
	{
		Concurrency::SchedulerPolicy sp(1, Concurrency::MaxConcurrency, 8);
	}

	void Draw( IndexedTriangleList<Vertex>& triList )
	{
		ProcessVertices( triList.vertices,triList.indices );
	}

	// needed to reset the z-buffer after each frame
	void BeginFrame()
	{
		sb.Clear();
		zb.Clear();
	}

	void switchZBufferSet(bool enableSet_in)
	{
		zb.enableSet = enableSet_in;
	}

	void switchZBufferEqualTest( bool enableEqualTest_in)
	{
		zb.enableEqualTest = enableEqualTest_in;
	}

	void switchWriteOnGFX(bool writeongfx_in)
	{
		writeongfx = writeongfx_in;
	}

	void switchTurnFacing(bool turnfacing_in)
	{
		turnfacing = turnfacing_in;
	}

private:
	// vertex processing function
	// transforms vertices using vs and then passes vtx & idx lists to triangle assembler
	void ProcessVertices( const std::vector<Vertex>& vertices, const std::vector<size_t>& indices )
	{
		// transform vertices with VS and assemble triangles from stream of indices and vertices
		AssembleTriangles( effect.vs(vertices, indices) );
	}
	// triangle assembly function
	// assembles indexed vertex stream into triangles and passes them to post process
	// culls (does not send) back facing triangles
	void AssembleTriangles(const IndexedTriangleList<VSOut>& list)
	{
		// assemble triangles in the stream and process
		for( size_t i = 0,end = list.indices.size() / 3;
			 i < end; i++ )
		{
			// determine triangle vertices via indexing
			VSOut v0 = list.vertices[list.indices[i * 3]];
			VSOut v1 = list.vertices[list.indices[i * 3 + 1]];
			VSOut v2 = list.vertices[list.indices[i * 3 + 2]];
			// avoid backfacing culling if it is enabled
			if (turnfacing)
			{
				std::swap(v1, v2);
			}
			// cull backfacing triangles with cross product (%) shenanigans and check if there are at least partially in front of the viewport
			if(((v1.pos - v0.pos) % (v2.pos - v0.pos) * v0.pos <= 0.0f ) && (v0.pos.z <= -1.0f || v1.pos.z <= -1.0f || v2.pos.z <= -1.0f))
			{
				// process 3 vertices into a triangle
				ProcessTriangle( effect.gs(v0, v1, v2, i) );
			}
		}
	}
	// triangle processing function
	// takes 3 vertices to generate triangle
	// sends generated triangle to post-processing
	void ProcessTriangle(const Triangle<GSOut> defaultTriangle)
	{
		// store v0, v1, v2 at extented vertex that carries 1/z
		ExtVertex<GSOut> EXTv0 (defaultTriangle.v0);
		ExtVertex<GSOut> EXTv1 (defaultTriangle.v1);
		ExtVertex<GSOut> EXTv2 (defaultTriangle.v2);

		perspt.TransformMatrix(EXTv0);
		perspt.TransformMatrix(EXTv1);
		perspt.TransformMatrix(EXTv2);

		//Start clipping at near plane
		std::vector<ExtVertex<GSOut>> input;
		input.reserve(4);
		std::vector<ExtVertex<GSOut>> output;
		output.reserve(4);

		input.push_back(EXTv0);
		input.push_back(EXTv1);
		input.push_back(EXTv2);

		//Clip the triangle at the near plane
		//"Unsafe" insane tricks with a pointer on a vector

		ExtVertex<GSOut>* ePrev = &input.back();
		for (auto& eThis : input)
		{
			if (!(eThis.Vertex.pos.z > 1.0f))
			{
				if (ePrev->Vertex.pos.z > 1.0f)
					output.push_back(ComputeIntersectionPLANE<GSOut>(eThis, *ePrev));
				output.push_back(eThis);
			}
			else if (!(ePrev->Vertex.pos.z > 1.0f))
				output.push_back(ComputeIntersectionPLANE<GSOut>(*ePrev, eThis));
			ePrev = &eThis;
		}

		// move output at the device normalized space
		// find where "you should look for points of the vectors"

		OutCode shapeOutCode = 0;
		OutCode pointsCommonSpace = trueAll;
		for (auto& EXTv : output)
		{
			perspt.TransformDivision(EXTv);
			shapeOutCode |= ClippingOutCode(EXTv.Vertex.pos);
			pointsCommonSpace &= ClippingOutCode(EXTv.Vertex.pos);
		}

		if (!pointsCommonSpace)
		{
			if (shapeOutCode | checkAllButNear) 
			{
				// Sutherland–Hodgman algorithm optimized to avoid checking surfaces that dont cut the triangle

				if (shapeOutCode & FARC) {
					input = std::move(output);
					ExtVertex<GSOut>* ePrev = &input.back();
					for (auto& eThis : input)
					{
						if (!(eThis.Vertex.pos.z > 1.0f))
						{
							if (ePrev->Vertex.pos.z > 1.0f)
								output.push_back(ComputeIntersectionFAR<GSOut>(eThis, *ePrev));
							output.push_back(eThis);
						}
						else if (!(ePrev->Vertex.pos.z > 1.0f))
							output.push_back(ComputeIntersectionFAR<GSOut>(*ePrev, eThis));
						ePrev = &eThis;
					}
				}

				if (shapeOutCode & LEFTC) {
					input = std::move(output);
					ExtVertex<GSOut>* ePrev = &input.back();
					for (auto& eThis : input)
					{
						if (!(eThis.Vertex.pos.x < -1.0f))
						{
							if (ePrev->Vertex.pos.x < -1.0f)
								output.push_back(ComputeIntersectionLEFT<GSOut>(eThis, *ePrev));
							output.push_back(eThis);
						}
						else if (!(ePrev->Vertex.pos.x < -1.0f))
							output.push_back(ComputeIntersectionLEFT<GSOut>(*ePrev, eThis));
						ePrev = &eThis;
					}
				}
				if (shapeOutCode & RIGHTC) {
					input = std::move(output);
					ExtVertex<GSOut>* ePrev = &input.back();
					for (auto& eThis : input)
					{
						if (!(eThis.Vertex.pos.x > 1.0f))
						{
							if (ePrev->Vertex.pos.x > 1.0f)
								output.push_back(ComputeIntersectionRIGHT<GSOut>(eThis, *ePrev));
							output.push_back(eThis);
						}
						else if (!(ePrev->Vertex.pos.x > 1.0f))
							output.push_back(ComputeIntersectionRIGHT<GSOut>(*ePrev, eThis));
						ePrev = &eThis;
					}
				}

				if (shapeOutCode & BOTTOMC) {
					input = std::move(output);
					ExtVertex<GSOut>* ePrev = &input.back();
					for (auto& eThis : input)
					{
						if (!(eThis.Vertex.pos.y < -1.0f))
						{
							if (ePrev->Vertex.pos.y < -1.0f)
								output.push_back(ComputeIntersectionBOTTOM<GSOut>(eThis, *ePrev));
							output.push_back(eThis);
						}
						else if (!(ePrev->Vertex.pos.y < -1.0f))
							output.push_back(ComputeIntersectionBOTTOM<GSOut>(*ePrev, eThis));
						ePrev = &eThis;
					}
				}
				if (shapeOutCode & TOPC) {
					input = std::move(output);
					ExtVertex<GSOut>* ePrev = &input.back();
					for (auto& eThis : input)
					{
						if (!(eThis.Vertex.pos.y > 1.0f))
						{
							if (ePrev->Vertex.pos.y > 1.0f)
								output.push_back(ComputeIntersectionTOP<GSOut>(eThis, *ePrev));
							output.push_back(eThis);
						}
						else if (!(ePrev->Vertex.pos.y > 1.0f))
							output.push_back(ComputeIntersectionTOP<GSOut>(*ePrev, eThis));
						ePrev = &eThis;
					}
				}

			}

			// at this point at output vector there is a list of the triangles that the main triangle broke down


			// prepare Vertexs of ExtVertexs to be send to the next function
			for (auto& eThis : output)
				eThis.WtoVertexZ();

			// send all the triangles that created to render
			for (int i = 0, end = static_cast<int> (output.size()) - 2; i < end; i++) 
				PostProcessTriangleVertices(Triangle<GSOut>{ output[0].Vertex, output[i + 1].Vertex, output[i + 2].Vertex });
		}
	}
	// vertex post-processing function
	// perform perspective and viewport transformations
	void PostProcessTriangleVertices( Triangle<GSOut>& triangle )
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
	void DrawTriangle( const Triangle<GSOut>& triangle)
	{
		// using pointers so we can swap (for sorting purposes)
		const GSOut* pv0 = &triangle.v0;
		const GSOut* pv1 = &triangle.v1;
		const GSOut* pv2 = &triangle.v2;

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
	void DrawFlatTopTriangle( const GSOut& it0,
							  const GSOut& it1,
							  const GSOut& it2 )
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
	void DrawFlatBottomTriangle( const GSOut& it0,
								 const GSOut& it1,
								 const GSOut& it2 )
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
	void DrawFlatTriangle( const GSOut& it0,
						   const GSOut& it1,
						   const GSOut& it2,
						   const GSOut& dv0,
						   const GSOut& dv1,
						   GSOut itEdge1 )
	{
		// create edge interpolant for left edge (always v0)
		auto itEdge0 = it0;

		// calculate start and end scanlines
		const int yStart = (int)ceil( it0.pos.y - 0.5f );
		const int yEnd = (int)ceil( it2.pos.y - 0.5f );					// the scanline AFTER the last line drawn

		// do interpolant prestep
		itEdge0 += dv0 * (float( yStart ) + 0.5f - it0.pos.y);
		itEdge1 += dv1 * (float( yStart ) + 0.5f - it0.pos.y);

		Concurrency::parallel_for( 0, yEnd - yStart, [&](int t)
		{
			int y = t + yStart;

			auto itEdgeLoop0 = dv0 * (float)t + itEdge0;
			auto itEdgeLoop1 = dv1 * (float)t + itEdge1;
			// calculate start and end pixels
			const int xStart = (int)ceil( itEdgeLoop0.pos.x - 0.5f );
			const int xEnd = (int)ceil( itEdgeLoop1.pos.x - 0.5f ); // the pixel AFTER the last pixel drawn

			// create scanline interpolant startpoint
			// (some waste for interpolating x,y,z, but makes life easier not having
			//  to split them off, and z will be needed in the future anyways...)
			auto iLine = itEdgeLoop0;

			// calculate delta scanline interpolant / dx
			const float dx = itEdgeLoop1.pos.x - itEdgeLoop0.pos.x;
			const auto diLine = (itEdgeLoop1 - iLine) / dx;

			// prestep scanline interpolant
			iLine += diLine * (float( xStart ) + 0.5f - itEdgeLoop0.pos.x);

			for( int x = xStart; x < xEnd; x++,iLine = diLine + iLine)
			{
				// do w rejection / update of w buffer
				// skip shading step if w rejected (early w)
				if( zb.TestAndSet( x,y, iLine.pos.z) )
				{
					// recover z from 1/w
					const float z = 1.0f / iLine.pos.z;
					// recover interpolated attributes
					// (wasted effort in multiplying pos (x,y,z) here, but
					//  not a huge deal, not worth the code complication to fix)
					const auto attr = iLine * z;
					// invoke pixel shader with interpolated vertex attributes
					// and use result to set the pixel color on the screen
					// send a "smart" reference of stencil buffer
					StencilBufferPtr sbSmartPtr(x, y, sb);
					auto color(effect.ps(attr, sbSmartPtr));
					if( writeongfx == true)
						gfx.PutPixel(x, y, color);
				}
			}
		}, Concurrency::static_partitioner());
	}
public:
	Effect effect;
private:
	Graphics& gfx;
	WBuffer& zb;
	StencilBuffer& sb;
	PubeScreenTransformer pst;
	PerspectiveTransformer perspt;
	Mat3 rotation;
	Vec3 translation;
	Mat3 orientation = Mat3::Identity();
	Vec3 position;

	bool writeongfx;
	bool turnfacing;
};