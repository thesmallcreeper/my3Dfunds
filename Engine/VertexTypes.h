#pragma once

#include <cmath>
#include "Vec3.h"

class DefaultVertex
{
public:
	DefaultVertex() = default;
	DefaultVertex(const Vec3& pos)
		:
		pos(pos)
	{}
	DefaultVertex(const Vec3& pos, const DefaultVertex& src)
		:
		pos(pos)
	{}
	DefaultVertex& operator+=(const DefaultVertex& rhs)
	{
		pos += rhs.pos;
		return *this;
	}
	DefaultVertex operator+(const DefaultVertex& rhs) const
	{
		return DefaultVertex(*this) += rhs;
	}
	DefaultVertex& operator-=(const DefaultVertex& rhs)
	{
		pos -= rhs.pos;
		return *this;
	}
	DefaultVertex operator-(const DefaultVertex& rhs) const
	{
		return DefaultVertex(*this) -= rhs;
	}
	DefaultVertex& operator*=(float rhs)
	{
		pos *= rhs;
		return *this;
	}
	DefaultVertex operator*(float rhs) const
	{
		return DefaultVertex(*this) *= rhs;
	}
	DefaultVertex& operator/=(float rhs)
	{
		pos /= rhs;
		return *this;
	}
	DefaultVertex operator/(float rhs) const
	{
		return DefaultVertex(*this) /= rhs;
	}
public:
	Vec3 pos;
};

class DefaultVertexWithTC
{
public:
	DefaultVertexWithTC() = default;
	DefaultVertexWithTC(const Vec3& pos)
		:
		pos(pos)
	{}
	DefaultVertexWithTC(const Vec3& pos, const DefaultVertexWithTC& src)
		:
		t(src.t),
		pos(pos)
	{}
	DefaultVertexWithTC(const Vec3& pos, const Vec2& t)
		:
		t(t),
		pos(pos)
	{}
	DefaultVertexWithTC& operator+=(const DefaultVertexWithTC& rhs)
	{
		pos += rhs.pos;
		t += rhs.t;
		return *this;
	}
	DefaultVertexWithTC operator+(const DefaultVertexWithTC& rhs) const
	{
		return DefaultVertexWithTC(*this) += rhs;
	}
	DefaultVertexWithTC& operator-=(const DefaultVertexWithTC& rhs)
	{
		pos -= rhs.pos;
		t -= rhs.t;
		return *this;
	}
	DefaultVertexWithTC operator-(const DefaultVertexWithTC& rhs) const
	{
		return DefaultVertexWithTC(*this) -= rhs;
	}
	DefaultVertexWithTC& operator*=(float rhs)
	{
		pos *= rhs;
		t *= rhs;
		return *this;
	}
	DefaultVertexWithTC operator*(float rhs) const
	{
		return DefaultVertexWithTC(*this) *= rhs;
	}
	DefaultVertexWithTC& operator/=(float rhs)
	{
		pos /= rhs;
		t /= rhs;
		return *this;
	}
	DefaultVertexWithTC operator/(float rhs) const
	{
		return DefaultVertexWithTC(*this) /= rhs;
	}
public:
	Vec3 pos;
	Vec2 t;
};
