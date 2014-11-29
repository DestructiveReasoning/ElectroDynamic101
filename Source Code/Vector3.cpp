#include "Vector3.h"

float Vector3::distanceFrom(const Vector3& vec)
{
	return (float)sqrt((x - vec.getX()) * (x - vec.getX()) + (y - vec.getY()) * (y - vec.getY()) + (z - vec.getZ()) * (z - vec.getZ()));	
}

float Vector3::length()
{
	return (float)sqrt(x*x + y*y + z*z);
}

float Vector3::dot(const Vector3& vec)
{
	return  x * vec.getX() + y * vec.getY() + z * vec.getZ();
}

void Vector3::normalize()
{
	float norm = length();
	x/=norm;
	y/=norm;
	z/=norm;
}

Vector3& Vector3::normalized()
{
	float X,Y,Z;
	float norm = length();
	X = x / norm;
	Y = y / norm;
	Z = z / norm;
	return Vector3(X,Y,Z);
}

void Vector3::increment(float amount, float xmask, float ymask, float zmask)
{
	x += xmask * amount;
	y += ymask * amount;
	z += zmask * amount;
}

void Vector3::set(float _x, float _y, float _z)
{
	x = _x;
	y = _y;
	z = _z;
}

void Vector3::print()
{
	printf("[%.3e, %.3e, %.3e]\n", x, y, z);
}

void Vector3::print(Unit unit)
{
	switch(unit)
	{
	case Position:
	{
		Vector3 v = *this;
		v = v - Vector3(0,0,_zdefault);
		printf("[%.3e, %.3e, %.3e]\n", v.getX(), v.getY(), v.getZ());
		break;
	}
	case Velocity:
		printf("[%.3e, %.3e, %.3e] m/s\n",x,y,z);
		break;
	case Acceleration:
		printf("%.3e, %.3e, %.3e] m/s^2\n",x,y,z);
		break;
	case Force:
		printf("[%.3e, %.3e, %.3e] N\n",x,y,z);
		break;
	case Electric_Field:
		printf("[%.3e, %.3e, %.3e] N/C\n",x,y,z);
		break;
	default:
		printf("Invalid Unit Commanded...\n");
		print();
		break;
	}
}
