//****************************************************************************
//**
//**    BE_Sprite.cpp
//**
//**    Copyright (c) 2009 Matthew Robbins
//**
//**    Author:  Matthew Robbins
//**    Created: 12/2009
//**
//****************************************************************************

#include <iostream>

#include "Application.h"
#include "RenderDevice.h"

#include "Sprite.h"

#include "SWR_Math.h"

#include "Texture.h"
#include "TextureManager.h"

#include "MemoryLeak.h"

namespace SWR
{
	Sprite::Sprite()
		: texture(0)
		, isBad(true)
		, position(0.0f,0.0f,0.0f)
		, rotation(0.0f)
		, rotationCentre(0.0f, 0.0f, 0.0f)
		, scaleX(1.0f)
		, scaleY(1.0f)
		, animCols(1)
		, animRows(1)
		, frameWidth(1)
		, frameHeight(1)
		, maxFrameTime(1.0f)
		, curFrameTime(0.0f)
		, loop(false)
		, startFrame(1)
		, endFrame(1)
		, curFrame(1)
		, play(false)
		, transform(true)
	{
		srcRect.top = 0;
		srcRect.left = 0;
		srcRect.bottom = 1;
		srcRect.right = 1;
	}

	Sprite::Sprite(char *file,
				  float frameTimer,
				  int width,
				  int height,
				  int frameRows,
				  int frameCols,
				  int frameWidth,
				  int frameHeight,
				  bool loops)
		: texture(0)
		, position(0.0f,0.0f,0.0f)
		, rotationCentre(0.0f, 0.0f, 0.0f)
		, rotation(0.0f)
		, scaleX(1.0f)
		, scaleY(1.0f)
		, animCols(frameCols)
		, animRows(frameRows)
		, maxFrameTime(frameTimer)
		, curFrameTime(0.0f)
		, loop(loops)
		, startFrame(1)
		, endFrame(1)
		, curFrame(1)
		, play(false)
		, transform(true)
	{
		this->width = width;
		this->height = height;
		isBad = true;

		this->LoadTexture(file, width, height);

		// The texture failed to load.
		if (isBad)
			return;


		this->frameWidth = frameWidth;
		this->frameHeight = frameHeight;
	}

	Sprite::~Sprite()
	{
		if (this->texture != NULL)
		{
			delete texture;
			texture = NULL;
		}
	}

	void Sprite::SetFrameDimensions(int width, int height)
	{
		this->frameWidth = width;
		this->frameHeight = height;
	}

	void Sprite::LoadTexture(char* file, int width, int height)
	{
		if ((file) && texture == 0)
		{
			unsigned short result = TextureManager::Instance().LoadTexture(file, texture, width, height, true);
			if (result == SWR_OK)
			{
				isBad = false;
				frameWidth = width;
				frameHeight = height;
				return;
			}
			else
				return;

		}
		
		return;
	}

	void Sprite::CorrectPosition()
	{
		correctedPosition.x = position.x - (frameWidth / 2);
		correctedPosition.y = position.y - (frameHeight / 2);
	}

	bool Sprite::IsBad() const
	{
		return isBad;
	}

	int Sprite::GetFrameHeight() const
	{
		return frameHeight;
	}

	int Sprite::GetFrameWidth() const
	{
		return frameWidth;
	}

	void Sprite::SetAnimationColumns(int cols)
	{
		animCols = cols;
	}

	void Sprite::SetAnimationRows(int rows)
	{
		animRows = rows;
	}

	void Sprite::SetAnimationTimer(float t)
	{
		maxFrameTime = t;
	}

	bool Sprite::IsPlaying() const
	{
		return play;
	}

	Vector3 Sprite::GetPosition() const
	{
		return position;
	}

	void Sprite::SetPosition(Vector3 p)
	{
		position = p;
		CorrectPosition();
		//rotationCentre.Set(this->correctedPosition);
	}

	void Sprite::SetRotationCentre(const Vector3& p)
	{
		rotationCentre.Set(p);
	}

	void Sprite::SetEndFrame(int frame)
	{
		endFrame = frame;
	}

	void Sprite::SetStartFrame(int frame)
	{
		startFrame = frame;
	}

	void Sprite::SetScaleX(float amount)
	{
		this->scaleX = amount;
	}

	void Sprite::SetScaleY(float amount)
	{
		this->scaleY = amount;
	}

	void Sprite::SetRotation(float theta)
	{
		this->rotation = ToRadian(theta);
	}

	void Sprite::Play()
	{
		play = true;
	}

	void Sprite::Stop()
	{
		play = false;
		curFrameTime = 0.0f;
		curFrame = 1;
	}

	void Sprite::Pause()
	{
		play = false;
		// Pause holds the current frame & current frame time.
	}

	// FIx this...
	bool Sprite::IsPaused()
	{
		if (!play) // Is currently playing.
			return false;

		// The animation is stopped if the curFrameTime is 0 and curFrame is 1.
		if (curFrameTime <= 0.0f && curFrame == 1)
			return false;

		// Must be paused as the frame timer is > 0 &&  but is not playing
		return true;
	}

	float Sprite::GetRotation() const
	{
		return rotation;
	}

	float Sprite::GetScaleX() const
	{
		return scaleX;
	}

	float Sprite::GetScaleY() const
	{
		return scaleY;
	}

	void Sprite::SetLoop(bool loop)
	{
		this->loop = loop;
	}

	bool Sprite::IsLooping() const
	{
		return loop;
	}

	void Sprite::SetTransformToCamera(bool enable)
	{
		transform = enable;
	}

	bool Sprite::IsTransformingToCamera() const
	{
		return transform;
	}

	const char* Sprite::GetImageFile() const
	{
		return texture->GetFileName().c_str();
	}

	void Sprite::CalculateTextureCoords()
	{
		srcRect.left = ((curFrame-1) % animCols) * frameWidth; 
		srcRect.top =  ((curFrame-1) / animCols) * frameHeight; 
		srcRect.right = srcRect.left + frameWidth;
		srcRect.bottom = srcRect.top + frameHeight;
	}

	void Sprite::UpdateAnimation(float t)
	{
		if (!play)
			return;

		curFrameTime += t;
		if (curFrameTime >= maxFrameTime)
		{
			curFrameTime = 0.0f;
			this->curFrame++;
			if (curFrame > endFrame)
			{
				if (loop)
					curFrame = startFrame;
				else
					play = false;
			}
		}
	}

	void Sprite::SetAnimationFrames(int start, int end)
	{
		this->SetStartFrame(start);
		this->SetEndFrame(end);
	}

	void Sprite::SetCurrentFrame(int frame)
	{
		this->curFrame = frame;
		this->curFrameTime = 0.0f;
	}

	void Sprite::Render(float t)
	{
		UpdateAnimation(t);
		CalculateTextureCoords();

		Application::Instance().GetRenderDevice()->DrawTexture2D((U16)correctedPosition.x, (U16)correctedPosition.y, this->texture, &srcRect);
	}

	void Sprite::Render(float t, U32 alphaFilter)
	{
		UpdateAnimation(t);
		CalculateTextureCoords();

		Application::Instance().GetRenderDevice()->DrawTexture2D((U16)correctedPosition.x, (U16)correctedPosition.y, this->texture, &srcRect, alphaFilter);
	}

}; // End namespace SWR.
