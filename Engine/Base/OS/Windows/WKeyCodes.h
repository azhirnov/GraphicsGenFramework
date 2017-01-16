// Copyright © 2014-2016  Zhirnov Andrey. All rights reserved.

#pragma once

#ifdef PLATFORM_WINDOWS

namespace Engine
{
namespace Base
{
	
	struct EKey
	{
		enum type
		{
			_UNKNOWN		=   0,

			// mouse -------------------
			MOUSE_1			=   1,		//	MOUSE 1
			MOUSE_2			=   2,		//	MOUSE 2
			MOUSE_3			=   3,		//	MOUSE 3
			MOUSE_4			=   4,		//	MOUSE 4
			MOUSE_5			=   5,		//	MOUSE 5

			M_LEFT			= MOUSE_1,
			M_RIGHT			= MOUSE_2,
			M_MIDDLE		= MOUSE_3,
			M_X1			= MOUSE_4,
			M_X2			= MOUSE_5,

			M_WHEEL_UP		=   6,		//	MOUSE WHEEL UP
			M_WHEEL_DOWN	=   7,		//	MOUSE WHEEL DOWN
			// mouse -------------------

			_KEY_BEGIN		=   8,

			BACKSPACE		=   8,		//	BACK SPACE
			TAB				=   9,		//	TAB

			CLEAR			=  12,		//	CLEAR
			RETURN			=  13,		//	ENTER

			L_SHIFT			=  16,		//	LEFT SHIFT
			L_CONTROL		=  17,		//	LEFT CONTROL
			L_ALT			=  18,		//	LEFT ALT
			PAUSE			=  19,		//	PAUSE BREAK
			CAPSLOCK		=  20,		//	CAPS LOCK

			ESCAPE			=  27,		//	ESC

			SPACE			=  32,		//	SPACE
			NUMPAD_PGUP		=  33,		//	NUMPAD PG UP
			NUMPAD_PGDN		=  34,		//	NUMPAD PG DW
			NUMPAD_END		=  35,		//	NUMPAD END
			NUMPAD_HOME		=  36,		//	NUMPAD HOME

			NUMPAD_LEFT		=  37,		//	NUMPAD LEFT
			NUMPAD_UP		=  38,		//	NUMPAD UP
			NUMPAD_RIGHT	=  39,		//	NUMPAD RIGHT
			NUMPAD_DOWN		=  40,		//	NUMPAD DOWN

			SELECT			=  41,		//	SELECT
			PRINT			=  42,		//	PRINT
			EXECUTE			=  43,		//	EXECUTE
			SNAPSHOT		=  44,		//	SNAPSHOT / PRINTSCREEN
			NUMPAD_INSERT	=  45,		//	NUMPAD INSERT
			NUMPAD_DELETE	=  46,		//	NUMPAD DELETE
			HELP			=  47,		//	HELP

			n0				=  48,		//	0
			n1				=  49,		//	1
			n2				=  50,		//	2
			n3				=  51,		//	3
			n4				=  52,		//	4
			n5				=  53,		//	5
			n6				=  54,		//	6
			n7				=  55,		//	7
			n8				=  56,		//	8
			n9				=  57,		//	9

			A				=  65,		//	A
			B				=  66,		//	B
			C				=  67,		//	C
			D				=  68,		//	D
			E				=  69,		//	E
			F				=  70,		//	F
			G				=  71,		//	G
			H				=  72,		//	H
			I				=  73,		//	I
			J				=  74,		//	J
			K				=  75,		//	K
			L				=  76,		//	L
			M				=  77,		//	M
			N				=  78,		//	N
			O				=  79,		//	O
			P				=  80,		//	P
			Q				=  81,		//	Q
			R				=  82,		//	R
			S				=  83,		//	S
			T				=  84,		//	T
			U				=  85,		//	U
			V				=  86,		//	V
			W				=  87,		//	W
			X				=  88,		//	X
			Y				=  89,		//	Y
			Z				=  90,		//	Z

			R_WIN			=  92,		//	RIGHT WIN
			L_APPS			=  93,		//	LEFT APPS

			NUMPAD_0		=  96,		//	0	// NUMPAD INSERT
			NUMPAD_1		=  97,		//	1	// NUMPAD END
			NUMPAD_2		=  98,		//	2	// NUMPAD DOWN
			NUMPAD_3		=  99,		//	3	// NUMPAD PG DW
			NUMPAD_4		= 100,		//	4	// NUMPAD LEFT
			NUMPAD_5		= 101,		//	5	// CLEAR
			NUMPAD_6		= 102,		//	6	// NUMPAD RIGHT
			NUMPAD_7		= 103,		//	7	// NUMPAD HOME
			NUMPAD_8		= 104,		//	8	// NUMPAD UP
			NUMPAD_9		= 105,		//	9	// NUMPAD PG UP
			NUMPAD_STAR		= 106,		//	* 
			NUMPAD_PLUS		= 107,		//	+ 
			NUMPAD_SEPARATOR= 108,		//	SEPARATOR
			NUMPAD_MINUS	= 109,		//	- 
			NUMPAD_PERIOD	= 110,		//	.	// NUMPAD DELETE
			//NUMPAD_DEVIDE	= 111,		// /

			F1				= 112,		//	F1
			F2				= 113,		//	F2
			F3				= 114,		//	F3
			F4				= 115,		//	F4
			F5				= 116,		//	F5
			F6				= 117,		//	F6
			F7				= 118,		//	F7
			F8				= 119,		//	F8
			F9				= 120,		//	F9
			F10				= 121,		//	F10
			F11				= 122,		//	F11
			F12				= 123,		//	F12
			F13				= 124,		//	F13
			F14				= 125,		//	F14
			F15				= 126,		//	F15
			F16				= 127,		//	F16
			F17				= 128,		//	F17
			F18				= 129,		//	F18
			F19				= 130,		//	F19
			F20				= 131,		//	F20
			F21				= 132,		//	F21
			F22				= 133,		//	F22
			F23				= 134,		//	F23
			F24				= 135,		//	F24

			NUMLOCK			= 144,		//	NUM LOCK
			SCROLL			= 145,		//	SCROLL LOCK 

			L_MENU			= 164,		//	LEFT MENU
			R_MENU			= 165,		//	RIGHT MENU

			SEMICOLON		= 186,		//	;
			PLUS			= 187,		//	+ 
			COMMA			= 188,		//	, 
			MINUS			= 189,		//	- 

			PERIOD			= 190,		//	.
			SLASH			= 191,		//	/ 
			GRAVE			= 192,		//	~ 

			L_BRACKET		= 219,		//	[ 
			BACKSLASH		= 220,		//	\ 
			R_BRACKET		= 221,		//	] 
			APOSTROPHE		= 222,		//	' 

			right_offset	= 256,
			_ro				= right_offset,
		
			NUMPAD_ENTER	= _ro + 13,		//	NUMPAD ENTER

			R_SHIFT			= _ro + 16,		//	RIGHT SHIFT
			R_CONTROL		= _ro + 17,		//	RIGHT CONTROL
			R_ALT			= _ro + 18,		//	RIGHT ALT
		
			PGUP			= _ro + 33,		//	PG UP
			PGDN			= _ro + 34,		//	PG DW
			END				= _ro + 35,		//	END
			HOME			= _ro + 36,		//	HOME

			LEFT			= _ro + 37,		//	LEFT
			UP				= _ro + 38,		//	UP
			RIGHT			= _ro + 39,		//	RIGHT
			DOWN			= _ro + 40,		//	DOWN
		
			SYSRQ			= _ro + 44,		//	SYS RQ (PRT SCR)
			INSERT			= _ro + 45,		//	INSERT
			DELETE			= _ro + 46,		//	DELETE

			L_WIN			= _ro + 91,		//	LEFT WIN
			R_APPS			= _ro + 93,		//	RIGHT APPS

			SLEEP			= _ro + 95,		//	SLEEP

			NUMPAD_SLASH	= _ro + 111,	//	/ 

			BROWSER_BACK	= _ro + 166,	//	BROWSER BACK
			BROWSER_FORWARD	= _ro + 167,	//	BROWSER FORWARD
			BROWSER_REFRESH	= _ro + 168,	//	BROWSER REFRESH
			BROWSER_STOP	= _ro + 169,	//	BROWSER STOP
			BROWSER_SEARCH	= _ro + 170,	//	BROWSER SEARCH
			BROWSER_FAVORITES = _ro + 171,	//	BROWSER FAVORITES
			BROWSER_HOME	= _ro + 172,	//	BROWSER HOME
		
			VOLUME_MUTE		= _ro + 173,	//	VOLUME MUTE
			VOLUME_DOWN		= _ro + 174,	//	VOLUME DOWN
			VOLUME_UP		= _ro + 175,	//	VOLUME UP
		
			MEDIA_NEXT		= _ro + 176,	//	MEDIA NEXT TRACK
			MEDIA_PREV		= _ro + 177,	//	MEDIA PREV TRACK
			MEDIA_STOP		= _ro + 178,	//	MEDIA STOP
			MEDIA_PLAY		= _ro + 179,	//	MEDIA PLAY / PAUSE

			LAUNCH_MAIL		= _ro + 180,	//	LAUNCH MAIL
			LAUNCH_MEDIA	= _ro + 181,	//	LAUNCH MEDIA SELECT
			LAUNCH_APP1		= _ro + 182,	//	LAUNCH APPLICATION 1
			LAUNCH_APP2		= _ro + 183,	//	LAUNCH APPLICATION 2

			PRINTSCREEN		= _ro + 255,	// PRING SCREEN (tested)

			_COUNT			= right_offset * 2
		};
	};


	/*
	// Android Emulation
	struct EKey
	{
		enum type
		{
			_UNKNOWN		= e_winkey::_UNKNOWN,

			// arrows
			DPAD_UP			= e_winkey::UP,
			DPAD_DOWN		= e_winkey::DOWN,
			DPAD_LEFT		= e_winkey::LEFT,
			DPAD_RIGHT		= e_winkey::RIGHT,
			DPAD_CENTER		= e_winkey::NUMPAD_5,
			
			// phone
			CALL			= e_winkey::F1,
			ENDCALL			= e_winkey::F2,
			CAMERA			= e_winkey::F3,
			SOFT_LEFT		= e_winkey::L_CONTROL,
			SOFT_RIGHT		= e_winkey::R_CONTROL,
			STAR			= e_winkey::F4,				// * 
			POUND			= e_winkey::F5,				// # 

			// numbers
			n0				= e_winkey::_0,
			n1				= e_winkey::_1,
			n2				= e_winkey::_2,
			n3				= e_winkey::_3,
			n4				= e_winkey::_4,
			n5				= e_winkey::_5,
			n6				= e_winkey::_6,
			n7				= e_winkey::_7,
			n8				= e_winkey::_8,
			n9				= e_winkey::_9,

			// chars
			A				= e_winkey::A,
			B				= e_winkey::B,
			C				= e_winkey::C,
			D				= e_winkey::D,
			E				= e_winkey::E,
			F				= e_winkey::F,
			G				= e_winkey::G,
			H				= e_winkey::H,
			I				= e_winkey::I,
			J				= e_winkey::J,
			K				= e_winkey::K,
			L				= e_winkey::L,
			M				= e_winkey::M,
			N				= e_winkey::N,
			O				= e_winkey::O,
			P				= e_winkey::P,
			Q				= e_winkey::Q,
			R				= e_winkey::R,
			S				= e_winkey::S,
			T				= e_winkey::T,
			U				= e_winkey::U,
			V				= e_winkey::V,
			W				= e_winkey::W,
			X				= e_winkey::X,
			Y				= e_winkey::Y,
			Z				= e_winkey::Z,

			// symbols
			COMMA			= e_winkey::COMMA,			// , 
			PERIOD			= e_winkey::PERIOD,			// . 
			TAB				= e_winkey::TAB,
			SPACE			= e_winkey::SPACE,
			GRAVE			= e_winkey::GRAVE,			// ` (~)
			MINUS			= e_winkey::MINUS,			// - 
			EQUALS			= e_winkey::PLUS,			// = 
			BACKSLASH		= e_winkey::BACKSLASH,		// / 
			SEMICOLON		= e_winkey::SEMICOLON,		// ; 
			APOSTROPHE		= e_winkey::APOSTROPHE,		// ' 
			SLASH			= e_winkey::SLASH,			// \ 
			PLUS			= e_winkey::NUMPAD_PLUS,	// +
			LEFT_BRACKET	= e_winkey::L_BRACKET,		// [
			RIGHT_BRACKET	= e_winkey::R_BRACKET,		// ]

			// edit
			DEL				= e_winkey::DELETE,
			ENTER			= e_winkey::RETURN,

			// controls
			ALT_LEFT		= e_winkey::L_ALT,
			ALT_RIGHT		= e_winkey::R_ALT,
			SHIFT_LEFT		= e_winkey::L_SHIFT,
			SHIFT_RIGHT		= e_winkey::R_SHIFT,
			
			// system
			POWER			= e_winkey::SLEEP,
			CLEAR			= e_winkey::CLEAR,
			SYM				= e_winkey::F8,
			EXPLORER		= e_winkey::F9,
			ENVELOPE		= e_winkey::F10,
			AT				= e_winkey::F11,
			NUM				= e_winkey::NUMLOCK,
			HEADSETHOOK		= e_winkey::F12,
			FOCUS			= e_winkey::F6,
			MENU			= e_winkey::L_MENU,
			NOTIFICATION	= e_winkey::F7,
			SEARCH			= e_winkey::BROWSER_SEARCH,
			HOME			= e_winkey::HOME,
			BACK			= e_winkey::ESCAPE,
			PAGE_UP			= e_winkey::PGUP,
			PAGE_DOWN		= e_winkey::PGDN,
			PICTSYMBOLS		= e_winkey::SYSRQ,	// == Print Screen
			SWITCH_CHARSET	= e_winkey::PAUSE,
			
			// media
			MEDIA_PLAY		= e_winkey::MEDIA_PLAY,
			MEDIA_STOP		= e_winkey::MEDIA_STOP,
			MEDIA_NEXT		= e_winkey::MEDIA_NEXT,
			MEDIA_PREVIOUS	= e_winkey::MEDIA_PREV,
			MEDIA_REWIND	= e_winkey::F13,
			MEDIA_FORWARD	= e_winkey::F14,
			MEDIA_MUTE		= e_winkey::VOLUME_MUTE,

			// volume
			VOLUME_UP		= e_winkey::VOLUME_UP,
			VOLUME_DOWN		= e_winkey::VOLUME_DOWN,
			
			// unknown
			BUTTON_A		= 0,
			BUTTON_B		= 0,
			BUTTON_C		= 0,
			BUTTON_X		= 0,
			BUTTON_Y		= 0,
			BUTTON_Z		= 0,
			BUTTON_L1		= 0,
			BUTTON_R1		= 0,
			BUTTON_L2		= 0,
			BUTTON_R2		= 0,
			BUTTON_THUMBL	= 0,
			BUTTON_THUMBR	= 0,
			BUTTON_START	= 0,
			BUTTON_SELECT	= 0,
			BUTTON_MODE		= 0,

			_COUNT			= e_winkey::_COUNT
		};
	};*/


}	// Base
}	// Engine

#endif	// PLATFORM_WINDOWS