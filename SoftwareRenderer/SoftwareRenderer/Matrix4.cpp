//****************************************************************************
//**
//**    Matrix4.cpp
//**
//**    Copyright (c) 2009 Matthew Robbins
//**
//**    Author:  Matthew Robbins
//**    Created: 12/2009
//**
//****************************************************************************

#include <memory>

#include "Matrix4.h"

#include "SWR_Math.h"
#include "Vector3.h"
#include "Vector4.h"

namespace SWR
{
	Matrix4::Matrix4()
	{
	}

	Matrix4::Matrix4(const Matrix4 &m)
	{
		// Copy first row (x axis vector)
		xX = m.xX;
		xY = m.xY;
		xZ = m.xZ;
		xW = m.xW;

		// Copy second row (y axis vector)
		yX = m.yX;
		yY = m.yY;
		yZ = m.yZ;
		yW = m.yW;

		// Copy third row (z axis vector)
		zX = m.zX;
		zY = m.zY;
		zZ = m.zZ;
		zW = m.zW;

		// Copy forth row (w axis vector)
		wX = m.wX;
		wY = m.wY;
		wZ = m.wZ;
		wW = m.wW;
	}

	void Matrix4::Identity()
	{
		// Set the x axis to a unit vector
		xX = 1.0f;
		xY = 0.0f;
		xZ = 0.0f;
		xW = 0.0f;

		// Set y axis to a unit vector
		yX = 0.0f;
		yY = 1.0f;
		yZ = 0.0f;
		yW = 0.0f;

		// Set the z axis to a unit vector
		zX = 0.0f;
		zY = 0.0f;
		zZ = 1.0f;
		zW = 0.0f;

		// Set the W/Translation axis to a unit vector
		wX = 0.0f;
		wY = 0.0f;
		wZ = 0.0f;
		wW = 1.0f;

	}	
	
	void Matrix4::operator*=(const Matrix4 &m)
	{
		*this = *this * m;
	}

	Matrix4 Matrix4::operator*(const Matrix4 &m)
	{
		Matrix4 result;

		// ------------------------
		// p' = xX' + yY' + zZ' + wW'
		// ------------------------

		// ********************************
		//              ROW MAJOR
		//            x   y   z   w
		//         /-               -\
		// xVector | m11 m12 m13 m14 |
		// yVector | m21 m22 m23 m24 |
		// zVector | m31 m32 m33 m34 |
		// wVector | m41 m42 m43 m44 |
		//         \-               -/
		// ********************************

		// First row of resultant Matrix4 (X axis)
		/*m11*/ result.xX = (xX * m.xX) + (xY * m.yX) + (xZ * m.zX) + (xW * m.wX);
		/*m12*/ result.xY = (xX * m.xY) + (xY * m.yY) + (xZ * m.zY) + (xW * m.wY);
		/*m13*/ result.xZ = (xX * m.xZ) + (xY * m.yZ) + (xZ * m.zZ) + (xW * m.wZ);
		/*m14*/ result.xW = (xX * m.xW) + (xY * m.yW) + (xZ * m.zW) + (xW * m.wW);

		// Second row of resultant Matrix4 (Y axis)
		/*m21*/ result.yX = (yX * m.xX) + (yY * m.yX) + (yZ * m.zX) + (yW * m.wX);
		/*m22*/ result.yY = (yX * m.xY) + (yY * m.yY) + (yZ * m.zY) + (yW * m.wY);
		/*m23*/ result.yZ = (yX * m.xZ) + (yY * m.yZ) + (yZ * m.zZ) + (yW * m.wZ);
		/*m24*/ result.yW = (yX * m.xW) + (yY * m.yW) + (yZ * m.zW) + (yW * m.wW);
		
		// Third row of resultant Matrix4 (Z axis)
		/*m31*/ result.zX = (zX * m.xX) + (zY * m.yX) + (zZ * m.zX) + (zW * m.wX);
		/*m32*/ result.zY = (zX * m.xY) + (zY * m.yY) + (zZ * m.zY) + (zW * m.wY);
		/*m33*/ result.zZ = (zX * m.xZ) + (zY * m.yZ) + (zZ * m.zZ) + (zW * m.wZ);
		/*m34*/ result.zW = (zX * m.xW) + (zY * m.yW) + (zZ * m.zW) + (zW * m.wW);

		// Forth row of resultant Matrix4 (W axis)
		/*m41*/ result.wX = (wX * m.xX) + (wY * m.yX) + (wZ * m.zX) + (wW * m.wX);
		/*m42*/ result.wY = (wX * m.xY) + (wY * m.yY) + (wZ * m.zY) + (wW * m.wY);
		/*m43*/ result.wZ = (wX * m.xZ) + (wY * m.yZ) + (wZ * m.zZ) + (wW * m.wZ);
		/*m44*/ result.wW = (wX * m.xW) + (wY * m.yW) + (wZ * m.zW) + (wW * m.wW);

		// Return the resutant Matrix4
		return result;
	}

	void Matrix4::operator=(const Matrix4 &m)
	{
		// Copy first row (x axis vector)
		xX = m.xX;
		xY = m.xY;
		xZ = m.xZ;
		xW = m.xW;

		// Copy second row (y axis vector)
		yX = m.yX;
		yY = m.yY;
		yZ = m.yZ;
		yW = m.yW;

		// Copy third row (z axis vector)
		zX = m.zX;
		zY = m.zY;
		zZ = m.zZ;
		zW = m.zW;

		// Copy forth row (w axis vector)
		wX = m.wX;
		wY = m.wY;
		wZ = m.wZ;
		wW = m.wW;
	}


	float Matrix4::Determinant()
	{
		// ******************************************************************************
		//                                   DETERMIANT
		// ******************************************************************************
		//                                  x   y   z   w 
		//                               /--            --\
		//                       xVector | m11 m12 m13 m14|
		//                       yVector | m21 m22 m23 m24|
		//                       zVector | m31 m32 m33 m34|
		//                       wVector | m41 m42 m43 m44|
		//                               \--            --/   
		//    
		//                                     EQUALS
		//  m11(m22(m33*m44 - m34*m43) + m23(m34*m42 - m32*m44) + m24(m32*m43 - m34*m42))
		//                                       +
		//  m12(m21(m33*m44 - m34*m43) + m23(m31*m44 - m34*m41) + m24(m31*m43 - m33*m41))
		//                                       +
		//  m13(m21(m32*m44 - m34*m42) + m22(m31*m44 - m34*m41) + m24(m31*m42 - m32*m41))
		//                                       +
		//  m14(m21(m32*m43 - m33*m42) + m22(m31*m43 - m33*m41) + m23(m31*m42 - m32*m41))
		//
		// GAARRGH! What the?
		//
		// Ok.. so an example of how this formula is derived is as so (apart from looking
		// at the textbook):
		// To find the determinant of the entire Matrix4 we first need to divide it up into
		// sub-matrices multiplied by a number of the top row.
		// For instance the first part (formula multiplied by m11) would be extracted like
		// this:
		//
		// Cancel out the row & column of our top row number (m11) =
		// 
		//                                   x   y   z   w 
		//                                /--            --\
		//                        xVector | m11 XXX XXX XXX|
		//                        yVector | XXX m22 m23 m24|
		//                        zVector | XXX m32 m33 m34|
		//                        wVector | XXX m42 m43 m44|
		//                                \--            --/ 
		//
		// So now we have a 3x3 Matrix4. From here we do the same step except this time
		// our top row number is from the sub-Matrix4 formed (m22). So we cancel out the
		// row & column of this number within our sub-Matrix4 =
		//
		//                                   x   y   z   w 
		//                                /--            --\
		//                        xVector | m11 XXX XXX XXX|
		//                        yVector | XXX m22 XXX XXX|
		//                        zVector | XXX XXX m33 m34|
		//                        wVector | XXX XXX m43 m44|
		//                                \--            --/ 
		//
		// Horay! We're getting somewhere. The next step is the same idea, we are left with
		// a 2x2 Matrix4 now so we cancel out the row and column of this mini-Matrix4 and we 
		// have the first part our final product. Sooo... =
		//
		//                                   x   y   z   w 
		//                                /--            --\
		//                        xVector | m11 XXX XXX XXX|
		//                        yVector | XXX m22 XXX XXX|
		//                        zVector | XXX XXX m33 XXX|
		//                        wVector | XXX XXX XXX m44|
		//                                \--            --/
		// 
		// Whew! Nearly there so now we repeat except we move across the row in the 2x2 
		// Matrix4 and we get the next part of the final product.
		//
		//                                   x   y   z   w 
		//                                /--            --\
		//                        xVector | m11 XXX XXX XXX|
		//                        yVector | XXX m22 XXX XXX|
		//                        zVector | XXX XXX XXX m34|
		//                        wVector | XXX XXX m43 XXX|
		//                                \--            --/
		//
		// We have completed this 2x2 Matrix4 so now we step back up, move along columns in 
		// the 3x3 Matrix4 and repeat the process of canceling and multiplying.
		// If we continue this process for the first column of the top row we eventually
		// end up with the formula:
		//
		//  m11(m22(m33*m44 - m34*m43) + m23(m34*m42 - m32*m44) + m24(m32*m43 - m34*m42))
		//
		// This process is the same for the rest of the formulas components and can also
		// be applied to 5x5, 6x6 etc matrices. I'd love to show it applied buuut, the
		// amount of space this explanation hogged is tiny compared to what a 5x5 or 6x6
		// would require. So for my sanity (and yours) I wont.
		// ******************************************************************************

		// I had originally just done a straight calculation for the return section but
		// that was for a 3x3. In order to make the code less hideous I have broken each
		// formula section into independant variables which I will add together in the 
		// return.
		
		//  m11(m22(m33*m44 - m34*m43) + m23(m34*m42 - m32*m44) + m24(m32*m43 - m34*m42))
		float part1 = xX * (yY*(zZ*wW - zW*wZ) + yZ*(zW*wY - zY*wW) + yW*(zY*wZ - zZ*wY));
										
		// m12(m21(m33*m44 - m34*m43) + m23(m31*m44 - m34*m41) + m24(m31*m43 - m33*m41))
		float part2 = xY * (yX*(zZ*wW - zW*wZ) + yZ*(zW*wX - zX*wW) + yW*(zX*wZ - zZ*wX));
										
		// m13(m21(m32*m44 - m34*m42) + m22(m31*m44 - m34*m41) + m24(m31*m42 - m32*m41))
		float part3 = xZ * (yX*(zY*wW - zW*wY) + yY*(zW*wX - zX*wW) + yW*(zX*wY - zY*wX));
										
		// m14(m21(m32*m43 - m33*m42) + m22(m31*m43 - m33*m41) + m23(m31*m42 - m32*m41))
		float part4 = xW * (yX*(zY*wZ - zZ*wY) + yY*(zZ*wX - zX*wZ) + yZ*(zX*wY - zY*wX));

		return (part1 - part2 + part3 - part4);
	}

	void RotateMatrix4X(float angle, Matrix4 &result)
	{
		// X axis rotation 
		// /--           --\
		// | 1    0     0  |
		// | 0  cosT -sinT |
		// | 0  sinT  cosT |
		// \--           --/

		result.Identity();

		// Convert angle to radian
		float theta = ToRadian(angle);
		float sine, cosine;
		SinCos(cosine, sine, theta);

		// Y axis rotation
		result.yY = cosine;
		result.yZ = -sine;

		// Z axis rotation
		result.zY = sine;
		result.zZ = cosine;
	}


	void RotateMatrix4Y(float angle, Matrix4 &result)
	{
		// Y axis rotation 
		// /--           --\
		// | cosT  0 -sinT |
		// | 0     1    0  |
		// | sinT  0  cosT |
		// \--           --/
		result.Identity();

		// Convert angle to radian
		float theta = ToRadian(angle);
		float sine, cosine;
		SinCos(cosine, sine, theta);

		// X axis rotation
		result.xX = cosine;
		result.xZ = -sine;

		// Z axis rotation
		result.zX = sine;
		result.zZ = cosine;
	}


	void RotateMatrix4Z(float angle, Matrix4 &result)
	{
		// Z axis rotation 
		// /--           --\
		// | cosT sinT  0  |
		// |-sinT cosT  0  |
		// | 0    0     1  |
		// \--           --/

		result.Identity();

		// Convert angle to radian
		float theta = ToRadian(angle);
		float sine, cosine;
		SinCos(cosine, sine, theta);

		// X axis rotation
		result.xX = cosine;
		result.xY = sine;

		// Y axis rotation;
		result.yX = -sine;
		result.yY = cosine;
	}

	void UniformScaleMatrix4(float amount, Matrix4 &result)
	{
		result.Identity();

		// Scale all dimensions by the amount.
		// As the result is identity, we remove unnecassary multiplications that will result in 0.
		// X axis vector
		result.xX *= amount;

		// Y axis vector
		result.yY *= amount;

		// Z axis vector
		result.zZ *= amount;
	}


	void NonUniformScaleMatrix4(float xAmount, float yAmount, float zAmount, Matrix4 &result)
	{
		result.Identity();

		// Scale all dimensions by the ENGINEsponding amount.
		// As the result is identity, we remove unnecassary multiplications that will result in 0.
		// X axis vector * xAmount
		result.xX *= xAmount;

		// Y axis vector * yAmount
		result.yY *= yAmount;

		// Z axis vector * zAmount
		result.zZ *= zAmount;
	}

	void TranslateMatrix4(const Vector3 &v, Matrix4 &result)
	{
		result.Identity();

		result.wX = v.x;
		result.wY = v.y;
		result.wZ = v.z;
	}

	void CardinalOrthoProject(bool xAxis, bool yAxis, bool zAxis, Matrix4 &result)
	{
		result.Identity();
		if (xAxis)
			result.xX = 0;

		if (yAxis)
			result.yY = 0;

		if (zAxis)
			result.zZ = 0;
	}

	bool IsLinear(const Matrix4 &m)
	{
		// Linear matrices do NOT have any translation component to them.
		// Therefore, if any w values are not ZERO then it is not a linear transform.
		if (Equal(m.wX, 0.0f) == true 
			&& Equal(m.wY, 0.0f) == true
			&& Equal(m.wZ, 0.0f) == true)
		{
			return true;
		}

		// As we have a translation component that is !0 then the transformation is Affine.
		return false;
	}

	
	bool IsOrthonormal(const Matrix4 &m)
	{
		// Axis components
		Vector3 xAxis(m.xX, m.xY, m.xZ);
		Vector3 yAxis(m.yX, m.yY, m.yZ);
		Vector3 zAxis(m.zX, m.zY, m.zZ);

		// Angle relation ships between the axis
		float xyTheta = FindAngle(xAxis, yAxis);
		float yzTheta = FindAngle(yAxis, zAxis);
		float xzTheta = FindAngle(xAxis, zAxis);

		// Determine if all axis are perpendicular to each other
		if (Equal(xyTheta, 90.0f) == true
			&& Equal(yzTheta, 90.0f) == true
			&& Equal(xzTheta, 90.0f) == true)
		{
			// Orthonormal Matrix4 as all axis are perpidicular.
			return true;
		}

		// Fall through if axis are not perpindicular
		return false;
	}


	bool IsOrthogonal(const Matrix4 &m)
	{
		// Axis components
		Vector3 xAxis(m.xX, m.xY, m.xZ);
		Vector3 yAxis(m.yX, m.yY, m.yZ);
		Vector3 zAxis(m.zX, m.zY, m.zZ);

		// Determine if all axis are perpendicular to each other
		if (IsOrthonormal(m))
		{
			// Axis are perpendicular so now check to see if axis are unit vectors...
			if (Equal(xAxis.Magnitude(), 1) == true
				&& Equal(yAxis.Magnitude(), 1) == true
				&& Equal(zAxis.Magnitude(), 1) == true)
			{
				// Whew! We have an Orthagonal Matrix4, too much effort if you ask me.
				return true;
			}
		}

		// Nwaaah! Its not orthogonal...
		return false;
	}

	void Transpose(const Matrix4 &m, Matrix4 &result)
	{
		// ************************************************************
		// To transpose a Matrix4 we simply flip it diagonally.
		//
		// So:
		//                    /--            --\   
		//                    | m11 m12 m13 m14| <\
		//                    | m21 m22 m23 m24|   \
		//                    | m31 m32 m33 m34|   /
		//                  < | m41 m42 m43 m44|  /
		//                 V  \--            --/ / 
		//                  V                   /
		//                   V_________________/
		//
		//                          EQUALS
		//                    /--            --\   
		//                    | m11 m21 m31 m41| 
		//                    | m12 m22 m32 m42|  
		//                    | m13 m23 m33 m43| 
		//                    | m14 m24 m34 m44| 
		//                    \--            --/  
		//                 
		// ************************************************************

		// Firstly copy diagonal as it remains unchanged
		// m11 = m11.
		result.xX = m.xX;
		// m22 = m22.
		result.yY = m.yY;
		// m33 = m33.
		result.zZ = m.zZ;
		// m44 = m44.
		result.wW = m.wW;

		// Copy other elements into ENGINEsponding places.

		// First row.
		// m12 = m21.
		result.xY = m.yX;
		// m13 = m31.
		result.xZ = m.zX;
		// m14 = m41.
		result.xW = m.wX;

		// Second row.
		// m21 = m12.
		result.yX = m.xY;
		// m23 = m32.
		result.yZ = m.zY;
		// m24 = m42.
		result.yW = m.wY;

		// Third row.
		// m31 = m13.
		result.zX = m.xZ;
		// m32 = m23.
		result.zY = m.yZ;
		// m34 = m43.
		result.zW = m.wZ;

		// Forth row
		// m41 = m14.
		result.wX = m.xW;
		// m42 = m24.
		result.wY = m.yW;
		// m43 = m34.
		result.wZ = m.zW;
		
	}

	void Inverse(const Matrix4 &m, Matrix4 &result)
	{
		// This entire function desparately needs to be optimised as im repeating many a operation several times.
		// However, I've left these as is for clarities sake(whatever hope that has with an operation like inverse).

		// If the Matrix4 is Orthogonal then we can just transpose it to make things easier.
		if (IsOrthogonal(m))
		{
			return Transpose(m, result); 
		}

		// If it is not orthogonal then we have lots and lots and lots of calculations to do.
		// So we need to calculate the co-factors for each element in the new Matrix4....
		// The co-factor for a given point in a new Matrix4 is given by.
		//
		// Determinat of sub-Matrix4
		//     /--            --\       /--            --\       /--            --\   
		//     | RES XXX XXX XXX|       | RES XXX XXX XXX|       | RES XXX XXX XXX|
		//     | XX (m22) XX XXX| MINUS | X (m22) XXX XXX|  ADD  | XXX XX (m23) XX| 
		//     | XXX XX (m33) XX| MINUS | X XXX XXX (m34)|  ADD  | XXX XX (m33) XX|
		//     | XXX XXX X (m44)|       | X XXX (m43) XXX|       | XXX XX XX (m44)|
		//     \--            --/       \--            --/       \--            --/ ... ETC
		//
		// 
		// To break it down more, for m11 in the inverse the equation is similar to the determinant.
		// We cancel out the rows and columns of the subject.
		//
		// EXAMPLE OF ROW & COLUMN CANCELLING
		//     /--            --\       /--            --\       /--            --\   
		//     | m11 XXX XXX XXX|       | m11 XXX XXX XXX|       | m11 XXX XXX XXX|
		//     | XXX m22 m23 m24| >   > | XXX m22 XXX XXX| >   > | XXX m22 XXX XXX| 
		//     | XXX m32 m33 m34| >   > | XXX XXX m33 m34| >   > | XXX XXX m33 XXX|
		//     | XXX m42 m43 m44|       | XXX XXX m43 m44|       | XXX XXX XXX m44|
		//     \--            --/       \--            --/       \--            --/
		//
		// m11(-1) = m22*(m33*m44 - m34*m43) + m23*(m34*m42 - m32*m44) + m24*(m32*m43 - m33*m42);
		//
		// Finally after getting the co-factor we need divide the entire Matrix4 by the determinant of the
		// old Matrix4.


		// Co-factors of the X Axis vector.
		float cof11, cof12, cof13, cof14;

		// Co-factors of the Y Axis vector.
		float cof21, cof22, cof23, cof24;

		// Co-factors of the Z Axis vector.
		float cof31, cof32, cof33, cof34;

		// Co-factors of the W Axis vector.
		float cof41, cof42, cof43, cof44;

		// The determinant of the inputed Matrix4.
		Matrix4 md(m);
		float detInv = 1.0f / md.Determinant();

		// And so it starts...
		
		//*********************************************************************************************************
		//                                             X AXIS VECTOR
		//*********************************************************************************************************

		// Cofactor of element m11 aka X component of the x axis vector.
		cof11 = m.yY*(m.zZ*m.wW - m.zW*m.wZ) + m.yZ*(m.zW*m.wY - m.zY*m.wW) + m.yW*(m.zY*m.wZ - m.zZ*m.wY);

		// Cofactor of element m12 aka Y component of the x axis vector.
		cof12 = -(m.yX*(m.zZ*m.wW - m.zW*m.wZ) + m.yZ*(m.zW*m.wX - m.zX*m.wW) + m.yW*(m.zX*m.wZ - m.zZ*m.wX));

		// Cofactor of element m13 aka Z component of the x axis vector.
		cof13 = m.yX*(m.zY*m.wW - m.zW*m.wY) + m.yY*(m.zW*m.wX - m.zX*m.wW) + m.yW*(m.zX*m.wY - m.zY*m.wX);

		// Cofactor of element m14 aka W component of the x axis vector.
		cof14 = -(m.yX*(m.zY*m.wZ - m.zZ*m.wY) + m.yY*(m.zZ*m.wX - m.zX*m.wZ) + m.yZ*(m.zX*m.wY - m.zY*m.wX));
	
		//*********************************************************************************************************
		//                                             Y AXIS VECTOR
		//*********************************************************************************************************

		// Cofactor of element m21 aka X component of the y axis vector.
		cof21 = -(m.xY*(m.zZ*m.wW - m.zW*m.wZ) + m.xY*(m.zY*m.wW - m.zW*m.wY) + m.xW*(m.zY*m.wZ - m.zZ*m.wY));
		
		// Cofactor of element m22 aka Y component of the y axis vector.
		cof22 = m.xX*(m.zZ*m.wW - m.zW*m.wZ) + m.xZ*(m.wX*m.zW - m.wW*m.zX) + m.xW*(m.zX*m.wZ - m.zZ*m.wX);

		// Cofactor of element m23 aka Z component of the y axis vector.
		cof23 = -(m.xX*(m.zY*m.wW - m.zW*m.wY) + m.xY*(m.wX*m.zW - m.wW*m.zX) + m.xW*(m.zX*m.wY - m.zY*m.wX));

		// Cofactor of element m24 aka W component of the y axis vector.
		cof24 = m.xX*(m.zY*m.wZ - m.zZ*m.wY) + m.xY*(m.wX*m.zZ - m.wZ*m.zX) + m.xZ*(m.zX*m.wY - m.zY*m.wX);
	
		//*********************************************************************************************************
		//                                             Z AXIS VECTOR
		//*********************************************************************************************************

		// Cofactor of element m31 aka X component of the z axis vector.
		cof31 = m.xY*(m.yZ*m.wW - m.yW*m.wZ) + m.xZ*(m.wY*m.yW - m.wW*m.yY) + m.xW*(m.yY*m.wZ - m.yZ*m.wY);

		// Cofactor of element m32 aka Y component of the z axis vector.
		cof32 = -(m.xX*(m.yZ*m.wW - m.yW*m.wZ) + m.xZ*(m.wX*m.yW - m.wW*m.yX) + m.xW*(m.yX*m.wZ - m.yZ*m.wX));
		
		// Cofactor of element m33 aka Z component of the z axis vector.
		cof33 = m.xX*(m.yY*m.wW - m.yW*m.wY) + m.xY*(m.wX*m.yW - m.wW*m.yX) + m.xW*(m.yX*m.wY - m.yY*m.wX);
		
		// Cofactor of element m34 aka W component of the z axis vector.
		cof34 = -(m.xX*(m.yY*m.wZ - m.yZ*m.wY) + m.xY*(m.wX*m.yZ - m.wZ*m.yX) + m.xZ*(m.yX*m.wY - m.yY*m.wX));
	
		//*********************************************************************************************************
		//                                             W AXIS VECTOR
		//*********************************************************************************************************

		// Cofactor of element m41 aka X component of the w axis vector.
		cof41 = m.xY*(m.yZ*m.zW - m.yW*m.zZ) + m.xZ*(m.zY*m.yW - m.zW*m.yY) + m.xW*(m.yY*m.zZ - m.yZ*m.zY);

		// Cofactor of element m42 aka Y component of the w axis vector.
		cof42 = -(m.xX*(m.yZ*m.zW - m.yW*m.zZ) + m.xZ*(m.yW*m.zX - m.yX*m.zW) + m.xW*(m.yX*m.zZ - m.yZ*m.zX));
		
		// Cofactor of element m43 aka Z component of the w axis vector.
		cof43 = m.xX*(m.yY*m.zW - m.yW*m.zY) + m.xY*(m.zX*m.yW - m.zW*m.yX) + m.xW*(m.yX*m.zY - m.yY*m.zX);
		
		// Cofactor of element m44 aka Z component of the w axis vector.
		cof44 = -(m.xX*(m.yY*m.zZ - m.yZ*m.zY) + m.xY*(m.zX*m.yZ - m.zZ*m.yX) + m.xZ*(m.yX*m.zY - m.yY*m.zX));

		// Now we have all the co-factors we punch them into our resultant Matrix4 and divide by the determinant.

		// Inverse X axis of m.
		result.xX = (cof11 * detInv);
		result.xY = (cof12 * detInv);
		result.xZ = (cof13 * detInv);
		result.xW = (cof14 * detInv);

		// Inverse Y axis of m.
		result.yX = (cof21 * detInv);
		result.yY = (cof22 * detInv);
		result.yZ = (cof23 * detInv);
		result.yW = (cof24 * detInv);

		// Inverse Z axis of m.
		result.zX = (cof31 * detInv);
		result.zY = (cof32 * detInv);
		result.zZ = (cof33 * detInv);
		result.zW = (cof34 * detInv);

		// Inverse W axis of m.
		result.wX = (cof41 * detInv);
		result.wY = (cof42 * detInv);
		result.wZ = (cof43 * detInv);
		result.wW = (cof44 * detInv);
	}

	Vector3 Transform(const Matrix4 &m, const Vector3 &v)
	{
		// ************************************************************
		// The formula to transform the vector/point is as follows:
		// ------------------------------------------------------------
		//		        p' = xX' + yY' + zZ' + T
		//					      OR
		// p'.x = (x * X'.x) + (x * Y'.x) + (x * Z'.x) + T.x
		// p'.y = (y * X'.y) + (y * Y'.y) + (y * Z'.y) + T.y
		// p'.z = (z * X'.z) + (z * Y'.z) + (z * Z'.z) + T.z
		// ------------------------------------------------------------
		// Where:
		//		 p' is the transformed point/vector.
		//		 x is the x component/scalar of the vector.
		//       X' is the x axis component of the Matrix4.
		//		 y is the y component/scalar of the vector.
		//       Y' is the y axis component of the Matrix4.
		//		 z is the z component/scalar of the vector.
		//       Z' is the z axis component of the Matrix4.
		//       T is the translation component of the Matrix4 (W axis).
		// ************************************************************

		Vector3 result;

		Vector3 xAx(0,0,0); 
		Vector3 yAx(0,0,0);
		Vector3 zAx(0,0,0);

		// xX' component of formula
		xAx.x = v.x * m.xX;
		xAx.y = v.x * m.xY;
		xAx.z = v.x * m.xZ;

		// yY' component of formula
		yAx.x = v.y * m.yX;
		yAx.y = v.y * m.yY;
		yAx.z = v.y * m.yZ;

		// zZ' component of formula
		zAx.x = v.z * m.zX;
		zAx.y = v.z * m.zY;
		zAx.z = v.z * m.zZ;

		// Using the above equation, we now add the components (including translation components).
		// This isnt a true transform as I'm cheating by simply adding on the translation component.
		result.x = xAx.x + yAx.x + zAx.x + m.wX;
		result.y = xAx.y + yAx.y + zAx.y + m.wY;
		result.z = xAx.z + yAx.z + zAx.z + m.wZ;

		return result;
	}

	Vector3 TransformNoTranslate(const Matrix4 &m, const Vector3 &v)
	{
		// ************************************************************
		// The formula to transform the vector/point is as follows:
		// ------------------------------------------------------------
		//		        p' = xX' + yY' + zZ' + T
		//					      OR
		// p'.x = (x * X'.x) + (x * Y'.x) + (x * Z'.x) + T.x
		// p'.y = (y * X'.y) + (y * Y'.y) + (y * Z'.y) + T.y
		// p'.z = (z * X'.z) + (z * Y'.z) + (z * Z'.z) + T.z
		// ------------------------------------------------------------
		// Where:
		//		 p' is the transformed point/vector.
		//		 x is the x component/scalar of the vector.
		//       X' is the x axis component of the Matrix4.
		//		 y is the y component/scalar of the vector.
		//       Y' is the y axis component of the Matrix4.
		//		 z is the z component/scalar of the vector.
		//       Z' is the z axis component of the Matrix4.
		//       T is the translation component of the Matrix4 (W axis).
		// ************************************************************

		Vector3 result;

		Vector3 xAx(0,0,0); 
		Vector3 yAx(0,0,0);
		Vector3 zAx(0,0,0);

		// xX' component of formula
		xAx.x = v.x * m.xX;
		xAx.y = v.x * m.xY;
		xAx.z = v.x * m.xZ;

		// yY' component of formula
		yAx.x = v.y * m.yX;
		yAx.y = v.y * m.yY;
		yAx.z = v.y * m.yZ;

		// zZ' component of formula
		zAx.x = v.z * m.zX;
		zAx.y = v.z * m.zY;
		zAx.z = v.z * m.zZ;

		result.x = xAx.x + yAx.x + zAx.x;
		result.y = xAx.y + yAx.y + zAx.y;
		result.z = xAx.z + yAx.z + zAx.z;

		return result;
	}

	Vector4 Transform(const Matrix4 &m, const Vector4 &v)
	{
		// ************************************************************
		// The formula to transform the vector/point is as follows:
		// ------------------------------------------------------------
		//		        p' = xX' + yY' + zZ' + T
		//					      OR
		// p'.x = (x * X'.x) + (x * Y'.x) + (x * Z'.x) + T.x
		// p'.y = (y * X'.y) + (y * Y'.y) + (y * Z'.y) + T.y
		// p'.z = (z * X'.z) + (z * Y'.z) + (z * Z'.z) + T.z
		// ------------------------------------------------------------
		// Where:
		//		 p' is the transformed point/vector.
		//		 x is the x component/scalar of the vector.
		//       X' is the x axis component of the Matrix4.
		//		 y is the y component/scalar of the vector.
		//       Y' is the y axis component of the Matrix4.
		//		 z is the z component/scalar of the vector.
		//       Z' is the z axis component of the Matrix4.
		//       T is the translation component of the Matrix4 (W axis).
		// ************************************************************

		Vector4 result;

		Vector4 xAx(0,0,0,0); 
		Vector4 yAx(0,0,0,0); 
		Vector4 zAx(0,0,0,0); 
		Vector4 wAx(0,0,0,0); 

		// xX' component of formula
		xAx.x = v.x * m.xX;
		xAx.y = v.x * m.xY;
		xAx.z = v.x * m.xZ;
		xAx.w = v.x * m.xW;

		// yY' component of formula
		yAx.x = v.y * m.yX;
		yAx.y = v.y * m.yY;
		yAx.z = v.y * m.yZ;
		yAx.w = v.y * m.yW;

		// zZ' component of formula
		zAx.x = v.z * m.zX;
		zAx.y = v.z * m.zY;
		zAx.z = v.z * m.zZ;
		zAx.w = v.z * m.zW;

		//
		wAx.x = v.w * m.wX;
		wAx.y = v.w * m.wY;
		wAx.z = v.w * m.wZ;
		wAx.w = v.w * m.wW;

		result.x = xAx.x + yAx.x + zAx.x + wAx.x;
		result.y = xAx.y + yAx.y + zAx.y + wAx.y;
		result.z = xAx.z + yAx.z + zAx.z + wAx.z;
		result.w = v.w;

		return result;
	}


}; // End namespace SWR