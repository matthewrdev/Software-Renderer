//****************************************************************************
//**
//**    TextureManager.cpp
//**
//**    Copyright (c) 2010 Matthew Robbins
//**
//**    Author:  Matthew Robbins
//**    Created: 07/2010
//**
//****************************************************************************

#include <Windows.h>

#include "TextureManager.h"
#include "Texture.h"

#include "Colour.h"

#include "BMPLoader.h"

#include "Logger.h"
#include "MemoryLeak.h"

namespace SWR
{
	// Bitmap loading structs.
	typedef struct 
	{
		WORD bfType;
		DWORD bfSize;
		DWORD bfReserved;
		DWORD bfOffBits;
	} BITMAPFILEHEADER;

	//BITMAPINFOHEADER
	typedef struct 
	{
		DWORD biSize;
		LONG biWidth;
		LONG biHeight;
		WORD biPlanes;
		WORD biBitCount;
		DWORD biCompression;
		DWORD biSizeImage;
		LONG biXPelsPerMeter;
		LONG biYPelsPerMeter;
		DWORD biClrUsed;
		DWORD biClrImportant;
	} BITMAPINFOHEADER;

	TextureManager::TextureManager()
	{
	}

	TextureManager::~TextureManager()
	{
	}

	TextureManager& TextureManager::Instance()
	{
		static TextureManager instance;
		return instance;
	}

	Texture* TextureManager::CreateDirtyTexture(int width, int height)
	{
		Texture* tex = new Texture();
		size_t size = width * height * 4;
		tex->m_bytes = (U8*)malloc(size);
		U8* bytes = tex->m_bytes;

		U32 dirtyCol = Colour32::DIRTY.ToUINT32();

		// Copy the 
		for (int i = 0; i < size; i+=4)
		{
			memcpy(&(bytes[i]), &dirtyCol, 4);
		}

		tex->m_height = height;
		tex->m_width = width;
		tex->m_file = "INVALID FILE";

		return tex;
	}

	Texture* TextureManager::AlignTo32BitTexture(U8* bytes, const char* filename, int width, int height, bool flip)
	{
		Texture* tex = new Texture();
		size_t sizeSource = width * height * 3; // 3 bytes as it is 24 bit.
		size_t sizeTarget = width * height * 4; // 4 bytes as our target is 32 bit.

		// Allocate our new texture buffer.
		tex->m_bytes = (U8*)malloc(sizeTarget);
		U8* targetBuffer = tex->m_bytes;

		// The current byte index for our
		U32 byteIndex = 0;

		// Iterate over our input byte buffer and copy over the pixels.
		for (unsigned int i = 0; i < sizeSource; i+=3)
		{
			targetBuffer[byteIndex] = bytes[i + 2];      // Red channel.
			targetBuffer[byteIndex + 1] = bytes[i + 1];  // Green channel.
			targetBuffer[byteIndex + 2] = bytes[i];      // Blue channel.
			targetBuffer[byteIndex + 3] = 0;             // Alpha channel.

			byteIndex += 4;
		}

		// Flip the textures Y axis.
		if (flip)
		{
			U8* flippedTex = FlipTextureYAxis(tex->m_bytes, width, height);

			// Release the old buffer.
			delete [] tex->m_bytes;
			tex->m_bytes = flippedTex;
		}


		tex->m_file = filename;
		tex->m_height = height;
		tex->m_width = width;

		return tex;
	}

	U8* TextureManager::FlipTextureYAxis(U8* texture, int width, int height)
	{
		// Allocate a new buffer.
		U8* buffer = (U8*)malloc(width * height * 4);

		U16 targetRow = 0;
		U16 lineSpan = width << 2;

		for (U16 sourceRow = height - 1; sourceRow > 0; sourceRow--)
		{
			memcpy(&(buffer[width * targetRow * 4]), &(texture[width * sourceRow * 4]), lineSpan);
			targetRow++;
		}

		return buffer;
	}

	SWR_ERR TextureManager::LoadTexture(const char* filename, Texture* &target, int width, int height, bool flip)
	{
		BMPLoader::BMPClass* bitmap = new BMPLoader::BMPClass();
		char result = BMPLoader::BMPLoad(std::string(filename), *bitmap);
		if (result != BMPNOERROR)
		{
			char buffer[256] = {0};
			switch (result)
			{	
			case BMPNOTABITMAP:
				sprintf(buffer, "%s texture failed to load. \nFile format is not a bitmap.", filename);
				break;
			case BMPUNKNOWNFORMAT:
				sprintf(buffer, "%s texture failed to load. \nUnknown file format.", filename);
				break;
			case BMPBADINT:
			case BMPFILEERROR:
			case BMPNOOPEN:
			default:
				sprintf(buffer, "%s texture failed to load. \nRequested file failed to open.", filename);
				break;
			};

			LOG(buffer, LOG_Warning);

			target = CreateDirtyTexture(width, height);

			// Clean up after our selves.
			delete bitmap;

			return SWR_FAIL;
		}
		else
		{
			// Great success! Now rebuild the texture so that it is in 32 bit format.
			target = this->AlignTo32BitTexture(bitmap->bytes, filename, width, height, flip);

			// Clean up after our selves.
			delete bitmap;
		}

		return SWR_OK;
	}
	
}; // End namespace SWR.
