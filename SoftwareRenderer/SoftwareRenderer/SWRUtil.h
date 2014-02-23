#pragma once

#ifndef SWR_UTIL_H
#define SWR_UTIL_H

//****************************************************************************
//**
//**    SWRUtil.h
//**
//**    Copyright (c) 2010 Matthew Robbins
//**
//**    Author:  Matthew Robbins
//**    Created: 06/2010
//**
//****************************************************************************

#include "DataTypes.h"

namespace SWR
{
	template<typename T>
	inline void Swap(T& lhs, T& rhs)
	{
		T temp = rhs;
		rhs = lhs;
		lhs = temp;
	}

	inline U32 U32NextPow2(U32 val)
	{
		val--;
		val |= val >> 1;
		val |= val >> 2;
		val |= val >> 4;
		val |= val >> 8;
		val |= val >> 16;
		val++;
		return val;
	}

	// This cast for a float to a byte is from.
	// http://www.devmaster.net/forums/showthread.php?t=10153
	// Not entirely sure on how it works.
	// I am using this for my colour combining rather than straight casts.
	// Note: This is SLOWER than a straight cast. Funny that hey?
	inline U8 Flt2Byte (float a)
	{
	  float x = a + 256.0f;
	  return ((*(int*)&x)&0x7fffff)>>15;
	}
	
}; // End namespace SWR.

#endif // #ifndef SWR_UTIL_H