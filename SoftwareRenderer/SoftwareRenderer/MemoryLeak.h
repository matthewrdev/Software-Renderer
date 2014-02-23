#pragma once

#ifndef MEMORY_LEAK_H
#define MEMORY_LEAK_H

//****************************************************************************
//**
//**    MemoryLeak.h
//**
//**    Copyright (c) 2009 Matthew Robbins
//**
//**    Author:  Matthew Robbins
//**    Created: 12/2009
//**
//**                  DO NOT PLACE THIS FILE IN YOUR HEADERS!
//**
//****************************************************************************

// This file is to enable memory leak checking.
// Put this file in your cpps after all includes if you want to check memory leaks / allocation issues.
// If this file is before any other includes you will most likely get crazy compiler errors...


// Enable / disable memory leak checking
// Comment this block out to disable memory leak checking.
#define CHECK_MEMORY_LEAKS

#ifdef CHECK_MEMORY_LEAKS

#define _CRTDBG_MAP_ALLOC

#ifdef _DEBUG

#include <crtdbg.h>
#define DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
#define new DEBUG_NEW

#endif // #ifdef _DEBUG

#endif // #ifdef CHECK_MEMORY_LEAKS


#endif // #ifndef MEMORY_LEAK_H