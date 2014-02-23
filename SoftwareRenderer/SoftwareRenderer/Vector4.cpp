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

#include <cmath>
#include <iostream>

#include "Vector4.h"

#include "SWR_Math.h"

namespace SWR
{
	const Vector4 Vector4::UNIT_X = Vector4(1.0f, 0.0f, 0.0f, 0.0f);
	const Vector4 Vector4::UNIT_Y = Vector4(0.0f, 1.0f, 0.0f, 0.0f);
	const Vector4 Vector4::UNIT_Z = Vector4(0.0f, 0.0f, 1.0f, 0.0f);
	const Vector4 Vector4::UNIT_W = Vector4(0.0f, 0.0f, 0.0f, 1.0f);

	void Vector4::Zero()
	{
		x = 0.0f;
		y = 0.0f;
		z = 0.0f;
		w = 0.0f;
	}

	void Vector4::operator =(const Vector4 &v)
	{
		x = v.x;
		y = v.y;
		z = v.z;
		w = v.w;
	}

	bool Vector4::operator ==(const Vector4 &v)
	{
		return ((x == v.x) && (y = v.y) && (z == v.z));
	}

	void Vector4::operator /=(float value)
	{
		x = x / value;
		y = y / value;
		z = z / value;
	}

	void Vector4::operator /=(const Vector4 &v)
	{
		x = x / v.x;
		y = y / v.y;
		z = z / v.z;
	}

	void Vector4::operator *=(const Vector4 &v)
	{
		x = x * v.x;
		y = y * v.y;
		z = z * v.z;
	}

	void Vector4::operator *=(float value)
	{
		x = x * value;
		y = y * value;
		z = z * value;
	}

	void Vector4::operator +=(const Vector4 &v)
	{
		x = x + v.x;
		y = y + v.y;
		z = z + v.z;
	}

	void Vector4::operator +=(float value)
	{
		x = x + value;
		y = y + value;
		z = z + value;
	}

	void Vector4::operator -=(const Vector4 &v)
	{
		x = x - v.x;
		y = y - v.y;
		z = z - v.z;
	}

	void Vector4::operator -=(float value)
	{
		x = x - value;
		y = y - value;
		z = z - value;
	}

	Vector4 Vector4::operator -()
	{	
		return Vector4(-x, -y, -z, -w);
	}

	void Vector4::Normalise()
	{
		float magInv = 1.0f / Magnitude();
		x *= magInv;
		y *= magInv;
		z *= magInv;
		w *= magInv;
	}


	float Vector4::Magnitude()
	{
		return sqrt(this->SquaredMagnitude());
	}

	float Vector4::SquaredMagnitude()
	{
		return (x*x)+(y*y)+(z*z)+(w*w);
	}

	void Vector4::Set(const Vector4 &v)
	{
		Set(v.x, v.y, v.z, v.w);
	}

	void Vector4::Set(float xIn, float yIn, float zIn, float wIn)
	{
		x = xIn;
		y = yIn;
		z = zIn;
		w = wIn;
	}

	
	// Prints the vector to the console.
	void Vector4::Print()
	{
		std::cout << "X: " << x << " Y: " << y << "Z: " << z << " W: " << w << std::endl;
	}

	Vector4 Vector4::CROSS(const Vector4 &lhs, const Vector4 &rhs)
	{
		return Vector4(lhs.y * rhs.z + lhs.z * rhs.y,
					   lhs.z * rhs.x + lhs.x * rhs.z,
					   lhs.x * rhs.y + lhs.y * rhs.x, 0.0f);
	}
	
	float Vector4::DOT(const Vector4 &lhs, const Vector4 &rhs)
	{
		return (lhs.x * rhs.x) + (lhs.y * rhs.y) + (lhs.z * rhs.z);
	}
		
	float FindAngle(const Vector4 &v1, const Vector4 &v2)
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

		Vector4 a(v1);
		Vector4 b(v2);

		a.Normalise();
		b.Normalise();

		theta = acos((float)Vector4::DOT(a, b));
		return ToDegree(theta);
	}

	Vector4 Project(const Vector4 &v1, const Vector4 &v2)
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

		Vector4 result = v1;
		result.Normalise();

		float cosine = Vector4::DOT(result, v2);
		result *= cosine;
		
		return result;
	}
	
	
}; // End namespace SWR.
