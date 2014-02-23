#pragma once

#ifndef COLOUR_H
#define COLOUR_H

//****************************************************************************
//**
//**    Colour32.h
//**
//**    Copyright (c) 2010 Matthew Robbins
//**
//**    Author:  Matthew Robbins
//**    Created: 06/2010
//**
//****************************************************************************

#include "DataTypes.h"

#define ALPHA_MASK	0xFF000000
#define RED_MASK	0x00FF0000
#define GREEN_MASK	0x0000FF00
#define BLUE_MASK   0x000000FF

#define ALPHA_BIT_SHIFT  24
#define RED_BIT_SHIFT	 16
#define GREEN_BIT_SHIFT  8
#define BLUE_BIT_SHIFT   0

namespace SWR
{
	static const int COLOUR_SIZE = 4;

	// Constants for accessing the different colour bytes if a colour is stored as an array.
	static const int ALPHA_BYTE  = 0;
	static const int RED_BYTE    = 1;
	static const int GREEN_BYTE  = 2;
	static const int BLUE_BYTE   = 3;

	class Colour128;

	// ------------------------------------------------------------------------
	//								Colour32
	// ------------------------------------------------------------------------
	// Desc:
	// A 4 byte colour class used to represent a 32 bit colour including an 
	// alpha channel. Can be easily flattened into a 32 bit integer for colour
	// copying into a buffer.
	// ------------------------------------------------------------------------
	class Colour32
	{
	private:
	protected:
	public:
		U8 R, G, B, A;

		Colour32(){} // Default constructor with no initilisation.

		Colour32(U8 r, U8 g, U8 b, U8 a)
			: R(r), G(g), B(b), A(a)
		{	}

		Colour32(const Colour32 &c)
		{
			Set(c);
		}

		~Colour32(){}

		void Set(U8 r, U8 g, U8 b, U8 a);
		void Set(const Colour32 &c);

		inline U32 ToUINT32() const
		{
			return (0 | U32(R << RED_BIT_SHIFT) | U32(G << GREEN_BIT_SHIFT) | U32(B));
		}

		void FromUINT32(U32 value);
		void FromColour128(const Colour128 &c);

		void operator=(const Colour32 &c);

		static Colour32 RandomColour();


		// Sets of colour constants to use throughout the program if need be.
		// COLOUR.................               R    G    B
		static const Colour32 BLACK;	      // 0    0    0
		static const Colour32 WHITE;		  // 255  255  255
		static const Colour32 DARK_GREY;      // 96   96   96   
		static const Colour32 LIGHT_GREY;     // 192  192  192
		static const Colour32 RED;            // 255  0    0
		static const Colour32 BLUE;           // 0    0    255
		static const Colour32 GREEN;          // 0    255  0
		static const Colour32 CYAN;           // 0    255  255
		static const Colour32 MAGENTA;        // 255  0    255
		static const Colour32 YELLOW;         // 255  255  0
		static const Colour32 DIRTY;		  // 219  0    201

	};

	inline bool operator==(const Colour32 &lhs, const Colour32 &rhs)
	{
		return (lhs.R == rhs.R && lhs.G == rhs.G && lhs.B == rhs.B && lhs.A == rhs.A);
	}

	inline Colour32 operator+(const Colour32 &lhs, const Colour32 &rhs)
	{
		return Colour32(lhs.R + rhs.R, lhs.G + rhs.G, lhs.B + rhs.B, lhs.A + rhs.A);
	}
	
	inline Colour32 operator-(const Colour32 &lhs, const Colour32 &rhs)
	{
		return Colour32(lhs.R - rhs.R, lhs.G - rhs.G, lhs.B - rhs.B, lhs.A - rhs.A);
	}

	inline Colour32 operator*(const Colour32 &lhs, const Colour32 &rhs)
	{
		return Colour32(lhs.R * rhs.R, lhs.G * rhs.G, lhs.B * rhs.B, lhs.A * rhs.A);
	}

	inline Colour32 operator/(const Colour32 &lhs, const Colour32 &rhs)
	{
		return Colour32(lhs.R / rhs.R, lhs.G / rhs.G, lhs.B / rhs.B, lhs.A / rhs.A);
	}

	inline Colour32 operator+(const Colour32 &lhs, U8 rhs)
	{
		return Colour32(lhs.R + rhs, lhs.G + rhs, lhs.B + rhs, 0); // Alpha channel is ignored.
	}
	
	inline Colour32 operator-(const Colour32 &lhs, U8 rhs)
	{
		return Colour32(lhs.R - rhs, lhs.G - rhs, lhs.B - rhs, 0); // Alpha channel is ignored.
	}

	inline Colour32 operator*(const Colour32 &lhs, U8 rhs)
	{
		return Colour32(lhs.R * rhs, lhs.G * rhs, lhs.B * rhs, 0); // Alpha channel is ignored.
	}

	inline Colour32 operator/(const Colour32 &lhs, U8 rhs)
	{
		return Colour32(lhs.R / rhs, lhs.G / rhs, lhs.B / rhs, 0); // Alpha channel is ignored.
	}

	
	// ------------------------------------------------------------------------
	//								Colour128
	// ------------------------------------------------------------------------
	// Desc:
	// A 16 byte colour class used to represent a 32 bit colour including an 
	// alpha channel. Colour32 channels are represented in float so that they
	// are scalable && lerpable.
	// Colour channels range between 0 to 255.
	// ------------------------------------------------------------------------
	class Colour128
	{
	private:
	protected:
	public:
		float R, G, B, A;

		Colour128(){} // Default constructor with no initilisation.

		Colour128(float r, float g, float b, float a)
			: R(r), G(g), B(b), A(a)
		{	}

		Colour128(const Colour128 &c)
		{
			Set(c);
		}

		~Colour128(){}

		void Set(float r, float g, float b, float a);
		void Set(const Colour128 &c);
		void FromColour32(const Colour32 &c);
		void ToColour32(Colour32 &c);

		inline U32 ToUINT32() const
		{
			return (U32((int)A << ALPHA_BIT_SHIFT) | U32((int)R << RED_BIT_SHIFT) | U32((int)G << GREEN_BIT_SHIFT) | U32(B));
		}

		void FromUINT32(U32 value);

		void operator=(const Colour128 &c);

		Colour128 ClampColourChannels();

		static Colour128 RandomColour();
	};

	inline bool operator==(const Colour128 &lhs, const Colour128 &rhs)
	{
		return (lhs.R == rhs.R && lhs.G == rhs.G && lhs.B == rhs.B && lhs.A == rhs.A);
	}

	inline Colour128 operator+(const Colour128 &lhs, const Colour128 &rhs)
	{
		return Colour128(lhs.R + rhs.R, lhs.G + rhs.G, lhs.B + rhs.B, lhs.A + rhs.A);
	}
	
	inline Colour128 operator-(const Colour128 &lhs, const Colour128 &rhs)
	{
		return Colour128(lhs.R - rhs.R, lhs.G - rhs.G, lhs.B - rhs.B, lhs.A - rhs.A);
	}

	inline Colour128 operator*(const Colour128 &lhs, const Colour128 &rhs)
	{
		return Colour128(lhs.R * rhs.R, lhs.G * rhs.G, lhs.B * rhs.B, lhs.A * rhs.A);
	}

	inline Colour128 operator/(const Colour128 &lhs, const Colour128 &rhs)
	{
		return Colour128(lhs.R / rhs.R, lhs.G / rhs.G, lhs.B / rhs.B, lhs.A / rhs.A);
	}

	inline Colour128 operator+(const Colour128 &lhs, float rhs)
	{
		return Colour128(lhs.R + rhs, lhs.G + rhs, lhs.B + rhs, 0); // Alpha channel is ignored.
	}
	
	inline Colour128 operator-(const Colour128 &lhs, float rhs)
	{
		return Colour128(lhs.R - rhs, lhs.G - rhs, lhs.B - rhs, 0); // Alpha channel is ignored.
	}

	inline Colour128 operator*(const Colour128 &lhs, float rhs)
	{
		return Colour128(lhs.R * rhs, lhs.G * rhs, lhs.B * rhs, 0); // Alpha channel is ignored.
	}

	inline Colour128 operator/(const Colour128 &lhs, float rhs)
	{
		return Colour128(lhs.R / rhs, lhs.G / rhs, lhs.B / rhs, 0); // Alpha channel is ignored.
	}
	
}; // End namespace SWR.

#endif // #ifndef COLOUR_H