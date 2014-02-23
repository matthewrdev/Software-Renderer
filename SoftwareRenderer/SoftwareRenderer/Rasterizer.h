#pragma once

#ifndef RASTERIZER_H
#define RASTERIZER_H

//****************************************************************************
//**
//**    Rasterizer.h
//**
//**    Copyright (c) 2010 Matthew Robbins
//**
//**    Author:  Matthew Robbins
//**    Created: 06/2010
//**
//****************************************************************************

#include "DataTypes.h"

#include "Vertex.h"
#include "Colour.h"

// Forward Declarations
namespace SWR
{
	class BackBuffer;
	class ZDepthBuffer;
	class Colour32;
	class Texture;
};

namespace SWR
{
	struct ScanlineDataCol
	{
		U32 y;					   // The Y position for the scan line in the back buffer.
		Real xStart, xEnd;		   // The start and end x positions for the scan-line.
		Real zStart, zSlope;	   // The start z position and slope for the scan-line.
		Real rStart, rSlope;	   // The red channel start and slope for the scan-line.
		Real gStart, gSlope;	   // The red channel start and slope for the scan-line.
		Real bStart, bSlope;	   // The red channel start and slope for the scan-line.

		ScanlineDataCol(const ScanlineDataCol& data);
		ScanlineDataCol(){}
	};

	struct ScanlineDataTex
	{
		U32 y;					   // The Y position for the scan line in the back buffer.
		Real xStart, xEnd;		   // The start and end x positions for the scan-line.
		Real zStart, zSlope;	   // The start z position and slope for the scan-line.
		Real uStart, uSlope;	   // The start U position and slope for the texture referenced in the scanline.
		Real vStart, vSlope;	   // The start V position and slope for the texture referenced in the scanline.

		ScanlineDataTex(const ScanlineDataTex &data);
		ScanlineDataTex(){}
	};

	// ------------------------------------------------------------------------
	//								TriangleEdgeType
	// ------------------------------------------------------------------------
	// Desc:
	// When calculating how to render the scan lines for a triangle, the 
	// rasterizer will determine what type of triangle it is.
	// The type is figured from when the longest edge in the Y axis lies 
	// relative to the two smaller edges and the topmost vertices.
	// A major triangle means the longest edge lies in to the left of the two
	// smaller edges.
	// A minor triangle means the two smaller edges lie to the left of the 
	// longest edge.
	// ------------------------------------------------------------------------
	enum TriangleEdgeType
	{
		TRIANGLE_Minor,			
		TRIANGLE_Major,
	};

	// ------------------------------------------------------------------------
	//								Rasterizer
	// ------------------------------------------------------------------------
	// Desc:
	// The class used to plot pixels, lines and circles to the back-buffer.
	// It has lots of functions that do similar things however they are used
	// to try avoid if statements and help create linear code.
	// Uses a top-left fill convention by ceiling floating point numbers.
	// ------------------------------------------------------------------------
	class Rasterizer
	{
	private:
		U8* m_targetBackBuffer;
		U32 m_bufferWidth;
		U32 m_bufferHeight;
		ZDepthBuffer* m_targetZBuffer;

		Real m_near, m_far;
		Texture* m_targetTexture;
		bool m_useZTest;

		// THe base scan line buffer. This will always be the size of the largest scanline type * screen height.
		void* m_scanLineBuffer;

		// The cast pointer of m_scanLineBuffer. Used to share with the texture buffer.
		ScanlineDataCol* m_scanLineColBuffer;
		
		// The cast pointer of m_scanLineBuffer. Used to share with the color buffer.
		ScanlineDataTex* m_scanLineTexBuffer;
		
		inline float ColourSlope(float totalPixelsInv, float start, float end)
		{
			return (end - start) * totalPixelsInv;
		}

		// *********************************************************************************
		// Coloured line plotting and scan-line plotting
		// *********************************************************************************

		void ScanLineCol(ScanlineDataCol* scanline);

		// *********************************************************************************
		// Textured line plotting and scan-line plotting
		// *********************************************************************************
		void ScanLineTexAffine(ScanlineDataTex* scanline);
		void ScanLineTexPerspective(ScanlineDataTex* scanline);

		// Helper function to sort the triangle by Y.
		void SortByY(Vertex* target, Vertex* source);

		// Helper functions for building the edge lists for major and minor triangles.
		void GenerateMajorEdgeListCol(Vertex* verts, float invDeltaYTB, float invDeltaYTM, float invDeltaYMB);
		void GenerateMinorEdgeListCol(Vertex* verts, float invDeltaYTB, float invDeltaYTM, float invDeltaYMB);
		void GenerateMajorEdgeListTex(Vertex* verts, float invDeltaYTB, float invDeltaYTM, float invDeltaYMB);
		void GenerateMinorEdgeListTex(Vertex* verts, float invDeltaYTB, float invDeltaYTM, float invDeltaYMB);

		// Batch render functionality.
		void BatchRasterizeEdgeListCol(int edges);
		void BatchRasterizeEdgeListTex(int edges);

		enum VertexLocation
		{
			TOP,
			MIDDLE,
			BOTTOM,
		};

	protected:
	public:
		Rasterizer();
		~Rasterizer();

		void SetTargetBuffers(U8* backBuffer, ZDepthBuffer* zBuffer, U32 width, U32 height);
		void Release();
		void Reset();

		void EnableZTesting(bool enable);
		bool IsZTestingEnabled() const;

		void SetTargetTexture(Texture* texture);

		// Renders a single line.
		void PlotLineCol(U32 x1, U32 x2, U32 y1, U32 y2, S16 z1, S16 z2, const Colour32 &c1, const Colour32 &c2);

		// Renders the edges of the triangle.
		void RasterizeTriEdges(const Vertex& v1, const Vertex& v2, const Vertex& v3);

		// Renders the triangle with only colours and applies gourad shading.
		void RasterizeTriSolid(Vertex* tri);

		// Renders the triangle with texture mapping.
		void RasterizeTriTex(Vertex* tri);

		// Renders the textured triangle through the edge list buffer. This is cleaner, but slower than the
		// straight rasterize function.
		void RasterizeTriTex_EdgeList(Vertex* tri);

		// Renders the triangle with texture mapping and applies lighting through the gourad shading.
		void RasterizeTriTexLight(Vertex* vertices);
	};
	
}; // End namespace SWR.

#endif // #ifndef RASTERIZER_H