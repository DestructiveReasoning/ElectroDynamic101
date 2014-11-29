#pragma once

#ifndef _VECTOR3_
#define _VECTOR3_
#include "destructive_reasoning.h"

class Vector3
{
public:
	Vector3(void) : x(0.0f), y(0.0f), z(_zdefault) {};
	Vector3(float _x, float _y, float _z) : x(_x), y(_y), z(_z) {};

	inline float getX() const { return x; };
	inline float getY() const { return y; };
	inline float getZ() const { return z; };

	void increment(float amount, float xmask, float ymask, float zmask);
	void set(float _x, float _y, float _z);

	inline Vector3	operator+(const Vector3& vec) const;
	inline Vector3	operator+=(const Vector3& vec);
	inline Vector3	operator-(const Vector3& vec);
	inline Vector3	operator-=(const Vector3& vec);
	inline Vector3	operator*(const float k);
	inline Vector3	operator*=(const float k);
	inline Vector3	operator/(const float k);
	inline Vector3	operator/=(const float k);
	inline Vector3	operator=(const Vector3& vec);
	inline bool		operator==(const Vector3& vec);

	float length();
	float dot(const Vector3& vec);
	void normalize();
	inline Vector3& normalized();
	float distanceFrom(const Vector3& vec);


	enum Unit {Position, Force, Velocity, Acceleration, Electric_Field};
	void print();
	void print(Unit unit);

private:
	float x;
	float y;
	float z;
};

Vector3 Vector3::operator +(const Vector3& vec) const
{
	Vector3& res = Vector3(this->x + vec.getX(), this->y + vec.getY(), this->z + vec.getZ());
	return res;
}

Vector3 Vector3::operator+=(const Vector3& vec)
{
	x += vec.getX();
	y += vec.getY();
	z += vec.getZ();
	return Vector3(x,y,z);
}

Vector3 Vector3::operator-(const Vector3& vec)
{
	return Vector3(x - vec.getX(), y - vec.getY(), z - vec.getZ());
}

Vector3 Vector3::operator-=(const Vector3& vec)
{
	x -= vec.getX();
	y -= vec.getY();
	z -= vec.getZ();
	return Vector3(x,y,z);
}

Vector3 Vector3::operator*(const float k)
{
	return Vector3(x * k, y * k, z * k);	
}

Vector3 Vector3::operator*=(const float k)
{
	x *= k;
	y *= k;
	z *= k;
	return Vector3(x,y,z);
}

Vector3 Vector3::operator/(const float k)
{
	return Vector3(x / k, y / k, z / k);	
}

Vector3 Vector3::operator/=(const float k)
{
	x /= k;
	y /= k;
	z /= k;
	return Vector3(x,y,z);
}

Vector3 Vector3::operator=(const Vector3& vec)
{
	x = vec.getX();
	y = vec.getY();
	z = vec.getZ();
	return Vector3(x,y,z);
}

bool Vector3::operator==(const Vector3& vec)
{
	if(x == vec.getX() && y == vec.getY() && z == vec.getZ()) return true;
	return false;
}

#endif

