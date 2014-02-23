#pragma once

#ifndef DATA_TYPES_H
#define DATA_TYPES_H

//****************************************************************************
//**
//**    DataTypes.h
//**
//**    Copyright (c) 2010 Matthew Robbins
//**
//**    Author:  Matthew Robbins
//**    Created: 06/2010
//**
//****************************************************************************

// Defines that the software renderer functions will return to denote success or failure.
#define SWR_FAIL			0x00
#define SWR_OK				0x01

// The return type for a SWR function.
#define SWR_ERR unsigned short



namespace SWR
{
	// Typedef for more explicit data types
	typedef unsigned char U8;
	typedef signed char S8;
	typedef unsigned short U16;
	typedef signed short S16;
	typedef unsigned int U32;
	typedef signed int S32;
	typedef float Real;
	
}; // End namespace SWR.

#endif // #ifndef DATA_TYPES_H