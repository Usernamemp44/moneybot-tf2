#pragma once
#include <math.h>
#include <cmath>
#include <algorithm>
#define M_RADPI		57.295779513082
#define PI			3.14159265358979323846
#define DEG2RAD(x)	((float)(x) * (float)((float)(PI) / 180.0f))
#define RAD2DEG(x)	((float)(x) * (float)(180.0f / (float)(PI)))
#define SQUARE(x)	(x * x)
#include "../SourceSDK/Misc/Defines.h"
#include "../SourceSDK/Interfaces/CEngineClient/CEngineClient.h"

namespace Math
{
	inline double fsqrt(double n)
	{
		return sqrt(n);
	}

	inline void SinCos(float radians, float *sine, float *cosine)
	{
		*sine = sin(radians);
		*cosine = cos(radians);

	}

	inline void VectorAngles(Vec3 vForward, Vec3 &vAngles)
	{
		float tmp, yaw, pitch;

		if (vForward.y == 0.0f && vForward.x == 0.0f)
		{
			yaw = 0.0f;

			if (vForward.z > 0.0f)
				pitch = 270.0f;
			else
				pitch = 90.0f;
		}

		else
		{
			yaw = RAD2DEG(atan2f(vForward.y, vForward.x));

			if (yaw < 0.0f)
				yaw += 360.0f;

			tmp = vForward.Length2d();
			pitch = RAD2DEG(atan2f(-vForward.z, tmp));

			if (pitch < 0.0f)
				pitch += 360.0f;
		}

		vAngles.x = pitch;
		vAngles.y = yaw;
		vAngles.z = 0.0f;
	}

	inline void MakeVector(Vec3 vAngle, Vec3 &vVector)
	{
		float fPitch = float(vAngle.x * PI / 180.0f);
		float fYaw = float(vAngle.y * PI / 180.0f);
		float fTmp = float(cos(fPitch));

		vVector.x = (-fTmp * -cosf(fYaw));
		vVector.y = (sinf(fYaw) * fTmp);
		vVector.z = -sinf(fPitch);
	}

	inline void AngleVectors(const Vec3 &angles, Vec3 *forward)
	{
		float sp, sy, cp, cy;

		sy = sinf(DEG2RAD(angles.y));
		cy = cosf(DEG2RAD(angles.y));

		sp = sinf(DEG2RAD(angles.x));
		cp = cosf(DEG2RAD(angles.x));

		forward->x = (cp * cy);
		forward->y = (cp * sy);
		forward->z = -sp;
	}

	inline void AngleVectors(const Vec3 &angles, Vec3 *forward, Vec3 *right, Vec3 *up)
	{
		float sr, sp, sy, cr, cp, cy;
		SinCos(DEG2RAD(angles.x), &sp, &cp);
		SinCos(DEG2RAD(angles.y), &sy, &cy);
		SinCos(DEG2RAD(angles.z), &sr, &cr);

		if (forward) {
			forward->x = cp * cy;
			forward->y = cp * sy;
			forward->z = -sp;
		}

		if (right) {
			right->x = (-1 * sr * sp * cy + -1 * cr * -sy);
			right->y = (-1 * sr * sp * sy + -1 * cr * cy);
			right->z = -1 * sr * cp;
		}

		if (up) {
			up->x = (cr * sp * cy + -sr * -sy);
			up->y = (cr * sp * sy + -sr * cy);
			up->z = cr * cp;
		}
	}

	inline float DotProduct(const float *v1, const float *v2)
	{
		return v1[0] * v2[0] + v1[1] * v2[1] + v1[2] * v2[2];
	}

	inline void VectorTransform(const Vec3 &vSome, const matrix3x4 &vMatrix, Vec3 &vOut)
	{
		for (auto i = 0; i < 3; i++)
			vOut[i] = vSome.Dot((Vec3 &)vMatrix[i]) + vMatrix[i][3];
	}

	inline void MatrixSetColumn(const Vec3 &in, int column, matrix3x4 &out)
	{
		out[0][column] = in.x;
		out[1][column] = in.y;
		out[2][column] = in.z;
	}

	inline void MatrixCopy(const matrix3x4 &source, matrix3x4 &target)
	{
		for (int i = 0; i < 3; i++) {
			for (int j = 0; j < 4; j++) {
				target[i][j] = source[i][j];
			}
		}
	}

	inline void AngleMatrix(const Vec3 &angles, matrix3x4 &matrix)
	{
		float sr, sp, sy, cr, cp, cy;

		SinCos(DEG2RAD(angles.y), &sy, &cy);
		SinCos(DEG2RAD(angles.x), &sp, &cp);
		SinCos(DEG2RAD(angles.z), &sr, &cr);

		// matrix = (YAW * PITCH) * ROLL
		matrix[0][0] = cp * cy;
		matrix[1][0] = cp * sy;
		matrix[2][0] = -sp;

		float crcy = cr * cy;
		float crsy = cr * sy;
		float srcy = sr * cy;
		float srsy = sr * sy;
		matrix[0][1] = sp * srcy - crsy;
		matrix[1][1] = sp * srsy + crcy;
		matrix[2][1] = sr * cp;

		matrix[0][2] = sp * crcy + srsy;
		matrix[1][2] = sp * crsy - srcy;
		matrix[2][2] = cr * cp;

		matrix[0][3] = 0.0f;
		matrix[1][3] = 0.0f;
		matrix[2][3] = 0.0f;
	}

	inline void AngleMatrix(const Vec3 &angles, const Vec3 &origin, matrix3x4 &matrix)
	{
		AngleMatrix(angles, matrix);
		MatrixSetColumn(origin, 3, matrix);
	}

	inline void ConcatTransforms(const matrix3x4 &in1, const matrix3x4 &in2, matrix3x4 &out)
	{
		if (&in1 == &out) {
			matrix3x4 in1b;
			MatrixCopy(in1, in1b);
			ConcatTransforms(in1b, in2, out);
			return;
		}

		if (&in2 == &out) {
			matrix3x4 in2b;
			MatrixCopy(in2, in2b);
			ConcatTransforms(in1, in2b, out);
			return;
		}

		out[0][0] = in1[0][0] * in2[0][0] + in1[0][1] * in2[1][0] +
			in1[0][2] * in2[2][0];
		out[0][1] = in1[0][0] * in2[0][1] + in1[0][1] * in2[1][1] +
			in1[0][2] * in2[2][1];
		out[0][2] = in1[0][0] * in2[0][2] + in1[0][1] * in2[1][2] +
			in1[0][2] * in2[2][2];
		out[0][3] = in1[0][0] * in2[0][3] + in1[0][1] * in2[1][3] +
			in1[0][2] * in2[2][3] + in1[0][3];
		out[1][0] = in1[1][0] * in2[0][0] + in1[1][1] * in2[1][0] +
			in1[1][2] * in2[2][0];
		out[1][1] = in1[1][0] * in2[0][1] + in1[1][1] * in2[1][1] +
			in1[1][2] * in2[2][1];
		out[1][2] = in1[1][0] * in2[0][2] + in1[1][1] * in2[1][2] +
			in1[1][2] * in2[2][2];
		out[1][3] = in1[1][0] * in2[0][3] + in1[1][1] * in2[1][3] +
			in1[1][2] * in2[2][3] + in1[1][3];
		out[2][0] = in1[2][0] * in2[0][0] + in1[2][1] * in2[1][0] +
			in1[2][2] * in2[2][0];
		out[2][1] = in1[2][0] * in2[0][1] + in1[2][1] * in2[1][1] +
			in1[2][2] * in2[2][1];
		out[2][2] = in1[2][0] * in2[0][2] + in1[2][1] * in2[1][2] +
			in1[2][2] * in2[2][2];
		out[2][3] = in1[2][0] * in2[0][3] + in1[2][1] * in2[1][3] +
			in1[2][2] * in2[2][3] + in1[2][3];
	}

	inline void MatrixMultiply(const matrix3x4 &in1, const matrix3x4 &in2, matrix3x4 &out)
	{
		ConcatTransforms(in1, in2, out);
	}

	inline void VectorRotate(const float *in1, const matrix3x4 &in2, float *out)
	{
		out[0] = DotProduct(in1, in2[0]);
		out[1] = DotProduct(in1, in2[1]);
		out[2] = DotProduct(in1, in2[2]);
	}

	inline void VectorRotate(const Vec3 &in1, const matrix3x4 &in2, Vec3 &out)
	{
		VectorRotate(&in1.x, in2, &out.x);
	}

	inline void VectorRotate(const Vec3 &in1, const Vec3 &in2, Vec3 &out)
	{
		matrix3x4 matRotate;
		AngleMatrix(in2, matRotate);
		VectorRotate(in1, matRotate, out);
	}
	inline float AngleDifference(float destAngle, float srcAngle)
	{
		float delta;

		delta = fmodf(destAngle - srcAngle, 360.0f);
		if (destAngle > srcAngle)
		{
			if (delta >= 180)
				delta -= 360;
		}
		else
		{
			if (delta <= -180)
				delta += 360;
		}
		return delta;
	}
	inline float NormalizeAngle(float ang)
	{
		if (!std::isfinite(ang))
			return 0.0f;

		return std::remainder(ang, 360.0f);
	}
	inline float AngleNormalizePositive(float angle)
	{
		angle = fmodf(angle, 360.0f);

		if (angle < 0.0f)
		{
			angle += 360.0f;
		}

		return angle;
	}
	inline void ClampAngles(Vec3 &v)
	{
		v.x = fmax(-89.0f, fmin(89.0f, NormalizeAngle(v.x)));
		v.y = NormalizeAngle(v.y);
		v.z = 0.0f;
	}

	inline Vec3 CalcAngle(Vec3 src, Vec3 dst)
	{
		Vec3 ang = Vec3(0.0f, 0.0f, 0.0f);
		Vec3 delta = (src - dst);
		float fHyp = sqrtf((delta.x * delta.x) + (delta.y * delta.y));

		ang.x = (atanf(delta.z / fHyp) * (float) M_RADPI);
		ang.y = (atanf(delta.y / delta.x) * (float) M_RADPI);
		ang.z = 0.0f;

		if (delta.x >= 0.0f)
			ang.y += 180.0f;

		return ang;
	}

	inline float LerpFloat(float v0, float v1, float t) {
		return (1 - t) * v0 + t * v1;
	}

	inline float MapFloat(float x, float in_min, float in_max, float out_min, float out_max) {
		return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
	}

	inline float ExponentialEaseOut(float p) {
		return (p == 1.0f) ? p : 1 - powf(2, -10 * p);
	}

	inline float ExponentialEaseIn(float p) {
		return (p == 0.0f) ? p : powf(2, 10 * (p - 1));
	}

	inline float QuadraticEaseInOut(float p) {
		if (p < 0.5f)
			return 2 * p * p;

		else return (-2 * p * p) + (4 * p) - 1;
	}

	inline float VectorNormalize(Vec3 &vec)
	{
		float mag = vec.Length();
		float den = 1.0f / (mag + FLT_EPSILON);
		vec.x *= den;
		vec.y *= den;
		vec.z *= den;
		return mag;
	}

	inline float LinearInterpolation(float x1, float x2, float x3, float y1, float y3) {
		float y2 = (((x2 - x1)*(y3 - y1)) / (x3 - x1)) + y1;
		return y2;
	}

	__forceinline Vec3 WorldToScreen(Vec3 world) {
		Vec3 vScreen;
		int screenw, screenh;
		CEngineClient::factory()->GetScreenSize(screenw, screenh);
		const matrix4x4& ourWorldToScreen = CEngineClient::factory()->WorldToScreenMatrix();
		float w = ourWorldToScreen[3][0] * world[0] + ourWorldToScreen[3][1] * world[1] + ourWorldToScreen[3][2] * world[2] + ourWorldToScreen[3][3];
		vScreen.z = 0;

		if (w > 0.001) {
			float fl1DBw = 1 / w;
			vScreen.x = (screenw / 2) + (0.5f * ((ourWorldToScreen[0][0] * world[0] + ourWorldToScreen[0][1] * world[1] + ourWorldToScreen[0][2] * world[2] + ourWorldToScreen[0][3]) * fl1DBw) * screenw + 0.5f);
			vScreen.y = (screenh / 2) - (0.5f * ((ourWorldToScreen[1][0] * world[0] + ourWorldToScreen[1][1] * world[1] + ourWorldToScreen[1][2] * world[2] + ourWorldToScreen[1][3]) * fl1DBw) * screenh + 0.5f);
			return vScreen;
		}

		vScreen.x = -1;
		vScreen.y = -1;
		return vScreen;
	}

	__forceinline float Remap(float value, float inMin, float inMax, float outMin, float outMax) {
		return outMin + (((value - inMin) / (inMax - inMin)) * (outMax - outMin));
	}
}

#define InvalidWorldToScreen(WTSVector)(WTSVector.x == -1 || WTSVector.y == -1)