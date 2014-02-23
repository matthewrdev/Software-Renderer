#pragma once

#ifndef NAME
#define NAME

//****************************************************************************
//**
//**    RenderThread.h
//**
//**    Copyright (c) 2010 Matthew Robbins
//**
//**    Author:  Matthew Robbins
//**    Created: 11/2010
//**
//****************************************************************************


// WORK IN PROGRESS!!!

// Forward Declarations
namespace SWR
{
	class RenderDevice;
	class Rasterizer;
	class TriangleClipper2D;
	struct Vertex;
};

namespace SWR
{
	enum RenderJobType
	{
		JOB_Tex_List_Indice,
		JOB_Tex_List,
		JOB_Tex_Strip_Indice,
		JOB_Tex_Strip_Indice,
		JOB_Tex_List_Indice,
		JOB_Tex_List_Indice,
	};

	struct ThreadJob
	{
		Vertex* triangle;
		RenderJobType type;

		bool complete;

	};
	// ------------------------------------------------------------------------
	//								RenderThread
	// ------------------------------------------------------------------------
	// Desc:
	// 
	// ------------------------------------------------------------------------
	class RenderThread
	{
	private:

		bool m_isWaitingJob;

		Rasterizer* m_rasterizer;
		TriangleClipper2D* m_triClipper;

		BackBuffer* m_backBuffer;

		// The render queue.
		ThreadJob* m_jobQueue;
		int m_currenJob; 
		int m_jobQueueLength;

		int m_targetBufferHeight;
		int m_targetBufferStartRow;

		// Adds a pointer to the beginng of a vertex triplet.
		void AddToRenderQueue(Vertex* triangle);

		void Idle();

		friend class RenderDevice;
	protected:
	public:

		RenderThread();
		~RenderThread();

		void Initilise(int renderQueueLength, BackBuffer* buffer, int height, int startRow);
		void Release();

		void SpawnThread();
	};
	
}; // End namespace SWR.

#endif // #ifndef NAME