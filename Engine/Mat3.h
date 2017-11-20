/******************************************************************************************
*	Chili DirectX Framework Version 16.10.01											  *
*	Mat2.h																				  *
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

#include "Vec3.h"

template <typename T>
class _Mat3
{
public:
	_Mat3& operator=( const _Mat3& rhs )
	{
		memcpy( elements,rhs.elements,sizeof( elements ) );
		return *this;
	}
	_Mat3& operator*=( T rhs )
	{
		for( auto& row : elements )
		{
			for( T& e : row )
			{
				e *= rhs;
			}
		}
		return *this;
	}
	_Mat3 operator*( T rhs ) const
	{
		_Mat3 result = *this;
		return result *= rhs;
	}
	_Mat3& operator*=( const _Mat3& rhs )
	{
		return *this = *this * rhs;
	}
	_Mat3 operator*( const _Mat3& rhs ) const
	{
		_Mat3 result;
		for( size_t j = 0; j < 3; j++ )
		{
			for( size_t k = 0; k < 3; k++ )
			{
				T sum = (T)0.0;
				for( size_t i = 0; i < 3; i++ )
				{
					sum += elements[j][i] * rhs.elements[i][k];
				}
				result.elements[j][k] = sum;
			}
		}
		return result;
	}
	static _Mat3 Identity()
	{
		return { 
			(T)1.0,(T)0.0,(T)0.0,
			(T)0.0,(T)1.0,(T)0.0,
			(T)0.0,(T)0.0,(T)1.0
		};
	}
	static _Mat3 Scaling( T factor )
	{
		return{
			factor,(T)0.0,(T)0.0,
			(T)0.0,factor,(T)0.0,
			(T)0.0,(T)0.0,factor 
		};
	}
	static _Mat3 RotationZ( T theta )
	{
		const T sinTheta = sin( theta );
		const T cosTheta = cos( theta );
		return{
			 cosTheta, sinTheta, (T)0.0,
			-sinTheta, cosTheta, (T)0.0,
			(T)0.0,    (T)0.0,   (T)1.0
		};
	}
	static _Mat3 RotationY( T theta )
	{
		const T sinTheta = sin( theta );
		const T cosTheta = cos( theta );
		return{
			 cosTheta, (T)0.0,-sinTheta,
			 (T)0.0,   (T)1.0, (T)0.0,
			 sinTheta, (T)0.0, cosTheta
		};
	}
	static _Mat3 RotationX(T theta)
	{
		const T sinTheta = sin(theta);
		const T cosTheta = cos(theta);
		return{
			(T)1.0, (T)0.0,   (T)0.0,
			(T)0.0, cosTheta, sinTheta,
			(T)0.0,-sinTheta, cosTheta
		};
	}
	static _Mat3 ChangeView(const _Vec3<T> dir, const _Vec3<T> up) 
	{
		_Vec3<T> zAxis = -dir;
		_Vec3<T> xAxis = up % zAxis;
		_Vec3<T> yAxis = zAxis % xAxis;
		xAxis.Normalize();
		yAxis.Normalize();
		zAxis.Normalize();

		return {
			xAxis.x		,yAxis.x	,zAxis.x	,
			xAxis.y		,yAxis.y	,zAxis.y	,
			xAxis.z		,yAxis.z	,zAxis.z
		};

	}
		static _Mat3 OrientationEuler(T head, T pitch, T roll)
	{
		const T sinH = sin(head);
		const T cosH = cos(head);
		const T sinP = sin(pitch);
		const T cosP = cos(pitch);
		const T sinR = sin(roll);
		const T cosR = cos(roll);
		return{
			cosR * cosH - sinR * sinP * sinH, -sinR * cosP	, cosR * sinH + sinR * sinP * cosH,
			sinR * cosH + cosR * sinP * sinH, cosR * cosP	, sinR * sinH - cosR * sinP * cosH,
			-cosP * sinH					, sinP			, cosP * cosH

		};
	}
	static _Mat3 OrientationQuat(const _Vec3<T> defDir, const _Vec3<T> newDir)
	{
		const T e = defDir * newDir;
		const T f = (e < 0) ? -e : e;
		_Mat3<T> mtx;

		if (f > static_cast<T>(1.0) - static_cast<T>(0.00001))     /* "defDir" and "newDir"-vecnewDirr almost parallel */
		{
			_Vec3<T> u, v;     /* temporary snewDirrage vecnewDirrs */
			_Vec3<T> x;       /* vecnewDirr most nearly orthogonal newDir "defDir" */

			x.x = (defDir.x > 0.0) ? defDir.x : -defDir.x;
			x.y = (defDir.y > 0.0) ? defDir.y : -defDir.y;
			x.z = (defDir.z > 0.0) ? defDir.z : -defDir.z;

			if (x.x < x.y)
			{
				if (x.x < x.z)
				{
					x.x = static_cast<T>(1.0);
					x.y = x.z = static_cast<T>(0.0);
				}
				else
				{
					x.z = static_cast<T>(1.0);
					x.x = x.y = static_cast<T>(0.0);
				}
			}
			else
			{
				if (x.y < x.z)
				{
					x.y = static_cast<T>(1.0);
					x.x = x.z = static_cast<T>(0.0);
				}
				else
				{
					x.z = static_cast<T>(1.0);
					x.x = x.y = static_cast<T>(0.0);
				}
			}

			u.x = x.x - defDir.x; u.y = x.y - defDir.y; u.z = x.z - defDir.z;
			v.x = x.x - newDir.x; v.y = x.y - newDir.y; v.z = x.z - newDir.z;

			const T c1 = static_cast<T>(2.0) / (u * u);
			const T c2 = static_cast<T>(2.0) / (v * v);
			const T c3 = c1 * c2  * (u * v);

			for (unsigned int i = 0; i < 3; i++)
			{
				for (unsigned int j = 0; j < 3; j++)
				{
					mtx.elements[i][j] = -c1 * u.elements[i] * u.elements[j] - c2 * v.elements[i] * v.elements[j]
						+ c3 * v.elements[i] * u.elements[j];
				}
				mtx.elements[i][i] += static_cast<T>(1.0);
			}
		}
		else  /* the most common case, unless "defDir"="newDir", or "defDir"=-"newDir" */
		{
			const _Vec3<T> v = defDir % newDir;
			/* ... use this hand optimized version (9 mults less) */
			const T h = static_cast<T>(1.0) / (static_cast<T>(1.0) + e);      /* optimization by Gottfried Chen */
			const T hvx = h * v.x;
			const T hvz = h * v.z;
			const T hvxy = hvx * v.y;
			const T hvxz = hvx * v.z;
			const T hvyz = hvz * v.y;
			mtx.elements[0][0] = e + hvx * v.x;
			mtx.elements[0][1] = hvxy - v.z;
			mtx.elements[0][2] = hvxz + v.y;

			mtx.elements[1][0] = hvxy + v.z;
			mtx.elements[1][1] = e + h * v.y * v.y;
			mtx.elements[1][2] = hvyz - v.x;

			mtx.elements[2][0] = hvxz - v.y;
			mtx.elements[2][1] = hvyz + v.x;
			mtx.elements[2][2] = e + hvz * v.z;
		}
		return mtx;
	}

public:
	// [ row ][ col ]
	T elements[3][3];
};

template<typename T>
_Vec3<T>& operator*=( _Vec3<T>& lhs,const _Mat3<T>& rhs )
{
	return lhs = lhs * rhs;
}

template<typename T>
_Vec3<T> operator*( const _Vec3<T>& lhs,const _Mat3<T>& rhs )
{
	return{
		lhs.x * rhs.elements[0][0] + lhs.y * rhs.elements[1][0] + lhs.z * rhs.elements[2][0],
		lhs.x * rhs.elements[0][1] + lhs.y * rhs.elements[1][1] + lhs.z * rhs.elements[2][1],
		lhs.x * rhs.elements[0][2] + lhs.y * rhs.elements[1][2] + lhs.z * rhs.elements[2][2]
	};
}

typedef _Mat3<float> Mat3;
typedef _Mat3<double> Mad3;