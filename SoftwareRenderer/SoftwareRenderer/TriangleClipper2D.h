#pragma once

#ifndef TRIANGLE_CLIPPER_H
#define TRIANGLE_CLIPPER_H

//****************************************************************************
//**
//**    TriangleClipper2D.h
//**
//**    Copyright (c) 2010 Matthew Robbins
//**
//**    Author:  Matthew Robbins
//**    Created: 07/2010
//**
//****************************************************************************

#include "Vertex.h"

namespace SWR
{
	struct Line
	{
		float x1, x2, y1, y2;
	};

	struct Edge
	{
		Vertex start;
		Vertex end;
	};
	
	// ------------------------------------------------------------------------
	//								ViewSpaceLocation
	// ------------------------------------------------------------------------
	// Desc:
	// The location a line lies within the view-space rectangle.
	// "VSL_In" denotes the line is completly within the view-space rectangle, 
	// and so requires no clipping.
	// "VSL_Intersect_Single" denotes the line is intersecting with a single 
	// side of the view-space rectangle, and therefore requires to be clipped.
	// "VSL_Intersect_Double" denotes the line is intersecting with 2 sides of
	// the view-space rectangle, and therefore requires to be clipped.
	// "VSL_Out" denotes the line is completly outside of the view-space
	// rectangle and can therefore be discarded.
	// ------------------------------------------------------------------------
	enum ViewSpaceLocation
	{
		VSL_In,
		VSL_Intersect_Single,
		VSL_Intersect_Double,
		VSL_Out,
	};

	// ------------------------------------------------------------------------
	//								TriangleClipper2D
	// ------------------------------------------------------------------------
	// Desc:
	// This class encapsulates all the methods needed to clip a triangle into 
	// subsections in screen space for rendering.
	// Uses a hybrid of the Weiler-Atherton and Cohen-Sutherlan clipping
	// algorithms build the new points for the resultant triangle(s).
	// ------------------------------------------------------------------------
	class TriangleClipper2D
	{
	private:
		enum ViewEdges { Left, Right, Bottom, Top, };

		float m_screenWidth;
		float m_screenHeight;
		float m_nearPlane;
		float m_farPlane;

		Line* m_screenEdges;

		void BuildScreenEdges();

		// Performs a line-to-line intersection test. Returns the scale the intersection
		// occured at.
		bool Intersects(Line* triEdge, Line* screenEdge, float* distance);

		// Returns the region code for where the point lies.
		int ClasifyPoint(float x, float y);

		
		int ClipEdge(Edge* target);

		// Builds a new vertex based on the 
		Vertex BuildNewVertex(Edge* target, float scale);

		// Maps the region to the corresponding edge index in the screen-space edge array.
		// Trivial implementation, will fail if the region is a hybrid. If fails, returns -1.
		int RegionToEdgeIndex(int region);

		// Finds the best match for a hybrid index using the lines slope.
		int FindBestEdgeIndex(int region, float slope);

	protected:
	public:

		TriangleClipper2D();
		~TriangleClipper2D();

		void SetViewDimensions(float width, float height);
		void SetViewPlanes(float nearPlane, float farPlane);

		// Clips the triangle, returning the amount of resultant triangles.
		// Triangle will always be carried across to result 1, even if no clipping was performed.
		int ClipTriangle(Vertex* tri, Vertex* result);

		// Tests if any Z depth on the triangle is < NearPlane or > FarPlane.
		// If so, the entire triangle will be discarded. This is due to 2D clipping not 
		// being able to perform
		bool ZTestForDiscard(Vertex* tri);
	};
	
}; // End namespace SWR.

#endif // #ifndef TRIANGLE_CLIPPER_H