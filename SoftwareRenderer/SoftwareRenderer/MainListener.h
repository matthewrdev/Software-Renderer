#pragma once

#ifndef MAIN_LISTENER_H
#define MAIN_LISTENER_H

//****************************************************************************
//**
//**    MainListener.h
//**
//**    Copyright (c) 2010 Matthew Robbins
//**
//**    Author:  Matthew Robbins
//**    Created: 06/2010
//**
//****************************************************************************

#include "FrameListener.h"

#include "Application.h"
#include "RenderDevice.h"
#include "InputHandler.h"
#include "SWR_Math.h"
#include "TextureManager.h"
#include "Texture.h"
#include "Logger.h"
#include "LightingManager.h"

#include "Font.h"

#include "Vertex.h"
#include "IndexBuffer.h"

#include "Vector2.h"
#include "Matrix4.h"

namespace SWR
{
	// ------------------------------------------------------------------------
	//								MainListener
	// ------------------------------------------------------------------------
	// Desc:
	// The core listener for the application.
	// ------------------------------------------------------------------------
	class MainListener : public FrameListener
	{
	private:
		// Cheaty pointer to the render device...
		RenderDevice* device;

		// Cheaty pointer to lighting manager.
		LightingManager* lighting;
		int pointLightHandle;
		int directionalLightHandle;

		enum ViewMode
		{
			BlazeModel,
			Crate,
			LitCrate,

			ViewModeCount,
		};

		int mode;

		// Cube constructs.
		VertexBuffer* cubeVerts;
		IndexBuffer* cubeIndices;
		Texture* cubeTexture;
		Texture* testTexture;

		VertexBuffer* lightIconVerts;
		IndexBuffer* lightIconIndices;
		Texture* lightIconTexture;

		float lightOffset;
		Vector3 lightStartPos;
		float lightRotTheta;
		
		// 3D model constructs.
		VertexBuffer* blazeModelVerts;
		IndexBuffer* blazeModelIndices;
		Texture* blazeTextureMap;

		Matrix4 modelTransform;
		Matrix4 lightTransform;

		// Font we will use to render text with.
		Font* arialFont;

		// Default field of view for the scene.
		float FOV;

		int trisCulled;
		int trisDrawn;
		int trisSubmitted;
	protected:
	public:
		MainListener()
		{
			device = NULL;
			cubeVerts = NULL;
			cubeIndices = NULL;
			cubeTexture = NULL;
			blazeModelVerts = NULL;
			blazeModelIndices = NULL;
			blazeTextureMap = NULL;
			arialFont = NULL;
			mode = BlazeModel;
			trisCulled = 0;
			trisDrawn = 0;
			trisSubmitted = 0;
		}

		~MainListener(){}

		void Initilise()
		{
			LOG("Main listener initilised.", LOG_Init);
			device = ENGINE.GetRenderDevice();
			lighting = device->GetLightingManager();

			// Build our point light.
			Light light;
			light.colour.FromColour32(Colour32::CYAN);
			light.type = LIGHT_Point;
			light.position = Vector3(0.0f, 2.0f, 3.0f);
			light.falloff = 50.0f;
			light.atten[0] = 0.0f;
			light.atten[1] = 0.125f;
			light.atten[2] = 0.0f;

			lightStartPos.Set(0.0f, 2.0f, 6.0f);
			lightOffset = 3.0f;
			lightRotTheta = 0.0f;

			pointLightHandle = lighting->FindNextLightHandle();
			lighting->AddLight(light, pointLightHandle);

			// Build our directional light.
			light.colour.FromColour32(Colour32::WHITE);
			light.type = LIGHT_Directional;
			light.direction = Vector3(0.577f, -0.577f, -0.577f);
			//directionalLightHandle = lighting->FindNextLightHandle();
			//lighting->AddLight(light, directionalLightHandle);

			lighting->EnableLight(pointLightHandle);

			arialFont = new Font("Resources/Arial24.bmp", 16, 16, 256, 256);
			SWR_ERR result = TextureManager::Instance().LoadTexture("Resources/Crate24.bmp", cubeTexture, 256, 256, true);
			result = TextureManager::Instance().LoadTexture("Resources/SampleTex24.bmp", testTexture, 128, 128, true);

			// Load the light icon texture.
			result = TextureManager::Instance().LoadTexture("Resources/LightIcon.bmp", this->lightIconTexture, 64, 64, true);

			GenerateCube();
			LoadTestModel();
			GenerateLightIcon();

			SetupRenderMode();

			device->SetFOV(45.0f);

			Matrix4 identity;
			identity.Identity();

			device->SetCameraTransform(identity);
			device->SetWorldTransform(identity);
			device->CommitMatrixChanges();

			device->EnableBackfaceCulling(true);

			FOV = 70.0f;
			device->SetFOV(FOV);
			device->SetClipPlanes(1.0f, 100.0f);
		}

		void SetupRenderMode()
		{
			switch (mode)
			{
			case BlazeModel:
				device->SetVertexBuffer(this->blazeModelVerts);
				device->SetIndexBuffer(blazeModelIndices);
				device->SetSourceTexture(blazeTextureMap);
				break;
			case Crate:
			case LitCrate:
				device->SetVertexBuffer(cubeVerts);
				device->SetIndexBuffer(cubeIndices);
				device->SetSourceTexture(cubeTexture);
				break;
			};
		}

		void GenerateLightIcon()
		{
			// Create our vertex and index buffers.
			Vertex verts[] = 
			{
				Vertex( -0.05f, -0.05, 0.05, Colour32::WHITE,     0.0f, 0.0f,  0.0f, 0.0f, 1.0f),    // side 1
				Vertex( 0.05, -0.05, 0.05,  Colour32::WHITE,     1.0f, 0.0f,  0.0f, 0.0f, 1.0f),
				Vertex( -0.05, 0.05, 0.05,  Colour32::WHITE,     0.0f, 1.0f,  0.0f, 0.0f, 1.0f),
				Vertex( 0.05, 0.05, 0.05,   Colour32::WHITE,     1.0f, 1.0f,  0.0f, 0.0f, 1.0f),

				Vertex( -0.05, -0.05, -0.05,Colour32::WHITE,     0.0f, 0.0f,  0.0f, 0.0f, -1.0f),    // side 2
				Vertex( -0.05, 0.05, -0.05, Colour32::WHITE,     1.0f, 0.0f,  0.0f, 0.0f, -1.0f),
				Vertex( 0.05, -0.05, -0.05, Colour32::WHITE,     0.0f, 1.0f,  0.0f, 0.0f, -1.0f),
				Vertex( 0.05, 0.05, -0.05,  Colour32::WHITE,     1.0f, 1.0f, 0.0f, 0.0f, -1.0f),

				Vertex( -0.05, 0.05, -0.05, Colour32::WHITE,     0.0f, 0.0f,  0.0f, 1.0f, 0.0f),    // side 3
				Vertex( -0.05, 0.05, 0.05,  Colour32::WHITE,     1.0f, 0.0f,  0.0f, 1.0f, 0.0f),
				Vertex( 0.05, 0.05, -0.05,  Colour32::WHITE,     0.0f, 1.0f,  0.0f, 1.0f, 0.0f),
				Vertex( 0.05, 0.05, 0.05,   Colour32::WHITE,     1.0f, 1.0f,  0.0f, 1.0f, 0.0f),

				Vertex( -0.05, -0.05, -0.05, Colour32::WHITE,     0.0f, 0.0f,  0.0f, -1.0f, 0.0f),    // side 4
				Vertex( 0.05, -0.05, -0.05,  Colour32::WHITE,     1.0f, 0.0f,  0.0f, -1.0f, 0.0f),
				Vertex( -0.05, -0.05, 0.05,  Colour32::WHITE,     0.0f, 1.0f,  0.0f, -1.0f, 0.0f),
				Vertex( 0.05, -0.05, 0.05,   Colour32::WHITE,     1.0f, 1.0f,  0.0f, -1.0f, 0.0f),

				Vertex( 0.05, -0.05, -0.05,  Colour32::WHITE,     0.0f, 0.0f,  1.0f, 0.0f, 0.0f),    // side 5
				Vertex( 0.05, 0.05, -0.05,   Colour32::WHITE,     1.0f, 0.0f,  1.0f, 0.0f, 0.0f),
				Vertex( 0.05, -0.05, 0.05,   Colour32::WHITE,     0.0f, 1.0f,  1.0f, 0.0f, 0.0f),
				Vertex( 0.05, 0.05, 0.05,    Colour32::WHITE,     1.0f, 1.0f,  1.0f, 0.0f, 0.0f),

				Vertex( -0.05, -0.05, -0.05, Colour32::WHITE,     0.0f, 0.0f,  -1.0f, 0.0f, 0.0f),    // side 6
				Vertex( -0.05, -0.05, 0.05,  Colour32::WHITE,     1.0f, 0.0f,  -1.0f, 0.0f, 0.0f),
				Vertex( -0.05, 0.05, -0.05,  Colour32::WHITE,     0.0f, 1.0f,  -1.0f, 0.0f, 0.0f),
				Vertex( -0.05, 0.05, 0.05,   Colour32::WHITE,     1.0f, 1.0f,  -1.0f, 0.0f, 0.0f),
			};

			CreateVertexBuffer(&verts, 24, lightIconVerts);
		
			U16 indices[] = 
			{
				0, 1, 2,    // side 1
				2, 1, 3,
				4, 5, 6,    // side 2
				6, 5, 7,
				8, 9, 10,    // side 3
				10, 9, 11,
				12, 13, 14,    // side 4
				14, 13, 15,
				16, 17, 18,    // side 5
				18, 17, 19,
				20, 21, 22,    // side 6
				22, 21, 23,
			};

			CreateIndexBuffer(indices, 36, lightIconIndices);
		}

		void GenerateCube()
		{
			// Create our vertex and index buffers.
			Vertex verts[] = 
			{
				Vertex( -0.5f, -0.5f, 0.5f, Colour32::WHITE,     0.0f, 0.0f,  0.0f, 0.0f, 1.0f),    // side 1
				Vertex( 0.5f, -0.5f, 0.5f,  Colour32::WHITE,     1.0f, 0.0f,  0.0f, 0.0f, 1.0f),
				Vertex( -0.5f, 0.5f, 0.5f,  Colour32::WHITE,     0.0f, 1.0f,  0.0f, 0.0f, 1.0f),
				Vertex( 0.5f, 0.5f, 0.5f,   Colour32::WHITE,     1.0f, 1.0f,  0.0f, 0.0f, 1.0f),

				Vertex( -0.5f, -0.5f, -0.5f,Colour32::WHITE,     0.0f, 0.0f,  0.0f, 0.0f, -1.0f),    // side 2
				Vertex( -0.5f, 0.5f, -0.5f, Colour32::WHITE,     1.0f, 0.0f,  0.0f, 0.0f, -1.0f),
				Vertex( 0.5f, -0.5f, -0.5f, Colour32::WHITE,     0.0f, 1.0f,  0.0f, 0.0f, -1.0f),
				Vertex( 0.5f, 0.5f, -0.5f,  Colour32::WHITE,     1.0f, 1.0f, 0.0f, 0.0f, -1.0f),

				Vertex( -0.5f, 0.5f, -0.5f, Colour32::WHITE,     0.0f, 0.0f,  0.0f, 1.0f, 0.0f),    // side 3
				Vertex( -0.5f, 0.5f, 0.5f,  Colour32::WHITE,     1.0f, 0.0f,  0.0f, 1.0f, 0.0f),
				Vertex( 0.5f, 0.5f, -0.5f,  Colour32::WHITE,     0.0f, 1.0f,  0.0f, 1.0f, 0.0f),
				Vertex( 0.5f, 0.5f, 0.5f,   Colour32::WHITE,     1.0f, 1.0f,  0.0f, 1.0f, 0.0f),

				Vertex( -0.5f, -0.5f, -0.5f, Colour32::WHITE,     0.0f, 0.0f,  0.0f, -1.0f, 0.0f),    // side 4
				Vertex( 0.5f, -0.5f, -0.5f,  Colour32::WHITE,     1.0f, 0.0f,  0.0f, -1.0f, 0.0f),
				Vertex( -0.5f, -0.5f, 0.5f,  Colour32::WHITE,     0.0f, 1.0f,  0.0f, -1.0f, 0.0f),
				Vertex( 0.5f, -0.5f, 0.5f,   Colour32::WHITE,     1.0f, 1.0f,  0.0f, -1.0f, 0.0f),

				Vertex( 0.5f, -0.5f, -0.5f,  Colour32::WHITE,     0.0f, 0.0f,  1.0f, 0.0f, 0.0f),    // side 5
				Vertex( 0.5f, 0.5f, -0.5f,   Colour32::WHITE,     1.0f, 0.0f,  1.0f, 0.0f, 0.0f),
				Vertex( 0.5f, -0.5f, 0.5f,   Colour32::WHITE,     0.0f, 1.0f,  1.0f, 0.0f, 0.0f),
				Vertex( 0.5f, 0.5f, 0.5f,    Colour32::WHITE,     1.0f, 1.0f,  1.0f, 0.0f, 0.0f),

				Vertex( -0.5f, -0.5f, -0.5f, Colour32::WHITE,     0.0f, 0.0f,  -1.0f, 0.0f, 0.0f),    // side 6
				Vertex( -0.5f, -0.5f, 0.5f,  Colour32::WHITE,     1.0f, 0.0f,  -1.0f, 0.0f, 0.0f),
				Vertex( -0.5f, 0.5f, -0.5f,  Colour32::WHITE,     0.0f, 1.0f,  -1.0f, 0.0f, 0.0f),
				Vertex( -0.5f, 0.5f, 0.5f,   Colour32::WHITE,     1.0f, 1.0f,  -1.0f, 0.0f, 0.0f),
			};

			CreateVertexBuffer(&verts, 24, cubeVerts);

			U16 indices[] = 
			{
				0, 1, 2,    // side 1
				2, 1, 3,
				4, 5, 6,    // side 2
				6, 5, 7,
				8, 9, 10,    // side 3
				10, 9, 11,
				12, 13, 14,    // side 4
				14, 13, 15,
				16, 17, 18,    // side 5
				18, 17, 19,
				20, 21, 22,    // side 6
				22, 21, 23,
			};

			CreateIndexBuffer(indices, 36, cubeIndices);
		}

		void LoadTestModel()
		{
			// --------------------------------------------------------------
			// Load our 3D model.
			Vertex* tempVerts = NULL;
			LoadVertsFromFile(tempVerts, "Resources/BlazeVertexBinary.txt", 5733);

			CreateVertexBuffer(tempVerts, 5733, blazeModelVerts);

			U16* tempIndices = NULL;
			LoadIndicesFromFile(tempIndices, "Resources/BlazeIndexBinary.txt", 1911 * 3);

			CreateIndexBuffer(tempIndices, 1911 * 3, blazeModelIndices);

			// Load our texture.
			SWR_ERR result = TextureManager::Instance().LoadTexture("Resources/Blaze24.bmp", this->blazeTextureMap, 512, 512, false);

			// Destroy our temporaries.
			if (tempIndices != NULL)
			{
				delete [] tempIndices;
				tempIndices = NULL;
			}
			
			if (tempVerts != NULL)
			{
				delete [] tempVerts;
				tempVerts = NULL;
			}

			// --------------------------------------------------------------
		}

		void Cleanup()
		{
			LOG("Main listener cleaned.", LOG_Shutdown);

			if (cubeVerts != NULL)
			{
				delete cubeVerts;
				cubeVerts = NULL;
			}

			if (lightIconVerts != NULL)
			{
				delete lightIconVerts;
				lightIconVerts = NULL;
			}

			if (lightIconIndices != NULL)
			{
				delete lightIconIndices;
				lightIconIndices = NULL;
			}

			if (this->lightIconTexture != NULL)
			{
				delete lightIconTexture;
				lightIconTexture = NULL;
			}

			if (cubeIndices != NULL)
			{
				delete cubeIndices;
				cubeIndices = NULL;
			}

			if (cubeTexture != NULL)
			{
				delete cubeTexture;
				cubeTexture = NULL;
			}

			if (testTexture != NULL)
			{
				delete testTexture;
				testTexture = NULL;
			}

			// Destroy our 3D model.
			if (blazeModelVerts != NULL)
			{
				delete blazeModelVerts;
				blazeModelVerts = NULL;
			}

			if (blazeModelIndices != NULL)
			{
				delete blazeModelIndices;
				blazeModelIndices = NULL;
			}

			if (blazeTextureMap != NULL)
			{
				delete blazeTextureMap;
				blazeTextureMap = NULL;
			}

			if (arialFont != NULL)
			{
				delete arialFont;
				arialFont = NULL;
			}

		}

		void OnFrameStart(float FrameDelta)
		{
			if (INPUT_HANDLER->IsKeyHit(KEY_ESCAPE))
			{
				ENGINE.SafeExit();
			}

			static float tran = 6.0;
			
			if (INPUT_HANDLER->IsKeyDown(KEY_DOWN))
			{
				tran -=  3.0f * FrameDelta;
			}

			if (INPUT_HANDLER->IsKeyDown(KEY_UP))
			{
				tran += 3.0f * FrameDelta;
			}

			if (INPUT_HANDLER->IsKeyHit(KEY_SPACE))
			{
				// Swap modes.
				this->mode++;
				if (mode > ViewModeCount - 1)
				{
					mode = 0;
				}

				SetupRenderMode();
			}

			// Setup the triangle transformation.
			static float rotation = 0;

			//if (INPUT_HANDLER->IsKeyDown(KEY_RSHIFT) == false)
			{
				rotation += 20 * Clamp(0.001f, 0.05f, FrameDelta);

				Matrix4 rotateMat;
				RotateMatrix4Z(rotation, rotateMat);

				Matrix4 rotateMatX;
				RotateMatrix4X(0, rotateMatX);

				Matrix4 transMat;
				TranslateMatrix4(Vector3(0,0,tran), transMat);

				Matrix4 mat;
				mat.Identity();
				mat *= rotateMat;
				mat *= rotateMatX;
				mat *= transMat;
				modelTransform = mat;
				//mat = rotateMatX * transMat;
			}

			static float timer = 0.0f;
			timer += FrameDelta;
			if (timer >= 1.0f)
			{
				UpdateRendererStats();
				timer = 0.0f;
			}

			if (this->mode == LitCrate)
			{
				this->UpdateLightRotation(FrameDelta);
			}
		}

		void Render(float FrameDelta)
		{
			//RenderModel();

			switch (mode)
			{
			case BlazeModel:
				RenderModel();
				break;
			case Crate:
				RenderCrate();
				break;
			case LitCrate:
				RenderLitCrate();
				break;
			};

			DrawInfo();
		}

		void RenderCrate()
		{
			// Render the cube using our index list.
			device->SetWorldTransform(modelTransform);
			device->CommitMatrixChanges();
			device->DrawTrisTexList(true, 12, 0);
			//device->DrawTrisColList(true, 10, 6);
		}

		void RenderLitCrate()
		{
			// Update the world transform.
			device->SetWorldTransform(modelTransform);
			device->CommitMatrixChanges();

			// Update the targets.
			device->SetVertexBuffer(cubeVerts);
			device->SetIndexBuffer(cubeIndices);
			device->SetSourceTexture(cubeTexture);
			device->DrawTrisColLitList(true, 12, 0);

			if (INPUT_HANDLER->IsKeyDown(KEY_LSHIFT))
			{
				device->DrawNormals(this->cubeVerts, Colour32::GREEN, 0.2f);
			}

			// Render the light icon.
			device->SetWorldTransform(lightTransform);
			device->CommitMatrixChanges();

			// Update the render targets.
			device->SetVertexBuffer(this->lightIconVerts);
			device->SetIndexBuffer(this->lightIconIndices);
			device->SetSourceTexture(this->lightIconTexture);

			// Render the light icon.
			device->DrawTrisTexList(true, 12, 0);

		}

		void RenderModel()
		{
			device->SetWorldTransform(modelTransform);
			device->CommitMatrixChanges();
			if (INPUT_HANDLER->IsKeyDown(KEY_LCONTROL))
			{
				device->DrawWireFrame(false, 1911, 0, Colour32::CYAN);
			}
			else
			{
				device->DrawTrisTexList(false, 1911, 0);
			}

			if (INPUT_HANDLER->IsKeyDown(KEY_LSHIFT))
			{
				device->DrawNormals(this->blazeModelVerts, Colour32::GREEN, 0.05f);
			}
		}

		void DrawInfo()
		{
			int fps = ENGINE.GetFPS();
			static char buffer[64] = {0};
			memset(buffer, 0, 64);
			sprintf(buffer, "FPS:%i", fps);
			arialFont->PrintText(buffer, Vector3(8, 8, 0));
			memset(buffer, 0, 64);
			sprintf(buffer, "Tris culled:%i", trisCulled);
			arialFont->PrintText(buffer, Vector3(8, 24, 0));
			memset(buffer, 0, 64);
			sprintf(buffer, "Tris submitted:%i", trisSubmitted);
			arialFont->PrintText(buffer, Vector3(8, 40, 0));
			memset(buffer, 0, 64);
			sprintf(buffer, "Tris rendered:%i", trisDrawn);
			arialFont->PrintText(buffer, Vector3(8, 56, 0));
		}

		void UpdateRendererStats()
		{
			trisCulled = device->GetTrisCulled();// / ENGINE.GetFPS();
			trisDrawn = device->GetTrisRendered();// / ENGINE.GetFPS();
			trisSubmitted = device->GetTrisSubmittedForRender();// / ENGINE.GetFPS();

			device->ResetStatsCounters();
		}

		void DrawInstructions()
		{
		}

		void OnFrameEnd(float FrameDelta)
		{
		}

		void UpdateLightRotation(float delta)
		{
			lightRotTheta += 30.0f * delta;
			Matrix4 zRot;
			Matrix4 trans;
			RotateMatrix4Z(180.0f, zRot);
			Vector3 offset = Vector3(cos((float)lightRotTheta * RADIANS_PER_DEGREE), 0.0f, sin((float)lightRotTheta * RADIANS_PER_DEGREE));
			Vector3 newPos;
			newPos.x = modelTransform.wX + offset.x;
			newPos.y = modelTransform.wY + offset.y + 2.0f;
			newPos.z = modelTransform.wZ + offset.z;
			TranslateMatrix4(newPos, trans);


			lightTransform.Identity();
			lightTransform = zRot;
			lightTransform *= trans;

			this->device->GetLightingManager()->SetLightPosition(pointLightHandle, newPos);
		}
	};
	
}; // End namespace SWR.

#endif // #ifndef MAIN_LISTENER_H