#pragma once

#ifndef INPUT_HANDLER_H
#define INPUT_HANDLER_H

//****************************************************************************
//**
//**    InputHandler.h
//**
//**    Copyright (c) 2009 Matthew Robbins
//**
//**    Author:  Matthew Robbins
//**    Created: 09/2009 - 12/2009
//**
//****************************************************************************

#include <windows.h>
#include <windowsx.h>

#include <assert.h>

#include "Vector2.h"

namespace SWR
{

	// --------------------------------------------------------------------------------

	// Struct that holds whether keys are hit, up or down.
	struct KeyState
	{
	public:
		bool hit, up, down; 
		const int vk_code;
		const char* stringRepUpper;
		const char* stringRepLower;
	};

	const char* GetStringRep(int key);

	// Key Mappings.
	static KeyState keyDataArray[] = 
     {
        { false, false, false, VK_ESCAPE,     "Esc",           "Esc"     },     // KEY_ESCAPE
      
        { false, false, false, VK_F1,         "F1",            "F1"     },      // KEY_F1
        { false, false, false, VK_F2,         "F2",            "F2"     },      // KEY_F2
        { false, false, false, VK_F3,         "F3" ,           "F3"     },      // KEY_F3
        { false, false, false, VK_F4,         "F4",            "F4"     },		// KEY_F4
        { false, false, false, VK_F5,         "F5",            "F5"     },		// KEY_F5
        { false, false, false, VK_F6,         "F6",            "F6"     },		// KEY_F6
        { false, false, false, VK_F7,         "F7",            "F7"     },		// KEY_F7
        { false, false, false, VK_F8,         "F8",            "F8"     },		// KEY_F8 
        { false, false, false, VK_F9,         "F9",            "F9"     },		// KEY_F9
        { false, false, false, VK_F10,        "F10",           "F10"     },		// KEY_F10
        { false, false, false, VK_F11,        "F11",           "F11"     },		// KEY_F11
        { false, false, false, VK_F12,        "F12",           "F12"      },	// KEY_F12

        { false, false, false, VK_SNAPSHOT,   "PrintScreen",   "PrintScreen"},	// KEY_PRINT_SCREEN, 
        { false, false, false, VK_SCROLL,     "Scroll",        "Scroll"   },	// KEY_SCROLL

        { false, false, false, VK_OEM_3,      "`",             "`"     },		// KEY_APOSTROPHE
        { false, false, false, VK_TAB,        "Tab" ,          "Tab"   },		// KEY_TAB
        { false, false, false, VK_CAPITAL,	  "CapsLock",      "CapsLock" },	// KEY_CAPS
        { false, false, false, VK_LSHIFT,     "LShift",        "LShift"    },	// KEY_LSHIFT
        { false, false, false, VK_LCONTROL,   "LCtrl",         "LCtrl"    },	// KEY_LCTRL
        { false, false, false, VK_LMENU,      "LAlt",          "LAlt"    },		// KEY_LALT 
        { false, false, false, VK_SPACE,      " ",             " "    },		// KEY_SPACE 
        { false, false, false, VK_RSHIFT,     "RShift",        "RShift"    },	// KEY_RSHIFT
        { false, false, false, VK_RCONTROL,   "RCtrl",         "RCtrl"     },	// KEY_RCTRL
        { false, false, false, VK_RMENU,      "RAlt",          "RAlt"      },	// KEY_RALT, 
        { false, false, false, VK_OEM_MINUS,  "-",             "-"      },		// KEY_SUBTRACT
        { false, false, false, VK_OEM_PLUS,   "=",             "="      },		// KEY_EQUALS
        { false, false, false, VK_OEM_5,      "\\",            "\\"      },		// KEY_BACKSLASH
        { false, false, false, VK_BACK,       "Backspace",     "Backspace" },	// KEY_BACKSPACE
        { false, false, false, VK_RETURN,     "Enter",         "Enter"    },	// KEY_ENTER

        { false, false, false, VK_OEM_4,      "[",             "["        },	// KEY_LSQUARE_BRACKET
        { false, false, false, VK_OEM_6,      "]",             "]"        },	// KEY_RSQUARE_BRACKET
        { false, false, false, VK_OEM_1,      ";",             ";"        },	// KEY_SEMICOLON
        { false, false, false, VK_OEM_7,      "'",             "'"      },		// KEY_QUOTE
        { false, false, false, VK_OEM_COMMA,  ",",             ","      },		// KEY_COMMA
        { false, false, false, VK_OEM_PERIOD, ".",             "."      },		// KEY_FULLSTOP
        { false, false, false, VK_OEM_2,      "/",             "/"      },		// KEY_FORWARD_SLASH

        { false, false, false, '1',           "1",             "1"      },		// KEY_1
        { false, false, false, '2',           "2",             "2"      },		// KEY_2
        { false, false, false, '3',           "3",             "3"      },		// KEY_3
        { false, false, false, '4',           "4",             "4"     },		// KEY_4
        { false, false, false, '5',           "5",             "5"      },		// KEY_5
        { false, false, false, '6',           "6",             "6"      },		// KEY_6
        { false, false, false, '7',           "7",             "7"      },		// KEY_7
        { false, false, false, '8',           "8",             "8"      },		// KEY_8
        { false, false, false, '9',           "9",             "9"      },		// KEY_9
        { false, false, false, '0',           "0",             "0"      },		// KEY_0

        { false, false, false, 'A',           "A",             "a"      },		// KEY_A
        { false, false, false, 'B',           "B",             "b"      },		// KEY_B
        { false, false, false, 'C',           "C",             "c"      },		// KEY_C
        { false, false, false, 'D',           "D",             "d"      },		// KEY_D
        { false, false, false, 'E',           "E",             "e"      },		// KEY_E
        { false, false, false, 'F',           "F",             "f"      },		// KEY_F
        { false, false, false, 'G',           "G",             "g"      },		// KEY_G
        { false, false, false, 'H',           "H",             "h"      },		// KEY_H
        { false, false, false, 'I',           "I",             "i"      },		// KEY_I
        { false, false, false, 'J',           "J",             "j"      },		// KEY_J
        { false, false, false, 'K',           "K",             "k"      },		// KEY_K
        { false, false, false, 'L',           "L",             "l"      },		// KEY_L
        { false, false, false, 'M',           "M",             "m"      },		// KEY_M
        { false, false, false, 'N',           "N",             "n"      },		// KEY_N
        { false, false, false, 'O',           "O",             "o"      },		// KEY_O
        { false, false, false, 'P',           "P",             "p"      },		// KEY_P
        { false, false, false, 'Q',           "Q",             "q"      },		// KEY_Q
        { false, false, false, 'R',           "R",             "r"      },		// KEY_R
        { false, false, false, 'S',           "S",             "s"      },		// KEY_S
        { false, false, false, 'T',           "T",             "t"      },		// KEY_T
        { false, false, false, 'U',           "U",             "u"      },		// KEY_U
        { false, false, false, 'V',           "V",             "v"      },		// KEY_V
		{ false, false, false, 'W',           "W",             "w"      },		// KEY_W
        { false, false, false, 'X',           "X",             "x"      },		// KEY_X
        { false, false, false, 'Y',           "Y",             "y"      },		// KEY_Y
        { false, false, false, 'Z',           "Z",             "z"      },		// KEY_Z

        { false, false, false, VK_INSERT,     "Insert",        "Insert"  },		// KEY_INSERT
        { false, false, false, VK_HOME,       "Home",          "Home"  },		// KEY_HOME
        { false, false, false, VK_PRIOR,      "PageUp",        "PageUp" },		// KEY_PAGE_UP, 
        { false, false, false, VK_DELETE,     "Delete",        "Delete"  },		// KEY_DELETE
        { false, false, false, VK_END,        "End",           "End"  },		// KEY_END
        { false, false, false, VK_NEXT,       "PageDown",      "PageDown"  },	// KEY_PAGE_DOWN,

        { false, false, false, VK_LEFT,       "Left",          "Left"   },		// KEY_LEFT
        { false, false, false, VK_UP,         "Up",            "Up"    },		// KEY_UP
        { false, false, false, VK_RIGHT,      "Right",         "Right"    },	// KEY_RIGHT
        { false, false, false, VK_DOWN,       "Down",          "Down"    },		// KEY_DOWN

        { false, false, false, VK_NUMLOCK,    "NumLock",       "NumLock"  },	// KEY_NUM_LOCK

        { false, false, false, VK_DIVIDE,     "/",             "/"     },		// KEY_NUM_DIVIDE
        { false, false, false, VK_MULTIPLY,   "*",             "*"     },		// KEY_NUM_MULTIPLY, 
        { false, false, false, VK_SUBTRACT,   "-",             "-"     },		// KEY_NUM_SUBTRACT 
        { false, false, false, VK_ADD,        "+",             "+"     },		// KEY_NUM_ADD
     
        { false, false, false, VK_NUMPAD1,    "1",             "1"     },		// KEY_NUM_1
        { false, false, false, VK_NUMPAD2,    "2",             "2"     },		// KEY_NUM_2
        { false, false, false, VK_NUMPAD3,    "3",             "3"     },		// KEY_NUM_3
        { false, false, false, VK_NUMPAD4,    "4",             "4"     },		// KEY_NUM_4
        { false, false, false, VK_NUMPAD5,    "5",             "5"     },		// KEY_NUM_5
        { false, false, false, VK_NUMPAD6,    "6",             "6"     },		// KEY_NUM_6
        { false, false, false, VK_NUMPAD7,    "7",             "7"     },		// KEY_NUM_7
        { false, false, false, VK_NUMPAD8,    "8",             "8"     },		// KEY_NUM_8
        { false, false, false, VK_NUMPAD9,    "9",             "9"     },		// KEY_NUM_9
        { false, false, false, VK_NUMPAD0,    "0",             "0"      },		// KEY_NUM_0
        { false, false, false, VK_DECIMAL,    ".",             "."     },		// KEY_NUM_DECIMAL
        { false, false, false, VK_CLEAR,      "NumClear",      "NumClear"  },	// KEY_NUM_CLEAR
	};

	enum Keys
	{
		KEY_ESCAPE,

		KEY_F1, KEY_F2, KEY_F3, KEY_F4, KEY_F5, KEY_F6, KEY_F7, KEY_F8, 
		KEY_F9, KEY_F10, KEY_F11, KEY_F12,

		KEY_PRINT_SCREEN, KEY_SCROLL, 

		// KEY_PAUSE
		// Left out deliberately. Doesn't work like other keys.

		KEY_APOSTROPHE, KEY_TAB, KEY_CAPS, 
		KEY_LSHIFT, KEY_LCONTROL, KEY_LALT, 
		KEY_SPACE, 
		KEY_RSHIFT, KEY_RCONTROL, KEY_RALT, 
		KEY_SUBTRACT, KEY_EQUALS, KEY_BACKSLASH, KEY_BACKSPACE, KEY_ENTER, 

		KEY_LSQUARE_BRACKET, KEY_RSQUARE_BRACKET, 
		KEY_SEMICOLON, KEY_QUOTE,
		KEY_COMMA, KEY_FULLSTOP, KEY_FORWARD_SLASH,

		KEY_1, KEY_2, KEY_3, KEY_4, KEY_5, KEY_6, KEY_7, KEY_8, KEY_9, KEY_0,

		KEY_A, KEY_B, KEY_C, KEY_D, KEY_E, KEY_F, KEY_G, KEY_H, KEY_I, KEY_J,
		KEY_K, KEY_L, KEY_M, KEY_N, KEY_O, KEY_P, KEY_Q, KEY_R, KEY_S, KEY_T,
		KEY_U, KEY_V, KEY_W, KEY_X, KEY_Y, KEY_Z,

		KEY_INSERT, KEY_HOME, KEY_PAGE_UP, 
		KEY_DELETE, KEY_END, KEY_PAGE_DOWN,

		KEY_LEFT, KEY_UP, KEY_RIGHT, KEY_DOWN,

		KEY_NUM_LOCK, 
		KEY_NUM_DIVIDE, KEY_NUM_MULTIPLY, KEY_NUM_MINUS, KEY_NUM_ADD,
		KEY_NUM_1, KEY_NUM_2, KEY_NUM_3,
		KEY_NUM_4, KEY_NUM_5, KEY_NUM_6,
		KEY_NUM_7, KEY_NUM_8, KEY_NUM_9,
		KEY_NUM_0, KEY_NUM_DECIMAL,
		KEY_NUM_CLEAR /* 5 on the Num Pad when Num Lock not on */,

		KEY_COUNT,  // Not an actual key, tells how many keys in the enum
		KEY_INVALID = KEY_COUNT,
	};

	// --------------------------------------------------------------------------------


	// InputHandler Class used to manage input for keyboard and mouse.
	class InputHandler
	{
	private:
		Vector2 mousePos;
		Vector2 mouseDelta;
		float deltaX, deltaY;
		float wheelDelta;

		// Mouse button flags.
		bool leftDown, rightDown, middleDown;
		bool leftHit, rightHit, middleHit;
		bool leftDownLast, rightDownLast, middleDownLast;

		// Private methods only used by application.
		void PollKeys();
		void UpdateMouse(HWND hWnd);

		void SetLeftMouse(bool down);
		void SetMiddleMouse(bool down);
		void SetRightMouse(bool down);

		void UpdateInputDevices(HWND hWnd);


		InputHandler();

		friend class Application;

	public:

		static InputHandler* Instance();

		static void Release();

		~InputHandler();


		// Keyboard state accessors
		bool IsKeyHit(int KeyID);
		bool IsKeyDown(int KeyID);
		bool IsKeyUp(int KeyID);

		bool IsMouseLeftDown() const;
		bool IsMouseMiddleDown() const;
		bool IsMouseRightDown() const;

		bool IsMouseLeftHit() const;
		bool IsMouseMiddleHit() const;
		bool IsMouseRightHit() const;

		float GetMouseDeltaX() const;
		float GetMouseDeltaY() const;

		float GetMouseWheelDelta() const;

		Vector2 GetMouseToWindow() const;
		Vector2 GetMouseDelta() const;
	};


};// End namespace SWR

#define INPUT_HANDLER SWR::InputHandler::Instance()

#endif // #ifndef INPUT_HANDLER_H