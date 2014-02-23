//****************************************************************************
//**
//**    Texture.cpp
//**
//**    Copyright (c) 2010 Matthew Robbins
//**
//**    Author:  Matthew Robbins
//**    Created: 07/2010
//**
//****************************************************************************

#include "Texture.h"

namespace SWR
{
	Texture::Texture()
	{
		m_bytes = NULL;
	}

	Texture::~Texture()
	{
		if (m_bytes != NULL)
		{
			delete [] m_bytes;
			m_bytes = NULL;
		}
	}

	std::string Texture::GetFileName() const
	{
		return m_file;
	}

	U16 Texture::GetWidth() const
	{
 		return m_width;
	}

	U16 Texture::GetHeight() const
	{
		return m_height;
	}

	U8* Texture::GetBytes()
	{
		return m_bytes;
	}
}; // End namespace SWR.