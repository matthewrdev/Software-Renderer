//****************************************************************************
//**
//**    Colour32.cpp
//**
//**    Copyright (c) 2010 Matthew Robbins
//**
//**    Author:  Matthew Robbins
//**    Created: 06/2010
//**
//****************************************************************************

#include <Windows.h>

#include "Colour.h"
#include "MemoryLeak.h"

namespace SWR
{
	// Initilise the colour constants.
	// COLOUR.................                      R    G    B    A
	const Colour32 Colour32::BLACK		=  Colour32(0,   0,   0,   0);
	const Colour32 Colour32::WHITE		=  Colour32(255, 255, 255, 0);	
	const Colour32 Colour32::DARK_GREY	=  Colour32(96,  96,  96,  0);      
	const Colour32 Colour32::LIGHT_GREY	=  Colour32(192, 192, 192, 0);    
	const Colour32 Colour32::RED		=  Colour32(255, 0,   0,   0);        
	const Colour32 Colour32::BLUE		=  Colour32(0,   0,   255, 0);      
	const Colour32 Colour32::GREEN		=  Colour32(0,   255, 0,   0);        
	const Colour32 Colour32::CYAN		=  Colour32(0,   255, 255, 0);         
	const Colour32 Colour32::MAGENTA	=  Colour32(255, 0,   255, 0);       
	const Colour32 Colour32::YELLOW		=  Colour32(255, 255, 0,   0);      
	const Colour32 Colour32::DIRTY      =  Colour32(219, 0,   201, 0);

	void Colour32::Set(U8 r, U8 g, U8 b, U8 a)
	{
		R = r;
		G = g;
		B = b;
		A = a;
	}

	void Colour32::Set(const Colour32 &c)
	{
		Set(c.R, c.G, c.B, c.A);
	}

	void Colour32::FromUINT32(U32 value)
	{
		R = U8((value & RED_MASK) >> RED_BIT_SHIFT);
		G = U8((value & GREEN_MASK) >> GREEN_BIT_SHIFT);
		B = U8((value & BLUE_MASK)); // no bit shift as BLUE_BIT_SHIFT = 0.
		A = U8((value & ALPHA_MASK) >> ALPHA_BIT_SHIFT);
	}
	
	void Colour32::FromColour128(const Colour128 &c)
	{
		R = (U8)c.R;
		G = (U8)c.G;
		B = (U8)c.B;
		A = (U8)c.A;
	}

	void Colour32::operator=(const Colour32 &c)
	{
		Set(c);
	}

	Colour32 Colour32::RandomColour()
	{
		return Colour32(rand() % 256, rand() % 256, rand() % 256, 0);
	}


	void Colour128::Set(float r, float g, float b, float a)
	{
		R = r;
		G = g;
		B = b;
		A = a;
	}

	void Colour128::Set(const Colour128 &c)
	{
		Set(c.R, c.G, c.B, c.A);
	}

	void Colour128::FromUINT32(U32 value)
	{
		R = U8((value & RED_MASK) >> RED_BIT_SHIFT);
		G = U8((value & GREEN_MASK) >> GREEN_BIT_SHIFT);
		B = U8((value & BLUE_MASK)); // no bit shift as BLUE_BIT_SHIFT = 0.
		A = U8((value & ALPHA_MASK) >> ALPHA_BIT_SHIFT);
	}

	void Colour128::FromColour32(const Colour32 &c)
	{
		R = (float)c.R;
		G = (float)c.G;
		B = (float)c.B;
	}

	void Colour128::operator=(const Colour128 &c)
	{
		Set(c);
	}

	Colour128 Colour128::RandomColour()
	{
		return Colour128(rand() % 256, rand() % 256, rand() % 256, 0);
	}

	
	Colour128 Colour128::ClampColourChannels()
	{
		Colour128 c(R, G, B, A);
		c.R /= 255.0f;
		c.G /= 255.0f;
		c.B /= 255.0f;
		c.A /= 255.0f;
		return c;
	}
			
}; // End namespace SWR.
