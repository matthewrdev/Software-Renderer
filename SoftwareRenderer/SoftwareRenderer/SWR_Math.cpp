//****************************************************************************
//**
//**    SWR_Math.cpp
//**
//**    Copyright (c) 2010 Matthew Robbins
//**
//**    Author:  Matthew Robbins
//**    Created: 06/2010
//**
//****************************************************************************

#include "SWR_Math.h"

#include <cmath>

namespace SWR
{
	
	void SinCos(float &s, float &c, float theta)
	{
		s = sin((float)theta);
		c = cos((float)theta);
	}
	
}; // End namespace SWR.