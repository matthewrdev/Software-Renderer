//****************************************************************************
//**
//**    InputHandler.cpp
//**
//**    Copyright (c) 2009 Matthew Robbins
//**
//**    Author:  Matthew Robbins
//**    Created: 09/2009 - 12/2009
//**
//****************************************************************************

#include "InputHandler.h"
#include "Math.h"
#include "Vector2.h"

// Defines for simplifying input grabbing.
#define MOUSE_DOWN(state, button) ((state.rgbButtons[button] & 0&80 ? true : false)
#define KEY_DOWN(vk_code) (GetAsyncKeyState(vk_code) && (SHORT)0x8000U ? true : false)

namespace SWR
{

	const char* GetStringRepUpper(int key)
	{
		if (key >= KEY_COUNT)
			return NULL;
		
		return keyDataArray[key].stringRepUpper;
	}

	const char* GetStringRepLower(int key)
	{
		if (key >= KEY_COUNT)
			return NULL;
		
		return keyDataArray[key].stringRepLower;
	}

	InputHandler::~InputHandler()
	{	
	}

	InputHandler::InputHandler()
		: leftDown(false)
		, rightDown(false)
		, middleDown(false)
		, leftHit(false)
		, rightHit(false)
		, middleHit(false)
		, leftDownLast(false)
		, rightDownLast(false)
		, middleDownLast(false)
		, deltaX(0.0f)
		, deltaY(0.0f)
		, wheelDelta(0.0f)
		, mouseDelta(0.0f, 0.0f)
	{
	}

	InputHandler* InputHandler::Instance()
	{
		
		static InputHandler instance;
		return &instance;
	}


	void InputHandler::UpdateInputDevices(HWND hWnd)
	{
		UpdateMouse(hWnd);
		PollKeys();
	}


	void InputHandler::SetLeftMouse(bool down)
	{
		leftDown = down;
	}

	void InputHandler::SetRightMouse(bool down)
	{
		rightDown = down;
	}

	void InputHandler::SetMiddleMouse(bool down)
	{
		middleDown = down;
	}

	bool InputHandler::IsMouseLeftDown() const
	{
		return leftDown;
	}

	bool InputHandler::IsMouseMiddleDown() const
	{
		return middleDown;
	}

	bool InputHandler::IsMouseRightDown() const
	{
		return rightDown;
	}

	bool InputHandler::IsMouseLeftHit() const
	{
		return leftHit;
	}

	bool InputHandler::IsMouseMiddleHit() const
	{
		return middleHit;
	}

	bool InputHandler::IsMouseRightHit() const
	{
		return rightHit;
	}

	float InputHandler::GetMouseDeltaX() const
	{
		return deltaX;
	}

	float InputHandler::GetMouseDeltaY() const
	{
		return deltaY;
	}

	float InputHandler::GetMouseWheelDelta() const
	{
		return wheelDelta;
	}

	void InputHandler::UpdateMouse(HWND hWnd)
	{
		static Vector2 mousePrev;
		POINT point;
		GetCursorPos(&point);
		ScreenToClient(hWnd, &point);
		mousePos.x = (float)point.x;
		mousePos.y = (float)point.y;

		mouseDelta = mousePos - mousePrev;

		mousePrev = mousePos;

		// Update the mouse hits. 
		// Windows mouse msgs only occur when the event has happened and
		// does not keep posting the event if it is already down.
		// This solution is ugly but it works.

		// ----------------------------------------------------
		//                    LEFT MOUSE CLICKS
		// ----------------------------------------------------
		
		// Update the mouse hit variable.
		if (leftDown == true && leftDownLast == false)
			leftHit = true;
		else
			leftHit = false;
		
		// Update the mouse down last frame variable.
		if (leftDown)
			leftDownLast = true;
		else
			leftDownLast = false;

		// ----------------------------------------------------
		//                   RIGHT MOUSE CLICKS
		// ----------------------------------------------------

		// Update the mouse hit variable.
		if (rightDown == true && rightDownLast == false)
			rightHit = true;
		else
			rightDown = false;

		if (rightDown == false)
			rightHit = false;
		
		// Update the mouse down last frame variable.
		if (rightDown)
			rightDownLast = true;
		else
			rightDownLast = false;

		// ----------------------------------------------------
		//                   MIDDLE MOUSE CLICKS
		// ----------------------------------------------------

		// Update the mouse hit variable.
		if (middleDown == true && middleDownLast == false)
			middleHit = true;
		else
			middleHit = false;
		
		if (middleDown == false)
			middleHit = false;
		
		// Update the mouse down last frame variable.
		if (middleDown)
			middleDownLast = true;
		else
			middleDownLast = false;
	}

	Vector2 InputHandler::GetMouseToWindow() const
	{
		return mousePos;
	}

	Vector2 InputHandler::GetMouseDelta() const
	{
		return mouseDelta;
	}

	void InputHandler::PollKeys()
	{
		for (unsigned short index = 0; index < KEY_COUNT; index++)
		{
			if (KEY_DOWN(keyDataArray[index].vk_code))
			{
				// Find if the key was already down or has just been hit
				if (keyDataArray[index].hit == true)
				{
					keyDataArray[index].hit = false;
					keyDataArray[index].down = true;
					keyDataArray[index].up = false;
				}
				// Has just been pressed so set hit && down to true
				else if (keyDataArray[index].down == false && keyDataArray[index].hit == false)
				{
					keyDataArray[index].hit = true;
					keyDataArray[index].down = true;
					keyDataArray[index].up = false;
				}
			}
			// If not down then clear all the down states to false and up to true;
			else
			{
				keyDataArray[index].hit = false;
				keyDataArray[index].down = false;
				keyDataArray[index].up = true;
			}
		}
	}

	bool InputHandler::IsKeyDown(int KeyID)
	{
		assert(KeyID < KEY_COUNT && KeyID >= 0);
		return keyDataArray[KeyID].down;
	}

	bool InputHandler::IsKeyHit(int KeyID)
	{
		assert(KeyID < KEY_COUNT && KeyID >= 0);
		return keyDataArray[KeyID].hit;
	}

	bool InputHandler::IsKeyUp(int KeyID)
	{
		assert(KeyID < KEY_COUNT && KeyID >= 0);
		return keyDataArray[KeyID].up;
	}
	

}; // End namespace SWR