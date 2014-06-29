#include <stdlib.h>
#include <math.h>
#include "Point3.h"

Point3::Point3(float x, float y, float z): x(x), y(y), z(z)
{
}

Point3::Point3(const Point3 & p)
{
	*this = p;
}

float Point3::GetLength() const
{
	return sqrt(x * x + y * y + z * z);
}

Point3 Point3::Normalize() const
{
	Point3 r = *this / GetLength();
	return r;
}

Point3& Point3::operator = ( const Point3& p)
{
	this->x = p.x;
	this->y = p.y;
	this->z = p.z;
	return *this;
}

Point3& Point3::operator += ( const Point3& p)
{
	this->x += p.x;
	this->y += p.y;
	this->z += p.z;
	return *this;
}

Point3& Point3::operator -= ( const Point3& p)
{
	this->x -= p.x;
	this->y -= p.y;
	this->z -= p.z;
	return *this;
}

Point3& Point3::operator *= ( float k)
{
	this->x *= k;
	this->y *= k;
	this->z *= k;
	return *this;
}

Point3& Point3::operator /= ( float k)
{
	this->x /= k;
	this->y /= k;
	this->z /= k;
	return *this;
}

float Point3::operator *( const Point3& p) const
{
	return x * p.x + y * p.y + z * p.z;
}

Point3 Point3::operator %( const Point3& p) const
{
	return Point3((y * p.z) - (z * p.y),
				(z * p.x) - (x * p.z),
				(x * p.y) - (y * p.x));
}

Point3 Point3::operator + ( const Point3& p) const
{
	return Point3(x + p.x, y + p.y, z + p.z);
}

Point3 Point3::operator - ( const Point3& p) const
{
	return Point3(x - p.x, y - p.y, z - p.z);
}

Point3 Point3::operator * ( float k) const
{
	return Point3(x * k, y * k, z * k);
}

Point3 Point3::operator / ( float k) const
{
	return Point3(x / k, y / k, z / k);
}

Point3 operator * ( float k, const Point3& p) 
{
	return Point3(p.x * k, p.y * k, p.z * k);
}

Point3 Point3::operator - () const
{
	return Point3(-x, -y, -z);
}

bool Point3::operator ==( const Point3& p) const
{
	return x == p.x && x == p.x && z == p.z;
}

bool Point3::operator !=(const Point3 & p) const
{
	return ! (*this == p);
}