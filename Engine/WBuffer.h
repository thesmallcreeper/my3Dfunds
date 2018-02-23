#pragma once

#include <limits>
#include <cassert>

class WBuffer
{
public:
	WBuffer(int width, int height)
		:
		enableSet(true),
		enableEqualTest(false),
		width( width ),
		height( height ),
		pBuffer( new float[width*height] )
	{}
	~WBuffer()
	{
		delete[] pBuffer;
		pBuffer = nullptr;
	}
	WBuffer( const WBuffer& ) = delete;
	WBuffer& operator=( const WBuffer& ) = delete;
	void Clear()
	{
		const int nDepths = width * height;
		memset(pBuffer, 0, nDepths * sizeof(float));
	}
	float& At( int x,int y )
	{
	//	assert( x >= 0 );
	//	assert( x < width );
	//	assert( y >= 0 );
	//	assert( y < height );
		return pBuffer[y * width + x];
	}
	const float& At( int x,int y ) const
	{
		return const_cast<WBuffer*>(this)->At( x,y );
	}

	bool TestAndSet( int x,int y,float depth )
	{
		float& depthInBuffer = At( x,y );
		if( depth < depthInBuffer || ((depth == depthInBuffer) && enableEqualTest))
		{
			if (enableSet == true)
				depthInBuffer = depth;
			return true;
		}
		return false;
	}

public:
	bool enableEqualTest;
	bool enableSet;
private:
	int width;
	int height;
	float* pBuffer = nullptr;
};