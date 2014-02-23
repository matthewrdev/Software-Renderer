#pragma once

#ifndef RENDER_DEVICE_H
#define RENDER_DEVICE_H

//****************************************************************************
//**
//**    RenderDevice.h
//**
//**    Copyright (c) 2010 Matthew Robbins
//**
//**    Author:  Matthew Robbins
//**    Created: 06/2010
//**
//****************************************************************************

#include "DataTypes.h"

#include "Vertex.h"
#include "Matrix4.h"
#include "Vector3.h"

// Forward Declarations
namespace SWR
{
	class BackBuffer;
	class ZDepthBuffer;
	class Rasterizer;
	class VertexBuffer;
	class IndexBuffer;
	class Texture;
	class TriangleClipper2D;
	class Rectangle;
	class LightingManager;
};

namespace SWR
{
	// ------------------------------------------------------------------------
	//							TextureMappingTypeSet
	// ------------------------------------------------------------------------
	// Desc:
	// Flags that are used to denote what type of texture mapping technique the
	// renderer will use.
	// Affine uses linear interpelation between the UV co-ordinates of the 
	// vertices and therefore may result in slightly off looking texture 
	// mapping. It is a cheaper operation (avoids a divide) however so may be
	// suitable in cases where performance is valued over visual quality.
	//
	// Perspective mapping perspective corrects each UV co-ordinate to find the
	// correct  texel to index into. The result is the texture mapping is 
	// visually correct, and textures correspond correctly to the polygon.
	// It uses a 1/Z operation to do this so is a slightly more expensive 
	// operation than affine mapping.
	// ------------------------------------------------------------------------
	enum TextureMappingTypeSet
	{
		TEX_MAP_Affine,
		TEX_MAP_Perspective,

		TEX_MAP_Invalid,
	};
	
	// ------------------------------------------------------------------------
	//							   DisplayBitDepth
	// ------------------------------------------------------------------------
	// Desc:
	// Flags to configure the bit depth per pixel.
	// 16 bit colour has a smaller size but less colour range than 32 bit.
	// ------------------------------------------------------------------------
	enum DisplayBitDepth
	{
		DBP_Bit16 = 16,
		DBD_Bit32 = 32,
	};
	
	// ------------------------------------------------------------------------
	//							   BackfaceCullWinding
	// ------------------------------------------------------------------------
	// Desc:
	// Flags that configure how triangles are backface culled.
	// If we have a vertex buffer of vertices 1-2-3, and we create normals 
	// generated from 2-1, 2-3. 
	// Clockwise will generate a cross product of 2-1 & 2-3.
	// AntiClockwise will generate a cross product of 2-3 & 2-1.
	// ------------------------------------------------------------------------
	enum BackfaceCullWinding
	{
		BFCULL_Clockwise,
		BFCULL_AntiClockwise,
	};
	
	// ------------------------------------------------------------------------
	//						   TriangleRenderType
	// ------------------------------------------------------------------------
	// Desc:
	// Flags used to depict how the triangle submitted for rendering will be
	// rendered. 
	// List denotes that the triangles come in sets of 3.
	// Strip denotes that each triangle will use the previous
	// ------------------------------------------------------------------------
	enum TriangleRenderType
	{
		TRIANGLE_List,
		TRIANGLE_Strip,
	};


	// ------------------------------------------------------------------------
	//								 SWRInitParams
	// ------------------------------------------------------------------------
	// Desc:
	// A struct used to initilise the render device when it is created.
	// This includes back buffer dimensions, texture mapping techinique,
	// lights to apply to scene etc.
	// ------------------------------------------------------------------------
	struct SWRInitParams
	{
	private:
	protected:
	public:
		// The width of the display buffer.
		U16 bufferWidth;

		// The height of the display buffer
		U16 bufferHeight;

		// If to set the renderer up to use a z-buffer and z-depth tests.
		// See DisplayBitDepth enum for description.
		bool useZBuffer;

		// The bit depth per pixel for the display.
		DisplayBitDepth bitDepth;

		// The texture mapping techinique the renderer is to use.
		// See TextureMappingTypeSet enum for description.
		TextureMappingTypeSet texMapType;

		SWRInitParams(){}
		~SWRInitParams(){}
	};

	// ------------------------------------------------------------------------
	//								RenderDevice
	// ------------------------------------------------------------------------
	// Desc:
	// The render device is a software based renderer designed to emulate the 
	// functionality of a graphics device.
	// This handles the perspective projection, backface culling, triangle 
	// clipping etc before passing the finalised triangle onto the rasterizer
	// for plotting the pixels to the backbuffer.
	// Is also responsible for configuring the rasterizer based on the current
	// render flags.
	// ------------------------------------------------------------------------
	class RenderDevice
	{
	private:
		// The windows device context.
		HDC winDevContext;

		// The buffers for the render-device.
		BackBuffer* m_backBuffer;
		ZDepthBuffer* m_zBuffer;

		// The rasterizer.
		Rasterizer* m_rasterizer;

		// Lighter.
		LightingManager* m_lightManager;

		// The triangle clipper.
		TriangleClipper2D* m_triClipper;

		// Flags that configure the render device.
		TextureMappingTypeSet m_texMapType;
		DisplayBitDepth m_bitDepth;
		BackfaceCullWinding m_backfaceCullWinding;

		Texture* m_sourceTexture;

		// Render device initilisation functions.
		SWR_ERR SetDisplaySettings(U16 width, U16 height, DisplayBitDepth bitDepth);

		// Current vertex and index buffer pointers.
		VertexBuffer* m_vertexSource;
		IndexBuffer* m_indexSource;

		// Our vertex triplet that represents a triangle. When rendering we assign into this buffer.
		Vertex* m_triangle;
		Vertex* m_clippedVerts; // Up to 9 vertices.
		
		// *****************************************************************************************
		// Function pointers that configure the renderer.
		// *****************************************************************************************
		
		// *****************************************************************************************
		// Triangle culling and clipping.
		// *****************************************************************************************

		bool m_cullingEnabled;
		bool IsBackfacingCC(Vertex* verts);
		bool IsBackfacingAC(Vertex* verts);
		
		// *****************************************************************************************
		// Projection functions and variables.
		// *****************************************************************************************

		// Clipping planes.
		Real m_nearPlane;
		Real m_farPlane;
		Real m_fov;
		Real m_focalX;
		Real m_focalY;
		Real m_halfVPW;
		Real m_halfVPH;

		void CalculateFocal(float width, float height, float FOV);

		// Projects the vertex into camera space and then into screen space.
		inline void Project(Vertex* vert)
		{
			// Project the point onto the 2D image plane.
			// THis is essentially breaking apart a matrix operation to avoid unnecessery operations.
			// We are using the distance the pinhole camera is behind the projection plane to 
			// calculate the projected values.
			// Add half the viewport dimensions is to ensure that the screens origin appears in 
			// the centre (it would be in the top left other wise)
			// A standard projection matrix looks like so:
			// 
			//    /--         --\
			//    | w  0  0   0 |
			//    | 0  h  0   0 |
			//    | 0  0  Q   1 |
			//    | 0  0 -QZn 0 |
			//    \--         --/
			//
			// 

			float q = m_farPlane / (m_farPlane - m_nearPlane);
			float x = m_focalX * vert->x;// + m_halfVPW;  // w * x / z + (vW / 2).
			float y = m_focalY * -vert->y;//+ m_halfVPH; // h * y / z * -1 + (vH / 2).
			float z = vert->z * q - q * m_nearPlane;
			float w = vert->z * 1;
			x /= w;
			y /= w;
			z /= w;
			vert->x = x + m_halfVPW; // Centre correct.
			vert->y = y + m_halfVPH; // Centre correct.
			vert->z = z;
		}

		inline void Project(Vector3& v)
		{
			// Project the point onto the 2D image plane.
			// THis is essentially breaking apart a matrix operation to avoid unnecessery operations.
			// We are using the distance the pinhole camera is behind the projection plane to 
			// calculate the projected values.
			// Add half the viewport dimensions is to ensure that the screens origin appears in 
			// the centre (it would be in the top left other wise)
			// A standard projection matrix looks like so:
			// 
			//    /--         --\
			//    | w  0  0   0 |
			//    | 0  h  0   0 |
			//    | 0  0  Q   1 |
			//    | 0  0 -QZn 0 |
			//    \--         --/
			//
			// 

			float q = m_farPlane / (m_farPlane - m_nearPlane);
			float x = m_focalX * v.x;// + m_halfVPW;  // w * x / z + (vW / 2).
			float y = m_focalY * -v.y;//+ m_halfVPH; // h * y / z * -1 + (vH / 2).
			float z = v.z * q - q * m_nearPlane;
			float w = v.z * 1;
			x /= w;
			y /= w;
			z /= w;
			v.x = x + m_halfVPW; // Centre correct.
			v.y = y + m_halfVPH; // Centre correct.
			v.z = z;
		}
		
		// *****************************************************************************************
		// Transformation matrices.
		// *****************************************************************************************

		// The concatenated transformation matrix to convert a vertex to camera space.
		Matrix4 m_transform;

		// Camera space matrices
		Matrix4 m_cameraMat;
		Matrix4 m_cameraMatInv; // To Transform into camera space.
		Vector3 m_camLocation;

		Matrix4 m_world;
		Matrix4 m_worldInv;

		// Puts the triangle into camera space, ready to be projected into screen space.
		void ToCameraSpace(Vertex* vert);
		void ToWorldSpace(Vertex* vert);
		void ToWorldCameraSpace(Vertex* vert);

		// Stats gathering.
		int trisDrawn;
		int trisCulled;
		int trisSubmittedForDrawing;
		
	protected:
	public:
		RenderDevice();
		~RenderDevice();

		SWR_ERR Initilise(const SWRInitParams &params, HWND hWnd);
		SWR_ERR Release(HWND hWnd);

		//SWR_ERR Reset(

		void ClearBackBuffer(UINT32 value);
		void ClearZBuffer();

		LightingManager* GetLightingManager();

		void Present(HWND hWnd);

		// Functions that configure the set-up of the renderer
		void SetFOV(Real FOV);
		void SetTextureMappingType(TextureMappingTypeSet type);
		void SetClipPlanes(float nearPlane, float farPlane);

		void SetPixelColour(U16 x, U16 y, U32 colour);

		void EnableBackfaceCulling(bool enable);
		bool IsBackfaceCullingEnabled() const;

		void SetVertexBuffer(VertexBuffer* buffer);
		void SetIndexBuffer(IndexBuffer* buffer);

		void SetSourceTexture(Texture* texture);

		void SetWorldTransform(const Matrix4& m);
		void SetCameraTransform(const Matrix4& m);
		void CommitMatrixChanges(); // Recalculates the matrix inverses and concatenates camera / world into a transformation matrix.
		
		// *****************************************************************************************
		// Render functionality to render with triangles using only certain properties such as 
		// textures or colouring.
		// *****************************************************************************************

		// Draws from the current vertex buffer as a triangle list. Draws only colour component.
		void DrawTrisColList(bool useIndexBuffer, int totalTris, int start);

		// Draws from the current vertex buffer as a triangle strip. Draws only colour component.
		void DrawTrisColStrip(bool useIndexBuffer, int totalTris, int start);
		
		// Draws from the current vertex buffer as a triangle list. Draws colour component and applies lighting.
		void DrawTrisColLitList(bool useIndexBuffer, int totalTris, int start);

		// Draws from the current vertex buffer as a triangle strip. Draws colour component and applies lighting.
		void DrawTrisColLitStrip(bool useIndexBuffer, int totalTris, int start);
		
		// Draws from the current vertex buffer as a triangle list. Draws only texture component.
		void DrawTrisTexList(bool useIndexBuffer, int totalTris, int start);

		// Draws from the current vertex buffer as a triangle strip. Draws only texture component.
		void DrawTrisTexStrip(bool useIndexBuffer, int totalTris, int start);
		
		// Draws from the current vertex buffer as a triangle list. Draws texture component and applies lighting.
		void DrawTrisTexLitList(bool useIndexBuffer, int totalTris, int start);

		// Draws from the current vertex buffer as a triangle strip. Draws texture component and applies lighting.
		void DrawTrisTexLitStrip(bool useIndexBuffer, int totalTris, int start);

		void DrawNormals(VertexBuffer* buffer, Colour32 colour, float normalLength);
		void DrawWireFrame(bool useIndexBuffer, int totalTris, int start, Colour32 colour);

		// Draws the texture to the screen at the specified x/y position.
		void DrawTexture2D(U16 x, U16 y, Texture* texture);
		void DrawTexture2D(U16 x, U16 y, Texture* texture, Rectangle* srcRect);
		void DrawTexture2D(U16 x, U16 y, Texture* texture, Rectangle* srcRect, U32 alphaFilter);

		// Renders a 2D triangle.
		void DrawTrisTex2D(bool useIndexBuffer, int totalTris, int start);

		// Method for accessing statistics on the renderer.
		int GetTrisCulled() const;
		int GetTrisRendered() const;
		int GetTrisSubmittedForRender() const;

		void ResetStatsCounters();


	};
	
}; // End namespace SWR.

#endif // #ifndef RENDER_DEVICE_H