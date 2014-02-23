//****************************************************************************
//**
//**    LightingManager.cpp
//**
//**    Copyright (c) 2010 Matthew Robbins
//**
//**    Author:  Matthew Robbins
//**    Created: 07/2010
//**
//****************************************************************************

#include "LightingManager.h"

#include "Vertex.h"
#include "Matrix4.h"

#include "Logger.h"
#include "MemoryLeak.h"

namespace SWR
{

	LightingManager::LightingManager(int totalSceneLights)
		: m_totalSceneLights(totalSceneLights)
	{
		m_sceneLights = new Light[m_totalSceneLights];
	}

	LightingManager::~LightingManager()
	{
		if (m_sceneLights != NULL)
		{
			delete [] m_sceneLights;
		}
	}

	// Applies the scenes lighting to the vertex.
	void LightingManager::ProcessVertex(Vertex* vertex,  const Matrix4 &transform, LightRenderingOptions options)
	{
		Colour128 result(0,0,0,0);
		float totalLightsApplied = 0.0f;
		if (options > LRO_UseAll)
		{
			LOG("Invalid lighting filter has been used.", LOG_Warning);
			return;
		}

		for (int i = 0; i < this->m_totalSceneLights; i++)
		{
			if (m_sceneLights[i].active && (m_sceneLights[i].type & options) && m_sceneLights[i].type != LIGHT_Invalid)
			{
				Colour128 col;
				switch (m_sceneLights[i].type)
				{
				case LIGHT_Point:
					if (ApplyPointLight(i, vertex, col, transform) == true)
					{
						result = result + col;	
						totalLightsApplied += 1.0f;
					};
					break;
				case LIGHT_Directional:
					if (ApplyDirectionalLight(i, vertex, col, transform) == true)
					{
						result = result + col;	
						totalLightsApplied += 1.0f;	
					};
					break;
				};
			}
		}

		if (totalLightsApplied > 0.0f)
		{
			result = result / totalLightsApplied;
			vertex->colour.FromColour128(result);
		}
	}

	bool LightingManager::ApplyPointLight(int index, Vertex* vertex, Colour128 &out, const Matrix4 &transform)
	{
		// Get the vector from the point light to the vertex.
		Light light = m_sceneLights[index];
		Vector3 vPos;
		Vector3 normal = vertex->GetNormal();
		// Apply local rotation to world.
		//normal = TransformNoTranslate(transform, normal);

		vertex->ToVec3(vPos);
		Vector3 pToV = light.position - vPos;

		Colour128 vertCol;
		vertCol.FromColour32(vertex->colour);

		// Store the length.
		float distToVert = pToV.Magnitude();

		if (distToVert > light.falloff)
			return false;

		pToV.Normalise();
		float dot = Vector3::DOT(pToV, normal) * -1;
		if (dot < 0.0f)
			dot = 0.0f;

		float atten = 1.0f / (light.atten[0] + light.atten[1] * distToVert + light.atten[2] * (distToVert * distToVert));

		out = (vertCol * light.colour.ClampColourChannels()) * dot * atten;

		return true;
	}

	bool LightingManager::ApplyDirectionalLight(int index, Vertex* vertex, Colour128 &out, const Matrix4 &transform)
	{
		// Get the vector from the point light to the vertex.
		Light light = m_sceneLights[index];
		Vector3 vPos;
		Vector3 normal = vertex->GetNormal();

		// Apply local rotation to world.
		//normal = TransformNoTranslate(transform, normal);

		vertex->ToVec3(vPos);
		Vector3 pToV = vPos - light.position;

		Colour128 vertCol;
		vertCol.FromColour32(vertex->colour);

		pToV.Normalise();
		float dot = Vector3::DOT(pToV, normal);
		if (dot < 0.0f)
			dot = 0.0f;

		out = (vertCol * light.colour.ClampColourChannels()) * dot;

		return true;
	}

	void LightingManager::AddLight(Light light, int ID)
	{
		if (ID >= this->m_totalSceneLights || ID < 0)
			return;

		m_sceneLights[ID] = light;
	}

	void LightingManager::EnableLight(int ID)
	{
		if (ID >= this->m_totalSceneLights || ID < 0)
			return;

		m_sceneLights[ID].active = true;
	}

	void LightingManager::DisableLight(int ID)
	{
		if (ID >= this->m_totalSceneLights || ID < 0)
			return;

		m_sceneLights[ID].active = false;
	}

	void LightingManager::EnableAll()
	{
		for (int i = 0; i < m_totalSceneLights; i++)
		{
			if (m_sceneLights[i].type != LIGHT_Invalid)
			{
				m_sceneLights[i].active = true;
			}
		}
	}

	void LightingManager::DisableAll()
	{
		for (int i = 0; i < m_totalSceneLights; i++)
		{
			if (m_sceneLights[i].type != LIGHT_Invalid)
			{
				m_sceneLights[i].active = false;
			}
		}
	}

	int LightingManager::FindNextLightHandle()
	{
		for (int i = 0; i < m_totalSceneLights; i++)
		{
			if (m_sceneLights[i].type == LIGHT_Invalid)
			{
				return i;
			}
		}
	}

	void LightingManager::SetLightPosition(int handle, Vector3 newPos)
	{
		if (handle < 0 || handle > this->m_totalSceneLights -1)
			return;

		m_sceneLights[handle].position = newPos;
	}
	
}; // End namespace SWR.