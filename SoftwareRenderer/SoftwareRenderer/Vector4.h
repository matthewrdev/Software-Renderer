#pragma once

#ifndef VECTOR4_H
#define VECTOR4_H

//****************************************************************************
//**
//**    Vector4
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
	//								Vector4
	// ------------------------------------------------------------------------
	// Desc:
	// 
	// ------------------------------------------------------------------------
	class Vector4
	{
	private:
	public:		
		float x, y, z, w;
		
		Vector4()
			: x(0.0f)
			, y(0.0f)
			, z(0.0f)
			, w(0.0f)
		{}

		Vector4(float xIn, float yIn, float zIn, float wIn)
			: x(xIn)
			, y(yIn)
			, z(zIn)
			, w(wIn)
		{}


		Vector4(const Vector4 &v)
			: x(v.x)
			, y(v.y)
			, z(v.z)
			, w(v.w)
		{
		}

		static const Vector4 UNIT_X;
		static const Vector4 UNIT_Y;
		static const Vector4 UNIT_Z;
		static const Vector4 UNIT_W;

		void Set(float xIn, float yIn, float zIn, float wIn);
		void Set(const Vector4 &v);

		void Zero();

		// Operator overloads
		void operator=(const Vector4 &v);

		bool operator==(const Vector4 &v);

		void operator/=(const Vector4 &v);
		void operator/=(float num);

		void operator+=(const Vector4 &v);
		void operator+=(float num);
		
		void operator-=(const Vector4 &v);
		void operator-=(float num);

		void operator*=(const Vector4 &v);
		void operator*=(float num);


		// Vector negatation
		Vector4 operator-();

		// Converts the vector to a unit vector preserving direction
		void Normalise();

		// Calculates and returns the magnitude of the vector
		float Magnitude();

		// Calculates and returns the non square-rooted magnitude of the vector
		float SquaredMagnitude(); 

		// Prints the vector to the console.
		void Print();

		static Vector4 CROSS(const Vector4 &lhs, const Vector4 &rhs);
		static float DOT(const Vector4 &lhs, const Vector4 &rhs);
	};
	
	// ----------------------------------------------------------------------
	//                           Vector4 Operations
	// ----------------------------------------------------------------------
	Vector4 LERP(const Vector4 &p1, const Vector4 &p2, float t);
	float DotProduct(const Vector4 &v1, const Vector4 &v2);
	Vector4 CrossProduct(const Vector4 &v1, const Vector4 &v2);
	float FindAngle(const Vector4 &v1, const Vector4 &v2);
	Vector4 Project(const Vector4 &v1, const Vector4 &v2);

	inline Vector4 operator+(const Vector4 &lhs, const Vector4 &rhs)
	{
		return Vector4(lhs.x + rhs.x, lhs.y + rhs.y, lhs.z + rhs.z, 0.0f);
	}

	inline Vector4 operator-(const Vector4 &lhs, const Vector4 &rhs)
	{
		return Vector4(lhs.x - rhs.x, lhs.y - rhs.y, lhs.z - rhs.z, 0.0f);
	}


	inline Vector4 operator*(const Vector4 &lhs, const Vector4 &rhs)
	{
		return Vector4(lhs.x * rhs.x, lhs.y * rhs.y, lhs.z * rhs.z, 0.0f);
	}


	inline Vector4 operator/(const Vector4 &lhs, const Vector4 &rhs)
	{
		return Vector4(lhs.x / rhs.x, lhs.y / rhs.y, lhs.z / rhs.z, 0.0f);
	}

	inline Vector4 operator*(const Vector4 &lhs, float rhs)
	{
		return Vector4(lhs.x * rhs, lhs.y * rhs, lhs.z * rhs, 0.0f);
	}

	inline Vector4 operator/(const Vector4 &lhs, float rhs)
	{
		return Vector4(lhs.x / rhs, lhs.y / rhs, lhs.z / rhs, 0.0f);
	}

	inline Vector4 operator*(float lhs, const Vector4 &rhs)
	{
		return Vector4(lhs * rhs.x, lhs * rhs.y, lhs * rhs.z, 0.0f);
	}

	inline Vector4 operator/(float lhs, const Vector4 &rhs)
	{
		return Vector4(lhs / rhs.x, lhs / rhs.y, lhs / rhs.z, 0.0f);
	}

	
}; // End namespace SWR.

#endif // #ifndef VECTOR4_H