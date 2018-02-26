/******************************************************************************************
*	Chili DirectX Framework Version 16.10.01											  *
*	Vec2.h																				  *
*	Copyright 2016 PlanetChili <http://www.planetchili.net>								  *
*																						  *
*	This file is part of The Chili DirectX Framework.									  *
*																						  *
*	The Chili DirectX Framework is free software: you can redistribute it and/or modify	  *
*	it under the terms of the GNU General Public License as published by				  *
*	the Free Software Foundation, either version 3 of the License, or					  *
*	(at your option) any later version.													  *
*																						  *
*	The Chili DirectX Framework is distributed in the hope that it will be useful,		  *
*	but WITHOUT ANY WARRANTY; without even the implied warranty of						  *
*	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the						  *
*	GNU General Public License for more details.										  *
*																						  *
*	You should have received a copy of the GNU General Public License					  *
*	along with The Chili DirectX Framework.  If not, see <http://www.gnu.org/licenses/>.  *
******************************************************************************************/
#pragma once

#include <immintrin.h>
#include "ChiliMath.h"

class Vec3;
class Ved3;
class Vei3;

class Vec3
{
public:
	Vec3() {}
	Vec3(float x, float y, float z)
		:
		x(x),
		y(y),
		z(z)
	{}
	Vec3(const Vec3& vect)
		:
		Vec3(vect.x, vect.y, vect.z)
	{}

	operator Ved3();
	operator Vei3();

	float	LenSq() const
	{
		return sq(*this);
	}
	float	Len() const
	{
		return sqrt(LenSq());
	}
	Vec3&	Normalize()
	{
		const float length = Len();
		x /= length;
		y /= length;
		z /= length;
		return *this;
	}
	Vec3	GetNormalized() const
	{
		Vec3 norm = *this;
		norm.Normalize();
		return norm;
	}
	Vec3	GetReflection(Vec3 normal)
	{
		Vec3& ray = *this;
		return normal * (2 * (normal * ray)) - ray;
	}
	Vec3	operator-() const
	{
		return Vec3(-x, -y, -z);
	}
	Vec3&	operator=(const Vec3 &rhs)
	{
		x = rhs.x;
		y = rhs.y;
		z = rhs.z;
		return *this;
	}
	Vec3&	operator+=(const Vec3 &rhs)
	{
		x += rhs.x;
		y += rhs.y;
		z += rhs.z;
		return *this;
	}
	Vec3&	operator-=(const Vec3 &rhs)
	{
		x -= rhs.x;
		y -= rhs.y;
		z -= rhs.z;
		return *this;
	}
	float	operator*(const Vec3 &rhs) const
	{
		return x * rhs.x + y * rhs.y + z * rhs.z;
	}
	Vec3	operator+(const Vec3 &rhs) const
	{
		return Vec3(*this) += rhs;
	}
	Vec3	operator-(const Vec3 &rhs) const
	{
		return Vec3(*this) -= rhs;
	}
	Vec3&	operator*=(const float &rhs)
	{
		x *= rhs;
		y *= rhs;
		z *= rhs;
		return *this;
	}
	Vec3	operator*(const float &rhs) const
	{
		return Vec3(*this) *= rhs;
	}
	Vec3	operator%(const Vec3& rhs) const
	{
		return Vec3(
			y * rhs.z - z * rhs.y,
			z * rhs.x - x * rhs.z,
			x * rhs.y - y * rhs.x);
	}
	Vec3&	operator/=(const float &rhs)
	{
		x /= rhs;
		y /= rhs;
		z /= rhs;
		return *this;
	}
	Vec3	operator/(const float &rhs) const
	{
		return Vec3(*this) /= rhs;
	}
	float	operator [](const int i) {
		switch (i) {
		case 1:
			return x;
		case 2:
			return y;
		case 3:
			return z;
		}
	}
	template<typename T>
	bool	operator==(const T &rhs) const
	{
		return x == rhs.x && y == rhs.y && rhs.z = z;
	}
	template<typename T>
	bool	operator!=(const T &rhs) const
	{
		return !(*this == rhs);
	}
public:
	union
	{		
		struct { float x, y, z; };
		__m128 mmvalue;
	};
};

class Ved3
{
public:
	Ved3() {}
	Ved3(double x, double y, double z)
		:
		x(x),
		y(y),
		z(z)
	{}
	Ved3(const Ved3& vect)
		:
		Ved3(vect.x, vect.y, vect.z)
	{}

	operator Vec3();
	operator Vei3();

	double		LenSq() const
	{
		return sq(*this);
	}
	double		Len() const
	{
		return sqrt(LenSq());
	}
	Ved3&	Normalize()
	{
		const double length = Len();
		x /= length;
		y /= length;
		z /= length;
		return *this;
	}
	Ved3	GetNormalized() const
	{
		Ved3 norm = *this;
		norm.Normalize();
		return norm;
	}
	Ved3	GetReflection(Ved3 normal)
	{
		Ved3& ray = *this;
		return normal * (2 * (normal * ray)) - ray;
	}
	Ved3	operator-() const
	{
		return Ved3(-x, -y, -z);
	}
	Ved3&	operator=(const Ved3 &rhs)
	{
		x = rhs.x;
		y = rhs.y;
		z = rhs.z;
		return *this;
	}
	Ved3&	operator+=(const Ved3 &rhs)
	{
		x += rhs.x;
		y += rhs.y;
		z += rhs.z;
		return *this;
	}
	Ved3&	operator-=(const Ved3 &rhs)
	{
		x -= rhs.x;
		y -= rhs.y;
		z -= rhs.z;
		return *this;
	}
	double		operator*(const Ved3 &rhs) const
	{
		return x * rhs.x + y * rhs.y + z * rhs.z;
	}
	Ved3	operator+(const Ved3 &rhs) const
	{
		return Ved3(*this) += rhs;
	}
	Ved3	operator-(const Ved3 &rhs) const
	{
		return Ved3(*this) -= rhs;
	}
	Ved3&	operator*=(const double &rhs)
	{
		x *= rhs;
		y *= rhs;
		z *= rhs;
		return *this;
	}
	Ved3	operator*(const double &rhs) const
	{
		return Ved3(*this) *= rhs;
	}
	Ved3	operator%(const Ved3& rhs) const
	{
		return Ved3(
			y * rhs.z - z * rhs.y,
			z * rhs.x - x * rhs.z,
			x * rhs.y - y * rhs.x);
	}
	Ved3&	operator/=(const double &rhs)
	{
		x /= rhs;
		y /= rhs;
		z /= rhs;
		return *this;
	}
	Ved3	operator/(const double &rhs) const
	{
		return Ved3(*this) /= rhs;
	}
	double	operator [](const int i) {
		switch (i) {
		case 1:
			return x;
		case 2:
			return y;
		case 3:
			return z;
		}
	}
	template<typename T>
	bool	operator==(const T &rhs) const
	{
		return x == rhs.x && y == rhs.y && rhs.z = z;
	}
	template<typename T>
	bool	operator!=(const T &rhs) const
	{
		return !(*this == rhs);
	}

public:
	__declspec(align(4 * sizeof(double))) struct
	{
		double x;
		double y;
		double z;
	};
};

class Vei3
{
public:
	Vei3() {}
	Vei3(int x, int y, int z)
		:
		x(x),
		y(y),
		z(z)
	{}
	Vei3(const Vei3& vect)
		:
		Vei3(vect.x, vect.y, vect.z)
	{}

	operator Vec3();
	operator Ved3();

	int		LenSq() const
	{
		return sq(*this);
	}
	int		Len() const
	{
		return (int) sqrt(LenSq());
	}
	Vei3&	Normalize()
	{
		const int length = Len();
		x /= length;
		y /= length;
		z /= length;
		return *this;
	}
	Vei3	GetNormalized() const
	{
		Vei3 norm = *this;
		norm.Normalize();
		return norm;
	}
	Vei3	GetReflection(Vei3 normal)
	{
		Vei3& ray = *this;
		return normal * (2 * (normal * ray)) - ray;
	}
	Vei3	operator-() const
	{
		return Vei3(-x, -y, -z);
	}
	Vei3&	operator=(const Vei3 &rhs)
	{
		x = rhs.x;
		y = rhs.y;
		z = rhs.z;
		return *this;
	}
	Vei3&	operator+=(const Vei3 &rhs)
	{
		x += rhs.x;
		y += rhs.y;
		z += rhs.z;
		return *this;
	}
	Vei3&	operator-=(const Vei3 &rhs)
	{
		x -= rhs.x;
		y -= rhs.y;
		z -= rhs.z;
		return *this;
	}
	int		operator*(const Vei3 &rhs) const
	{
		return x * rhs.x + y * rhs.y + z * rhs.z;
	}
	Vei3	operator+(const Vei3 &rhs) const
	{
		return Vei3(*this) += rhs;
	}
	Vei3	operator-(const Vei3 &rhs) const
	{
		return Vei3(*this) -= rhs;
	}
	Vei3&	operator*=(const int &rhs)
	{
		x *= rhs;
		y *= rhs;
		z *= rhs;
		return *this;
	}
	Vei3	operator*(const int &rhs) const
	{
		return Vei3(*this) *= rhs;
	}
	Vei3	operator%(const Vei3& rhs) const
	{
		return Vei3(
			y * rhs.z - z * rhs.y,
			z * rhs.x - x * rhs.z,
			x * rhs.y - y * rhs.x);
	}
	Vei3&	operator/=(const int &rhs)
	{
		x /= rhs;
		y /= rhs;
		z /= rhs;
		return *this;
	}
	Vei3	operator/(const int &rhs) const
	{
		return Vei3(*this) /= rhs;
	}
	int	operator [](const int i) {
		switch (i) {
		case 1:
			return x;
		case 2:
			return y;
		case 3:
			return z;
		}
	}
	template<typename T>
	bool	operator==(const T &rhs) const
	{
		return x == rhs.x && y == rhs.y && rhs.z = z;
	}
	template<typename T>
	bool	operator!=(const T &rhs) const
	{
		return !(*this == rhs);
	}

public:
	__declspec(align(4 * sizeof(int))) struct
	{
		int x;
		int y;
		int z;
	};
};

inline Vec3 :: operator Ved3()
{
	return{ (double)x,(double)y,(double)z };
}
inline Vec3 :: operator Vei3()
{
	return{ (int)x,(int)y,(int)z };
}

inline Ved3 :: operator Vec3()
{
	return{ (float)x,(float)y,(float)z };
}
inline Ved3 :: operator Vei3()
{
	return{ (int)x,(int)y,(int)z };
}

inline Vei3 :: operator Vec3()
{
	return{ (float)x,(float)y,(float)z };
}
inline Vei3 :: operator Ved3()
{
	return{ (double)x,(double)y,(double)z };
}


