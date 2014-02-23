#pragma once

#ifndef LIGHTING_MANAGER_H
#define LIGHTING_MANAGER_H

//****************************************************************************
//**
//**    LightingManager
//**
//**    Copyright (c) 2010 Matthew Robbins
//**
//**    Author:  Matthew Robbins
//**    Created: 07/2010
//**
//****************************************************************************

#include "Colour.h"
#include "Vector3.h"

#ifndef MAX_SCENE_LIGHTS
#define MAX_SCENE_LIGHTS 4
#endif

// Forward Declarations
namespace SWR
{
	struct Vertex;
	class Matrix4;
};

namespace SWR
{

	enum LightType
	{
		LIGHT_Directional = 1, // 0-1
		LIGHT_Point = 2,       // 1-0

		LIGHT_Invalid,
	};

	enum LightRenderingOptions
	{
		LRO_PointOnly = 1,        
		LRO_DirectionalOnly = 2,  
		LRO_UseAll = 3,           // 1-1

		LRO_Invalid,
	};

	struct Light
	{
		Colour128 colour;
		Vector3 direction;
		Vector3 position;
		LightType type;
		float falloff;
		bool active;
		float atten[3];

		void operator=(const Light& l)
		{
			colour = l.colour;
			direction = l.direction;
			position = l.position;
			type = l.type;
			falloff = l.falloff;
			active = false;
			atten[0] = l.atten[0];
			atten[1] = l.atten[1];
			atten[2] = l.atten[2];
		}

		Light()
			: colour(255.0f,255.0f,255.0f,255.0f)
			, direction(0,0,0)
			, position(0,0,0)
			, type(LIGHT_Invalid)
			, falloff(0.0f)
			, active(false)
		{	}
	};

	// ------------------------------------------------------------------------
	//								LightingManager
	// ------------------------------------------------------------------------
	// Desc:
	// The manager of the lighting for the renderer.
	// Vertices and Normals are to be processed in world space.
	// ------------------------------------------------------------------------
	class LightingManager
	{
	private:

		// Our lighting structures.
		Light* m_sceneLights;
		const int m_totalSceneLights;

		bool ApplyPointLight(int index, Vertex* vertex, Colour128 &out, const Matrix4 &transform);
		bool ApplyDirectionalLight(int index, Vertex* vertex, Colour128 &out, const Matrix4 &transform);
	protected:
	public:

		LightingManager(int totalSceneLights);
		~LightingManager();

		// Applies the scenes lighting to the vertex.
		void ProcessVertex(Vertex* vertex,  const Matrix4 &transform, LightRenderingOptions options = LRO_UseAll);

		void AddLight(Light light, int ID);

		void EnableLight(int ID);
		void DisableLight(int ID);

		void EnableAll();
		void DisableAll();

		int FindNextLightHandle();

		void SetLightPosition(int handle, Vector3 newPos);
	};
	
}; // End namespace SWR.

#endif // #ifndef LIGHTING_MANAGER_H