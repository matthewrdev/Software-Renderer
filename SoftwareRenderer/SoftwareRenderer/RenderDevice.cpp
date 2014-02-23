//****************************************************************************
//**
//**    Renderer.cpp
//**
//**    Copyright (c) 2010 Matthew Robbins
//**
//**    Author:  Matthew Robbins
//**    Created: 06/2010
//**
//****************************************************************************

#include <Windows.h>
#include <assert.h>
#include <iostream>

#include "RenderDevice.h"

#include "Rasterizer.h"
#include "TriangleClipper2D.h"
#include "LightingManager.h"

#include "BackBuffer.h"
#include "ZDepthBuffer.h"

#include "Colour.h"
#include "IndexBuffer.h"
#include "Vertex.h"
#include "Texture.h"

#include "Matrix4.h"
#include "Vector3.h"
#include "SWR_Math.h"
#include "Rectangle.h"

#include "Logger.h"
#include "MemoryLeak.h"

namespace SWR
{
	RenderDevice::RenderDevice()
		: winDevContext(NULL)
		, m_backBuffer(NULL)
		, m_zBuffer(NULL)
		, m_rasterizer(NULL)
		, m_texMapType(TEX_MAP_Affine)
		, m_fov(45.0f)
		, m_vertexSource(NULL)
		, m_indexSource(NULL)
		, m_triangle(NULL)
		, m_nearPlane(1.0f)
		, m_farPlane(1000.0f)
		, m_cullingEnabled(false)
		, m_clippedVerts(NULL)
		, m_triClipper(NULL)
		, m_lightManager(NULL)
	{
		// Default initilises the renderer.
		// All construction should be done through initilise method.
	}

	RenderDevice::~RenderDevice()
	{
		// All destruction should be done through the release method.
		if (m_triangle != NULL)
		{
			delete [] m_triangle;
			m_triangle = NULL;
		}

		if (m_clippedVerts != NULL)
		{
			delete [] m_clippedVerts;
			m_clippedVerts = NULL;
		}
	}
	
	SWR_ERR RenderDevice::Initilise(const SWRInitParams &params, HWND hWnd)
	{
		// Validate back-buffer dimensions.
		if (params.bufferHeight < 1 || params.bufferWidth < 1)
		{	
			char buffer[256] = {0};
			sprintf(buffer, "Invalid back-buffer dimensions for render-device. W=%i H=%i", params.bufferWidth, params.bufferHeight);
			LOG(buffer, LOG_Error);
			return SWR_FAIL;
		}

		// Create the device context for the window.
		winDevContext = GetDC(hWnd);
		if (winDevContext == NULL)
		{
			LOG("Windows device context retrieval has failed.", LOG_Error);
			return SWR_FAIL;
		}

		// Create the back buffer we are drawing to.
		this->m_backBuffer = new BackBuffer();
		SWR_ERR result = m_backBuffer->CreateBuffer(params.bufferWidth, params.bufferHeight, winDevContext);
		if (result != SWR_OK)
		{
			LOG("Back-buffer creation has failed.", LOG_Error);
			return SWR_FAIL;
		}

		this->m_zBuffer = new ZDepthBuffer();
		m_zBuffer->Initilise(params.bufferWidth, params.bufferHeight);

		m_lightManager = new LightingManager(5);

		m_triangle = new Vertex[3];
		m_clippedVerts  = new Vertex[9];

		m_rasterizer = new Rasterizer();
		m_rasterizer->SetTargetBuffers(m_backBuffer->GetByteBuffer(), m_zBuffer, params.bufferWidth, params.bufferHeight);
		LOG("Render Device startup sucessful.", LOG_Init);

		m_triClipper = new TriangleClipper2D();
		m_triClipper->SetViewDimensions(params.bufferWidth, params.bufferHeight);

		ResetStatsCounters();

		return SWR_OK;
	}

	SWR_ERR RenderDevice::SetDisplaySettings(U16 width, U16 height, DisplayBitDepth bitDepth)
	{
		return SWR_OK;
	}

	SWR_ERR RenderDevice::Release(HWND hWnd)
	{
		if (m_backBuffer != NULL)
		{
			m_backBuffer->ReleaseBuffer();
			delete m_backBuffer;
			m_backBuffer = NULL;
		}

		if (m_zBuffer != NULL)
		{
			m_zBuffer->Release();
			delete m_zBuffer;
			m_zBuffer = NULL;
		}

		if (m_rasterizer != NULL)
		{
			delete m_rasterizer;
			m_rasterizer = NULL;
		}

		if (m_triangle != NULL)
		{
			delete [] m_triangle;
			m_triangle = NULL;
		}

		if (m_triClipper != NULL)
		{
			delete m_triClipper;
			m_triClipper = NULL;
		}

		if (m_lightManager != NULL)
		{
			delete m_lightManager;
			m_lightManager = NULL;
		}

		// Destroy the device context.
		if(winDevContext)
		{
			ReleaseDC(hWnd,winDevContext);
			winDevContext = NULL;
		}

		LOG("Render Device shutdown sucessful.", LOG_Shutdown);
		return SWR_OK;
	}

	void RenderDevice::ClearBackBuffer(UINT32 value)
	{
		m_backBuffer->Clear(value);
	}

	void RenderDevice::ClearZBuffer()
	{
		m_zBuffer->Clear(ZDepthBuffer::MAX_Z_DEPTH);
	}

	void RenderDevice::Present(HWND hWnd)
	{
		// Blit the back-buffer onto the screen.	
		assert(winDevContext != NULL);
		assert(m_backBuffer->GetDeviceContext() != NULL);

		RECT clRect;
		GetClientRect(hWnd,&clRect);

		// Blit to buffer to the window
		if(BitBlt(winDevContext,
				  clRect.left,
				  clRect.top,
				  (clRect.right - clRect.left) + 1, 
				  (clRect.bottom - clRect.top) + 1,
				  m_backBuffer->GetDeviceContext(),
				  0,
				  0,
				  SRCCOPY) == FALSE)
		{
			LOG("BackBuffer presentation failed.", LOG_Warning);
			return;
		}
	
		return;
	}
	
	void RenderDevice::SetClipPlanes(float nearPlane, float farPlane)
	{
		m_nearPlane = nearPlane;
		m_farPlane = farPlane;
		m_triClipper->SetViewPlanes(nearPlane, farPlane);
	}

	void RenderDevice::SetFOV(Real FOV)
	{
		m_fov = FOV;

		CalculateFocal(m_backBuffer->GetWidth(), m_backBuffer->GetHeight(), FOV);
	}

	void RenderDevice::CalculateFocal(float width, float height, float FOV)
	{
		m_halfVPH = height * 0.5f;
		m_halfVPW = width * 0.5f;

		float cotFOV = cotan((FOV * 0.5f) * RADIANS_PER_DEGREE);

		float aspect = height / width;

		this->m_focalY = m_halfVPH * cotFOV;
		this->m_focalX = height * aspect;
	}

	void RenderDevice::SetTextureMappingType(TextureMappingTypeSet type)
	{
		m_texMapType = type;

		// Reconfigure the function pointers so that the correct drawing function will be called.
	}
	
	LightingManager* RenderDevice::GetLightingManager()
	{
		return m_lightManager;
	}

	bool RenderDevice::IsBackfacingCC(Vertex* verts)
	{
		if (m_cullingEnabled == false)
			return false;

		static Vector3 vec1, vec2, vec3, viewNormal;
		verts[0].ToVec3(vec1);
		verts[1].ToVec3(vec2);
		verts[2].ToVec3(vec3);

		// As we are in camera space, just use on of the vertex positions for the view normal.
		viewNormal = vec1;
		viewNormal.Normalise();

		Vector3 normal = Vector3::CROSS(vec3 - vec1, vec3 - vec2);
		normal.Normalise();
		if (Vector3::DOT(normal, viewNormal) >= 0)
		{
			trisCulled++;
			return true;
		}

		return false;
	}

	bool RenderDevice::IsBackfacingAC(Vertex* verts)
	{
		if (m_cullingEnabled == false)
			return false;

		Vector3 vec1, vec2, vec3, viewNormal;
		verts[0].ToVec3(vec1);
		verts[1].ToVec3(vec2);
		verts[2].ToVec3(vec3);

		// As we are in camera space, just use on of the vertex positions for the view normal.
		viewNormal = vec1;
		viewNormal.Normalise();

		Vector3 normal = Vector3::CROSS(vec3 - vec2, vec3 - vec1);
		normal.Normalise();
		if (Vector3::DOT(normal, viewNormal) >= 0)
		{
			trisCulled++;
			return true;
		}

		return false;
	}

	void RenderDevice::SetWorldTransform(const Matrix4& m)
	{
		m_world = m;
		Inverse(m_world, m_worldInv);
	}

	void RenderDevice::SetCameraTransform(const Matrix4& m)
	{
		m_cameraMat = m;
		// Extract the camera location.
		m_camLocation.x = m.wX;
		m_camLocation.y = m.wY;
		m_camLocation.z = m.wZ;
		Inverse(m_cameraMat, m_cameraMatInv);
	}

	void RenderDevice::CommitMatrixChanges()
	{
		// Build the transformation matrix that takes a point from world space into camera space.
		m_transform = m_world * m_cameraMatInv;
	}

	void RenderDevice::ToWorldCameraSpace(Vertex* vert)
	{
		static Vector3 point;
		
		// Convert vertices to vectors.
		vert->ToVec3(point);

		// Convert back to points.
		vert->FromVec3(Transform(m_transform, point));
	}

	void RenderDevice::ToWorldSpace(Vertex* vert)
	{
		static Vector3 point;
		static Vector3 normal;
		
		// Convert vertices to vectors.
		vert->ToVec3(point);
		normal = vert->GetNormal();
		normal = TransformNoTranslate(m_world, normal);
		vert->x = normal.x;
		vert->y = normal.y;
		vert->z = normal.z;

		// Convert back to points.
		vert->FromVec3(Transform(m_world, point));
	}

	void RenderDevice::ToCameraSpace(Vertex* vert)
	{
		static Vector3 point;
		
		// Convert vertices to vectors.
		vert->ToVec3(point);

		// Convert back to points.
		vert->FromVec3(Transform(m_cameraMatInv, point));
	}

	void RenderDevice::SetPixelColour(U16 x, U16 y, U32 colour)
	{
		m_backBuffer->PlotPixel(x,y, colour);
	}

	void RenderDevice::EnableBackfaceCulling(bool enable)
	{
		this->m_cullingEnabled = enable;
	}

	bool RenderDevice::IsBackfaceCullingEnabled() const
	{
		return m_cullingEnabled;
	}
	
	void RenderDevice::SetVertexBuffer(VertexBuffer* buffer)
	{
		m_vertexSource = buffer;
	}

	void RenderDevice::SetIndexBuffer(IndexBuffer* buffer)
	{
		m_indexSource = buffer;
	}

	void RenderDevice::SetSourceTexture(Texture* texture)
	{
		m_sourceTexture = texture;
		m_rasterizer->SetTargetTexture(texture);
	}
	
	// *****************************************************************************************
	// Render functionality to render with triangles using only certain properties such as 
	// textures or colouring.
	// Each function is structured so that it wont cause an array overrun.
	// All of these functions are fairly similar and use the same principles. They each have 
	// different calls into the rasterizer however.
	// *****************************************************************************************

	void RenderDevice::DrawTrisColList(bool useIndexBuffer, int totalTris, int start)
	{
		Vertex* tri = m_triangle;
		Vertex* buffer  = m_vertexSource->GetVertices();
		U16 numOfVerts = m_vertexSource->GetTotalVerts();
		if (useIndexBuffer)
		{
			U16* indices = m_indexSource->GetBuffer();
			U16 numOfIndices = m_indexSource->GetTotalIndices();
			unsigned int end = (start + totalTris * 3)- 1;

			tri[0] = buffer[indices[start]];
			tri[1] = buffer[indices[start + 1]];
			tri[2] = buffer[indices[start + 2]];

			for (unsigned int i = start; i < end; i+=3)
			{

				trisSubmittedForDrawing++;
				// Transform.
				ToWorldCameraSpace(&tri[0]);
				ToWorldCameraSpace(&tri[1]);
				ToWorldCameraSpace(&tri[2]);

				if (IsBackfacingCC(tri) == false)
				{

					// Project.
					Project(&tri[0]);
					Project(&tri[1]);
					Project(&tri[2]);

					// Clip the triangle.
					int resultingTris = m_triClipper->ClipTriangle(tri, this->m_clippedVerts);

					if (resultingTris >= 1)
					{
						for (int j = 0; j < resultingTris; j++)
						{
							trisDrawn++;
							m_rasterizer->RasterizeTriSolid(&m_clippedVerts[j * 3]);
						}
					}
				}

				tri[0] = buffer[indices[i]];
				tri[1] = buffer[indices[i + 1]];
				tri[2] = buffer[indices[i + 2]];
			}
			
				trisSubmittedForDrawing++;
				// Transform.
				ToCameraSpace(&tri[0]);
				ToCameraSpace(&tri[1]);
				ToCameraSpace(&tri[2]);

			if (IsBackfacingCC(tri) == false)
			{

				// Project.
				Project(&tri[0]);
				Project(&tri[1]);
				Project(&tri[2]);
				
				// Clip the triangle.
				int resultingTris = m_triClipper->ClipTriangle(tri, this->m_clippedVerts);

				if (resultingTris >= 1)
				{
					for (int j = 0; j < resultingTris; j++)
					{
							trisDrawn++;
						m_rasterizer->RasterizeTriSolid(&m_clippedVerts[j * 3]);
					}
				}
			}
		}
		else
		{
			unsigned int end = (start + totalTris * 3) - 3;
			tri[0] = buffer[start];
			tri[1] = buffer[start + 1];
			tri[2] = buffer[start + 2];
			for (unsigned int i = start; i < end; i+=3)
			{
				trisSubmittedForDrawing++;

				// Transform.
				ToWorldCameraSpace(&tri[0]);
				ToWorldCameraSpace(&tri[1]);
				ToWorldCameraSpace(&tri[2]);
				
				if (IsBackfacingCC(tri) == false)
				{
					// Project.
					Project(&tri[0]);
					Project(&tri[1]);
					Project(&tri[2]);
					
					// Clip the triangle.
					int resultingTris = m_triClipper->ClipTriangle(tri, this->m_clippedVerts);

					if (resultingTris >= 1)
					{
						for (int j = 0; j < resultingTris; j++)
						{
							trisDrawn++;
							m_rasterizer->RasterizeTriSolid(&m_clippedVerts[j * 3]);
						}
					}
				}

				tri[0] = buffer[i];
				tri[1] = buffer[i + 1];
				tri[2] = buffer[i + 2];
			}
			
				trisSubmittedForDrawing++;
			// Transform.
			ToWorldCameraSpace(&tri[0]);
			ToWorldCameraSpace(&tri[1]);
			ToWorldCameraSpace(&tri[2]);
				
			if (IsBackfacingCC(tri) == false)
			{
				// Project.
				Project(&tri[0]);
				Project(&tri[1]);
				Project(&tri[2]);
				
				// Clip the triangle.
				int resultingTris = m_triClipper->ClipTriangle(tri, this->m_clippedVerts);

				if (resultingTris >= 1)
				{
					for (int j = 0; j < resultingTris; j++)
					{
							trisDrawn++;
						m_rasterizer->RasterizeTriSolid(&m_clippedVerts[j * 3]);
					}
				}
			}
		}
	}

	void RenderDevice::DrawTrisColStrip(bool useIndexBuffer, int totalTris, int start)
	{
		Vertex* tri = m_triangle;
		Vertex* buffer  = m_vertexSource->GetVertices();
		U16 numOfVerts = m_vertexSource->GetTotalVerts();
		if (useIndexBuffer)
		{
			U16* indices = m_indexSource->GetBuffer();
			U16 numOfIndices = m_indexSource->GetTotalIndices();

			tri[0] = buffer[indices[start]];
			tri[1] = buffer[indices[start + 1]];
			tri[2] = buffer[indices[start + 2]];
			
			// Transform.
			ToWorldCameraSpace(&tri[0]);
			ToWorldCameraSpace(&tri[1]);
			ToWorldCameraSpace(&tri[2]);

			// Project.
			Project(&tri[0]);
			Project(&tri[1]);
			Project(&tri[2]);

			// The next index to be changed in the triangle.
			int nextIndexToSwap = 0;
			int nextIndex = 0;
			
			unsigned int end = (start + totalTris) - 1;
			unsigned int i = start;
			for (i; i < end; i++)
			{
				trisSubmittedForDrawing++;
							trisDrawn++;
				nextIndex = nextIndexToSwap % 3;
				// Draw triangle.
				m_rasterizer->RasterizeTriSolid(tri);
				// Swap the next index in the triangle.
				tri[nextIndex] = buffer[indices[i + 3]];

				// Convert vertex to screen space.
				ToWorldCameraSpace(&tri[nextIndex]);
				Project(&tri[nextIndex]);

				nextIndexToSwap++;
			}

			// Render the last tri outside of the loop so we dont over-run the buffers.
			m_rasterizer->RasterizeTriSolid(tri);
		}
		else
		{
			tri[0] = buffer[start];
			tri[1] = buffer[start + 1];
			tri[2] = buffer[start + 2];
			
			// Transform.
			ToWorldCameraSpace(&tri[0]);
			ToWorldCameraSpace(&tri[1]);
			ToWorldCameraSpace(&tri[2]);

			// Project.
			Project(&tri[0]);
			Project(&tri[1]);
			Project(&tri[2]);

			// The next index to be changed in the triangle.
			int nextIndexToSwap = 0;
			int nextIndex = 0;
			
			unsigned int end = (start + totalTris) - 1;
			unsigned int i = start;
			for (i; i < end; i++)
			{
				trisSubmittedForDrawing++;
							trisDrawn++;
				nextIndex = nextIndexToSwap % 3;
				// Draw triangle.
				m_rasterizer->RasterizeTriSolid(tri);
				// Swap the next index in the triangle.
				tri[nextIndex] = buffer[i + 3];

				// Convert vertex to screen space.
				ToWorldCameraSpace(&tri[nextIndex]);
				Project(&tri[nextIndex]);

				nextIndexToSwap++;
			}
			
			// Render the last tri outside of the loop so we dont over-run the buffers.
			m_rasterizer->RasterizeTriSolid(tri);
		}
	}
		
	void RenderDevice::DrawTrisColLitList(bool useIndexBuffer, int totalTris, int start)
	{
		Vertex* tri = m_triangle;
		Vertex* buffer  = m_vertexSource->GetVertices();
		U16 numOfVerts = m_vertexSource->GetTotalVerts();
		if (useIndexBuffer)
		{
			U16* indices = m_indexSource->GetBuffer();
			U16 numOfIndices = m_indexSource->GetTotalIndices();
			unsigned int end = (start + totalTris * 3)- 1;

			tri[0] = buffer[indices[start]];
			tri[1] = buffer[indices[start + 1]];
			tri[2] = buffer[indices[start + 2]];

			for (unsigned int i = start; i < end; i+=3)
			{
				trisSubmittedForDrawing++;

				// Transform.
				ToWorldSpace(&tri[0]);
				ToWorldSpace(&tri[1]);
				ToWorldSpace(&tri[2]);

				// Apply gourad lighting
				m_lightManager->ProcessVertex(&tri[0], this->m_world,  LRO_UseAll);
				m_lightManager->ProcessVertex(&tri[1], this->m_world,  LRO_UseAll);
				m_lightManager->ProcessVertex(&tri[2], this->m_world,  LRO_UseAll);
				
				ToCameraSpace(&tri[0]);
				ToCameraSpace(&tri[1]);
				ToCameraSpace(&tri[2]);



				if (IsBackfacingCC(tri) == false)
				{

					// Project.
					Project(&tri[0]);
					Project(&tri[1]);
					Project(&tri[2]);

					// Clip the triangle.
					int resultingTris = m_triClipper->ClipTriangle(tri, this->m_clippedVerts);

					if (resultingTris >= 1)
					{
						for (int j = 0; j < resultingTris; j++)
						{
							trisDrawn++;
							m_rasterizer->RasterizeTriSolid(&m_clippedVerts[j * 3]);
						}
					}
				}

				tri[0] = buffer[indices[i]];
				tri[1] = buffer[indices[i + 1]];
				tri[2] = buffer[indices[i + 2]];
			}
			
				trisSubmittedForDrawing++;
				// Transform.
			ToWorldSpace(&tri[0]);
			ToWorldSpace(&tri[1]);
			ToWorldSpace(&tri[2]);
			
			// Apply gourad lighting
			m_lightManager->ProcessVertex(&tri[0], this->m_world,  LRO_UseAll);
			m_lightManager->ProcessVertex(&tri[1], this->m_world,  LRO_UseAll);
			m_lightManager->ProcessVertex(&tri[2], this->m_world,  LRO_UseAll);
				
			ToCameraSpace(&tri[0]);
			ToCameraSpace(&tri[1]);
			ToCameraSpace(&tri[2]);

			if (IsBackfacingCC(tri) == false)
			{

				// Project.
				Project(&tri[0]);
				Project(&tri[1]);
				Project(&tri[2]);
				
				// Clip the triangle.
				int resultingTris = m_triClipper->ClipTriangle(tri, this->m_clippedVerts);

				if (resultingTris >= 1)
				{
					for (int j = 0; j < resultingTris; j++)
					{
							trisDrawn++;
						m_rasterizer->RasterizeTriSolid(&m_clippedVerts[j * 3]);
					}
				}
			}
		}
		else
		{
			unsigned int end = (start + totalTris * 3) - 3;
			tri[0] = buffer[start];
			tri[1] = buffer[start + 1];
			tri[2] = buffer[start + 2];
			for (unsigned int i = start; i < end; i+=3)
			{
				trisSubmittedForDrawing++;

				// Transform.
				ToWorldSpace(&tri[0]);
				ToWorldSpace(&tri[1]);
				ToWorldSpace(&tri[2]);
				
				// Apply gourad lighting
				m_lightManager->ProcessVertex(&tri[0], this->m_world,  LRO_UseAll);
				m_lightManager->ProcessVertex(&tri[1], this->m_world,  LRO_UseAll);
				m_lightManager->ProcessVertex(&tri[2], this->m_world,  LRO_UseAll);
				
				ToCameraSpace(&tri[0]);
				ToCameraSpace(&tri[1]);
				ToCameraSpace(&tri[2]);
				
				if (IsBackfacingCC(tri) == false)
				{
					// Project.
					Project(&tri[0]);
					Project(&tri[1]);
					Project(&tri[2]);
					
					// Clip the triangle.
					int resultingTris = m_triClipper->ClipTriangle(tri, this->m_clippedVerts);

					if (resultingTris >= 1)
					{
						for (int j = 0; j < resultingTris; j++)
						{
							trisDrawn++;
							m_rasterizer->RasterizeTriSolid(&m_clippedVerts[j * 3]);
						}
					}
				}

				tri[0] = buffer[i];
				tri[1] = buffer[i + 1];
				tri[2] = buffer[i + 2];
			}
			
				trisSubmittedForDrawing++;
			// Transform.
			ToWorldSpace(&tri[0]);
			ToWorldSpace(&tri[1]);
			ToWorldSpace(&tri[2]);
			
			// Apply gourad lighting
			m_lightManager->ProcessVertex(&tri[0], this->m_world,  LRO_UseAll);
			m_lightManager->ProcessVertex(&tri[1], this->m_world,  LRO_UseAll);
			m_lightManager->ProcessVertex(&tri[2], this->m_world,  LRO_UseAll);
				
			ToCameraSpace(&tri[0]);
			ToCameraSpace(&tri[1]);
			ToCameraSpace(&tri[2]);
				
			if (IsBackfacingCC(tri) == false)
			{
				// Project.
				Project(&tri[0]);
				Project(&tri[1]);
				Project(&tri[2]);
				
				// Clip the triangle.
				int resultingTris = m_triClipper->ClipTriangle(tri, this->m_clippedVerts);

				if (resultingTris >= 1)
				{
					for (int j = 0; j < resultingTris; j++)
					{
							trisDrawn++;
						m_rasterizer->RasterizeTriSolid(&m_clippedVerts[j * 3]);
					}
				}
			}
		}
	}

	void RenderDevice::DrawTrisColLitStrip(bool useIndexBuffer, int totalTris, int start)
	{
		Vertex* tri = m_triangle;
		Vertex* buffer  = m_vertexSource->GetVertices();
		U16 numOfVerts = m_vertexSource->GetTotalVerts();
	}
		
	void RenderDevice::DrawTrisTexList(bool useIndexBuffer, int totalTris, int start)
	{
		Vertex* tri = m_triangle;
		Vertex* buffer  = m_vertexSource->GetVertices();
		U16 numOfVerts = m_vertexSource->GetTotalVerts();
		if (useIndexBuffer)
		{
			U16* indices = m_indexSource->GetBuffer();
			U16 numOfIndices = m_indexSource->GetTotalIndices();
			unsigned int end = (start + totalTris * 3) - 1;

			tri[0] = buffer[indices[start]];
			tri[1] = buffer[indices[start + 1]];
			tri[2] = buffer[indices[start + 2]];

			for (unsigned int i = start; i < end; i+=3)
			{
				trisSubmittedForDrawing++;

				// Transform.
				ToWorldCameraSpace(&tri[0]);
				ToWorldCameraSpace(&tri[1]);
				ToWorldCameraSpace(&tri[2]);

				if (IsBackfacingCC(tri) == false)
				{

					// Project.
					Project(&tri[0]);
					Project(&tri[1]);
					Project(&tri[2]);
					
					// Clip the triangle.
					int resultingTris = m_triClipper->ClipTriangle(tri, this->m_clippedVerts);

					if (resultingTris >= 1)
					{
						for (int j = 0; j < resultingTris; j++)
						{
							trisDrawn++;
							m_rasterizer->RasterizeTriTex(&m_clippedVerts[j * 3]);
						}
					}
				}

				tri[0] = buffer[indices[i]];
				tri[1] = buffer[indices[i + 1]];
				tri[2] = buffer[indices[i + 2]];
			}
			
				trisSubmittedForDrawing++;
				// Transform.
				ToWorldCameraSpace(&tri[0]);
				ToWorldCameraSpace(&tri[1]);
				ToWorldCameraSpace(&tri[2]);

			if (IsBackfacingCC(tri) == false)
			{

				// Project.
				Project(&tri[0]);
				Project(&tri[1]);
				Project(&tri[2]);
				
				// Clip the triangle.
				int resultingTris = m_triClipper->ClipTriangle(tri, this->m_clippedVerts);

				if (resultingTris >= 1)
				{
					for (int j = 0; j < resultingTris; j++)
					{
							trisDrawn++;
						m_rasterizer->RasterizeTriTex(&m_clippedVerts[j * 3]);
					}
				}
			}
		}
		else
		{
			unsigned int end = (start + totalTris * 3) - 3;
			tri[0] = buffer[start];
			tri[1] = buffer[start + 1];
			tri[2] = buffer[start + 2];
			for (unsigned int i = start; i < end; i+=3)
			{
				trisSubmittedForDrawing++;

				// Transform.
				ToWorldCameraSpace(&tri[0]);
				ToWorldCameraSpace(&tri[1]);
				ToWorldCameraSpace(&tri[2]);
				
				if (IsBackfacingCC(tri) == false)
				{
					// Project.
					Project(&tri[0]);
					Project(&tri[1]);
					Project(&tri[2]);
					
					// Clip the triangle.
					int resultingTris = m_triClipper->ClipTriangle(tri, this->m_clippedVerts);

					if (resultingTris >= 1)
					{
						for (int j = 0; j < resultingTris; j++)
						{
							trisDrawn++;
							m_rasterizer->RasterizeTriTex(&m_clippedVerts[j * 3]);
						}
					}
				}

				tri[0] = buffer[i];
				tri[1] = buffer[i + 1];
				tri[2] = buffer[i + 2];
			}
			
				trisSubmittedForDrawing++;
			// Transform.
			ToWorldCameraSpace(&tri[0]);
			ToWorldCameraSpace(&tri[1]);
			ToWorldCameraSpace(&tri[2]);
				
			if (IsBackfacingCC(tri) == false)
			{
				// Project.
				Project(&tri[0]);
				Project(&tri[1]);
				Project(&tri[2]);
				
				// Clip the triangle.
				int resultingTris = m_triClipper->ClipTriangle(tri, this->m_clippedVerts);

				if (resultingTris >= 1)
				{
					for (int j = 0; j < resultingTris; j++)
					{
							trisDrawn++;
						m_rasterizer->RasterizeTriTex(&m_clippedVerts[j * 3]);
					}
				}
			}
		}
	}

	void RenderDevice::DrawTrisTexStrip(bool useIndexBuffer, int totalTris, int start)
	{
		Vertex* tri = m_triangle;
		Vertex* buffer  = m_vertexSource->GetVertices();
		U16 numOfVerts = m_vertexSource->GetTotalVerts();	
		if (useIndexBuffer)
		{
			U16* indices = m_indexSource->GetBuffer();
			U16 numOfIndices = m_indexSource->GetTotalIndices();

			tri[0] = buffer[indices[start]];
			tri[1] = buffer[indices[start + 1]];
			tri[2] = buffer[indices[start + 2]];
			
			// Transform.
			ToWorldCameraSpace(&tri[0]);
			ToWorldCameraSpace(&tri[1]);
			ToWorldCameraSpace(&tri[2]);

			// Project.
			Project(&tri[0]);
			Project(&tri[1]);
			Project(&tri[2]);

			// The next index to be changed in the triangle.
			int nextIndexToSwap = 0;
			int nextIndex = 0;
			
			unsigned int end = (start + totalTris) - 1;
			unsigned int i = start;
			for (i; i < end; i++)
			{
							trisDrawn++;
				trisSubmittedForDrawing++;
				nextIndex = nextIndexToSwap % 3;
				// Draw triangle.
				m_rasterizer->RasterizeTriTex(tri);
				// Swap the next index in the triangle.
				tri[nextIndex] = buffer[indices[i + 3]];

				// Convert vertex to screen space.
				ToWorldCameraSpace(&tri[nextIndex]);
				Project(&tri[nextIndex]);

				nextIndexToSwap++;
			}

			// Render the last tri outside of the loop so we dont over-run the buffers.
			m_rasterizer->RasterizeTriTex(tri);
		}
		else
		{
			tri[0] = buffer[start];
			tri[1] = buffer[start + 1];
			tri[2] = buffer[start + 2];
			
			// Transform.
			ToWorldCameraSpace(&tri[0]);
			ToWorldCameraSpace(&tri[1]);
			ToWorldCameraSpace(&tri[2]);

			// Project.
			Project(&tri[0]);
			Project(&tri[1]);
			Project(&tri[2]);

			// The next index to be changed in the triangle.
			int nextIndexToSwap = 0;
			int nextIndex = 0;
			
			unsigned int end = (start + totalTris) - 1;
			unsigned int i = start;
			for (i; i < end; i++)
			{
							trisDrawn++;
				trisSubmittedForDrawing++;
				nextIndex = nextIndexToSwap % 3;
				// Draw triangle.
				m_rasterizer->RasterizeTriTex(tri);
				// Swap the next index in the triangle.
				tri[nextIndex] = buffer[i + 3];

				// Convert vertex to screen space.
				ToCameraSpace(&tri[nextIndex]);
				Project(&tri[nextIndex]);

				nextIndexToSwap++;
			}
			
			// Render the last tri outside of the loop so we dont over-run the buffers.
			m_rasterizer->RasterizeTriTex(tri);
		}
	}
	
	// Renders a 2D triangle.
	void RenderDevice::DrawTrisTex2D(bool useIndexBuffer, int totalTris, int start)
	{
		Vertex* tri = m_triangle;
		Vertex* buffer  = m_vertexSource->GetVertices();
		U16 numOfVerts = m_vertexSource->GetTotalVerts();
		if (useIndexBuffer)
		{
			U16* indices = m_indexSource->GetBuffer();
			U16 numOfIndices = m_indexSource->GetTotalIndices();
			unsigned int end = (start + totalTris * 3);
			for (unsigned int i = start; i < end; i+=3)
			{
				tri[0] = buffer[indices[i]];
				tri[1] = buffer[indices[i + 1]];
				tri[2] = buffer[indices[i + 2]];

				m_rasterizer->RasterizeTriTex(tri);
			}
		}
		else
		{
			unsigned int end = (start + totalTris * 3);
			for (unsigned int i = start; i < end; i+=3)
			{
				tri[0] = buffer[i];
				tri[1] = buffer[i + 1];
				tri[2] = buffer[i + 2];
				
				m_rasterizer->RasterizeTriTex(tri);
			}
		}
	}
		
	void RenderDevice::DrawTrisTexLitList(bool useIndexBuffer, int totalTris, int start)
	{
		Vertex* tri = m_triangle;
		Vertex* buffer  = m_vertexSource->GetVertices();
		U16 numOfVerts = m_vertexSource->GetTotalVerts();
		if (useIndexBuffer)
		{
			U16* indices = m_indexSource->GetBuffer();
			U16 numOfIndices = m_indexSource->GetTotalIndices();
		}
		else
		{
		}
	}

	void RenderDevice::DrawTrisTexLitStrip(bool useIndexBuffer, int totalTris, int start)
	{
		Vertex* tri = m_triangle;
		Vertex* buffer  = m_vertexSource->GetVertices();
		U16 numOfVerts = m_vertexSource->GetTotalVerts();
		if (useIndexBuffer)
		{
			U16* indices = m_indexSource->GetBuffer();
			U16 numOfIndices = m_indexSource->GetTotalIndices();
		}
		else
		{
		}
	}

	void RenderDevice::DrawTexture2D(U16 x, U16 y, Texture* texture)
	{
		// Firstly get the byte buffers.
		U8* texels = texture->GetBytes();
		U8* backbuffer = m_backBuffer->GetByteBuffer();
		U8* bytes = NULL;

		// Get the textures width and height.
		U16 texWidth = texture->GetWidth();
		U16 texHeight = texture->GetHeight();

		// Calculate the scanline size in bytes.
		size_t lineSpan = texWidth << 2;

		U32 byteIndex = 0;
		U32 texelIndex = 0;

		for (U32 row = 0; row < texHeight; row++)
		{
			byteIndex = (x + (y * this->m_backBuffer->GetWidth())) << 2;
			texelIndex = (row * texWidth) << 2;

			memcpy(&(backbuffer[byteIndex]), &(texels[texelIndex]), lineSpan);
			y++;
		}
	}

	void RenderDevice::DrawTexture2D(U16 x, U16 y, Texture* texture, Rectangle* srcRect)
	{
		// Firstly get the byte buffers.
		U8* texels = texture->GetBytes();
		U8* backbuffer = m_backBuffer->GetByteBuffer();
		U8* bytes = NULL;

		// Get the textures width and height.
		U16 texWidth = texture->GetWidth();
		U16 texHeight = texture->GetHeight();

		// Calculate the scanline size in bytes.
		size_t lineSpan = (srcRect->right - srcRect->left) << 2;

		U32 byteIndex = 0;
		U32 texelIndex = 0;

		for (U32 row = srcRect->top; row < srcRect->bottom; row++)
		{
			byteIndex = (x + (y * this->m_backBuffer->GetWidth())) << 2;
			texelIndex = ((row * texWidth) + srcRect->left) << 2;

			memcpy(&(backbuffer[byteIndex]), &(texels[texelIndex]), lineSpan);
			y++;
		}
	}

	void RenderDevice::DrawTexture2D(U16 x, U16 y, Texture* texture, Rectangle* srcRect, U32 alphaFilter)
	{
		// Firstly get the byte buffers.
		U32* texels = (U32*)texture->GetBytes();
		U32* backbuffer = (U32*)m_backBuffer->GetByteBuffer();

		// Get the textures width and height.
		U16 texWidth = texture->GetWidth();
		U16 texHeight = texture->GetHeight();

		U32 texColour =  0;

		U32 bufferIndex = 0;
		U32 texelIndex = 0;

		for (U32 row = srcRect->top; row < srcRect->bottom; row++)
		{
			for (U32 col = srcRect->left; col < srcRect->right; col++)
			{
				texelIndex = ((row * texWidth) + col);
				bufferIndex = (x + (col - srcRect->left) + (y * this->m_backBuffer->GetWidth()));
				texColour = (U32)texels[texelIndex];

				if (texColour != alphaFilter)
				{
					backbuffer[bufferIndex] = texColour;
				}

			}

			y++;
		}
	}


	void RenderDevice::DrawNormals(VertexBuffer* buffer, Colour32 colour, float normalLength)
	{
		Vector3 start;
		Vector3 end;
		Vertex* verts = buffer->GetVertices();

		for (int i = 0; i < buffer->GetTotalVerts(); i++)
		{
			 verts[i].ToVec3(start);
			 end = start + (verts[i].GetNormal() * normalLength);

			 start = Transform(m_transform, start);
			 end = Transform(m_transform, end);
			 Project(start);
			 Project(end);

			 m_rasterizer->PlotLineCol(ceil(start.x), ceil(end.x), ceil(start.y), ceil(end.y), start.z, end.z, colour, colour);
		}
	}

	void RenderDevice::DrawWireFrame(bool useIndexBuffer, int totalTris, int start, Colour32 colour)
	{
		Vertex* tri = m_triangle;
		Vertex* buffer  = m_vertexSource->GetVertices();
		U16 numOfVerts = m_vertexSource->GetTotalVerts();
		if (useIndexBuffer)
		{
			U16* indices = m_indexSource->GetBuffer();
			U16 numOfIndices = m_indexSource->GetTotalIndices();
			unsigned int end = (start + totalTris * 3)- 1;

			tri[0] = buffer[indices[start]];
			tri[1] = buffer[indices[start + 1]];
			tri[2] = buffer[indices[start + 2]];
			tri[0].colour = colour;
			tri[1].colour = colour;
			tri[2].colour = colour;

			for (unsigned int i = start; i < end; i+=3)
			{

				// Transform.
				ToWorldCameraSpace(&tri[0]);
				ToWorldCameraSpace(&tri[1]);
				ToWorldCameraSpace(&tri[2]);


				// Project.
				Project(&tri[0]);
				Project(&tri[1]);
				Project(&tri[2]);

				// Clip the triangle.
				int resultingTris = m_triClipper->ClipTriangle(tri, this->m_clippedVerts);

				if (resultingTris >= 1)
				{
					for (int j = 0; j < resultingTris * 3; j+=3)
					{
						m_rasterizer->RasterizeTriEdges(m_clippedVerts[j], m_clippedVerts[j + 1], m_clippedVerts[j + 2]);
					}
				}

				tri[0] = buffer[indices[i]];
				tri[1] = buffer[indices[i + 1]];
				tri[2] = buffer[indices[i + 2]];
				tri[0].colour = colour;
				tri[1].colour = colour;
				tri[2].colour = colour;
			}

			// Transform.
			ToCameraSpace(&tri[0]);
			ToCameraSpace(&tri[1]);
			ToCameraSpace(&tri[2]);
			// Project.
			Project(&tri[0]);
			Project(&tri[1]);
			Project(&tri[2]);
				
			// Clip the triangle.
			int resultingTris = m_triClipper->ClipTriangle(tri, this->m_clippedVerts);

			if (resultingTris >= 1)
			{
				for (int j = 0; j < resultingTris * 3; j+=3)
				{
					m_rasterizer->RasterizeTriEdges(m_clippedVerts[j], m_clippedVerts[j + 1], m_clippedVerts[j + 2]);
				}
			}
		}
		else
		{
			unsigned int end = (start + totalTris * 3) - 3;
			tri[0] = buffer[start];
			tri[1] = buffer[start + 1];
			tri[2] = buffer[start + 2];
			tri[0].colour = colour;
			tri[1].colour = colour;
			tri[2].colour = colour;
			for (unsigned int i = start; i < end; i+=3)
			{

				// Transform.
				ToWorldCameraSpace(&tri[0]);
				ToWorldCameraSpace(&tri[1]);
				ToWorldCameraSpace(&tri[2]);
				
				// Project.
				Project(&tri[0]);
				Project(&tri[1]);
				Project(&tri[2]);
					
				// Clip the triangle.
				int resultingTris = m_triClipper->ClipTriangle(tri, this->m_clippedVerts);

				if (resultingTris >= 1)
				{
					for (int j = 0; j < resultingTris * 3; j+=3)
					{
						m_rasterizer->RasterizeTriEdges(m_clippedVerts[j], m_clippedVerts[j + 1], m_clippedVerts[j + 2]);
					}
				}

				tri[0] = buffer[i];
				tri[1] = buffer[i + 1];
				tri[2] = buffer[i + 2];
				tri[0].colour = colour;
				tri[1].colour = colour;
				tri[2].colour = colour;
			}

			// Transform.
			ToWorldCameraSpace(&tri[0]);
			ToWorldCameraSpace(&tri[1]);
			ToWorldCameraSpace(&tri[2]);
				
			// Project.
			Project(&tri[0]);
			Project(&tri[1]);
			Project(&tri[2]);
				
			// Clip the triangle.
			int resultingTris = m_triClipper->ClipTriangle(tri, this->m_clippedVerts);

			if (resultingTris >= 1)
			{
				for (int j = 0; j < resultingTris * 3; j+=3)
				{
					m_rasterizer->RasterizeTriEdges(m_clippedVerts[j], m_clippedVerts[j + 1], m_clippedVerts[j + 2]);
				}
			}
		}
	}

	int RenderDevice::GetTrisCulled() const
	{
		return trisCulled;
	}

	int RenderDevice::GetTrisRendered() const
	{
		return this->trisDrawn;
	}

	int RenderDevice::GetTrisSubmittedForRender() const
	{
		return this->trisSubmittedForDrawing;
	}

	void RenderDevice::ResetStatsCounters()
	{
		trisDrawn = 0;
		trisCulled = 0;
		trisSubmittedForDrawing = 0;
	}


}; // End namespace SWR.