#ifndef Z_MATH_H
#define Z_MATH_H

#include "core/object/ref_counted.h"
#include "core/math/vector3.h"

class Z_Math : public RefCounted {
	GDCLASS(Z_Math, RefCounted);
public:

protected:
	static void _bind_methods();

public:
	// Float interpolation functions
	static real_t FInterp(real_t current, real_t target, real_t delta, real_t speed = 5.0);
	static real_t FInterpConst(real_t current, real_t target, real_t delta, real_t speed);
	static real_t Lerp(real_t current, real_t target, real_t alpha);

	// Vector3 interpolation functions
	static Vector3 V3Interp(const Vector3 &current, const Vector3 &target, real_t delta, real_t speed = 5.0);
	static Vector3 V3InterpConst(const Vector3 &current, const Vector3 &target, real_t delta, real_t speed);
	static Vector3 V3Lerp(const Vector3 &current, const Vector3 &target, real_t alpha);
	static Vector3 V3slerp(const Vector3 &current, const Vector3 &target, real_t delta, real_t speed = 5.0);

	// Easing functions
	static real_t ease_in(real_t alpha, real_t exponent = 2.0);
	static real_t ease_out(real_t alpha, real_t exponent = 2.0);
	static real_t ease_in_out(real_t alpha, real_t exponent = 2.0);

};

#endif // Z_MATH_H
