//****************************************************************************
//**
//**    BE_Font.cpp
//**
//**    Copyright (c) 2010 Matthew Robbins
//**
//**    Author:  Matthew Robbins
//**    Created: 01/2010
//**
//****************************************************************************

#include <iostream>

#include "Font.h"

#include "Sprite.h"
#include "Colour.h"

#include "MemoryLeak.h"

namespace SWR
{

	Font::Font(char* file, int frameWidth, int frameHeight, int width, int height)
		: fontSize(12)
	{
		// Font size cannot be changed...
		sprite = new Sprite(file,  0.0f, width, height,16, 16, frameWidth, frameHeight, false);
		sprite->SetTransformToCamera(false);
	}

	Font::~Font()
	{
		if (sprite)
		{
			delete sprite;
			sprite = 0;
		}
	}

	void Font::PrintText(const char *text, Vector3 start)
	{
		int length = strlen(text);
		int character = 0;
		Vector3 pos(start);
		if (length <= 0)
			return;
		for (int index = 0; index < length; index++)
		{
			character = text[index] + 1;
			sprite->SetCurrentFrame(character);
			sprite->SetPosition(pos);
			sprite->Render(0.0f, Colour32::BLACK.ToUINT32());

			pos.x += sprite->GetFrameWidth() - sprite->GetFrameWidth()  * 0.3f;
		}
	}

	void Font::SetFontSize(unsigned char size)
	{
		fontSize = size;

		// Convert to a scale ratio between for the sprite.
		sprite->SetScaleX((float)(fontSize / DEFAULT_FONT_SIZE));
		sprite->SetScaleY((float)(fontSize / DEFAULT_FONT_SIZE));
	}

	int Font::GetFrameWidth() const
	{
		return sprite->GetFrameWidth();
	}

	int Font::GetFrameHeight() const
	{
		return sprite->GetFrameHeight();
	}
	
}; // End namespace SWR.
