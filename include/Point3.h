#pragma once

class Point3
{
public:
	float x, y, z;
	float GetLength() const;
	Point3 Normalize() const;

	Point3& operator += ( const Point3& p);

	Point3& operator -= ( const Point3& p);

	Point3& operator *= ( float k);

	Point3& operator /= ( float k);

	Point3& operator = ( const Point3& p);
	// binary operators
	float operator * ( const Point3& ) const;
	Point3 operator % ( const Point3& ) const;
	Point3 operator + ( const Point3& ) const;
	Point3 operator - ( const Point3& ) const;
	Point3 operator * ( float ) const;
	Point3 operator / ( float ) const;

	Point3 operator - () const;

	friend Point3 operator * ( float, const Point3& );

	bool operator == ( const Point3& ) const;
	bool operator != ( const Point3& ) const;

	Point3() { x = y = z = 0; };
	Point3(const Point3 & p);
	Point3(float x, float y, float z);

};

Point3 operator * ( float, const Point3& );