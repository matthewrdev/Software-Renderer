#pragma once

#ifndef VECTOR3_H
#define VECTOR3_H

//****************************************************************************
//**
//**    Vector3.h
//**
//**    Copyright (c) 2010 Matthew Robbins
//**
//**    Author:  Matthew Robbins
//**    Created: 06/2010
//**
//****************************************************************************

namespace SWR
{
	// ------------------------------------------------------------------------
	//								Vector3
	// ------------------------------------------------------------------------
	// Desc:
	// 
	// ------------------------------------------------------------------------
	class Vector3
	{
	private:
	public:		
		float x, y, z;
		
		Vector3()
			: x(0)
			, y(0)
			, z(0)
		{}

		Vector3(float xIn, float yIn, float zIn)
			: x(xIn)
			, y(yIn)
			, z(zIn)
		{}


		Vector3(const Vector3 &v)
			: x(v.x)
			, y(v.y)
			, z(v.z)
		{
		}

		static const Vector3 UNIT_X;
		static const Vector3 UNIT_Y;
		static const Vector3 UNIT_Z;

		void Set(float xIn, float yIn, float zIn);
		void Set(const Vector3 &v);

		void Zero();

		// Converts the vector to a unit vector preserving direction
		void Normalise();

		// Calculates and returns the magnitude of the vector
		float Magnitude();

		// Calculates and returns the non square-rooted magnitude of the vector
		float SquaredMagnitude(); 

		// Prints the vector to the console.
		void Print();
		// Operator overloads
		void operator=(const Vector3 &v);

		Vector3 operator-();

		bool operator==(const Vector3 &v);

		void operator/=(const Vector3 &v);
		void operator/=(float num);

		void operator+=(const Vector3 &v);
		void operator+=(float num);
		
		void operator-=(const Vector3 &v);
		void operator-=(float num);

		void operator*=(const Vector3 &v);
		void operator*=(float num);

		static float DOT(const Vector3 &lhs, const Vector3 &rhs);
		static Vector3 CROSS(const Vector3 &lhs, const Vector3 &rhs);
	};
	
	// ----------------------------------------------------------------------
	//                           Vector3 Operations
	// ----------------------------------------------------------------------
	float FindAngle(const Vector3 &v1, const Vector3 &v2);
	Vector3 Project(const Vector3 &v1, const Vector3 &v2);

	inline Vector3 operator+(const Vector3 &lhs, const Vector3 &rhs)
	{
		return Vector3(lhs.x + rhs.x, lhs.y + rhs.y, lhs.z + rhs.z);
	}

	inline Vector3 operator-(const Vector3 &lhs, const Vector3 &rhs)
	{
		return Vector3(lhs.x - rhs.x, lhs.y - rhs.y, lhs.z - rhs.z);
	}


	inline Vector3 operator*(const Vector3 &lhs, const Vector3 &rhs)
	{
		return Vector3(lhs.x * rhs.x, lhs.y * rhs.y, lhs.z * rhs.z);
	}

	inline Vector3 operator/(const Vector3 &lhs, const Vector3 &rhs)
	{
		return Vector3(lhs.x / rhs.x, lhs.y / rhs.y, lhs.z / rhs.z);
	}

	inline Vector3 operator*(const Vector3 &lhs, float rhs)
	{
		return Vector3(lhs.x * rhs, lhs.y * rhs, lhs.z * rhs);
	}

	inline Vector3 operator/(const Vector3 &lhs, float rhs)
	{
		return Vector3(lhs.x / rhs, lhs.y / rhs, lhs.z / rhs);
	}

	inline Vector3 operator*(float lhs, const Vector3 &rhs)
	{
		return Vector3(lhs * rhs.x, lhs * rhs.y, lhs * rhs.z);
	}

	inline Vector3 operator/(float lhs, const Vector3 &rhs)
	{
		return Vector3(lhs / rhs.x, lhs / rhs.y, lhs / rhs.z);
	}

	
}; // End namespace SWR.

#endif // #ifndef VECTOR3_H