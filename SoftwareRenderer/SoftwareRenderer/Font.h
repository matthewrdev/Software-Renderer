#pragma once

#ifndef FONT_H
#define FONT_H

//****************************************************************************
//**
//**    Font.h
//**
//**    Copyright (c) 2010 Matthew Robbins
//**
//**    Author:  Matthew Robbins
//**    Created: 01/2010
//**
//****************************************************************************

#include "Vector3.h"

// Forward Declarations
namespace SWR
{
	class Sprite;
};

namespace SWR
{

	// The current font size stored in the class is divided by this to get a scale value for the sprite.
	const unsigned int DEFAULT_FONT_SIZE = 12;
	class Font
	{
	private:
		Sprite* sprite;
		unsigned char fontSize;
	public:
		Font(char* file, int frameWidth, int frameHeight, int width, int height);
		~Font();

		void PrintText(const char* text, Vector3 start);
		void SetFontSize(unsigned char size);

		int GetFrameWidth() const;
		int GetFrameHeight() const;
	};
	
}; // End namespace SWR.

#endif // #ifndef _FONT_H