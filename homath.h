#pragma once
#include <math.h>

#define DEGTORAD(degree) ((degree) * (3.141592654f / 180.0f))
#define RADTODEG(radian) ((radian) * (180.0f / 3.141592654f))

namespace hm {

	struct vec2 {
		float x;
		float y;

		vec2() {}
		vec2(float x, float y) : x(x), y(y) {}

		static inline float dot(vec2 l, vec2 r) {
			return l.x * r.x + l.y * r.y;
		}

		static inline float length(vec2 v) {
			return sqrtf(v.x * v.x + v.y * v.y);
		}

		static inline vec2& normalize(vec2& v) {
			float len = length(v);
			v.x /= len;
			v.y /= len;
			return v;
		}

		vec2 operator+(const vec2& r) {
			vec2 result;
			result.x = x + r.x;
			result.y = y + r.y;
			return result;
		}
		vec2 operator-(const vec2& r) {
			vec2 result;
			result.x = x - r.x;
			result.y = y - r.y;
			return result;
		}
		vec2 operator-() {
			vec2 result;
			result.x = -x;
			result.y = -y;
			return result;
		}
	};
	static vec2 operator*(float l, vec2& r) {
		vec2 result;
		result.x = l * r.x;
		result.y = l * r.y;
		return result;
	}
	static vec2 operator*(vec2& l, float r) {
		vec2 result;
		result.x = r * l.x;
		result.y = r * l.y;
		return result;
	}

	struct vec3 {
		union {
			struct {
				float x;
				float y;
				float z;
			};
			struct {
				float r;
				float g;
				float b;
			};
		};

		vec3() {}
		vec3(float x, float y, float z) : x(x), y(y), z(z) {}

		static inline float dot(vec3 l, vec3 r) {
			return l.x * r.x + l.y * r.y + l.z * r.z;
		}

		static inline float length(vec3 v) {
			return sqrtf(v.x * v.x + v.y * v.y + v.z * v.z);
		}

		static inline vec3& normalize(vec3& v) {
			float len = length(v);
			v.x /= len;
			v.y /= len;
			v.z /= len;
			return v;
		}

		vec3 operator+(const vec3& r) {
			vec3 result;
			result.x = x + r.x;
			result.y = y + r.y;
			result.z = z + r.z;
			return result;
		}
		vec3 operator-(const vec3& r) {
			vec3 result;
			result.x = x - r.x;
			result.y = y - r.y;
			result.z = z - r.z;
			return result;
		}
		vec3 operator-() {
			vec3 result;
			result.x = -x;
			result.y = -y;
			result.z = -z;
			return result;
		}

		bool operator==(const vec3& r) {
			if (x == r.x && y == r.y && z == r.z) return true;
			return false;
		}

		static inline vec3 cross(vec3 l, vec3 r) {
			vec3 result;
			result.x = l.y * r.z - l.z * r.y;
			result.y = -l.x * r.z + l.z * r.x;
			result.z = l.x * r.y - l.y * r.x;
			return result;
		}
	};

	static vec3 operator*(float l, vec3& r) {
		vec3 result;
		result.x = l * r.x;
		result.y = l * r.y;
		result.z = l * r.z;
		return result;
	}
	static vec3 operator*(vec3& l, float r) {
		vec3 result;
		result.x = r * l.x;
		result.y = r * l.y;
		result.z = r * l.z;
		return result;
	}

	static vec3 operator/(vec3& l, float r) {
		vec3 result;
		result.x = l.x / r;
		result.y = l.y / r;
		result.z = l.z / r;
		return result;
	}

	struct vec4 {
		union {
			struct {
				float x;
				float y;
				float z;
				float w;
			};
			struct {
				float r;
				float g;
				float b;
				float a;
			};
		};

		vec4() {}
		vec4(float x, float y, float z, float w) : x(x), y(y), z(z), w(w) {}

		static inline float dot(vec4 l, vec4 r) {
			return l.x * r.x + l.y * r.y + l.z * r.z + l.w * r.w;
		}

		static inline float length(vec4 v) {
			return sqrtf(v.x * v.x + v.y * v.y + v.z * v.z + v.w * v.w);
		}

		static inline vec4& normalize(vec4& v) {
			float len = length(v);
			v.x /= len;
			v.y /= len;
			v.z /= len;
			v.w /= len;
			return v;
		}

		vec4 operator+(const vec4& r) {
			vec4 result;
			result.x = x + r.x;
			result.y = y + r.y;
			result.z = z + r.z;
			result.w = w + r.w;
			return result;
		}
		vec4 operator-(const vec4& r) {
			vec4 result;
			result.x = x - r.x;
			result.y = y - r.y;
			result.z = z - r.z;
			result.w = w - r.w;
			return result;
		}
		vec4 operator-() {
			vec4 result;
			result.x = -x;
			result.y = -y;
			result.z = -z;
			result.w = -w;
			return result;
		}
	};

	static vec4 operator*(float l, vec4& r) {
		vec4 result;
		result.x = l * r.x;
		result.y = l * r.y;
		result.z = l * r.z;
		result.w = l * r.w;
		return result;
	}
	static vec4 operator*(vec4& l, float r) {
		vec4 result;
		result.x = r * l.x;
		result.y = r * l.y;
		result.z = r * l.z;
		result.w = r * l.w;
		return result;
	}

	// ----------------
	// --- Matrices ---
	// ----------------

	struct mat4 {
		union {
			float m[4][4];
			float data[16];
		};

		mat4() {
#ifdef USE_CRT
			memset(data, 0, 16 * sizeof(float));
#else
			for (int i = 0; i < 16; ++i) {
				data[i] = 0.0f;
			}
#endif
		}

		static inline void identity(mat4& mat) {
#ifdef USE_CRT
			memset(mat.data, 0, 16 * sizeof(float));
#else
			for (int i = 0; i < 16; ++i) {
				mat.data[i] = 0.0f;
			}
#endif
			mat.m[0][0] = 1.0f;
			mat.m[1][1] = 1.0f;
			mat.m[2][2] = 1.0f;
			mat.m[3][3] = 1.0f;
		}

		static void print(mat4& m) {
			//printf("%f %f %f %f\n", m.m[0][0], m.m[0][1], m.m[0][2], m.m[0][3]);
			//printf("%f %f %f %f\n", m.m[1][0], m.m[1][1], m.m[1][2], m.m[1][3]);
			//printf("%f %f %f %f\n", m.m[2][0], m.m[2][1], m.m[2][2], m.m[2][3]);
			//printf("%f %f %f %f\n", m.m[3][0], m.m[3][1], m.m[3][2], m.m[3][3]);
		}

		static inline mat4 translate(float x, float y, float z) {
			mat4 result;
			identity(result);
			result.m[0][3] = x;
			result.m[1][3] = y;
			result.m[2][3] = z;
			return result;
		}
		static inline mat4 translate(const vec3& vec) {
			mat4 result;
			identity(result);
			result.m[0][3] = vec.x;
			result.m[1][3] = vec.y;
			result.m[2][3] = vec.z;
			return result;
		}
		static inline mat4 scale(float amt) {
			mat4 result;
			result.m[0][0] = amt;
			result.m[1][1] = amt;
			result.m[2][2] = amt;
			result.m[3][3] = 1;
			return result;
		}

		static inline mat4 rotate(vec3& axis, float angle) {
			mat4 result;
			angle = DEGTORAD(angle);
			float c = cosf(angle);
			float t = 1.0f - c;
			float s = sinf(angle);
			vec3 a = vec3::normalize(axis);

			result.m[0][0] = t * a.x * a.x + c;			result.m[0][1] = t * a.x * a.y + s * a.z;	result.m[0][2] = t * a.x * a.z - s * a.y;	result.m[0][3] = 0;
			result.m[1][0] = t * a.x * a.y - s * a.z;	result.m[1][1] = t * a.y * a.y + c;			result.m[1][2] = t * a.y * a.z + s * a.x;	result.m[1][3] = 0;
			result.m[2][0] = t * a.x * a.z + s * a.y;	result.m[2][1] = t * a.y * a.z - s * a.x;	result.m[2][2] = t * a.z * a.z + c;			result.m[2][3] = 0;
			result.m[3][0] = 0;							result.m[3][1] = 0;							result.m[3][2] = 0;							result.m[3][3] = 1;

			return result;
		}

		static inline mat4 perspective(float fovy, float aspect, float zNear, float zFar) {
			mat4 result;
			identity(result);

			float range = tanf(DEGTORAD(fovy / 2.0f)) * zNear;
			float left = -range * aspect;
			float right = range * aspect;
			float bottom = -range;
			float top = range;

			result.m[0][0] = (2.0f * zNear) / (right - left);
			result.m[1][1] = (2.0f * zNear) / (top - bottom);
			result.m[2][2] = -(zFar + zNear) / (zFar - zNear);
			result.m[2][3] = -1.0f;
			result.m[3][2] = -(2.0f * zFar * zNear) / (zFar - zNear);
			result.m[3][3] = 0;

			return result;
		}

		static inline mat4 look_at(vec3 position, vec3 target, vec3 world_up);
		static inline mat4 ortho(float left, float right, float top, float bottom);
	};

	static vec4 operator*(const mat4& m, const vec4& v) {
		vec4 result;
		result.x = m.m[0][0] * v.x + m.m[0][1] * v.y + m.m[0][2] * v.z + m.m[0][3] * v.w;
		result.y = m.m[1][0] * v.x + m.m[1][1] * v.y + m.m[1][2] * v.z + m.m[1][3] * v.w;
		result.z = m.m[2][0] * v.x + m.m[2][1] * v.y + m.m[2][2] * v.z + m.m[2][3] * v.w;
		result.w = m.m[3][0] * v.x + m.m[3][1] * v.y + m.m[3][2] * v.z + m.m[3][3] * v.w;
		return result;
	}

	static vec3 operator*(const mat4& m, const vec3& v) {
		vec3 result;
		result.x = m.m[0][0] * v.x + m.m[0][1] * v.y + m.m[0][2] * v.z + m.m[0][3] * 1.0f;
		result.y = m.m[1][0] * v.x + m.m[1][1] * v.y + m.m[1][2] * v.z + m.m[1][3] * 1.0f;
		result.z = m.m[2][0] * v.x + m.m[2][1] * v.y + m.m[2][2] * v.z + m.m[2][3] * 1.0f;
		return result;
	}

	static mat4 operator*(const mat4& left, const mat4& right) {
		mat4 r;

		r.m[0][0] = left.m[0][0] * right.m[0][0] + left.m[0][1] * right.m[1][0] + left.m[0][2] * right.m[2][0] + left.m[0][3] * right.m[3][0];
		r.m[0][1] = left.m[0][0] * right.m[0][1] + left.m[0][1] * right.m[1][1] + left.m[0][2] * right.m[2][1] + left.m[0][3] * right.m[3][1];
		r.m[0][2] = left.m[0][0] * right.m[0][2] + left.m[0][1] * right.m[1][2] + left.m[0][2] * right.m[2][2] + left.m[0][3] * right.m[3][2];
		r.m[0][3] = left.m[0][0] * right.m[0][3] + left.m[0][1] * right.m[1][3] + left.m[0][2] * right.m[2][3] + left.m[0][3] * right.m[3][3];

		r.m[1][0] = left.m[1][0] * right.m[0][0] + left.m[1][1] * right.m[1][0] + left.m[1][2] * right.m[2][0] + left.m[1][3] * right.m[3][0];
		r.m[1][1] = left.m[1][0] * right.m[0][1] + left.m[1][1] * right.m[1][1] + left.m[1][2] * right.m[2][1] + left.m[1][3] * right.m[3][1];
		r.m[1][2] = left.m[1][0] * right.m[0][2] + left.m[1][1] * right.m[1][2] + left.m[1][2] * right.m[2][2] + left.m[1][3] * right.m[3][2];
		r.m[1][3] = left.m[1][0] * right.m[0][3] + left.m[1][1] * right.m[1][3] + left.m[1][2] * right.m[2][3] + left.m[1][3] * right.m[3][3];

		r.m[2][0] = left.m[2][0] * right.m[0][0] + left.m[2][1] * right.m[1][0] + left.m[2][2] * right.m[2][0] + left.m[2][3] * right.m[3][0];
		r.m[2][1] = left.m[2][0] * right.m[0][1] + left.m[2][1] * right.m[1][1] + left.m[2][2] * right.m[2][1] + left.m[2][3] * right.m[3][1];
		r.m[2][2] = left.m[2][0] * right.m[0][2] + left.m[2][1] * right.m[1][2] + left.m[2][2] * right.m[2][2] + left.m[2][3] * right.m[3][2];
		r.m[2][3] = left.m[2][0] * right.m[0][3] + left.m[2][1] * right.m[1][3] + left.m[2][2] * right.m[2][3] + left.m[2][3] * right.m[3][3];

		r.m[3][0] = left.m[3][0] * right.m[0][0] + left.m[3][1] * right.m[1][0] + left.m[3][2] * right.m[2][0] + left.m[3][3] * right.m[3][0];
		r.m[3][1] = left.m[3][0] * right.m[0][1] + left.m[3][1] * right.m[1][1] + left.m[3][2] * right.m[2][1] + left.m[3][3] * right.m[3][1];
		r.m[3][2] = left.m[3][0] * right.m[0][2] + left.m[3][1] * right.m[1][2] + left.m[3][2] * right.m[2][2] + left.m[3][3] * right.m[3][2];
		r.m[3][3] = left.m[3][0] * right.m[0][3] + left.m[3][1] * right.m[1][3] + left.m[3][2] * right.m[2][3] + left.m[3][3] * right.m[3][3];

		return r;
	}

	static void transpose(mat4& m) {
		mat4 temp = m;

		m.m[0][1] = temp.m[1][0];	m.m[0][2] = temp.m[2][0];	m.m[0][3] = temp.m[3][0];
		m.m[1][0] = temp.m[0][1];								m.m[1][2] = temp.m[2][1];	m.m[1][3] = temp.m[3][1];
		m.m[2][0] = temp.m[0][2];	m.m[2][1] = temp.m[1][2];								m.m[2][3] = temp.m[3][2];
		m.m[3][0] = temp.m[0][3];	m.m[3][1] = temp.m[1][3];	m.m[3][2] = temp.m[2][3];
	}

	static mat4 get_transpose(mat4& m) {
		mat4 temp = m;
		mat4 res = m;

		res.m[0][1] = temp.m[1][0];	res.m[0][2] = temp.m[2][0];	res.m[0][3] = temp.m[3][0];
		res.m[1][0] = temp.m[0][1];								res.m[1][2] = temp.m[2][1];	res.m[1][3] = temp.m[3][1];
		res.m[2][0] = temp.m[0][2];	res.m[2][1] = temp.m[1][2];								res.m[2][3] = temp.m[3][2];
		res.m[3][0] = temp.m[0][3];	res.m[3][1] = temp.m[1][3];	res.m[3][2] = temp.m[2][3];

		return res;
	}

	inline mat4 mat4::look_at(vec3 position, vec3 target, vec3 world_up) {
		// 1. Position = known
		// 2. Calculate cameraDirection
		vec3 diff = position - target;
		vec3 zaxis = vec3::normalize(diff);
		// 3. Get positive right axis vector
		vec3 normalized_world_up = vec3::normalize(world_up);
		vec3 cross_nwu_zax = vec3::cross(normalized_world_up, zaxis);
		vec3 xaxis = vec3::normalize(cross_nwu_zax);
		// 4. Calculate camera up vector
		vec3 yaxis = vec3::cross(zaxis, xaxis);

		// Create translation and rotation matrix
		// In glm we access elements as mat[col][row] due to column-major layout
		mat4 translation; // Identity matrix by default
		mat4::identity(translation);
		translation.m[3][0] = -position.x; // Third column, first row
		translation.m[3][1] = -position.y;
		translation.m[3][2] = -position.z;
		mat4 rotation;
		mat4::identity(rotation);
		rotation.m[0][0] = xaxis.x; // First column, first row
		rotation.m[1][0] = xaxis.y;
		rotation.m[2][0] = xaxis.z;
		rotation.m[0][1] = yaxis.x; // First column, second row
		rotation.m[1][1] = yaxis.y;
		rotation.m[2][1] = yaxis.z;
		rotation.m[0][2] = zaxis.x; // First column, third row
		rotation.m[1][2] = zaxis.y;
		rotation.m[2][2] = zaxis.z;

		// Return lookAt matrix as combination of translation and rotation matrix
		return translation * rotation; // Remember to read from right to left (first translation then rotation)
	}

	inline mat4 mat4::ortho(float left, float right, float bottom, float top)
	{
		float f = 1.0f;
		float n = -1.0f;
		mat4 result;
		result.m[0][0] = 2.0f / (right - left);	result.m[0][1] = 0;						result.m[0][2] = 0;					result.m[0][3] = -(right + left) / (right - left);
		result.m[1][0] = 0;						result.m[1][1] = 2.0f / (top - bottom);	result.m[1][2] = 0;					result.m[1][3] = -(top + bottom) / (top - bottom);
		result.m[2][0] = 0;						result.m[2][1] = 0;						result.m[2][2] = 1;					result.m[2][3] = 0;
		result.m[3][0] = 0;						result.m[3][1] = 0;						result.m[3][2] = 0;					result.m[3][3] = 1;

		return result;
	}

	struct quat
	{
		float x, y, z, w;

		quat() : x(0), y(0), z(0), w(0) { }

		quat(float x, float y, float z, float w) : x(x), y(y), z(z), w(w) { }

		quat(float angle) {
			x = sinf(angle) / 2.0f;
			y = sinf(angle) / 2.0f;
			z = sinf(angle) / 2.0f;
			w = cosf(angle) / 2.0f;
		}

		inline quat operator*(const quat r)
		{
			quat res;
			res.w = this->w*r.w - this->x*r.x - this->y*r.y - this->z*r.z;
			res.x = this->w*r.x + this->x*r.w + this->y*r.z - this->z*r.y;
			res.y = this->w*r.y + this->y*r.w + this->z*r.x - this->x*r.z;
			res.z = this->w*r.z + this->z*r.w + this->x*r.y - this->y*r.x;

			return res;
		}

		static inline float length(quat& q) {
			return sqrtf(q.x * q.x + q.y * q.y + q.z * q.z + q.w * q.w);
		}

		static inline void normalize(quat& q) {
			float len = length(q);
			q.x /= len;
			q.y /= len;
			q.z /= len;
			q.w /= len;
		}

		inline quat normalize() {
			quat q;
			float length = sqrtf(q.x * q.x + q.y * q.y + q.z * q.z + q.w * q.w);

			q.x /= length;
			q.y /= length;
			q.z /= length;
			q.w /= length;

			return(q);
		}

		static inline quat conjugate(quat& q) {
			quat result;
			result.x = -q.x;
			result.y = -q.y;
			result.z = -q.z;
			return (result);
		}

	};

	static inline mat4 quat_rotate(const quat& quat)
	{
		mat4 result;

		result.m[0][0] = 1.0f - 2.0f * quat.y * quat.y - 2.0f * quat.z * quat.z;
		result.m[1][0] = 2.0f * quat.x * quat.y + 2.0f * quat.w * quat.z;
		result.m[2][0] = 2.0f * quat.x * quat.z - 2.0f * quat.w * quat.y;
		result.m[3][0] = 0;

		result.m[0][1] = 2.0f * quat.x * quat.y - 2.0f * quat.w * quat.z;
		result.m[1][1] = 1.0f - (2.0f * quat.x * quat.x) - (2.0f * quat.z * quat.z);
		result.m[2][1] = 2.0f * quat.y * quat.z + 2.0f * quat.w * quat.x;
		result.m[3][1] = 0;

		result.m[0][2] = 2.0f * quat.x * quat.z + 2.0f * quat.w * quat.y;
		result.m[1][2] = 2.0f * quat.y * quat.z - 2.0f * quat.w * quat.x;
		result.m[2][2] = 1.0f - (2.0f * quat.x * quat.x) - (2.0f * quat.y * quat.y);
		result.m[3][2] = 0;

		result.m[0][3] = 0;
		result.m[1][3] = 0;
		result.m[2][3] = 0;
		result.m[3][3] = 1;

		return(result);
	}

	static inline quat quat_from_axis_angle(vec3 axis, float angle)
	{
		vec3::normalize(axis);
		float sang = sinf(DEGTORAD(angle) / 2.0f);

		quat q;
		q.w = cosf(DEGTORAD(angle / 2.0f));
		q.x = axis.x * sang;
		q.y = axis.y * sang;
		q.z = axis.z * sang;

		return q;
	}

} // namespace hm
