#pragma once

#include <cassert>

class StencilBuffer
{
public:
	StencilBuffer(int width, int height)
		:
		width(width),
		height(height),
		pBuffer(new __int16[width*height])
	{}
	~StencilBuffer()
	{
		delete[] pBuffer;
		pBuffer = nullptr;
	}
	StencilBuffer(const StencilBuffer&) = delete;
	StencilBuffer& operator=(const StencilBuffer&) = delete;
	void Clear()
	{
		const int nStencils = width * height;
		memset(pBuffer, 0, nStencils * sizeof(__int16));
	}
	bool At(int x, int y)
	{
	//	assert(x >= 0);
	//	assert(x < width);
	//	assert(y >= 0);
	//	assert(y < height);
		return !(pBuffer[y * width + x]);
	}
	void increaseStencilAt(int x, int y)
	{
	//	assert(x >= 0);
	//	assert(x < width);
	//	assert(y >= 0);
	//	assert(y < height);
		pBuffer[y * width + x]++;
	}
	void decreaseStencilAt(int x, int y)
	{
	//	assert(x >= 0);
	//	assert(x < width);
	//	assert(y >= 0);
	//	assert(y < height);
		pBuffer[y * width + x]--;
	}

private:
	int width;
	int height;
	__int16* pBuffer = nullptr;
};

class StencilBufferPtr 
{
public:
	StencilBufferPtr(int x, int y, StencilBuffer& sb)
		:
		x(x),
		y(y),
		sbRef(sb)
	{}
	bool get()
	{
		return sbRef.At(x, y);
	}
	void increase()
	{
		sbRef.increaseStencilAt(x, y);
	}
	void decrease()
	{
		sbRef.decreaseStencilAt(x, y);
	}
private:
	int x;
	int y;
	StencilBuffer& sbRef;
};
