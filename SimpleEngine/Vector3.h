#pragma once
#include <DirectXMath.h>

class Vector3
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
	[[nodiscard]] Vector3 Normalise();
	[[nodiscard]] DirectX::XMFLOAT3 ToDXFloat3();
	

public:
	static float Dot(Vector3 lhs, Vector3 rhs);
	static float Angle(Vector3 lhs, Vector3 rhs);
	static float Magnitude(Vector3 vector);
	static Vector3 Normalise(Vector3 vector);
	static DirectX::XMFLOAT3 ToDXFloat3(Vector3 vector);

	static Vector3 Zero;
};
