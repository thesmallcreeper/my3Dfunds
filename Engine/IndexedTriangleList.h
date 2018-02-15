#pragma once

#include <vector>
#include "Vec3.h"

template<class T>
class IndexedTriangleList
{
public:
	IndexedTriangleList( std::vector<T> verts_in,std::vector<size_t> indices_in )
		:
		vertices( std::move( verts_in ) ),
		indices( std::move( indices_in ) )
	{
		assert( vertices.size() > 2 );
		assert( indices.size() % 3 == 0 );
	}
	std::vector<T> vertices;
	std::vector<size_t> indices;
};

template<class T>
class IndexedTriangleListWithTC
{
public:
	IndexedTriangleListWithTC( std::vector<T> verts_in, std::vector<size_t> indices_in, std::vector<Vec2> tc_in, std::vector<size_t> uvMapping_in )
		:
		itlist( verts_in, indices_in ),
		tc( std::move(tc_in) ),
		uvMapping( std::move(uvMapping_in) )
	{
		assert(tc.size() > 2);
		assert(uvMapping.size() % 3 == 0);
	}
	IndexedTriangleList<T> itlist;
	std::vector<Vec2> tc;
	std::vector<size_t> uvMapping;
};