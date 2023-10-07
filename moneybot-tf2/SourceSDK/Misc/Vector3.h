#pragma once
#include <math.h>

#define M_RADPI		57.295779513082
#define PI			3.14159265358979323846
#define DEG2RAD(x)	((float)(x) * (float)((float)(PI) / 180.0f))
#define RAD2DEG(x)	((float)(x) * (float)(180.0f / (float)(PI)))

typedef struct Vector3 {
	union
	{
		float x;
		float X;
	};
	union 
	{
		float y;
		float Y;
	};
	union
	{
		float z;
		float Z;
	};

	Vector3(void)
	{
		x = y = z = 0.0f;
	}

	Vector3(float X, float Y, float Z)
	{
		x = X; y = Y; z = Z;
	}

	Vector3(float* v)
	{
		x = v[0]; y = v[1]; z = v[2];
	}

	Vector3(const float* v)
	{
		x = v[0]; y = v[1]; z = v[2];
	}

	Vector3(const Vector3& v)
	{
		x = v.x; y = v.y; z = v.z;
	}

	Vector3& operator=(const Vector3& v)
	{
		x = v.x; y = v.y; z = v.z; return *this;
	}

	float& operator[](int i)
	{
		return ((float*)this)[i];
	}

	float operator[](int i) const
	{
		return ((float*)this)[i];
	}

	Vector3& operator+=(const Vector3& v)
	{
		x += v.x; y += v.y; z += v.z; return *this;
	}

	Vector3& operator-=(const Vector3& v)
	{
		x -= v.x; y -= v.y; z -= v.z; return *this;
	}

	Vector3& operator*=(const Vector3& v)
	{
		x *= v.x; y *= v.y; z *= v.z; return *this;
	}

	Vector3& operator/=(const Vector3& v)
	{
		x /= v.x; y /= v.y; z /= v.z; return *this;
	}

	Vector3& operator+=(float v)
	{
		x += v; y += v; z += v; return *this;
	}

	Vector3& operator-=(float v)
	{
		x -= v; y -= v; z -= v; return *this;
	}

	Vector3& operator*=(float v)
	{
		x *= v; y *= v; z *= v; return *this;
	}

	Vector3& operator/=(float v)
	{
		x /= v; y /= v; z /= v; return *this;
	}

	Vector3 operator+(const Vector3& v) const
	{
		return Vector3(x + v.x, y + v.y, z + v.z);
	}

	Vector3 operator-(const Vector3& v) const
	{
		return Vector3(x - v.x, y - v.y, z - v.z);
	}

	Vector3 operator-() const
	{
		return Vector3(-x, -y, -z);
	}

	bool operator==(const Vector3& v) const
	{
		return (v.x == x) && (v.y == y) && (v.z == z);
	}
	bool operator!=(const Vector3& v) const
	{
		return (v.x != x) || (v.y != y) || (v.z != z);
	}
	Vector3 operator*(const Vector3& v) const
	{
		return Vector3(x * v.x, y * v.y, z * v.z);
	}

	Vector3 operator/(const Vector3& v) const
	{
		return Vector3(x / v.x, y / v.y, z / v.z);
	}

	Vector3 operator+(float v) const
	{
		return Vector3(x + v, y + v, z + v);
	}

	Vector3 operator-(float v) const
	{
		return Vector3(x - v, y - v, z - v);
	}

	Vector3 operator*(float v) const
	{
		return Vector3(x * v, y * v, z * v);
	}

	Vector3 operator/(float v) const
	{
		return Vector3(x / v, y / v, z / v);
	}
	float* Base() {
		return (float*)this;
	}
	void Set(float X = 0.0f, float Y = 0.0f, float Z = 0.0f)
	{
		x = X; y = Y; z = Z;
	}

	float Length(void) const
	{
		return sqrtf(x * x + y * y + z * z);
	}

	float Size(void) const
	{
		return sqrtf(powf(x, 2) + powf(y, 2) + powf(z, 2));
	}

	float LengthSqr(void) const
	{
		return (x * x + y * y + z * z);
	}

	float Length2d(void) const
	{
		return sqrtf(x * x + y * y);
	}

	float Length2dSqr(void) const
	{
		return (x * x + y * y);
	}

	float Normalize()
	{
		Vector3& v = *this;

		float iradius = 1.f / (this->Length() + 1.192092896e-07F); //FLT_EPSILON

		v.x *= iradius;
		v.y *= iradius;
		v.z *= iradius;
		return iradius;
	}

	Vector3 Clamp() {
		//for (size_t axis{ }; axis < 2; axis++) {
		//	auto& cur_axis = operator[](axis);
		//	if (!std::isfinite(cur_axis)) {
		//		cur_axis = 0.f;
		//	}
		//}

		//x = std::clamp(x, -89.f, 89.f);
		//y = std::clamp(std::remainder(y, 360.f), -180.f, 180.f);
		//z = 0.f;
		return *this;
	}

	float DistTo(const Vector3& v) const
	{
		return (*this - v).Length();
	}

	float DistToSqr(const Vector3& v) const
	{
		return (*this - v).LengthSqr();
	}

	float Dot(const Vector3& v) const
	{
		return (x * v.x + y * v.y + z * v.z);
	}

	float FovTo(const Vector3& to) 
	{
		const float from_length = Length();
		const float to_length = to.Length();
		if (from_length && to_length) {
			return acosf(Dot(to) / from_length * to_length);
		}
		return 0.f;
	}

	Vector3 Cross(const Vector3& v) const
	{
		return Vector3(y * v.z - z * v.y, z * v.x - x * v.z, x * v.y - y * v.x);
	}

	Vector3 ToAngle() const
	{
		return Vector3(
			RAD2DEG(atan2(-z, hypot(x, y))),
			RAD2DEG(atan2(y, x)),
			0.0f
		);
	}

	Vector3 FromAngle() const
	{
		return Vector3( cosf(DEG2RAD(x)) * cosf(DEG2RAD(y)),
					   cosf(DEG2RAD(x)) * sinf(DEG2RAD(y)),
					  -sinf(DEG2RAD(x)) );
	}

	bool IsZero(void) const
	{
		return (x > -0.01f && x < 0.01f
			&& y > -0.01f && y < 0.01f
			&& z > -0.01f && z < 0.01f);
	}
} Vec3;


struct __declspec(align(16))VectorAligned : Vector3
{
public:
	inline VectorAligned(void)
	{

	};

	inline VectorAligned(float X, float Y, float Z)
	{
		Set(X, Y, Z);
	}

	explicit VectorAligned(const Vector3 &vOther)
	{
		Set(vOther.x, vOther.y, vOther.z);
	}

	VectorAligned &operator=(const Vector3 &vOther)
	{
		Set(vOther.x, vOther.y, vOther.z);
		return *this;
	}

	float w;
};