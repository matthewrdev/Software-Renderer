#pragma once

#ifndef VECTOR2_H
#define VECTOR2_H

//****************************************************************************
//**
//**    Vector2.h
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
	//								Vector2
	// ------------------------------------------------------------------------
	// Desc:
	// 
	// ------------------------------------------------------------------------	
	class Vector2
	{
	private:
	public:		
		float x, y;
		
		Vector2()
			: x(0.0f)
			, y(0.0f)
		{}

		Vector2(float xIn, float yIn)
			: x(xIn)
			, y(yIn)
		{}


		Vector2(const Vector2 &v)
			: x(v.x)
			, y(v.y)
		{
		}


		void Set(float xIn, float yIn);
		void Set(const Vector2 &v);

		void Zero();

		// Operator overloads
		void operator=(const Vector2 &v);

		bool operator==(const Vector2 &v);

		void operator/=(const Vector2 &v);
		void operator/=(float value);

		void operator+=(const Vector2 &v);
		void operator+=(float value);
		
		void operator-=(const Vector2 &v);
		void operator-=(float value);

		void operator*=(const Vector2 &v);
		void operator*=(float value);


		// Vector negatation
		Vector2 operator-();

		// Converts the vector to a unit vector preserving direction
		void Normalise();

		// Calculates and returns the magnitude of the vector
		float Magnitude();

		// Calculates and returns the non square-rooted magnitude of the vector
		float SquaredMagnitude(); 

		// Prints the vector to the console.
		void Print();

		static float DOT(const Vector2 &lhs, const Vector2 &rhs);
		static Vector2 PERP(const Vector2& vec);

		static const Vector2 UNIT_X;
		static const Vector2 UNIT_Y;
	};
	
	// ----------------------------------------------------------------------
	//                           Vector2 Operations
	// ----------------------------------------------------------------------
	float FindAngle(const Vector2 &lhs, const Vector2 &rhs);
	Vector2 Project(const Vector2 &lhs, const Vector2 &rhs);

	inline Vector2 operator+(const Vector2 &lhs, const Vector2 &rhs)
	{
		return Vector2(lhs.x + rhs.x, lhs.y + rhs.y);
	}

	inline Vector2 operator-(const Vector2 &lhs, const Vector2 &rhs)
	{
		return Vector2(lhs.x - rhs.x, lhs.y - rhs.y);
	}


	inline Vector2 operator*(const Vector2 &lhs, const Vector2 &rhs)
	{
		return Vector2(lhs.x * rhs.x, lhs.y * rhs.y);
	}


	inline Vector2 operator/(const Vector2 &lhs, const Vector2 &rhs)
	{
		return Vector2(lhs.x / rhs.x, lhs.y / rhs.y);
	}

	inline Vector2 operator*(const Vector2 &lhs, float rhs)
	{
		return Vector2(lhs.x * rhs, lhs.y * rhs);
	}

	inline Vector2 operator/(const Vector2 &lhs, float rhs)
	{
		return Vector2(lhs.x / rhs, lhs.y / rhs);
	}

	inline Vector2 operator*(float lhs, const Vector2 &rhs)
	{
		return Vector2(lhs * rhs.x, lhs * rhs.y);
	}

	inline Vector2 operator/(float lhs, const Vector2 &rhs)
	{
		return Vector2(lhs / rhs.x, lhs / rhs.y);
	}
	
}; // End namespace SWR.

#endif // #ifndef VECTOR2_H