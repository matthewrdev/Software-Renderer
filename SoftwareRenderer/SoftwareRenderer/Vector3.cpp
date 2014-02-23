//****************************************************************************
//**
//**    Vector3.cpp
//**
//**    Copyright (c) 2010 Matthew Robbins
//**
//**    Author:  Matthew Robbins
//**    Created: 06/2010
//**
//****************************************************************************

#include <iostream>
#include <cmath>

#include "Vector3.h"

#include "SWR_Math.h"

namespace SWR
{

	const Vector3 Vector3::UNIT_X = Vector3(1.0f, 0.0f, 0.0f);
	const Vector3 Vector3::UNIT_Y = Vector3(0.0f, 1.0f, 0.0f);
	const Vector3 Vector3::UNIT_Z = Vector3(0.0f, 0.0f, 1.0f);

	void Vector3::Zero()
	{
		x = 0.0f;
		y = 0.0f;
		z = 0.0f;
	}

	void Vector3::operator =(const Vector3 &v)
	{
		x = v.x;
		y = v.y;
		z = v.z;
	}

	bool Vector3::operator ==(const Vector3 &v)
	{
		return ((x == v.x) && (y = v.y) && (z == v.z));
	}

	void Vector3::operator /=(float value)
	{
		x = x / value;
		y = y / value;
		z = z / value;
	}

	void Vector3::operator /=(const Vector3 &v)
	{
		x = x / v.x;
		y = y / v.y;
		z = z / v.z;
	}

	void Vector3::operator *=(const Vector3 &v)
	{
		x = x * v.x;
		y = y * v.y;
		z = z * v.z;
	}

	void Vector3::operator *=(float value)
	{
		x = x * value;
		y = y * value;
		z = z * value;
	}

	void Vector3::operator +=(const Vector3 &v)
	{
		x = x + v.x;
		y = y + v.y;
		z = z + v.z;
	}

	void Vector3::operator +=(float value)
	{
		x = x + value;
		y = y + value;
		z = z + value;
	}

	void Vector3::operator -=(const Vector3 &v)
	{
		x = x - v.x;
		y = y - v.y;
		z = z - v.z;
	}

	void Vector3::operator -=(float value)
	{
		x = x - value;
		y = y - value;
		z = z - value;
	}

	Vector3 Vector3::operator -()
	{	
		return Vector3(-x, -y, -z);
	}

	void Vector3::Normalise()
	{
		float magnitude = 1.0f / Magnitude();
		x *= magnitude;
		y *= magnitude;
		z *= magnitude;
	}


	float Vector3::Magnitude()
	{
		return sqrt(this->SquaredMagnitude());
	}

	float Vector3::SquaredMagnitude()
	{
		return (x*x)+(y*y)+(z*z);
	}

	void Vector3::Set(const Vector3 &v)
	{
		Set(v.x, v.y, v.z);
	}

	void Vector3::Set(float xIn, float yIn, float zIn)
	{
		x = xIn;
		y = yIn;
		z = zIn;
	}

	
	// Prints the vector to the console.
	void Vector3::Print()
	{
		std::cout << "X: " << x << " Y: " << y << "Z: " << z << std::endl;
	}

	Vector3 Vector3::CROSS(const Vector3 &lhs, const Vector3 &rhs)
	{
		return Vector3(lhs.y * rhs.z - lhs.z * rhs.y,
					   lhs.z * rhs.x - lhs.x * rhs.z,
					   lhs.x * rhs.y - lhs.y * rhs.x);
	}
	
	float Vector3::DOT(const Vector3 &lhs, const Vector3 &rhs)
	{
		return (lhs.x * rhs.x) + (lhs.y * rhs.y) + (lhs.z * rhs.z);
	}
		
	float FindAngle(const Vector3 &v1, const Vector3 &v2)
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

		Vector3 a(v1);
		Vector3 b(v2);

		a.Normalise();
		b.Normalise();

		theta = acos((float)Vector3::DOT(a, b));
		return ToDegree(theta);
	}

	Vector3 Project(const Vector3 &v1, const Vector3 &v2)
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

		Vector3 result = v1;
		result.Normalise();

		float cosine = Vector3::DOT(result, v2);
		result *= cosine;
		
		return result;
	}
	
}; // End namespace SWR.