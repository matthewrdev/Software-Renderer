#pragma once

#ifndef SWR_MATH_H
#define SWR_MATH_H

//****************************************************************************
//**
//**    SWR_Math.h
//**
//**    Copyright (c) 2010 Matthew Robbins
//**
//**    Author:  Matthew Robbins
//**    Created: 06/2010
//**
//****************************************************************************

#include <cmath>

namespace SWR
{
	const float PI = 3.1415926f;
	const float EPSILON = 1e-4;
	const float RADIANS_PER_DEGREE = PI/180.0f; 
	const float DEGREES_PER_RADIAN = 180.0f/PI;

	inline float cotan(float x)
	{
		return cos(x) / sin(x);
	}

	template<typename T>
	inline T Clamp(T min, T max, T value)
	{
		if (value < min)
			return min;

		if (value > max)
			return max;

		return value;
	}

	template<typename T>
	inline T LERP(T start, T end, float t)
	{
		return start + ((end - start) * t);
	}

	inline float ToDegree(float theta)
	{
		return theta * DEGREES_PER_RADIAN;
	}

	inline float ToRadian(float theta)
	{
		return theta * RADIANS_PER_DEGREE;
	}

	inline bool Equal(float f1, float f2)
	{
		if (fabs(f1 - f1) < EPSILON)
			return true;
		return false;
	}

	
	void SinCos(float &s, float &c, float theta);

	
}; // End namespace SWR.

#endif // #ifndef SWR_MATH_H