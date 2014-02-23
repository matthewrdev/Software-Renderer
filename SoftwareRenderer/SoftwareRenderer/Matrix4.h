#pragma once

#ifndef MATRIX4_H
#define MATRIX4_H

//****************************************************************************
//**
//**    Matrix4
//**
//**    Copyright (c) 2010 Matthew Robbins
//**
//**    Author:  Matthew Robbins
//**    Created: 06/2010
//**
//****************************************************************************

// Forward Declarations
namespace SWR
{
	class Vector3;
	class Vector4;
};

namespace SWR
{
	// ------------------------------------------------------------------------
	//								Matrix4
	// ------------------------------------------------------------------------
	// Desc:
	// 
	// ------------------------------------------------------------------------
	class Matrix4
	{
	private:
	public:
		// X Axis vector
		float xX, xY, xZ, xW;

		// Y Axis vector
		float yX, yY, yZ, yW;

		// Z axis vector
		float zX, zY, zZ, zW;

		// W Axis vector
		float wX, wY, wZ, wW;

		Matrix4();
		Matrix4(const Matrix4 &m);

		// Sets the Matrix4 values to identity (unit vectors)
		void Identity();

		// Operator Overloads
		void operator=(const Matrix4 &m);
		Matrix4 operator*(const Matrix4 &m);
		void operator*=(const Matrix4 &m);

		Matrix4 operator/(float num);
		Matrix4 operator/=(float num);

		// Calculates and returns the determinant of the Matrix4.
		float Determinant();

		// Constants.
		//static Matrix4 IDENTITY;
	};

	void RotateMatrix4X(float angle, Matrix4 &result);
	void RotateMatrix4Y(float angle, Matrix4 &result);
	void RotateMatrix4Z(float angle, Matrix4 &result);
	void UniformScaleMatrix4(float amount, Matrix4 &result);
	void NonUniformScaleMatrix4(float xAmount, float yAmount, float zAmount, Matrix4 &result);
	void TranslateMatrix4(const Vector3 &v, Matrix4 &result);
	void CardinalOrthoProject(bool xAxis, bool yAxis, bool zAxis, Matrix4 &result);
	bool IsLinear(const Matrix4 &m);
	bool IsOrthogonal(const Matrix4 &m);
	bool IsOrthonormal(const Matrix4 &m);
	void Transpose(const Matrix4 &m, Matrix4 &result);
	void Inverse(const Matrix4 &m, Matrix4 &result);
	Vector3 Transform(const Matrix4 &m, const Vector3 &v);
	Vector3 TransformNoTranslate(const Matrix4 &m, const Vector3 &v);
	Vector4 Transform(const Matrix4 &m, const Vector4 &v);
	
}; // End namespace SWR.

#endif // #ifndef MATRIX4_H