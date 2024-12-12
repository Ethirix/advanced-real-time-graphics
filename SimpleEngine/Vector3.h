#pragma once
#include <DirectXMath.h>

class  Vector3
{
public:
	Vector3(float x = 0, float y = 0, float z = 0);
	Vector3(DirectX::XMFLOAT3 v3);

	float X, Y, Z;

	Vector3 operator+(Vector3 rhs);
	Vector3 operator+(DirectX::XMFLOAT3 rhs);
	
	Vector3 operator-(Vector3 rhs);
	Vector3 operator-(DirectX::XMFLOAT3 rhs);

	Vector3 operator*(float scalar);
	Vector3 operator/(float scalar);
	
	Vector3& operator=(DirectX::XMFLOAT3& rhs);
	Vector3& operator+=(Vector3 rhs);
	Vector3& operator+=(DirectX::XMFLOAT3 rhs);
	Vector3& operator-=(Vector3 rhs);
	Vector3& operator-=(DirectX::XMFLOAT3 rhs);
	Vector3& operator*=(float scalar);
	Vector3& operator/=(float scalar);

	[[nodiscard]] float Dot(Vector3 rhs);
	[[nodiscard]] float Angle(Vector3 rhs);
	[[nodiscard]] float Magnitude();
	[[nodiscard]] float MagnitudeSqr();
	[[nodiscard]] Vector3 Cross(Vector3 rhs);
	[[nodiscard]] Vector3 Normalise();
	[[nodiscard]] DirectX::XMFLOAT3 ToDXFloat3();
	

public:
	[[nodiscard]] static float Dot(Vector3 lhs, Vector3 rhs);
	[[nodiscard]] static float Angle(Vector3 lhs, Vector3 rhs);
	[[nodiscard]] static float Magnitude(Vector3 vector);
	[[nodiscard]] static float MagnitudeSqr(Vector3 vector);
	[[nodiscard]] static Vector3 Cross(Vector3 lhs, Vector3 rhs);
	[[nodiscard]] static Vector3 Normalise(Vector3 vector);
	[[nodiscard]] static DirectX::XMFLOAT3 ToDXFloat3(Vector3 vector);

	[[nodiscard]] static Vector3 Zero();
	[[nodiscard]] static Vector3 One();
	[[nodiscard]] static Vector3 Up();
	[[nodiscard]] static Vector3 Down();
	[[nodiscard]] static Vector3 Forward();
	[[nodiscard]] static Vector3 Back();
	[[nodiscard]] static Vector3 Right();
	[[nodiscard]] static Vector3 Left();
};
