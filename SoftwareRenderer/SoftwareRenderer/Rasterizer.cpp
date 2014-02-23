//****************************************************************************
//**
//**    Rasterizer.cpp
//**
//**    Copyright (c) 2010 Matthew Robbins
//**
//**    Author:  Matthew Robbins
//**    Created: 06/2010
//**
//****************************************************************************

#include <assert.h>
#include <cmath>

#include "Rasterizer.h"

#include "BackBuffer.h"
#include "ZDepthBuffer.h"
#include "Colour.h"
#include "Texture.h"

#include "SWR_Math.h"
#include "SWRUtil.h"

#include "Logger.h"
#include "MemoryLeak.h"

#define FIXED_INTEGER_SHIFT 12

namespace SWR
{

	Rasterizer::Rasterizer()
	{
		Reset();
	}

	Rasterizer::~Rasterizer()
	{
		// Just unassign the buffers, as the deletion is done in the render-device.
		m_targetBackBuffer = NULL;
		m_targetZBuffer = NULL;

		// Unassign the buffer pointers.
		m_scanLineColBuffer = NULL;
		m_scanLineTexBuffer =NULL;
		
		if (m_scanLineBuffer != NULL)
		{
			delete [] m_scanLineBuffer;
		}

	}

	void Rasterizer::SetTargetBuffers(U8* backBuffer, ZDepthBuffer* zBuffer, U32 width, U32 height)
	{
		m_targetBackBuffer = backBuffer;
		m_targetZBuffer = zBuffer;
		m_bufferHeight = height;
		m_bufferWidth = width;

		// Generate the edge list buffers.
		//int scanLineCol = 
		int size = sizeof(ScanlineDataCol) > sizeof(ScanlineDataTex) ? sizeof(ScanlineDataCol) : sizeof(ScanlineDataTex);
		
		this->m_scanLineBuffer = new char[(height * 2 * size)];
		this->m_scanLineColBuffer = (ScanlineDataCol*)m_scanLineBuffer;
		this->m_scanLineTexBuffer = (ScanlineDataTex*)m_scanLineBuffer;
	}

	void Rasterizer::Reset()
	{
		EnableZTesting(false);

		// Clear the references to the back-buffer and z-buffer.
		m_targetBackBuffer = NULL;
		m_targetZBuffer = NULL;
		m_targetTexture = NULL;
		m_bufferWidth = 0;
		m_bufferHeight = 0;
	}

	void Rasterizer::EnableZTesting(bool enable)
	{
		m_useZTest = enable;
	}

	void Rasterizer::SetTargetTexture(Texture* texture)
	{
		m_targetTexture = texture;
	}
	
	void Rasterizer::ScanLineCol(ScanlineDataCol* scanline)
	{
		// Scanline end will be < 0 and cause a wrap-around.
		if (scanline->xEnd <= 1.0f - EPSILON)
			return;

		// Apply top-left fill convention.
		U32 xStart = ceil(scanline->xStart);
		U32 xEnd = ceil(scanline->xEnd);// - 1;

		// Load the back buffer at the start point for our render.
		U32* buffer = (U32*)(m_targetBackBuffer + ((xStart + (scanline->y * m_bufferWidth)) << 2));

		static U32 colour;
		static S32 rCol, rSlope, gCol, gSlope, bCol, bSlope;
		rCol = (S32)(scanline->rStart * ( 1 << FIXED_INTEGER_SHIFT));
		rSlope = (S32)(scanline->rSlope * ( 1 << FIXED_INTEGER_SHIFT));
		gCol = (S32)(scanline->gStart * ( 1 << FIXED_INTEGER_SHIFT));
		gSlope = (S32)(scanline->gSlope * ( 1 << FIXED_INTEGER_SHIFT));
		bCol = (S32)(scanline->bStart * ( 1 << FIXED_INTEGER_SHIFT));
		bSlope = (S32)(scanline->bSlope * ( 1 << FIXED_INTEGER_SHIFT));

		// Copy the colour into the scan line.
		for (unsigned int index = xStart; index < xEnd; index++)
		{
			// Increment the buffer and assign the new colour.
			colour = 0 | ((rCol >> FIXED_INTEGER_SHIFT) << RED_BIT_SHIFT) | ((gCol >> FIXED_INTEGER_SHIFT) << GREEN_BIT_SHIFT) | (bCol >> FIXED_INTEGER_SHIFT);
			*buffer++ = colour;
			rCol += rSlope;
			gCol += gSlope;
			bCol += bSlope;
		}
	}
	
	void Rasterizer::ScanLineTexAffine(ScanlineDataTex* scanline)
	{
		// Scanline end will be < 0 and cause a wrap-around.
		if (scanline->xEnd <= 1.0f - EPSILON)
			return;

		static U32 xStart, xEnd, texelIndex;
		static S32 uVal, vVal, uSlope, vSlope, texWidth, texHeight;

		xStart = ceil(scanline->xStart);
		xEnd = ceil(scanline->xEnd);
		
		// Load the back buffer at the start point for our render.
		U32* buffer = (U32*)(m_targetBackBuffer + ((xStart + (scanline->y * m_bufferWidth)) << 2));

		// Load the texels we are going to be referencing into a buffer.
		U32* texels = (U32*)(m_targetTexture->GetBytes());

		// Get the width and height for our texture so we can use this to get the appropriate texel
		// in the texture by scaling our U / V.
		texWidth = m_targetTexture->GetWidth();
		texHeight = m_targetTexture->GetHeight();

		texelIndex = 0;

		uVal = (S32)(scanline->uStart * (1 << FIXED_INTEGER_SHIFT));
		vVal = (S32)(scanline->vStart * (1 << FIXED_INTEGER_SHIFT));
		uSlope = (S32)(scanline->uSlope * (1 << FIXED_INTEGER_SHIFT));
		vSlope = (S32)(scanline->vSlope * (1 << FIXED_INTEGER_SHIFT));

		for (int index = xStart; index < xEnd; index++)
		{
			texelIndex = (uVal >> FIXED_INTEGER_SHIFT) + ((vVal >> FIXED_INTEGER_SHIFT) * texWidth);
			// Calculate our texel index.
			*buffer++ = texels[texelIndex];
			uVal += uSlope;
			vVal += vSlope;
		}
	}

	void Rasterizer::PlotLineCol(U32 x1, U32 x2, U32 y1, U32 y2, S16 z1, S16 z2, const Colour32 &c1, const Colour32 &c2)
	{
		// This is a generic implementation of Bresenhams Line Drawing Algorithm.
		// Based from the example below, but with several modifications to help improve speed.
		// http://www.gamedev.net/reference/articles/article1275.asp

		// A local pointer to the back buffer. Cast it to U32s so that we dont have to use a memcpy.
		U32* backBuffer = (U32*)m_targetBackBuffer;

		U32 colour = 0;

		// The difference between the X and Y positions of our points.
		int deltaX = abs(int(x2 - x1));
		int deltaY = abs(int(y2 - y1));

		// The X and Y position for the pixel we are plotting.
		int xPos = x1;
		int yPos = y1;

		// The increment values for the x and y values of the line.
		int xInc1 = 0;
		int xInc2 = 0;
		int yInc1 = 0;
		int yInc2 = 0;

		// These values provide a way to interpret a lines slope in integer fashion rather than float.
		int den = 0;		  // The denominator.
		int num = 0;		  // The numerator.
		int numInc = 0;		  // The increment value for the numerator.

		int numOfPixels = 0;  // The total pixels we are writing to.
		int currentPixel = 0; // The current pixel to write to.

		// Initilise the increment values so that pixels are plotted to the correct places.
		// The following if statements are used to adjust the plotting for loop by altering the incrementers
		// for the X and Y values of the line.
		// Is designed to accomidate for the 8 cases of a line:
		// Where:
		// ------------------------------------------------------------------------------------
		// Line is going from left to right, up to down, and is changing more rapidly in X axis.
		// 1) x1 <= x2, y1 <= y2, deltax >= deltay
		// ------------------------------------------------------------------------------------
		// Line is going from left to right, up to down, and is changing more rapidly in Y axis.
		// 2) x1 <= x2, y1 <= y2, deltax <  deltay
		// ------------------------------------------------------------------------------------
		// Line is going from left to right, down to up, and is changing more rapidly in X axis.
		// 3) x1 <= x2, y1 >  y2, deltax >= deltay
		// ------------------------------------------------------------------------------------
		// Line is going from left to right, down to up, and is changing more rapidly in Y axis.
		// 4) x1 <= x2, y1 >  y2, deltax <  deltay
		// ------------------------------------------------------------------------------------
		// Line is going from right to left, up to down, and is changing more rapidly in X axis.
		// 5) x1 >  x2, y1 <= y2, deltax >= deltay
		// ------------------------------------------------------------------------------------
		// Line is going from right to left, up to down, and is changing more rapidly in Y axis.
		// 6) x1 >  x2, y1 <= y2, deltax <  deltay
		// ------------------------------------------------------------------------------------
		// Line is going from right to left, down to up, and is changing more rapidly in X axis.
		// 7) x1 >  x2, y1 >  y2, deltax >= deltay
		// ------------------------------------------------------------------------------------
		// Line is going from right to left, down to up, and is changing more rapidly in Y axis.
		// 8) x1 >  x2, y1 >  y2, deltax <  deltay
		// ------------------------------------------------------------------------------------

		// Initilise the x increment values.
		if (x2 >= x1)                 // The x-values are increasing
		{
		  xInc1 = 1;
		  xInc2 = 1;
		}
		else                          // The x-values are decreasing
		{
		  xInc1 = -1;
		  xInc2 = -1;
		}

		// Initilise the y increment values.
		if (y2 >= y1)                 // The y-values are increasing
		{
		  yInc1 = 1;
		  yInc2 = 1;
		}
		else                          // The y-values are decreasing
		{
		  yInc1 = -1;
		  yInc2 = -1;
		}

		// There is more pixels in the X direction than the Y. So increment in the X direction for the line.
		if (deltaX >= deltaY)         // There is at least one x-value for every y-value
		{
		  xInc1 = 0;                 
		  yInc2 = 0;                 
		  den = deltaX;
		  num = deltaX >> 1; // or / 2.
		  numInc = deltaY;				// The incrementer for the numerator is the Y delta.
		  numOfPixels = deltaX;         // The number of pixels is the total delta in the x direction.
		}
		// There is more Y values than X values. So we increment in the Y direction for the line.
		else                          
		{
		  xInc2 = 0;                  // Don't change the x for every iteration
		  yInc1 = 0;                  // Don't change the y when numerator >= denominator
		  den = deltaY;
		  num = deltaY >> 1; // or / 2.
		  numInc = deltaX;
		  numOfPixels = deltaY;         // The number of pixels is the total delta in the y direction
		}
		
		float numOfPixelsInv = 1 / (float)numOfPixels;
		// Calculate the colour slopes.
		float rCol = c1.R;
		float rSlope = ColourSlope(numOfPixelsInv, c1.R, c2.R);
		float gCol = c1.G;
		float gSlope = ColourSlope(numOfPixelsInv, c1.G, c2.G);
		float bCol = c1.B;
		float bSlope = ColourSlope(numOfPixelsInv, c1.B, c2.B);


		// The pixels location within the backbuffer.
		U32 byteIndex = 0;

		// Copy the pixels into the backbuffer. I cannot use the same trick that I use in backbuffer clearing here but 
		// interpreting the backbuffer as an array of U32s seems to work faster than using a memcpy. 
		// This is probably due to not having call a function pointer ...
		for (currentPixel = 0; currentPixel <= numOfPixels; currentPixel++)
		{
			rCol += rSlope;
			gCol += gSlope;
			bCol += bSlope;

			// Get rid of these casts. Find something on fast float conversions.
			colour = 0 | (((U8)rCol) << RED_BIT_SHIFT) | (((U8)gCol) << GREEN_BIT_SHIFT) | (((U8)bCol) << BLUE_BIT_SHIFT);

			byteIndex = (xPos + (yPos * m_bufferWidth));// Calculate the write index for the back buffer.
			backBuffer[byteIndex] = colour;
			num += numInc;              // Increase the numerator by the top of the fraction
					
			if (num < den) 
			{
				xPos += xInc2;                 // Change the x as appropriate
				yPos += yInc2;                 // Change the y as appropriate
			}
			else
			{
				num -= den;               // Calculate the new numerator value
				xPos += xInc1;               // Change the x as appropriate
				yPos += yInc1;               // Change the y as appropriate

				xPos += xInc2;                 // Change the x as appropriate
				yPos += yInc2;                 // Change the y as appropriate
			}
		}
	}


	void Rasterizer::RasterizeTriEdges(const Vertex& v1, const Vertex& v2, const Vertex& v3)
	{
		// Plot the edges from 1-3, 2-3, 1-2.
		PlotLineCol(v1.x, v2.x, v1.y, v2.y, v1.z, v2.z, v1.colour, v2.colour);
		PlotLineCol(v1.x, v3.x, v1.y, v3.y, v1.z, v3.z, v1.colour, v3.colour);
		PlotLineCol(v2.x, v3.x, v2.y, v3.y, v2.z, v3.z, v2.colour, v3.colour);
	}

	void Rasterizer::SortByY(Vertex* target, Vertex* source)
	{
		bool usedIndices[3] = {false, false, false};
		
		Real bestY = this->m_bufferHeight;
		int bestVert = -1;

		// Selection sort the triangle for the lowest Y value (the top, middle and bottom of the triangle).
		for (int i = 0; i < 3; i++)
		{
			for (int j = 0; j < 3; j++)
			{
				if (source[j].y < bestY && usedIndices[j] == false)
				{
					bestY = source[j].y;
					bestVert = j;
				}
			}
			// Copy the data into the vertices buffer.
			target[i] = source[bestVert];
			usedIndices[bestVert] = true;
			bestVert = -1;
			bestY = this->m_bufferHeight;
		}
	}

	// THis function draws a filled gourad shaded triangle.
	// THe code is pretty foul but there really isnt an easy way to make this clean without
	// killing whatever perfomance I have with a software renderer.
	// This is mostly to accomodate for each type of triangle that could occur and I have
	// written code to switch / prepare values for these cases.
	void Rasterizer::RasterizeTriSolid(Vertex* tri)
	{		
		enum VertexLocation
		{
			TOP,
			MIDDLE,
			BOTTOM,
		};
		// The type of triangle that is being drawn.
		// Major means that edge with greatest Y delta is on left, minor means it is on right.
		TriangleEdgeType triType;

		// Sort vertices up to down.
		Vertex verts[3];
		SortByY(verts, tri);

		// Determine if the triangle is major/minor.
		if (verts[BOTTOM].x > verts[MIDDLE].x)
		{
			triType = TRIANGLE_Minor;
		}
		else
		{
			triType = TRIANGLE_Major;
		}

		// Set up Y deltas and inverse multipliers for edge traversal.
		float invDeltaYTB = 1.0f / (verts[BOTTOM].y - verts[TOP].y);
		float invDeltaYTM = 1.0f / (verts[MIDDLE].y - verts[TOP].y);
		float invDeltaYMB = 1.0f / (verts[BOTTOM].y - verts[MIDDLE].y);
	
		// Slope values for screen pixels
		float x1, x2;
		float z1, z2;
		int yStart;
		int yEnd;
		float xSlopeLeft, xSlopeRight;
		float zSlopeLeft, zSlopeRight;
		float spanXInv = 0.0f;

		// Slope values for colour values.
		float r1, r2;
		float g1, g2;
		float b1, b2;
		float rSlopeLeft, gSlopeLeft, bSlopeLeft;
		float rSlopeRight, gSlopeRight, bSlopeRight;

		// To update the interpolants.
		float sub = 0.0f;

		ScanlineDataCol scanline;
		memset(&scanline, 0, sizeof(ScanlineDataCol));
		
		// Decide on how to setup the scanline rendering.
		if (triType == TRIANGLE_Minor)
		{
			// The two smaller triangle edges are to the left so we are basing our scan-line
			// rendering from this.

			// Calculate our values for the scan line interpolation.
			xSlopeLeft = (verts[MIDDLE].x - verts[TOP].x) * invDeltaYTM;
			xSlopeRight = (verts[BOTTOM].x - verts[TOP].x) * invDeltaYTB;
			x1 = verts[TOP].x;
			x2 = verts[TOP].x;
			yStart = ceil(verts[TOP].y);
			yEnd = ceil(verts[MIDDLE].y) - 1;

			// Set up the colour interpolation values.
			r1 = verts[TOP].colour.R;
			r2 = verts[TOP].colour.R;
			g1 = verts[TOP].colour.G;
			g2 = verts[TOP].colour.G;
			b1 = verts[TOP].colour.B;
			b2 = verts[TOP].colour.B;

			rSlopeLeft = (verts[MIDDLE].colour.R - verts[TOP].colour.R) * invDeltaYTM;
			rSlopeRight = (verts[BOTTOM].colour.R - verts[TOP].colour.R) * invDeltaYTB;
			gSlopeLeft = (verts[MIDDLE].colour.G - verts[TOP].colour.G) * invDeltaYTM;
			gSlopeRight = (verts[BOTTOM].colour.G - verts[TOP].colour.G) * invDeltaYTB;
			bSlopeLeft = (verts[MIDDLE].colour.B - verts[TOP].colour.B) * invDeltaYTM;
			bSlopeRight = (verts[BOTTOM].colour.B - verts[TOP].colour.B) * invDeltaYTB;
		
			// The incidental offset that may be generated by using the ceil function.
			// We may get 'graphical artifacts' / visaul errors if we dont correct starting values
			// by this.
			sub = (float)yStart - verts[TOP].y;
			x1 += xSlopeLeft * sub;
			x2 += xSlopeRight * sub;
			r1 += rSlopeLeft * sub;
			r2 += rSlopeRight * sub;
			g1 += gSlopeLeft * sub;
			g2 += gSlopeRight * sub;
			b1 += bSlopeLeft * sub;
			b2 += bSlopeRight * sub;

			// If our right slope is less than our left slope then we are going to get an incomplete triangle.
			// Swap the values to fix this.
			bool hasSwaped = false;
			if (xSlopeRight < xSlopeLeft)
			{
				// Swap x axis interpolation.
				Swap<float>(x2,x1);
				Swap<float>(xSlopeLeft,xSlopeRight);

				// Swap colour interpolation.
				Swap<float>(r1, r2);
				Swap<float>(rSlopeLeft, rSlopeRight);

				Swap<float>(g1, g2);
				Swap<float>(gSlopeLeft, gSlopeRight);

				Swap<float>(b1, b2);
				Swap<float>(bSlopeLeft, bSlopeRight);

				hasSwaped = true;
			}


			// Draw the upper triangle section.
			if (invDeltaYTM > EPSILON)
			{
				// Run down from the top to the middle of the triangle.
				for (int y = yStart; y <= yEnd; y++)
				{
					scanline.y = y;
					scanline.xStart = x1;
					scanline.xEnd = x2;

					spanXInv = 1.0f /  fabs(x1 - x2);

					scanline.rStart = r1;
					scanline.rSlope = (r2 - r1) * spanXInv;
					scanline.gStart = g1;
					scanline.gSlope = (g2 - g1) * spanXInv;
					scanline.bStart = b1;
					scanline.bSlope = (b2 - b1) * spanXInv;

					ScanLineCol(&scanline);

					// Update the slopes.
					x1 += xSlopeLeft;
					x2 += xSlopeRight;
					r1 += rSlopeLeft;
					r2 += rSlopeRight;
					g1 += gSlopeLeft;
					g2 += gSlopeRight;
					b1 += bSlopeLeft;
					b2 += bSlopeRight;
				}
			}
			
			// Swap back if we swapped.
			if (hasSwaped)
			{
				Swap<float>(x2,x1);
				Swap<float>(xSlopeLeft,xSlopeRight);

				// Swap colour interpolation.
				Swap<float>(r1, r2);
				Swap<float>(rSlopeLeft, rSlopeRight);

				Swap<float>(g1, g2);
				Swap<float>(gSlopeLeft, gSlopeRight);

				Swap<float>(b1, b2);
				Swap<float>(bSlopeLeft, bSlopeRight);
			}

			// Setup the new slope values for the lower left edge.
			xSlopeLeft = (verts[BOTTOM].x - verts[MIDDLE].x) * invDeltaYMB;
			x1 = verts[MIDDLE].x;
			yStart = ceil(verts[MIDDLE].y);
			yEnd = ceil(verts[BOTTOM].y) - 1;

			r1 = verts[MIDDLE].colour.R;
			g1 = verts[MIDDLE].colour.G;
			b1 = verts[MIDDLE].colour.B;

			// Calculate the new colour interpolations.
			rSlopeLeft = (verts[BOTTOM].colour.R - verts[MIDDLE].colour.R) * invDeltaYMB;
			gSlopeLeft = (verts[BOTTOM].colour.G - verts[MIDDLE].colour.G) * invDeltaYMB;
			bSlopeLeft = (verts[BOTTOM].colour.B - verts[MIDDLE].colour.B) * invDeltaYMB;
		
			if (x2 < x1)
			{
				Swap<float>(x2, x1);
				Swap<float>(xSlopeLeft, xSlopeRight);

				// Swap colour interpolation.
				Swap<float>(r1, r2);
				Swap<float>(rSlopeLeft, rSlopeRight);

				Swap<float>(g1, g2);
				Swap<float>(gSlopeLeft, gSlopeRight);

				Swap<float>(b1, b2);
				Swap<float>(bSlopeLeft, bSlopeRight);
			}

			// The incidental offset that may be generated by using the ceil function.
			// We may get 'graphical artifacts' / visual errors if we dont correct starting values
			// by this.
			sub = (float)yStart - verts[MIDDLE].y;
			

			if (invDeltaYMB > EPSILON)
			{				
				// Run down from the middle to the bottom of the triangle.
				for (int y = yStart; y <= yEnd; y++)
				{
					scanline.y = y;
					scanline.xStart = x1;
					scanline.xEnd = x2;

					// Calculate the inverse of the X delta.
					spanXInv = 1.0f /  fabs(x1 - x2);

					// Calculate the new colour values for the scan line.
					scanline.rStart = r1;
					scanline.rSlope = (r2 - r1) * spanXInv;
					scanline.gStart = g1;
					scanline.gSlope = (g2 - g1) * spanXInv;
					scanline.bStart = b1;
					scanline.bSlope = (b2 - b1) * spanXInv;

					ScanLineCol(&scanline);

					// Update the slopes.
					x1 += xSlopeLeft;
					x2 += xSlopeRight;
					r1 += rSlopeLeft;
					r2 += rSlopeRight;
					g1 += gSlopeLeft;
					g2 += gSlopeRight;
					b1 += bSlopeLeft;
					b2 += bSlopeRight;
				}
			}
		}
		else if (triType == TRIANGLE_Major)
		{
			// Calculate our values for the scan line interpolation.
			xSlopeLeft = (verts[BOTTOM].x - verts[TOP].x) * invDeltaYTB;
			xSlopeRight = (verts[MIDDLE].x - verts[TOP].x) * invDeltaYTM;
			x1 = verts[TOP].x;
			x2 = verts[TOP].x;
			yStart = ceil(verts[TOP].y);
			yEnd = ceil(verts[MIDDLE].y) - 1;

			r1 = verts[TOP].colour.R;
			r2 = verts[TOP].colour.R;
			g1 = verts[TOP].colour.G;
			g2 = verts[TOP].colour.G;
			b1 = verts[TOP].colour.B;
			b2 = verts[TOP].colour.B;

			rSlopeLeft = (verts[BOTTOM].colour.R - verts[TOP].colour.R) * invDeltaYTB;
			rSlopeRight = (verts[MIDDLE].colour.R - verts[TOP].colour.R) * invDeltaYTM;
			gSlopeLeft = (verts[BOTTOM].colour.G - verts[TOP].colour.G) * invDeltaYTB;
			gSlopeRight = (verts[MIDDLE].colour.G - verts[TOP].colour.G) * invDeltaYTM;
			bSlopeLeft = (verts[BOTTOM].colour.B - verts[TOP].colour.B) * invDeltaYTB;
			bSlopeRight = (verts[MIDDLE].colour.B - verts[TOP].colour.B) * invDeltaYTM;

			bool hasSwaped = false;
			if (xSlopeRight < xSlopeLeft)
			{
				Swap<float>(x2,x1);
				Swap<float>(xSlopeLeft,xSlopeRight);

				// Swap colour interpolation.
				Swap<float>(r1, r2);
				Swap<float>(rSlopeLeft, rSlopeRight);

				Swap<float>(g1, g2);
				Swap<float>(gSlopeLeft, gSlopeRight);

				Swap<float>(b1, b2);
				Swap<float>(bSlopeLeft, bSlopeRight);

				hasSwaped = true;
			}
			
			// The incidental offset that may be generated by using the ceil function.
			// We may get 'graphical artifacts' / visaul errors if we dont correct starting values
			// by this.
			sub = (float)yStart - verts[TOP].y;
			
			// Draw the upper triangle section.
			if (invDeltaYTM > EPSILON)
			{
				for (int y = yStart; y <= yEnd; y++)
				{
					scanline.y = y;
					scanline.xStart = x1;
					scanline.xEnd = x2;

					spanXInv = 1.0f /  fabs(x1 - x2);

					scanline.rStart = r1;
					scanline.rSlope = (r2 - r1) * spanXInv;
					scanline.gStart = g1;
					scanline.gSlope = (g2 - g1) * spanXInv;
					scanline.bStart = b1;
					scanline.bSlope = (b2 - b1) * spanXInv;

					ScanLineCol(&scanline);

					// Update the slopes.
					x1 += xSlopeLeft;
					x2 += xSlopeRight;
					r1 += rSlopeLeft;
					r2 += rSlopeRight;
					g1 += gSlopeLeft;
					g2 += gSlopeRight;
					b1 += bSlopeLeft;
					b2 += bSlopeRight;
				}
			}

			// Swap back if we swapped.
			if (hasSwaped)
			{
				Swap<float>(x2,x1);
				Swap<float>(xSlopeLeft,xSlopeRight);

				// Swap colour interpolation.
				Swap<float>(r1, r2);
				Swap<float>(rSlopeLeft, rSlopeRight);

				Swap<float>(g1, g2);
				Swap<float>(gSlopeLeft, gSlopeRight);

				Swap<float>(b1, b2);
				Swap<float>(bSlopeLeft, bSlopeRight);
			}

			// Setup the new slope values for the lower left edge.
			xSlopeRight = (verts[BOTTOM].x - verts[MIDDLE].x) * invDeltaYMB;
			x2 = verts[MIDDLE].x;
			yStart = ceil(verts[MIDDLE].y);
			yEnd = ceil(verts[BOTTOM].y) - 1;
			
			r2 = verts[MIDDLE].colour.R;
			g2 = verts[MIDDLE].colour.G;
			b2 = verts[MIDDLE].colour.B;

			// Calculate the new colour interpolations.
			rSlopeRight = (verts[BOTTOM].colour.R - verts[MIDDLE].colour.R) * invDeltaYMB;
			gSlopeRight = (verts[BOTTOM].colour.G - verts[MIDDLE].colour.G) * invDeltaYMB;
			bSlopeRight = (verts[BOTTOM].colour.B - verts[MIDDLE].colour.B) * invDeltaYMB;

			
			if (x2 < x1)
			{
				Swap<float>(x2, x1);
				Swap<float>(xSlopeLeft, xSlopeRight);

				// Swap colour interpolation.
				Swap<float>(r1, r2);
				Swap<float>(rSlopeLeft, rSlopeRight);

				Swap<float>(g1, g2);
				Swap<float>(gSlopeLeft, gSlopeRight);

				Swap<float>(b1, b2);
				Swap<float>(bSlopeLeft, bSlopeRight);
			}
			
			// The incidental offset that may be generated by using the ceil function.
			// We may get 'graphical artifacts' / visaul errors if we dont correct starting values
			// by this.
			sub = (float)yStart - verts[MIDDLE].y;

			if (invDeltaYMB > EPSILON)
			{				
				// Run down from the middle to the bottom of the triangle.
				for (int y = yStart; y <= yEnd; y++)
				{
					scanline.y = y;
					scanline.xStart = x1;
					scanline.xEnd = x2;

					spanXInv = 1.0f /  fabs(x1 - x2);

					scanline.rStart = r1;
					scanline.rSlope = (r2 - r1) * spanXInv;
					scanline.gStart = g1;
					scanline.gSlope = (g2 - g1) * spanXInv;
					scanline.bStart = b1;
					scanline.bSlope = (b2 - b1) * spanXInv;

					ScanLineCol(&scanline);

					// Update the slopes.
					x1 += xSlopeLeft;
					x2 += xSlopeRight;
					r1 += rSlopeLeft;
					r2 += rSlopeRight;
					g1 += gSlopeLeft;
					g2 += gSlopeRight;
					b1 += bSlopeLeft;
					b2 += bSlopeRight;
				}
			}
		}
		else
		{
			LOG("Triangle failed to be classified as a valid render type", LOG_Error);
			assert(false);
		}
	}

	// The texture based triangle rasterization is the same as the Gourad shaded filled triangle procedure.
	// We use the same steps and procedures of deducing the type of triangle (Major or minor) however here
	// we are interpolating UV co-ordinates based on the slopes between each vertex.
	// Each UV co-ordinate is also scaled up by the source texture width and height to avoid getting visaul artifacts.
	void Rasterizer::RasterizeTriTex(Vertex* tri)
	{
		enum VertexLocation
		{
			TOP,
			MIDDLE,
			BOTTOM,
		};
		// The type of triangle that is being drawn.
		// Major means that edge with greatest Y delta is on left, minor means it is on right.
		TriangleEdgeType triType;

		// Sort vertices up to down.
		Vertex verts[3];
		
		SortByY(verts, tri);

		// Determine if the triangle is major/minor.
		if (verts[BOTTOM].x > verts[MIDDLE].x)
		{
			triType = TRIANGLE_Minor;
		}
		else
		{
			triType = TRIANGLE_Major;
		}

		// Set up Y deltas and inverse multipliers for edge traversal.
		float invDeltaYTB = 1.0f / (verts[BOTTOM].y - verts[TOP].y);
		float invDeltaYTM = 1.0f / (verts[MIDDLE].y - verts[TOP].y);
		float invDeltaYMB = 1.0f / (verts[BOTTOM].y - verts[MIDDLE].y);

		// Scale out the vertices UV's based on the current texture.
		for (int i = 0; i < 3; i++)
		{
			verts[i].u *= m_targetTexture->GetWidth();
			verts[i].v *= m_targetTexture->GetHeight();
		}
	
		// Slope values for screen pixels
		float x1, x2;
		float z1, z2;
		int yStart;
		int yEnd;
		float xSlopeLeft, xSlopeRight;
		float zSlopeLeft, zSlopeRight;
		float spanXInv = 0.0f;

		// Slope values for texel values.
		float u1, u2;
		float v1, v2;
		float uSlopeLeft, vSlopeLeft;
		float uSlopeRight, vSlopeRight;

		// To update the interpolants.
		float sub = 0.0f;

		static ScanlineDataTex scanline;
		memset(&scanline, 0, sizeof(ScanlineDataTex));
		
		// Decide on how to setup the scanline rendering.
		if (triType == TRIANGLE_Minor)
		{
			// The two smaller triangle edges are to the left so we are basing our scan-line
			// rendering from this.

			// Calculate our values for the scan line interpolation.
			xSlopeLeft = (verts[MIDDLE].x - verts[TOP].x) * invDeltaYTM;
			xSlopeRight = (verts[BOTTOM].x - verts[TOP].x) * invDeltaYTB;
			x1 = verts[TOP].x;
			x2 = verts[TOP].x;
			yStart = ceil(verts[TOP].y);
			yEnd = ceil(verts[MIDDLE].y) - 1;

			// Set up the colour interpolation values.
			u1 = verts[TOP].u;
			u2 = verts[TOP].u;
			v1 = verts[TOP].v;
			v2 = verts[TOP].v;

			uSlopeLeft = (verts[MIDDLE].u - verts[TOP].u) * invDeltaYTM;
			uSlopeRight = (verts[BOTTOM].u - verts[TOP].u) * invDeltaYTB;
			vSlopeLeft = (verts[MIDDLE].v - verts[TOP].v) * invDeltaYTM;
			vSlopeRight = (verts[BOTTOM].v - verts[TOP].v) * invDeltaYTB;
		
			// The incidental offset that may be generated by using the ceil function.
			// We may get 'graphical artifacts' / visaul errors if we dont correct starting values
			// by this.
			sub = (float)yStart - verts[TOP].y;
			// If our right slope is less than our left slope then we are going to get an incomplete triangle.
			// Swap the values to fix this.
			bool hasSwaped = false;
			if (xSlopeRight < xSlopeLeft)
			{
				// Swap x axis interpolation.
				Swap<float>(x2,x1);
				Swap<float>(xSlopeLeft,xSlopeRight);

				// Swap texel interpolation.
				Swap<float>(u1, u2);
				Swap<float>(uSlopeLeft, uSlopeRight);

				Swap<float>(v1, v2);
				Swap<float>(vSlopeLeft, vSlopeRight);

				hasSwaped = true;
			}


			// Draw the upper triangle section.
			if (invDeltaYTM > EPSILON)
			{
				// Run down from the top to the middle of the triangle.
				for (int y = yStart; y <= yEnd; y++)
				{
					// Clamp the UVs
					u1 = Clamp<double>(0.0, (double)m_targetTexture->GetWidth(), u1);
					u2 = Clamp<double>(0.0, (double)m_targetTexture->GetWidth(), u2);
					v1 = Clamp<double>(0.0, (double)m_targetTexture->GetHeight(), v1);
					v2 = Clamp<double>(0.0, (double)m_targetTexture->GetHeight(), v2);

					scanline.y = y;
					scanline.xStart = x1;
					scanline.xEnd = x2;

					spanXInv = 1.0f /  fabs(x1 - x2);
					
					scanline.uStart = u1;
					scanline.uSlope = (u2 - u1) * spanXInv;
					scanline.vStart = v1;
					scanline.vSlope = (v2 - v1) * spanXInv;

					ScanLineTexAffine(&scanline);

					// Update the slopes.
					x1 += xSlopeLeft;
					x2 += xSlopeRight;
					u1 += uSlopeLeft;
					u2 += uSlopeRight;
					v1 += vSlopeLeft;
					v2 += vSlopeRight;
				}
			}
			
			// Swap back if we swapped.
			if (hasSwaped)
			{
				Swap<float>(x2,x1);
				Swap<float>(xSlopeLeft,xSlopeRight);
				
				// Swap texel interpolation.
				Swap<float>(u1, u2);
				Swap<float>(uSlopeLeft, uSlopeRight);

				Swap<float>(v1, v2);
				Swap<float>(vSlopeLeft, vSlopeRight);
			}

			// Setup the new slope values for the lower left edge.
			xSlopeLeft = (verts[BOTTOM].x - verts[MIDDLE].x) * invDeltaYMB;
			x1 = verts[MIDDLE].x;
			yStart = ceil(verts[MIDDLE].y);
			yEnd = ceil(verts[BOTTOM].y) - 1;

			u1 = verts[MIDDLE].u;
			v1 = verts[MIDDLE].v;

			// Calculate the new colour interpolations.
			uSlopeLeft = (verts[BOTTOM].u - verts[MIDDLE].u) * invDeltaYMB;
			vSlopeLeft = (verts[BOTTOM].v - verts[MIDDLE].v) * invDeltaYMB;
		
			if (x2 < x1)
			{
				Swap<float>(x2, x1);
				Swap<float>(xSlopeLeft, xSlopeRight);
				
				// Swap texel interpolation.
				Swap<float>(u1, u2);
				Swap<float>(uSlopeLeft, uSlopeRight);

				Swap<float>(v1, v2);
				Swap<float>(vSlopeLeft, vSlopeRight);
			}

			// The incidental offset that may be generated by using the ceil function.
			// We may get 'graphical artifacts' / visual errors if we dont correct starting values
			// by this.
			sub = (float)yStart - verts[MIDDLE].y;
			

			if (invDeltaYMB > EPSILON)
			{				
				// Run down from the middle to the bottom of the triangle.
				for (int y = yStart; y <= yEnd; y++)
				{
					// Clamp the UVs
					u1 = Clamp<double>(0.0, (double)m_targetTexture->GetWidth(), u1);
					u2 = Clamp<double>(0.0, (double)m_targetTexture->GetWidth(), u2);
					v1 = Clamp<double>(0.0, (double)m_targetTexture->GetHeight(), v1);
					v2 = Clamp<double>(0.0, (double)m_targetTexture->GetHeight(), v2);

					scanline.y = y;
					scanline.xStart = x1;
					scanline.xEnd = x2;

					spanXInv = 1.0f /  fabs(x1 - x2);
					
					scanline.uStart = u1;
					scanline.uSlope = (u2 - u1) * spanXInv;
					scanline.vStart = v1;
					scanline.vSlope = (v2 - v1) * spanXInv;

					ScanLineTexAffine(&scanline);

					// Update the slopes.
					x1 += xSlopeLeft;
					x2 += xSlopeRight;
					u1 += uSlopeLeft;
					u2 += uSlopeRight;
					v1 += vSlopeLeft;
					v2 += vSlopeRight;

				}
			}
		}
		else if (triType == TRIANGLE_Major)
		{
			// Calculate our values for the scan line interpolation.
			xSlopeLeft = (verts[BOTTOM].x - verts[TOP].x) * invDeltaYTB;
			xSlopeRight = (verts[MIDDLE].x - verts[TOP].x) * invDeltaYTM;
			x1 = verts[TOP].x;
			x2 = verts[TOP].x;
			yStart = ceil(verts[TOP].y);
			yEnd = ceil(verts[MIDDLE].y) - 1;

			u1 = verts[TOP].u;
			u2 = verts[TOP].u;
			v1 = verts[TOP].v;
			v2 = verts[TOP].v;

			uSlopeLeft = (verts[BOTTOM].u- verts[TOP].u) * invDeltaYTB;
			uSlopeRight = (verts[MIDDLE].u - verts[TOP].u) * invDeltaYTM;
			vSlopeLeft = (verts[BOTTOM].v - verts[TOP].v) * invDeltaYTB;
			vSlopeRight = (verts[MIDDLE].v - verts[TOP].v) * invDeltaYTM;

			bool hasSwaped = false;
			if (xSlopeRight < xSlopeLeft)
			{
				Swap<float>(x2,x1);
				Swap<float>(xSlopeLeft,xSlopeRight);
				
				// Swap texel interpolation.
				Swap<float>(u1, u2);
				Swap<float>(uSlopeLeft, uSlopeRight);

				Swap<float>(v1, v2);
				Swap<float>(vSlopeLeft, vSlopeRight);

				hasSwaped = true;
			}
			
			// The incidental offset that may be generated by using the ceil function.
			// We may get 'graphical artifacts' / visaul errors if we dont correct starting values
			// by this.
			sub = (float)yStart - verts[TOP].y;

			// Draw the upper triangle section.
			if (invDeltaYTM > EPSILON)
			{
				for (int y = yStart; y <= yEnd; y++)
				{
					// Clamp the UVs
					u1 = Clamp<double>(0.0, (double)m_targetTexture->GetWidth(), u1);
					u2 = Clamp<double>(0.0, (double)m_targetTexture->GetWidth(), u2);
					v1 = Clamp<double>(0.0, (double)m_targetTexture->GetHeight(), v1);
					v2 = Clamp<double>(0.0, (double)m_targetTexture->GetHeight(), v2);

					scanline.y = y;
					scanline.xStart = x1;
					scanline.xEnd = x2;

					spanXInv = 1.0f /  fabs(x1 - x2);
					
					scanline.uStart = u1;
					scanline.uSlope = (u2 - u1) * spanXInv;
					scanline.vStart = v1;
					scanline.vSlope = (v2 - v1) * spanXInv;

					ScanLineTexAffine(&scanline);

					// Update the slopes.
					x1 += xSlopeLeft;
					x2 += xSlopeRight;
					u1 += uSlopeLeft;
					u2 += uSlopeRight;
					v1 += vSlopeLeft;
					v2 += vSlopeRight;
				}
			}

			// Swap back if we swapped.
			if (hasSwaped)
			{
				Swap<float>(x2,x1);
				Swap<float>(xSlopeLeft,xSlopeRight);
				
				// Swap texel interpolation.
				Swap<float>(u1, u2);
				Swap<float>(uSlopeLeft, uSlopeRight);

				Swap<float>(v1, v2);
				Swap<float>(vSlopeLeft, vSlopeRight);
			}

			// Setup the new slope values for the lower left edge.
			xSlopeRight = (verts[BOTTOM].x - verts[MIDDLE].x) * invDeltaYMB;
			x2 = verts[MIDDLE].x;
			yStart = ceil(verts[MIDDLE].y);
			yEnd = ceil(verts[BOTTOM].y) - 1;
			
			u2 = verts[MIDDLE].u;
			v2 = verts[MIDDLE].v;

			// Calculate the new colour interpolations.
			uSlopeRight = (verts[BOTTOM].u - verts[MIDDLE].u) * invDeltaYMB;
			vSlopeRight = (verts[BOTTOM].v - verts[MIDDLE].v) * invDeltaYMB;

			
			if (x2 < x1)
			{
				Swap<float>(x2, x1);
				Swap<float>(xSlopeLeft, xSlopeRight);
				
				// Swap texel interpolation.
				Swap<float>(u1, u2);
				Swap<float>(uSlopeLeft, uSlopeRight);

				Swap<float>(v1, v2);
				Swap<float>(vSlopeLeft, vSlopeRight);
			}
			
			// The incidental offset that may be generated by using the ceil function.
			// We may get 'graphical artifacts' / visaul errors if we dont correct starting values
			// by this.
			sub = (float)yStart - verts[MIDDLE].y;

			if (invDeltaYMB > EPSILON)
			{				
				// Run down from the middle to the bottom of the triangle.
				for (int y = yStart; y <= yEnd; y++)
				{
					// Clamp the UVs
					u1 = Clamp<double>(0.0, (double)m_targetTexture->GetWidth(), u1);
					u2 = Clamp<double>(0.0, (double)m_targetTexture->GetWidth(), u2);
					v1 = Clamp<double>(0.0, (double)m_targetTexture->GetHeight(), v1);
					v2 = Clamp<double>(0.0, (double)m_targetTexture->GetHeight(), v2);

					scanline.y = y;
					scanline.xStart = x1;
					scanline.xEnd = x2;

					spanXInv = 1.0f /  fabs(x1 - x2);
					
					scanline.uStart = u1;
					scanline.uSlope = (u2 - u1) * spanXInv;
					scanline.vStart = v1;
					scanline.vSlope = (v2 - v1) * spanXInv;

					ScanLineTexAffine(&scanline);

					// Update the slopes.
					x1 += xSlopeLeft;
					x2 += xSlopeRight;
					u1 += uSlopeLeft;
					u2 += uSlopeRight;
					v1 += vSlopeLeft;
					v2 += vSlopeRight;
				}
			}
		}
		else
		{
			LOG("Triangle failed to be classified as a valid render type", LOG_Error);
			assert(false);
		}
	}
	
	// The texture based triangle rasterization is the same as the Gourad shaded filled triangle procedure.
	// We use the same steps and procedures of deducing the type of triangle (Major or minor) however here
	// we are interpolating UV co-ordinates based on the slopes between each vertex.
	// Each UV co-ordinate is also scaled up by the source texture width and height to avoid getting visaul artifacts.
	void Rasterizer::RasterizeTriTex_EdgeList(Vertex* tri)
	{
		// The type of triangle that is being drawn.
		// Major means that edge with greatest Y delta is on left, minor means it is on right.
		TriangleEdgeType triType;

		// Sort vertices up to down.
		Vertex verts[3];
		
		SortByY(verts, tri);

		// Determine if the triangle is major/minor.
		if (verts[BOTTOM].x > verts[MIDDLE].x)
			triType = TRIANGLE_Minor;
		else
			triType = TRIANGLE_Major;

		// Set up Y deltas and inverse multipliers for edge traversal.
		float invDeltaYTB = 1.0f / (verts[BOTTOM].y - verts[TOP].y);
		float invDeltaYTM = 1.0f / (verts[MIDDLE].y - verts[TOP].y);
		float invDeltaYMB = 1.0f / (verts[BOTTOM].y - verts[MIDDLE].y);

		// Scale out the vertices UV's based on the current texture.
		for (int i = 0; i < 3; i++)
		{
			verts[i].u *= m_targetTexture->GetWidth();
			verts[i].v *= m_targetTexture->GetHeight();
		}

		// Cast the scanline list pointer.
		this->m_scanLineTexBuffer = (SWR::ScanlineDataTex*)this->m_scanLineBuffer;

		// Build the edge list
		if (triType == TRIANGLE_Minor)
			this->GenerateMinorEdgeListTex(verts, invDeltaYTB, invDeltaYTM, invDeltaYMB);
		else
			this->GenerateMajorEdgeListTex(verts, invDeltaYTB, invDeltaYTM, invDeltaYMB);

		// Calculate the total number of scanlines that we are going to need to render.
		int totalScanlines = (int)(verts[BOTTOM].y - verts[TOP].y);

		this->BatchRasterizeEdgeListTex(totalScanlines + 1);
	}

	void Rasterizer::RasterizeTriTexLight(Vertex* vertices)
	{
		// Unimplemented.
	}

	
	// Colour edge list generation.
	void Rasterizer::GenerateMajorEdgeListCol(Vertex* verts, float invDeltaYTB, float invDeltaYTM, float invDeltaYMB)
	{
	}

	void Rasterizer::GenerateMinorEdgeListCol(Vertex* verts, float invDeltaYTB, float invDeltaYTM, float invDeltaYMB)
	{
	}

	void Rasterizer::GenerateMajorEdgeListTex(Vertex* verts, float invDeltaYTB, float invDeltaYTM, float invDeltaYMB)
	{
		// Slope values for screen pixels
		float x1, x2;
		float z1, z2;
		int yStart;
		int yEnd;
		float xSlopeLeft, xSlopeRight;
		float zSlopeLeft, zSlopeRight;
		float spanXInv = 0.0f;

		// Slope values for texels.
		float u1, u2;
		float v1, v2;
		float uSlopeLeft, vSlopeLeft;
		float uSlopeRight, vSlopeRight;

		// Calculate our values for the scan line interpolation.
		xSlopeLeft = (verts[MIDDLE].x - verts[TOP].x) * invDeltaYTM;
		xSlopeRight = (verts[BOTTOM].x - verts[TOP].x) * invDeltaYTB;
		x1 = verts[TOP].x;
		x2 = verts[TOP].x;
		yStart = ceil(verts[TOP].y);
		yEnd = ceil(verts[MIDDLE].y) - 1;

		// Set up the colour interpolation values.
		u1 = verts[TOP].u;
		u2 = verts[TOP].u;
		v1 = verts[TOP].v;
		v2 = verts[TOP].v;

		uSlopeLeft = (verts[MIDDLE].u - verts[TOP].u) * invDeltaYTM;
		uSlopeRight = (verts[BOTTOM].u - verts[TOP].u) * invDeltaYTB;
		vSlopeLeft = (verts[MIDDLE].v - verts[TOP].v) * invDeltaYTM;
		vSlopeRight = (verts[BOTTOM].v - verts[TOP].v) * invDeltaYTB;
		
		// If our right slope is less than our left slope then we are going to get an incomplete triangle.
		// Swap the values to fix this.
		bool hasSwaped = false;
		if (xSlopeRight < xSlopeLeft)
		{
			// Swap x axis interpolation.
			Swap<float>(x2,x1);
			Swap<float>(xSlopeLeft,xSlopeRight);

			// Swap texel interpolation.
			Swap<float>(u1, u2);
			Swap<float>(uSlopeLeft, uSlopeRight);

			Swap<float>(v1, v2);
			Swap<float>(vSlopeLeft, vSlopeRight);

			hasSwaped = true;
		}

		int index = 0; 


		// Draw the upper triangle section.
		if (invDeltaYTM > EPSILON)
		{
			// Run down from the top to the middle of the triangle.
			for (int y = yStart; y <= yEnd; y++)
			{
				// Clamp the UVs
				u1 = Clamp<float>(0.0, (float)m_targetTexture->GetWidth(), u1);
				u2 = Clamp<float>(0.0, (float)m_targetTexture->GetWidth(), u2);
				v1 = Clamp<float>(0.0, (float)m_targetTexture->GetHeight(), v1);
				v2 = Clamp<float>(0.0, (float)m_targetTexture->GetHeight(), v2);

				m_scanLineTexBuffer[index].y = y;
				m_scanLineTexBuffer[index].xStart = x1;
				m_scanLineTexBuffer[index].xEnd = x2;

				spanXInv = 1.0f /  fabs(x1 - x2);
					
				m_scanLineTexBuffer[index].uStart = u1;
				m_scanLineTexBuffer[index].uSlope = (u2 - u1) * spanXInv;
				m_scanLineTexBuffer[index].vStart = v1;
				m_scanLineTexBuffer[index].vSlope = (v2 - v1) * spanXInv;

				// Update the slopes.
				x1 += xSlopeLeft;
				x2 += xSlopeRight;
				u1 += uSlopeLeft;
				u2 += uSlopeRight;
				v1 += vSlopeLeft;
				v2 += vSlopeRight;

				index++;
			}
		}
			
		// Swap back if we swapped.
		if (hasSwaped)
		{
			Swap<float>(x2,x1);
			Swap<float>(xSlopeLeft,xSlopeRight);
				
			// Swap texel interpolation.
			Swap<float>(u1, u2);
			Swap<float>(uSlopeLeft, uSlopeRight);

			Swap<float>(v1, v2);
			Swap<float>(vSlopeLeft, vSlopeRight);
		}

		// Setup the new slope values for the lower left edge.
		xSlopeLeft = (verts[BOTTOM].x - verts[MIDDLE].x) * invDeltaYMB;
		x1 = verts[MIDDLE].x;
		yStart = ceil(verts[MIDDLE].y);
		yEnd = ceil(verts[BOTTOM].y) - 1;

		u1 = verts[MIDDLE].u;
		v1 = verts[MIDDLE].v;

		// Calculate the new colour interpolations.
		uSlopeLeft = (verts[BOTTOM].u - verts[MIDDLE].u) * invDeltaYMB;
		vSlopeLeft = (verts[BOTTOM].v - verts[MIDDLE].v) * invDeltaYMB;
		
		if (x2 < x1)
		{
			Swap<float>(x2, x1);
			Swap<float>(xSlopeLeft, xSlopeRight);
				
			// Swap texel interpolation.
			Swap<float>(u1, u2);
			Swap<float>(uSlopeLeft, uSlopeRight);

			Swap<float>(v1, v2);
			Swap<float>(vSlopeLeft, vSlopeRight);
		}


		if (invDeltaYMB > EPSILON)
		{				
			// Run down from the middle to the bottom of the triangle.
			for (int y = yStart; y <= yEnd; y++)
			{
				// Clamp the UVs
				u1 = Clamp<float>(0.0, (float)m_targetTexture->GetWidth(), u1);
				u2 = Clamp<float>(0.0, (float)m_targetTexture->GetWidth(), u2);
				v1 = Clamp<float>(0.0, (float)m_targetTexture->GetHeight(), v1);
				v2 = Clamp<float>(0.0, (float)m_targetTexture->GetHeight(), v2);

				m_scanLineTexBuffer[index].y = y;
				m_scanLineTexBuffer[index].xStart = x1;
				m_scanLineTexBuffer[index].xEnd = x2;

				spanXInv = 1.0f /  fabs(x1 - x2);
					
				m_scanLineTexBuffer[index].uStart = u1;
				m_scanLineTexBuffer[index].uSlope = (u2 - u1) * spanXInv;
				m_scanLineTexBuffer[index].vStart = v1;
				m_scanLineTexBuffer[index].vSlope = (v2 - v1) * spanXInv;

				// Update the slopes.
				x1 += xSlopeLeft;
				x2 += xSlopeRight;
				u1 += uSlopeLeft;
				u2 += uSlopeRight;
				v1 += vSlopeLeft;
				v2 += vSlopeRight;

				index++;

			}
		}
	}

	void Rasterizer::GenerateMinorEdgeListTex(Vertex* verts, float invDeltaYTB, float invDeltaYTM, float invDeltaYMB)
	{
		// Slope values for screen pixels
		float x1, x2;
		float z1, z2;
		int yStart;
		int yEnd;
		float xSlopeLeft, xSlopeRight;
		float zSlopeLeft, zSlopeRight;
		float spanXInv = 0.0f;

		// Slope values for texels.
		float u1, u2;
		float v1, v2;
		float uSlopeLeft, vSlopeLeft;
		float uSlopeRight, vSlopeRight;
		bool hasSwaped = false;

		// Calculate our values for the scan line interpolation.
		xSlopeLeft = (verts[BOTTOM].x - verts[TOP].x) * invDeltaYTB;
		xSlopeRight = (verts[MIDDLE].x - verts[TOP].x) * invDeltaYTM;
		x1 = verts[TOP].x;
		x2 = verts[TOP].x;
		yStart = ceil(verts[TOP].y);
		yEnd = ceil(verts[MIDDLE].y) - 1;

		u1 = verts[TOP].u;
		u2 = verts[TOP].u;
		v1 = verts[TOP].v;
		v2 = verts[TOP].v;

		uSlopeLeft = (verts[BOTTOM].u- verts[TOP].u) * invDeltaYTB;
		uSlopeRight = (verts[MIDDLE].u - verts[TOP].u) * invDeltaYTM;
		vSlopeLeft = (verts[BOTTOM].v - verts[TOP].v) * invDeltaYTB;
		vSlopeRight = (verts[MIDDLE].v - verts[TOP].v) * invDeltaYTM;

		if (xSlopeRight < xSlopeLeft)
		{
			Swap<float>(x2,x1);
			Swap<float>(xSlopeLeft,xSlopeRight);
				
			// Swap texel interpolation.
			Swap<float>(u1, u2);
			Swap<float>(uSlopeLeft, uSlopeRight);

			Swap<float>(v1, v2);
			Swap<float>(vSlopeLeft, vSlopeRight);

			hasSwaped = true;
		}

		int index = 0;

		// Draw the upper triangle section.
		if (invDeltaYTM > EPSILON)
		{
			for (int y = yStart; y <= yEnd; y++)
			{
				// Clamp the UVs
				u1 = Clamp<float>(0.0, (float)m_targetTexture->GetWidth(), u1);
				u2 = Clamp<float>(0.0, (float)m_targetTexture->GetWidth(), u2);
				v1 = Clamp<float>(0.0, (float)m_targetTexture->GetHeight(), v1);
				v2 = Clamp<float>(0.0, (float)m_targetTexture->GetHeight(), v2);

				this->m_scanLineTexBuffer[index].y = y;
				this->m_scanLineTexBuffer[index].xStart = x1;
				this->m_scanLineTexBuffer[index].xEnd = x2;

				spanXInv = 1.0f /  fabs(x1 - x2);
					
				this->m_scanLineTexBuffer[index].uStart = u1;
				this->m_scanLineTexBuffer[index].uSlope = (u2 - u1) * spanXInv;
				this->m_scanLineTexBuffer[index].vStart = v1;
				this->m_scanLineTexBuffer[index].vSlope = (v2 - v1) * spanXInv;

				// Update the slopes.
				x1 += xSlopeLeft;
				x2 += xSlopeRight;
				u1 += uSlopeLeft;
				u2 += uSlopeRight;
				v1 += vSlopeLeft;
				v2 += vSlopeRight;
				index++;
			}
		}

		// Swap back if we swapped.
		if (hasSwaped)
		{
			Swap<float>(x2,x1);
			Swap<float>(xSlopeLeft,xSlopeRight);
				
			// Swap texel interpolation.
			Swap<float>(u1, u2);
			Swap<float>(uSlopeLeft, uSlopeRight);

			Swap<float>(v1, v2);
			Swap<float>(vSlopeLeft, vSlopeRight);
		}

		// Setup the new slope values for the lower left edge.
		xSlopeRight = (verts[BOTTOM].x - verts[MIDDLE].x) * invDeltaYMB;
		x2 = verts[MIDDLE].x;
		yStart = ceil(verts[MIDDLE].y);
		yEnd = ceil(verts[BOTTOM].y) - 1;
			
		u2 = verts[MIDDLE].u;
		v2 = verts[MIDDLE].v;

		// Calculate the new colour interpolations.
		uSlopeRight = (verts[BOTTOM].u - verts[MIDDLE].u) * invDeltaYMB;
		vSlopeRight = (verts[BOTTOM].v - verts[MIDDLE].v) * invDeltaYMB;

			
		if (x2 < x1)
		{
			Swap<float>(x2, x1);
			Swap<float>(xSlopeLeft, xSlopeRight);
				
			// Swap texel interpolation.
			Swap<float>(u1, u2);
			Swap<float>(uSlopeLeft, uSlopeRight);

			Swap<float>(v1, v2);
			Swap<float>(vSlopeLeft, vSlopeRight);
		}
	
		if (invDeltaYMB > EPSILON)
		{				
			// Run down from the middle to the bottom of the triangle.
			for (int y = yStart; y <= yEnd; y++)
			{
				// Clamp the UVs
				u1 = Clamp<float>(0.0, (float)m_targetTexture->GetWidth(), u1);
				u2 = Clamp<float>(0.0, (float)m_targetTexture->GetWidth(), u2);
				v1 = Clamp<float>(0.0, (float)m_targetTexture->GetHeight(), v1);
				v2 = Clamp<float>(0.0, (float)m_targetTexture->GetHeight(), v2);

				this->m_scanLineTexBuffer[index].y = y;
				this->m_scanLineTexBuffer[index].xStart = x1;
				this->m_scanLineTexBuffer[index].xEnd = x2;

				spanXInv = 1.0f /  fabs(x1 - x2);
					
				this->m_scanLineTexBuffer[index].uStart = u1;
				this->m_scanLineTexBuffer[index].uSlope = (u2 - u1) * spanXInv;
				this->m_scanLineTexBuffer[index].vStart = v1;
				this->m_scanLineTexBuffer[index].vSlope = (v2 - v1) * spanXInv;


				// Update the slopes.
				x1 += xSlopeLeft;
				x2 += xSlopeRight;
				u1 += uSlopeLeft;
				u2 += uSlopeRight;
				v1 += vSlopeLeft;
				v2 += vSlopeRight;
				index++;
			}
		}
	}

	
	// -------------------------------------------------------------------------------------
	// Batch Render Functionality

	void Rasterizer::BatchRasterizeEdgeListCol(int edges)
	{
		for (int i = 0; i < edges; i++)
		{
			ScanLineCol(&m_scanLineColBuffer[i]);
		}
	}

	void Rasterizer::BatchRasterizeEdgeListTex(int edges)
	{
		for (int i = 0; i < edges; i++)
		{
			ScanLineTexAffine(&m_scanLineTexBuffer[i]);
		}
	}

	// -------------------------------------------------------------------------------------


	// -------------------------------------------------------------------------------------
	// Copy constructors for the scanline data.
	ScanlineDataCol::ScanlineDataCol(const ScanlineDataCol& data)
	{
		xStart = data.xStart;
		xEnd = data.xEnd;
		y = data.y;
		zStart = data.zStart;
		zSlope = data.zSlope;
		rStart = data.rStart;
		rSlope = data.rSlope;
		gStart = data.gStart;
		gSlope = data.gSlope;
		bStart = data.bStart;
		bSlope = data.bSlope;
	}

	ScanlineDataTex::ScanlineDataTex(const ScanlineDataTex &data)
	{
		xStart = data.xStart;
		xEnd = data.xEnd;
		y = data.y;
		zStart = data.zStart;
		zSlope = data.zSlope;
		uStart = data.uStart;
		uSlope = data.uSlope;
		vStart = data.vStart;
		vSlope = data.vSlope;
	}
	// -------------------------------------------------------------------------------------
	
}; // End namespace SWR.
