#pragma once

struct TPoint2D
{
	float x, y;
	TPoint2D(const TPoint2D & p) { this->x = p.x; this->y = p.y;}
	TPoint2D(float x, float y) { this->x = x; this->y = y;}
	TPoint2D() { x = 0; y = 0; }
	TPoint2D operator + (const TPoint2D &p) const
	{
		return TPoint2D(x + p.x, y + p.y);
	};
	TPoint2D operator - (const TPoint2D &p) const
	{
		return TPoint2D(x - p.x, y - p.y);
	};
	TPoint2D operator * (float s) const
	{
		return TPoint2D(x * s, s * y);
	};
	TPoint2D operator / (float s) const
	{
		return TPoint2D(x / s, y / s);
	};
	TPoint2D operator - () const
	{
		return TPoint2D(-x, -y);
	};
	bool operator == (const TPoint2D &p)
	{
		return p.x == x && p.y == y;
	};
};