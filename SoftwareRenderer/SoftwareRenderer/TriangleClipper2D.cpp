//****************************************************************************
//**
//**    TriangleClipper2D.cpp
//**
//**    Copyright (c) 2010 Matthew Robbins
//**
//**    Author:  Matthew Robbins
//**    Created: 07/2010
//**
//****************************************************************************

#include <assert.h>

#include "TriangleClipper2D.h"

#include "SWR_Math.h"
#include "Vector2.h"

#include "Colour.h"

#include "Logger.h"
#include "MemoryLeak.h"

// -------------------------------------
// View space clipping region codes.

// 0-0-0-0
#define INNER_VIEW_REGION        0x00

// 0-0-0-1
#define LEFT_VIEW_REGION         0x01

// 0-0-1-0
#define RIGHT_VIEW_REGION        0x02

// 0-1-0-0
#define BOTTOM_VIEW_REGION       0x04

// 1-0-0-0
#define TOP_VIEW_REGION          0x08

// 1-0-0-1
#define TOP_LEFT_VIEW_REGION     0x09

// 1-0-1-0
#define TOP_RIGHT_VIEW_REGION    0x0A

// 0-1-0-1
#define BOTTOM_LEFT_VIEW_REGION  0x05

// 0-1-1-0
#define BOTTOM_RIGHT_VIEW_REGION 0x06
// -------------------------------------

namespace SWR
{
	TriangleClipper2D::TriangleClipper2D()
		: m_screenWidth(0.0f)
		, m_screenHeight(0.0f)
		, m_nearPlane(0.0f)
		, m_farPlane(0.0f)
	{
		// Create the screen edges.
		m_screenEdges = new Line[4];
	}

	TriangleClipper2D::~TriangleClipper2D()
	{
		if (m_screenEdges != NULL)
		{
			delete [] m_screenEdges;
			m_screenEdges = NULL;
		}
	}

	int TriangleClipper2D::RegionToEdgeIndex(int region)
	{
		if (region == LEFT_VIEW_REGION)
			return Left;

		if (region == RIGHT_VIEW_REGION)
			return Right;

		if (region == TOP_VIEW_REGION)
			return Top;

		if (region == BOTTOM_VIEW_REGION)
			return Bottom;

		return -1;
	}

	
	int TriangleClipper2D::FindBestEdgeIndex(int region, float slope)
	{
		// Compare the regions.
		if (region == TOP_LEFT_VIEW_REGION)
		{
			if (slope > 1.0f)
				return Left;
			else
				return Top;
		}

		if (region == TOP_RIGHT_VIEW_REGION)
		{
			if (slope > 1.0f)
				return Right;
			else
				return Top;
		}
		
		if (region == BOTTOM_LEFT_VIEW_REGION)
		{
			if (slope > 1.0f)
				return Left;
			else
				return Bottom;
		}

		if (region == BOTTOM_RIGHT_VIEW_REGION)
		{
			if (slope > 1.0f)
				return Right;
			else
				return Bottom;
		}

		// This shouldn't happen. Something has gone wrong if you hit here.
		assert(1 == 0);
		return 0;
	}

	void TriangleClipper2D::BuildScreenEdges()
	{
		// Build left view edge.
		m_screenEdges[Left].x1 = 0.0f;
		m_screenEdges[Left].x2 = 0.0f;
		m_screenEdges[Left].y1 = 0.0f;
		m_screenEdges[Left].y2 = m_screenHeight;

		// Build right view edge.
		m_screenEdges[Right].x1 = m_screenWidth;
		m_screenEdges[Right].x2 = m_screenWidth;
		m_screenEdges[Right].y1 = 0.0f;
		m_screenEdges[Right].y2 = m_screenHeight;

		// Build top view edge.
		m_screenEdges[Top].x1 = 0.0f;
		m_screenEdges[Top].x2 = m_screenWidth;
		m_screenEdges[Top].y1 = 0.0f;
		m_screenEdges[Top].y2 = 0.0f;

		// Build bottom view edge.
		m_screenEdges[Bottom].x1 = 0.0f;
		m_screenEdges[Bottom].x2 = m_screenWidth;
		m_screenEdges[Bottom].y1 = m_screenHeight - 1;
		m_screenEdges[Bottom].y2 = m_screenHeight - 1;

	}

	
	bool TriangleClipper2D::Intersects(Line* triEdge, Line* screenEdge, float* distance)
	{
		// Build the points and direction vectors
		Vector2 P1 = Vector2(triEdge->x1, triEdge->y1);
		Vector2 D1 = Vector2(triEdge->x2 - triEdge->x1, triEdge->y2 - triEdge->y1);

		Vector2 P2 = Vector2(screenEdge->x1, screenEdge->y1);
		Vector2 D2 = Vector2(screenEdge->x2 - screenEdge->x1, screenEdge->y2 - screenEdge->y1);

		Vector2 perpD1 = Vector2::PERP(D1);
		Vector2 perpD2 = Vector2::PERP(D2);

		if (Vector2::DOT(perpD2, D1) != 0.0f)
		{
			Vector2 P2MinusP1 = P2 - P1;

			float s = Vector2::DOT(perpD2, P2MinusP1) / Vector2::DOT(perpD2, D1);
			float t = Vector2::DOT(perpD1, P2MinusP1) / Vector2::DOT(perpD2, D1);

			bool intersects = s >= 0.0f && 0.0f <= t && t <= 1.0f;

			if (intersects)
			{
				*distance = s;
				return true;
			}
		}

		return false;
	}

	Vertex TriangleClipper2D::BuildNewVertex(Edge* target, float scale)
	{
		// REbuild the vertices from the scale factor.
		Colour128 colStart;
		Colour128 colEnd;
		Colour128 colResult;
		Vertex result;

		// Interpolate the postions.
		result.x = LERP<float>(target->start.x, target->end.x, scale);
		result.y = LERP<float>(target->start.y, target->end.y, scale);
		result.z = LERP<float>(target->start.z, target->end.z, scale);

		// Interpolate the texutre UV co-ordinates.
		result.u = LERP<float>(target->start.u, target->end.u, scale);
		result.v = LERP<float>(target->start.v, target->end.v, scale);

		// Interpolate the colour channels.
		colStart.FromColour32(target->start.colour);
		colEnd.FromColour32(target->end.colour);
		colResult = LERP<Colour128>(colStart, colEnd, scale);
		result.colour.FromUINT32(colResult.ToUINT32());

		return result;
	}

	int TriangleClipper2D::ClasifyPoint(float x, float y)
	{
		int result = INNER_VIEW_REGION;

		if (x < 0.0f)
			result |= LEFT_VIEW_REGION;

		if (x >= m_screenWidth)
			result |= RIGHT_VIEW_REGION;

		if (y < 0.0f)
			result |= TOP_VIEW_REGION;

		if (y >= m_screenHeight)
			result |= BOTTOM_VIEW_REGION;

		return result;
	}


	void TriangleClipper2D::SetViewDimensions(float width, float height)
	{
		m_screenWidth = width;
		m_screenHeight = height;

		// Rebuild the screen dimensions for the line tests.
		BuildScreenEdges();
	}

	void TriangleClipper2D::SetViewPlanes(float nearPlane, float farPlane)
	{
		m_nearPlane = nearPlane;
		m_farPlane = farPlane;
	}
		
	int TriangleClipper2D::ClipTriangle(Vertex* tri, Vertex* result)
	{
		Edge triEdges[3];

		// Build the triangle edges.
		triEdges[0].start = tri[0];
		triEdges[0].end = tri[1];


		triEdges[1].start = tri[1];
		triEdges[1].end = tri[2];

		triEdges[2].start = tri[2];
		triEdges[2].end = tri[0];

		// The resultant vertices from the clipping. There can only ever be a maximum of 7 for a
		// number of reasons that would take a while to explain.
		Vertex outputVerts[7];
		int totalOutputVerts = 0; // The amount of vertices that have been added to the output.

		int startRegion = INNER_VIEW_REGION;
		int endRegion = INNER_VIEW_REGION;
		float lineSlope = 0.0f;
		float scale = 0.0f;

		int edgeIndex = -1;

		Line line;

		// Clip the edges.
		for (int i = 0; i < 3; i++)
		{
			// Build the new line.
			line.x1 = triEdges[i].start.x;
			line.x2 = triEdges[i].end.x;
			line.y1 = triEdges[i].start.y;
			line.y2 = triEdges[i].end.y;

			lineSlope = fabs((line.x2 - line.x1) / fabs(line.y1 - line.y2));

			// Classify the start and end points of the 
			startRegion = ClasifyPoint(line.x1, line.y1);
			endRegion = ClasifyPoint(line.x2, line.y2);

			// Try for a trivial accept.
			if ((startRegion | endRegion) == 0)
			{
				// Add the start vertex.
				outputVerts[totalOutputVerts] = triEdges[i].start;
				totalOutputVerts++;
			}
			// Try for a trivial reject.
			else if ((startRegion & endRegion) > 0)
			{
				// Both points are not within the view-space.
				continue;
			}
			else
			{
				// Try add the start vertex to the output.
				if (startRegion == INNER_VIEW_REGION)
				{
					outputVerts[totalOutputVerts] = triEdges[i].start;
					totalOutputVerts++;
				}
				else
				{
					
					edgeIndex = RegionToEdgeIndex(startRegion);

					// Validate the index, it may be within a corner region.
					if (edgeIndex < 0)
						edgeIndex = FindBestEdgeIndex(startRegion, lineSlope);

					// Process the edge.
					if (Intersects(&line, &m_screenEdges[edgeIndex], &scale))
					{
						// Build the new vertex and give it to the output vertex.
						outputVerts[totalOutputVerts] = BuildNewVertex(&triEdges[i], scale);
						totalOutputVerts++;
					}
				}
					
				if (endRegion != INNER_VIEW_REGION)
				{
					// -----------------------------------------------------------------------
					// Process the end region.
					// Attempt to clasify the start region to an index.
					edgeIndex = RegionToEdgeIndex(endRegion);

					// Validate the index, it may be within a corner region.
					if (edgeIndex < 0)
						edgeIndex = FindBestEdgeIndex(endRegion, lineSlope);

					// Process the edge.
					if (Intersects(&line, &m_screenEdges[edgeIndex], &scale))
					{
						// Build the new vertex and give it to the output vertex.
						outputVerts[totalOutputVerts] = BuildNewVertex(&triEdges[i], scale);
						totalOutputVerts++;
					}
					// -----------------------------------------------------------------------
				}


			}
		}

		// No points were added to the output, therefore triangle is completely outside of the view-space.
		if (totalOutputVerts < 2)
			return 0;

		// As we will have a max of 5 points, we can easily build up to 3 new tris from them.
		// Build the new vertices.
		for (int i = 0; i < totalOutputVerts - 2; i++)
		{
			// Copy the new triangle.
			result[i * 3] = outputVerts[0];
			result[i * 3 + 1] = outputVerts[i + 1];
			result[i * 3 + 2] = outputVerts[i + 2];
		}

		// This will tell the renderer how many triangles to expect.
		return totalOutputVerts - 2;
	}

	bool TriangleClipper2D::ZTestForDiscard(Vertex* tri)
	{
		float zVal = 0.0f;
		for (int i = 0; i < 3; i++)
		{
			zVal = tri[i].z;

			// Test to see if the z depth is infront of the near plane or behind the far-plane.
			// If so, we discard the entire triangle as we are not doing 3D clipping so it will
			// break the clipper.
			if (zVal <= m_nearPlane || zVal >= m_farPlane)
				return true;
		}

		return false;
	}
	
}; // End namespace SWR.
