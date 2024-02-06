#pragma once
#include <cmath>

#include "Constants.h"
#include "Vector3.h"
//------------------------------------------------------------------------
// Quaternion Class and Quaternion functions
//------------------------------------------------------------------------

class Quaternion {
public:
    float      N;     // number (scalar) part
    Vector3    V;     // vector part: v.x, v.y, v.z

    Quaternion(void);
    Quaternion(DirectX::XMFLOAT4 q);
    Quaternion(float e0, float e1, float e2, float e3);

    float      Magnitude(void);
    Vector3    GetVector(void);
    float      GetScalar(void);

    DirectX::XMFLOAT4 ToDXFloat4();

    Quaternion operator+=(Quaternion q);
    Quaternion operator-=(Quaternion q);
    Quaternion operator*=(float s);
    Quaternion operator/=(float s);
    Quaternion operator~(void) const;
};

// Default Constructor
inline Quaternion::Quaternion(void)
{
    N = 1;
    V.X = 0;
    V.Y = 0;
    V.Z = 0;
}

inline Quaternion::Quaternion(DirectX::XMFLOAT4 q)
{
	N = q.w;
    V.X = q.x;
    V.Y = q.y;
    V.Z = q.z;
}

// Constructor
inline Quaternion::Quaternion(float e0, float e1, float e2, float e3)
{
    N = e0;
    V.X = e1;
    V.Y = e2;
    V.Z = e3;
}

inline DirectX::XMFLOAT4 Quaternion::ToDXFloat4()
{
    DirectX::XMFLOAT4 q;
    q.w = N;
    q.x = V.X;
    q.y = V.Y;
    q.z = V.Z;

    return q;
}


inline float Quaternion::Magnitude(void)
{
    return std::sqrt(N * N + V.X * V.X + V.Y * V.Y + V.Z * V.Z);
}

inline Vector3 Quaternion::GetVector(void)
{
    return {V.X, V.Y, V.Z};
}

inline float Quaternion::GetScalar(void)
{
    return N;
}

inline Quaternion Quaternion::operator+=(Quaternion q)
{
    N += q.N;
    V.X += q.V.X;
    V.Y += q.V.Y;
    V.Z += q.V.Z;
    return *this;
}

inline Quaternion Quaternion::operator-=(Quaternion q)
{
    N -= q.N;
    V.X -= q.V.X;
    V.Y -= q.V.Y;
    V.Z -= q.V.Z;
    return *this;
}

inline Quaternion Quaternion::operator*=(float s)
{
    N *= s;
    V.X *= s;
    V.Y *= s;
    V.Z *= s;
    return *this;
}

inline Quaternion Quaternion::operator/=(float s)
{
    N /= s;
    V.X /= s;
    V.Y /= s;
    V.Z /= s;
    return *this;
}

inline Quaternion Quaternion::operator~(void) const
{
    return {
	    N,
        -V.X,
        -V.Y,
        -V.Z
    };
}

inline Quaternion operator+(Quaternion q1, Quaternion q2)
{
    return {
	    q1.N + q2.N,
        q1.V.X + q2.V.X,
        q1.V.Y + q2.V.Y,
        q1.V.Z + q2.V.Z
    };
}

inline Quaternion operator-(Quaternion q1, Quaternion q2)
{
    return {
	    q1.N - q2.N,
        q1.V.X - q2.V.X,
        q1.V.Y - q2.V.Y,
        q1.V.Z - q2.V.Z
    };
}

inline Quaternion operator*(Quaternion q1, Quaternion q2)
{
    return {
	    q1.N * q2.N - q1.V.X * q2.V.X
        - q1.V.Y * q2.V.Y - q1.V.Z * q2.V.Z,
        q1.N * q2.V.X + q1.V.X * q2.N
        + q1.V.Y * q2.V.Z - q1.V.Z * q2.V.Y,
        q1.N * q2.V.Y + q1.V.Y * q2.N
        + q1.V.Z * q2.V.X - q1.V.X * q2.V.Z,
        q1.N * q2.V.Z + q1.V.Z * q2.N
        + q1.V.X * q2.V.Y - q1.V.Y * q2.V.X
    };
}

inline Quaternion operator*(Quaternion q, float s)
{
    return {q.N * s, q.V.X * s, q.V.Y * s, q.V.Z * s};
}

inline Quaternion operator*(float s, Quaternion q)
{
    return {q.N * s, q.V.X * s, q.V.Y * s, q.V.Z * s};
}

inline Quaternion operator*(Quaternion q, Vector3 v)
{
    return {
	    -(q.V.X * v.X + q.V.Y * v.Y + q.V.Z * v.Z),
        q.N * v.X + q.V.Y * v.Z - q.V.Z * v.Y,
        q.N * v.Y + q.V.Z * v.X - q.V.X * v.Z,
        q.N * v.Z + q.V.X * v.Y - q.V.Y * v.X
    };
}

inline Quaternion operator*(Vector3 v, Quaternion q)
{
    return {
	    -(q.V.X * v.X + q.V.Y * v.Y + q.V.Z * v.Z),
        q.N * v.X + q.V.Z * v.Y - q.V.Y * v.Z,
        q.N * v.Y + q.V.X * v.Z - q.V.Z * v.X,
        q.N * v.Z + q.V.Y * v.X - q.V.X * v.Y
    };
}

inline Quaternion operator/(Quaternion q, float s)
{
    return {q.N / s, q.V.X / s, q.V.Y / s, q.V.Z / s};
}

inline float QGetAngle(Quaternion q)
{
    return 2 * acos(q.N);
}

inline Vector3 QGetAxis(Quaternion q)
{
	Vector3 v = q.GetVector();
    float m = v.Magnitude();

    if (m != 0)
        return {};

	return v / m;
}

inline Quaternion QRotate(Quaternion q1, Quaternion q2)
{
    return q1 * q2 * (~q1);
}

inline Vector3 QVRotate(Quaternion q, Vector3 v)
{
	Quaternion t = q * v * (~q);
    return t.GetVector();
}

inline float DegreesToRadians(float deg)
{
    return deg * PI / 180.0f;
}

inline float RadiansToDegrees(float rad)
{
    return rad * 180.0f / PI;
}

inline Quaternion MakeQFromEulerAngles(float x, float y, float z)
{
    Quaternion     q;
    double     roll = DegreesToRadians(x);
    double     pitch = DegreesToRadians(y);
    double     yaw = DegreesToRadians(z);

    double cyaw = cos(0.5f * yaw);
    double cpitch = cos(0.5f * pitch);
    double croll = cos(0.5f * roll);
    double syaw = sin(0.5f * yaw);
    double spitch = sin(0.5f * pitch);
    double sroll = sin(0.5f * roll);

    double cyawcpitch = cyaw * cpitch;
    double syawspitch = syaw * spitch;
    double cyawspitch = cyaw * spitch;
    double syawcpitch = syaw * cpitch;

    q.N = static_cast<float>(cyawcpitch * croll + syawspitch * sroll);
    q.V.X = static_cast<float>(cyawcpitch * sroll - syawspitch * croll);
    q.V.Y = static_cast<float>(cyawspitch * croll + syawcpitch * sroll);
    q.V.Z = static_cast<float>(syawcpitch * croll - cyawspitch * sroll);

    return q;
}

inline Vector3 MakeEulerAnglesFromQ(Quaternion q)
{
	Vector3     u;

    double q00 = q.N * q.N;
    double q11 = q.V.X * q.V.X;
    double q22 = q.V.Y * q.V.Y;
    double q33 = q.V.Z * q.V.Z;

    double r11 = q00 + q11 - q22 - q33;
    double r21 = 2 * (q.V.X * q.V.Y + q.N * q.V.Z);
    double r31 = 2 * (q.V.X * q.V.Z - q.N * q.V.Y);
    double r32 = 2 * (q.V.Y * q.V.Z + q.N * q.V.X);
    double r33 = q00 - q11 - q22 + q33;

    double tmp = fabs(r31);
    if (tmp > 0.999999)
    {
        double r12 = 2 * (q.V.X * q.V.Y - q.N * q.V.Z);
        double r13 = 2 * (q.V.X * q.V.Z + q.N * q.V.Y);

        u.X = RadiansToDegrees(0.0f); //roll
        u.Y = RadiansToDegrees(static_cast<float>(-(PI / 2) * r31 / tmp));   // pitch
        u.Z = RadiansToDegrees(static_cast<float>(atan2(-r12, -r31 * r13))); // yaw
        return u;
    }

    u.X = RadiansToDegrees(static_cast<float>(atan2(r32, r33))); // roll
    u.Y = RadiansToDegrees(static_cast<float>(asin(-r31)));      // pitch
    u.Z = RadiansToDegrees(static_cast<float>(atan2(r21, r11))); // yaw
    return u;
}