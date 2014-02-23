//****************************************************************************
//**
//**    Vector2.cpp
//**
//**    Copyright (c) 2010 Matthew Robbins
//**
//**    Author:  Matthew Robbins
//**    Created: 06/2010
//**
//****************************************************************************

#include <iostream>
#include <cmath>

#include "Vector2.h"

#include "SWR_Math.h"

namespace SWR
{

	const Vector2 Vector2::UNIT_X = Vector2(1.0f, 0.0f);
	const Vector2 Vector2::UNIT_Y = Vector2(0.0f, 1.0f);

	void Vector2::Zero()
	{
		x = 0.0f;
		y = 0.0f;
	}

	void Vector2::operator =(const Vector2 &v)
	{
		x = v.x;
		y = v.y;
	}

	bool Vector2::operator ==(const Vector2 &v)
	{
		return ((x == v.x) && (y = v.y));
	}

	void Vector2::operator /=(float value)
	{
		x = x / value;
		y = y / value;
	}

	void Vector2::operator /=(const Vector2 &v)
	{
		x = x / v.x;
		y = y / v.y;
	}

	void Vector2::operator *=(const Vector2 &v)
	{
		x = x * v.x;
		y = y * v.y;
	}

	void Vector2::operator *=(float value)
	{
		x = x * value;
		y = y * value;
	}

	void Vector2::operator +=(const Vector2 &v)
	{
		x = x + v.x;
		y = y + v.y;
	}

	void Vector2::operator +=(float value)
	{
		x = x + value;
		y = y + value;
	}

	void Vector2::operator -=(const Vector2 &v)
	{
		x = x - v.x;
		y = y - v.y;
	}

	void Vector2::operator -=(float value)
	{
		x = x - value;
		y = y - value;
	}

	Vector2 Vector2::operator -()
	{	
		return Vector2(-x, -y);
	}

	void Vector2::Normalise()
	{
		float magnitude = 1.0f / Magnitude();
		x *= magnitude;
		y *= magnitude;
	}


	float Vector2::Magnitude()
	{
		return sqrt(this->SquaredMagnitude());
	}

	float Vector2::SquaredMagnitude()
	{
		return (x*x)+(y*y);
	}

	void Vector2::Set(const Vector2 &v)
	{
		Set(v.x, v.y);
	}

	void Vector2::Set(float xIn, float yIn)
	{
		x = xIn;
		y = yIn;
	}

	
	// Prints the vector to the console.
	void Vector2::Print()
	{
		std::cout << "X: " << x << " Y: " << y << std::endl;
	}
	
	float Vector2::DOT(const Vector2 &lhs, const Vector2 &rhs)
	{
		return (lhs.x * rhs.x) + (lhs.y * rhs.y);
	}

	Vector2 Vector2::PERP(const Vector2& vec)
	{
		return Vector2(vec.y, -vec.x);
	}
		
	float FindAngle(const Vector2 &v1, const Vector2 &v2)
	{

		// ***********************************************
		//            Angle through DotProduct
		// -----------------------------------------------
		//                  /-       -\
		//                  |  a . b  |
		//      theta = acos| ------- |
		//                  | |a|*|b| |
		//                  \-       -/
		// -----------------------------------------------
		// Where:
		//       T = theta.
		//       a = a vector.
		//      |a|= magnitude of a vector.
		//       b = b vector 
		//      |b|= magnitude of b vector
		// ***********************************************

		float theta = 0.0f;

		Vector2 a(v1);
		Vector2 b(v2);

		a.Normalise();
		b.Normalise();

		theta = acos((float)Vector2::DOT(a, b));
		return ToDegree(theta);
	}

	Vector2 Project(const Vector2 &v1, const Vector2 &v2)
	{
		// ***********************************************
		//               Vector Projection
		// -----------------------------------------------
		//                |   ^ (v2)
		//                |  /.
		//                | / .
		//         ------ X -----> (v1)
		//                |
		//                |
		//                |
		// To project onto another vector we get the 
		// the cosine of the angle formed between v1 & v2.
		// This can be done through normaliseing v1 and
		// then doing a dot product. We then rescale v1
		// to the cosine of the angle and then we have our
		// projected vector.
		// ***********************************************

		Vector2 result = v1;
		result.Normalise();

		float cosine = Vector2::DOT(result, v2);
		result *= cosine;
		
		return result;
	}
	
}; // End namespace SWR.