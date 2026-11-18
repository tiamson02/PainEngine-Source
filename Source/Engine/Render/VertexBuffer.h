// $Header: /Game/Source/Engine/Render/VertexBuffer.h 10    8/19/04 9:40p Bart $

// #pragma once
#ifndef _VERTEXBUFFER_
#define _VERTEXBUFFER_


#include "Engine/PCFSystem.h"
extern ENGINE_API PCFSystem* GEngine;

class RenderDevice;


#ifdef TRANSGAMING_PS2
template <class T, const int Type/*, const int MaxElems=1024*/>
class VertexBuffer
{
public:
	VertexBuffer(){
        VertexStream[0] = VertexStream[1] = EInvalidIndex;
        dynamic = false;
	    Count = 0;
	    FillPos = 0;
        RenderDev = NULL;
        MaxElems = 0;
        lastFrameCount = 0;
    }
	~VertexBuffer()
	{
		Release();
	}

	void Release()
	{
		if (VertexStream[0] != EInvalidIndex || VertexStream[1] != EInvalidIndex)
		{
			RenderDevice* dev=GEngine->GetRenderDevice();
			Assert(dev);
			dev->ReleaseResource(RenderDevice::EResBuffer, VertexStream, 2);
            
			VertexStream[0] = EInvalidIndex;
			VertexStream[1] = EInvalidIndex;

            lastFrameCount = 0;
		}
	}

#ifdef DEBUG
	BOOL Create(RenderDevice* dev, int maxElems /*= 1024*/, bool isDynamic = false, const char *file = NULL, const int line = 0)
#else
	BOOL Create(RenderDevice* dev, int maxElems /*= 1024*/, bool isDynamic = false)
#endif
	{
		Assert(dev);
		RenderDev=dev;
        Count=0;
        FillPos=0;
        lastFrameCount = 0;
        MaxElems = maxElems;

        VertexStream[0] = RenderDev->CreateStream(0,Type,MaxElems,sizeof(T));

        if (VertexStream[0] == EInvalidIndex){
            Assert(VertexStream[0] != EInvalidIndex);

	        return FALSE;
	    }


        if (isDynamic){
    	    VertexStream[1] = RenderDev->CreateStream(0,Type,MaxElems,sizeof(T));

            if (VertexStream[1] == EInvalidIndex){
                Assert(VertexStream[1] != EInvalidIndex);

	            return FALSE;
	        }
        }

#ifdef DEBUG
        filename = file;
        lineNumber = line;

        TRACE("VertexBuffer<>::Create(): created a vertex buffer for %d verticies (size = %d, isDynamic = %s, file = '%s' (%d))\n", MaxElems, sizeof(T), isDynamic ? "true" : "false", file, line);
#endif

        dynamic = isDynamic;


		return TRUE;
	}

	T* Lock(int needed)
	{
        if (dynamic && lastFrameCount != totalFrameCount){
            Count = 0;

            lastFrameCount = totalFrameCount;
        }

		FillPos=Count;
		Count+=needed;

		if(Count>MaxElems) {
			if(needed>MaxElems) {
				return NULL;
			}

#ifdef DEBUG
            if (dynamic){
                if (filename)
                    TRACE("VertexBuffer<>::Lock(): WARNING-> the vertex buffer created at '%s' (%d) with %d verticies is looping!  (requesting %d verticies, needed = %d)\n", filename, lineNumber, MaxElems, Count, needed);

                else
                    TRACE("VertexBuffer<>::Lock(): WARNING-> a vertex buffer with %d verticies is looping!  (requesting %d verticies, needed = %d)\n", MaxElems, Count, needed);
            }
#endif
			FillPos=0;
			Count=needed;
		}


        if (dynamic)
		    return (T*)RenderDev->LockStream(VertexStream[totalFrameCount & 1], FillPos, needed);

        else
		    return (T*)RenderDev->LockStream(VertexStream[0], FillPos, needed);
	}

	int Unlock()
	{
        if (dynamic)
		    RenderDev->UnlockStream(VertexStream[totalFrameCount & 1]);

        else
		    RenderDev->UnlockStream(VertexStream[0]);


		return FillPos;
	}

	void Align(const int N)
	{
        Count = (Count + (N-1) & -N);
	}

    RenderDevice* GetDevice() const { return RenderDev; }

    int GetStream() const {
        if (dynamic)
            return VertexStream[totalFrameCount & 1];

        else
            return VertexStream[0];
    }

    int GetMaxElems() const { return MaxElems; }
	int GetFillPos() const { return FillPos; }
    //Resync(): synchronizes this vertex buffer with another one of the same size.
    //  This can be used to resynchronize single component shared buffers after one
    //  was not used.
    template<class T2, const int Type2>
    void Resync(const VertexBuffer<T2, Type2> &buf){
        Assert(buf.GetMaxElems() == MaxElems);
        FillPos = buf.GetFillPos();
        Count = buf.GetNextStart();
        lastFrameCount = totalFrameCount;
    }
    int GetNextStart() const{
        return Count;
    }

    void CloneStream(void)
    {
        if (dynamic)
		{
			int f  = totalFrameCount & 1;
			void *s1 = (void*)RenderDev->LockStream(VertexStream[f  ], 0, 0);
			void *s2 = (void*)RenderDev->LockStream(VertexStream[f^1], 0, 0);
			memcpy(s2, s1, sizeof(T)*MaxElems);
		}
    }

private:
	RenderDevice* RenderDev;
    int MaxElems;
	int VertexStream[2];
	int Count;
	int FillPos;
    bool dynamic;
    int lastFrameCount;
#ifdef DEBUG
    const char *filename;
    int     lineNumber;
#endif
};

#else
template <class T, const int Type/*, const int MaxElems=1024*/>
class VertexBuffer
{
public:
	VertexBuffer() : VertexStream(EInvalidIndex) {
	    Count = 0;
	    FillPos = 0;
        RenderDev = NULL;
        MaxElems = 0;
    }
	~VertexBuffer()
	{
		Release();
	}

	void Release()
	{
		if (VertexStream != EInvalidIndex)
		{
			RenderDevice* dev=GEngine->GetRenderDevice();
			Assert(dev);
			dev->ReleaseResource(RenderDevice::EResBuffer, &VertexStream, 1);

			VertexStream = EInvalidIndex;

		}
	}

	BOOL Create(RenderDevice* dev, int maxElems /*= 1024*/, bool isDynamic = false)
	{
		Assert(dev);
		RenderDev=dev;
        Count=0;
        FillPos=0;
        MaxElems = maxElems;
        TRACE("VertexBuffer<>::Create(): creating a vertex buffer for %d verticies (size = %d)\n", MaxElems, sizeof(T));
    	VertexStream=RenderDev->CreateStream(0,Type,MaxElems,sizeof(T));
    	if(EInvalidIndex==VertexStream) {
            Assert(EInvalidIndex!=VertexStream);
	        return FALSE;
	    }

		return TRUE;
	}

	T* Lock(int needed)
	{
		FillPos=Count;
		Count+=needed;

		if(Count>MaxElems) {
			if(needed>MaxElems) {
				return NULL;
			}
			FillPos=0;
			Count=needed;
		}

		return (T*)RenderDev->LockStream(VertexStream,FillPos,needed);
	}

	int Unlock()
	{
		RenderDev->UnlockStream(VertexStream);
		return FillPos;
	}

	void Align(const int N)
	{
		Count += (Count & (N-1));
	}

    RenderDevice* GetDevice() const { return RenderDev; }
    int GetStream() const { return VertexStream; }
    int GetMaxElems() const { return MaxElems; }
	int GetFillPos() const { return FillPos; }
#ifdef TRANSGAMING_PS2
    //Resync(): synchronizes this vertex buffer with another one of the same size.
    //  This can be used to resynchronize single component shared buffers after one
    //  was not used.
    template<class T2, const int Type2>
    void Resync(const VertexBuffer<T2, Type2> &buf){
        Assert(buf.GetMaxElems() == MaxElems);
        FillPos = buf.GetFillPos();
        Count = buf.GetNextStart();
    }
    int GetNextStart() const{
        return Count;
    }
#endif
private:
	RenderDevice* RenderDev;
    int MaxElems;
	int VertexStream;
	int Count;
	int FillPos;
};
#endif


#endif //_VERTEXBUFFER_

