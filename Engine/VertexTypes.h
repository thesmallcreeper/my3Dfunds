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

class DefaultVertexWithPhong
{
public:
	DefaultVertexWithPhong() = default;
	DefaultVertexWithPhong(const Vec3& pos)
		:
		pos(pos)
	{}
	DefaultVertexWithPhong(const Vec3& pos, const DefaultVertexWithPhong& src)
		:
		normal(src.normal),
		tolightsrc(src.tolightsrc),
		pos(pos)
	{}
	DefaultVertexWithPhong(const Vec3& pos, const Vec3& tolightsrc, const Vec3& normal)
		:
		normal(normal),
		tolightsrc(tolightsrc),
		pos(pos)
	{}
	DefaultVertexWithPhong& operator+=(const DefaultVertexWithPhong& rhs)
	{
		pos += rhs.pos;
		tolightsrc += rhs.tolightsrc;
		normal += rhs.normal;
		return *this;
	}
	DefaultVertexWithPhong operator+(const DefaultVertexWithPhong& rhs) const
	{
		return DefaultVertexWithPhong(*this) += rhs;
	}
	DefaultVertexWithPhong& operator-=(const DefaultVertexWithPhong& rhs)
	{
		pos -= rhs.pos;
		tolightsrc -= rhs.tolightsrc;
		normal -= rhs.normal;
		return *this;
	}
	DefaultVertexWithPhong operator-(const DefaultVertexWithPhong& rhs) const
	{
		return DefaultVertexWithPhong(*this) -= rhs;
	}
	DefaultVertexWithPhong& operator*=(float rhs)
	{
		pos *= rhs;
		tolightsrc *= rhs;
		normal *= rhs;
		return *this;
	}
	DefaultVertexWithPhong operator*(float rhs) const
	{
		return DefaultVertexWithPhong(*this) *= rhs;
	}
	DefaultVertexWithPhong& operator/=(float rhs)
	{
		pos /= rhs;
		tolightsrc /= rhs;
		normal /= rhs;
		return *this;
	}
	DefaultVertexWithPhong operator/(float rhs) const
	{
		return DefaultVertexWithPhong(*this) /= rhs;
	}
public:
	Vec3 pos;
	Vec3 tolightsrc;
	Vec3 normal;
};

class DefaultVertexWithPhongAndTC
{
public:
	DefaultVertexWithPhongAndTC() = default;
	DefaultVertexWithPhongAndTC(const Vec3& pos)
		:
		pos(pos)
	{}
	DefaultVertexWithPhongAndTC(const Vec3& pos, const DefaultVertexWithPhongAndTC& src)
		:
		t(src.t),
		normal(src.normal),
		tolightsrc(src.tolightsrc),
		pos(pos)
	{}
	DefaultVertexWithPhongAndTC(const Vec3& pos, const Vec3& tolightsrc, const Vec3& normal, const Vec2& t)
		:
		t(t),
		normal(normal),
		tolightsrc(tolightsrc),
		pos(pos)
	{}
	DefaultVertexWithPhongAndTC& operator+=(const DefaultVertexWithPhongAndTC& rhs)
	{
		pos += rhs.pos;
		tolightsrc += rhs.tolightsrc;
		normal += rhs.normal;
		t += rhs.t;
		return *this;
	}
	DefaultVertexWithPhongAndTC operator+(const DefaultVertexWithPhongAndTC& rhs) const
	{
		return DefaultVertexWithPhongAndTC(*this) += rhs;
	}
	DefaultVertexWithPhongAndTC& operator-=(const DefaultVertexWithPhongAndTC& rhs)
	{
		pos -= rhs.pos;
		tolightsrc -= rhs.tolightsrc;
		normal -= rhs.normal;
		t -= rhs.t;
		return *this;
	}
	DefaultVertexWithPhongAndTC operator-(const DefaultVertexWithPhongAndTC& rhs) const
	{
		return DefaultVertexWithPhongAndTC(*this) -= rhs;
	}
	DefaultVertexWithPhongAndTC& operator*=(float rhs)
	{
		pos *= rhs;
		tolightsrc *= rhs;
		normal *= rhs;
		t *= rhs;
		return *this;
	}
	DefaultVertexWithPhongAndTC operator*(float rhs) const
	{
		return DefaultVertexWithPhongAndTC(*this) *= rhs;
	}
	DefaultVertexWithPhongAndTC& operator/=(float rhs)
	{
		pos /= rhs;
		tolightsrc /= rhs;
		normal /= rhs;
		t /= rhs;
		return *this;
	}
	DefaultVertexWithPhongAndTC operator/(float rhs) const
	{
		return DefaultVertexWithPhongAndTC(*this) /= rhs;
	}
public:
	Vec3 pos;
	Vec3 tolightsrc;
	Vec3 normal;
	Vec2 t;
};

