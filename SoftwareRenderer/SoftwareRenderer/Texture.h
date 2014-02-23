#pragma once

#ifndef TEXTURE_H
#define TEXTURE_H

//****************************************************************************
//**
//**    Texture.h
//**
//**    Copyright (c) 2010 Matthew Robbins
//**
//**    Author:  Matthew Robbins
//**    Created: 07/2010
//**
//****************************************************************************

#include <string>

#include "DataTypes.h"

namespace SWR
{
	// ------------------------------------------------------------------------
	//								Texture
	// ------------------------------------------------------------------------
	// Desc:
	// 
	// ------------------------------------------------------------------------
	class Texture
	{
	private:
		std::string m_file;
		U16 m_width, m_height;

		U8* m_bytes;

		friend class TextureManager;
	protected:
	public:
		Texture();
		~Texture();

		std::string GetFileName() const;
		U16 GetWidth() const;
		U16 GetHeight() const;
		U8* GetBytes();
	};
	
}; // End namespace SWR.

#endif // #ifndef TEXTURE_H