#include "Vector3.h"
#include <complex>

Vector3::Vector3(float x, float y, float z) : X(x), Y(y), Z(z) {}

Vector3::Vector3(DirectX::XMFLOAT3 v3) : X(v3.x), Y(v3.y), Z(v3.z) {}

#pragma region Static "Properties"

Vector3 Vector3::Zero()
{
	return {};
}

Vector3 Vector3::One()
{
	return {1, 1, 1};
}

Vector3 Vector3::Up()
{
	return {0, 1, 0};
}

Vector3 Vector3::Down()
{
	return {0, -1, 0};
}

Vector3 Vector3::Forward()
{
	return {0, 0, 1};
}

Vector3 Vector3::Back()
{
	return {0, 0, -1};
}

Vector3 Vector3::Right()
{
	return {1, 0, 0};
}

Vector3 Vector3::Left()
{
	return {-1, 0, 0};
}

#pragma endregion

#pragma region Functions

float Vector3::Magnitude()
{
	float x2 = X * X;
	float y2 = Y * Y;
	float z2 = Z * Z;

	return std::sqrt(x2 + y2 + z2);
}

float Vector3::MagnitudeSqr()
{
	float x2 = X * X;
	float y2 = Y * Y;
	float z2 = Z * Z;

	return x2 + y2 + z2;
}

Vector3 Vector3::Normalise()
{
	float magnitude = Magnitude();
	if (magnitude == 0)
		return Zero();

	float xN = X / magnitude;
	float yN = Y / magnitude;
	float zN = Z / magnitude;

	return {xN, yN, zN};
}

Vector3 Vector3::Cross(Vector3 rhs)
{
	Vector3 cross;
	cross.X = Y * rhs.Z - Z * rhs.Y;
	cross.Y = Z * rhs.X - X * rhs.Z;
	cross.Z = X * rhs.Y - Y * rhs.X;

	return cross;
}

float Vector3::Dot(Vector3 rhs)
{
	float lrX = X * rhs.X;
	float lrY = Y * rhs.Y;
	float lrZ = Z * rhs.Z;

	return lrX + lrY + lrZ;
}

float Vector3::Angle(Vector3 rhs)
{
	float dot = Dot(rhs);

	return std::acos(dot);
}

DirectX::XMFLOAT3 Vector3::ToDXFloat3()
{
	return {X, Y, Z};
}

#pragma endregion

#pragma region Static Functions

Vector3 Vector3::Normalise(Vector3 vector)
{
	float magnitude = Magnitude(vector);
	if (magnitude == 0)
		return Zero();

	float xN = vector.X / magnitude;
	float yN = vector.Y / magnitude;
	float zN = vector.Z / magnitude;

	return {xN, yN, zN};
}

Vector3 Vector3::Cross(Vector3 lhs, Vector3 rhs)
{
	Vector3 cross;
	cross.X = lhs.Y * rhs.Z - lhs.Z * rhs.Y;
	cross.Y = lhs.Z * rhs.X - lhs.X * rhs.Z;
	cross.Z = lhs.X * rhs.Y - lhs.Y * rhs.X;

	return cross;
}


float Vector3::Dot(Vector3 lhs, Vector3 rhs)
{
	float lrX = lhs.X * rhs.X;
	float lrY = lhs.Y * rhs.Y;
	float lrZ = lhs.Z * rhs.Z;

	return lrX + lrY + lrZ;
}

float Vector3::Angle(Vector3 lhs, Vector3 rhs)
{
	float dot = Dot(lhs, rhs);

	return std::acos(dot);
}

float Vector3::Magnitude(Vector3 vector)
{
	float x2 = vector.X * vector.X;
	float y2 = vector.Y * vector.Y;
	float z2 = vector.Z * vector.Z;

	return std::sqrt(x2 + y2 + z2);
}

float Vector3::MagnitudeSqr(Vector3 vector)
{
	float x2 = vector.X * vector.X;
	float y2 = vector.Y * vector.Y;
	float z2 = vector.Z * vector.Z;

	return x2 + y2 + z2;
}

DirectX::XMFLOAT3 Vector3::ToDXFloat3(Vector3 vector)
{
	return {vector.X, vector.Y, vector.Z};
}

#pragma endregion

#pragma region Operators

Vector3 Vector3::operator+(DirectX::XMFLOAT3 rhs)
{
	return {X + rhs.x, Y + rhs.y, Z + rhs.z};
}

Vector3 Vector3::operator+(Vector3 rhs)
{
	return {X + rhs.X, Y + rhs.Y, Z + rhs.Z};
}

Vector3 Vector3::operator-(DirectX::XMFLOAT3 rhs)
{
	return {X - rhs.x, Y - rhs.y, Z - rhs.z};
}

Vector3 Vector3::operator-(Vector3 rhs)
{
	return {X - rhs.X, Y - rhs.Y, Z - rhs.Z};
}

Vector3 Vector3::operator*(float scalar)
{
	return {X * scalar, Y * scalar, Z * scalar};
}

Vector3 Vector3::operator/(float scalar)
{
	return {X / scalar, Y / scalar, Z / scalar};
}

Vector3& Vector3::operator=(DirectX::XMFLOAT3& rhs)
{
	X = rhs.x;
	Y = rhs.y;
	Z = rhs.z;
	return *this;
}

Vector3& Vector3::operator+=(DirectX::XMFLOAT3 rhs)
{
	X += rhs.x;
	Y += rhs.y;
	Z += rhs.z;
	return *this;
}

Vector3& Vector3::operator+=(Vector3 rhs)
{
	X += rhs.X;
	Y += rhs.Y;
	Z += rhs.Z;
	return *this;
}

Vector3& Vector3::operator-=(DirectX::XMFLOAT3 rhs)
{
	X -= rhs.x;
	Y -= rhs.y;
	Z -= rhs.z;
	return *this;
}

Vector3& Vector3::operator-=(Vector3 rhs)
{
	X -= rhs.X;
	Y -= rhs.Y;
	Z -= rhs.Z;
	return *this;
}

Vector3& Vector3::operator*=(float scalar)
{
	X *= scalar;
	Y *= scalar;
	Z *= scalar;
	return *this;
}

Vector3& Vector3::operator/=(float scalar)
{
	X /= scalar;
	Y /= scalar;
	Z /= scalar;
	return *this;
}

#pragma endregion