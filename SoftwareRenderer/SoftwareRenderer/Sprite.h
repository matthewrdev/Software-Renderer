#ifndef _SPRITE_H
#define _SPRITE_H

//****************************************************************************
//**
//**    BE_Sprite.h
//**
//**    Copyright (c) 2009 Matthew Robbins
//**
//**    Author:  Matthew Robbins
//**    Created: 12/2009
//**
//****************************************************************************

#include "Vector3.h"
#include "Rectangle.h"
#include "DataTypes.h"

// Forward Declarations
namespace SWR
{
	class Texture;
};

namespace SWR
{
	class Sprite
	{
	private:
		Texture* texture;

		// If the sprite has not been loaded / texture failed to load.
		bool isBad;

		// If the sprite is to be transformed from world space into screen space.
		bool transform;

		Vector3 position;
		Vector3 correctedPosition; // The corrected position of the sprite 

		Vector3 rotationCentre;

		// Rotation data (in degrees).
		float rotation;

		// Scale data
		float scaleX;
		float scaleY;

		// --------------------------------------------
		//                Animation data.
		// --------------------------------------------
		// The dimensions of the sprite sheet.
		int animCols;
		int animRows;

		// Frame dimensions.
		int frameWidth;
		int frameHeight;

		int width, height;

		// Source rectangle for the sprite sheet.
		Rectangle srcRect;

		// The amount of time to sit on a single frame for before proceeding.
		float maxFrameTime;
		// Current amount of time spent of the frame.
		float curFrameTime;

		// If the animation is to loop.
		bool loop;

		// The start and end frames for the loop.
		int startFrame;
		int endFrame;

		// The current frame we are drawing from.
		int curFrame;

		// If we are to animate.
		bool play;

		// Transforms the 
		void Transform();

		// Updates the animations frames.
		void UpdateAnimation(float t);

		// Calculates the values for the srcRect used for rendering.
		void CalculateTextureCoords();

		// Corrects the position inputed so that it appears correctly on screen.
		void CorrectPosition();
	public:

		Sprite();
		Sprite(char* file,
			   float frameTimer,
			   int width, int height,
			   int frameRows,
			   int frameCols,
			   int frameWidth, 
			   int frameHeight,
			   bool loops);

		~Sprite();

		// Texture has its own custom error checking when loading so this does not return sucess or failure.
		void LoadTexture(char* file, int width, int height);
		bool IsBad() const;

		// Transforms and draws the sprite.
		// 't' is used to update the animation cycle.
		void Render(float t);
		void Render(float t, U32 alphaFilter);

		int GetFrameWidth() const;
		int GetFrameHeight() const;

		void SetAnimationFrames(int start, int end);
		void SetStartFrame(int frame);
		void SetEndFrame(int frame);
		void SetCurrentFrame(int frame);

		void SetFrameDimensions(int width, int height);

		void SetAnimationColumns(int cols);
		void SetAnimationRows(int rows);

		void SetAnimationTimer(float t);

		void SetLoop(bool loop);
		bool IsLooping() const;

		void Play();
		void Stop();
		void Pause();

		bool IsPlaying() const;
		bool IsPaused();

		void SetScaleX(float amount);
		void SetScaleY(float amount);

		float GetScaleX() const;
		float GetScaleY() const;

		void SetRotation(float theta);
		float GetRotation() const;

		void SetPosition(Vector3 p);
		Vector3 GetPosition() const;

		// This must be set in a local space position.
		void SetRotationCentre(const Vector3& p);

		void SetTransformToCamera(bool enable);
		bool IsTransformingToCamera() const;

		const char* GetImageFile() const;

	};
	
}; // End namespace SWR.

#endif // #ifndef _SPRITE_H