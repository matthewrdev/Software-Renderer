#pragma once

#ifndef TEXTURE_MANAGER_H
#define TEXTURE_MANAGER_H

//****************************************************************************
//**
//**    TextureManager
//**
//**    Copyright (c) 2010 Matthew Robbins
//**
//**    Author:  Matthew Robbins
//**    Created: 07/2010
//**
//****************************************************************************

#include "DataTypes.h"

namespace SWR
{
	class Texture;
};

namespace SWR
{
	// ------------------------------------------------------------------------
	//								TextureManager
	// ------------------------------------------------------------------------
	// Desc:
	// This is a simple manager class to govern the loading and lifetime of
	// texture resources. It uses the BMPLoader functionality taken from:
	// http://users.ox.ac.uk/~orie1330/bmploader.html
	// On top of the loader, there is functionality that converts bitmaps to 32 
	// bit colours so I can use them natively within the renderer.
	// ------------------------------------------------------------------------
	class TextureManager
	{
	private:
		TextureManager();
		~TextureManager();

		// Builds a dirty texture.
		Texture* CreateDirtyTexture(int width, int height);

		// Takes a 24bit bitmap and re-creates it so that it fits into a standard texture used for 
		Texture* AlignTo32BitTexture(U8* bytes, const char* filename, int width, int height, bool flip);

		// Flips the texture so that the Y axis will run from top to bottom.
		U8* FlipTextureYAxis(U8* texture, int width, int height);

	protected:
	public:
		
		static TextureManager& Instance();

		// Loads a bitmap from disk. If the bitmap is 24 bit, Will add 8 bytes of padding so that
		// when they are used for rendering we have a pow2 for clean pixel copying and
		// it will also align into memory better.
		SWR_ERR LoadTexture(const char* filename, Texture* &target, int width, int height, bool flip);

	};
	
}; // End namespace SWR.

#endif // #ifndef TEXTURE_MANAGER_H