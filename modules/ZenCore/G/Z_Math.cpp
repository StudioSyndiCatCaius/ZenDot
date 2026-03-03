#include "Z_Math.h"
#include "core/math/math_funcs.h"
#include <cmath>


void Z_Math::_bind_methods() {
    // Float interpolation
    ClassDB::bind_static_method("Z_Math", D_METHOD("FInterp", "current", "target", "delta", "speed"), &Z_Math::FInterp, DEFVAL(5.0));
    ClassDB::bind_static_method("Z_Math", D_METHOD("FInterpConst", "current", "target", "delta", "speed"), &Z_Math::FInterpConst);
    ClassDB::bind_static_method("Z_Math", D_METHOD("Lerp", "current", "target", "alpha"), &Z_Math::Lerp);

    // Vector3 interpolation
    ClassDB::bind_static_method("Z_Math", D_METHOD("V3Interp", "current", "target", "delta", "speed"), &Z_Math::V3Interp, DEFVAL(5.0));
    ClassDB::bind_static_method("Z_Math", D_METHOD("V3InterpConst", "current", "target", "delta", "speed"), &Z_Math::V3InterpConst);
    ClassDB::bind_static_method("Z_Math", D_METHOD("V3Lerp", "current", "target", "alpha"), &Z_Math::V3Lerp);
    ClassDB::bind_static_method("Z_Math", D_METHOD("V3slerp", "current", "target", "delta", "speed"), &Z_Math::V3slerp, DEFVAL(5.0));

    // Easing functions
    ClassDB::bind_static_method("Z_Math", D_METHOD("ease_in", "alpha", "exponent"), &Z_Math::ease_in, DEFVAL(2.0));
    ClassDB::bind_static_method("Z_Math", D_METHOD("ease_out", "alpha", "exponent"), &Z_Math::ease_out, DEFVAL(2.0));
    ClassDB::bind_static_method("Z_Math", D_METHOD("ease_in_out", "alpha", "exponent"), &Z_Math::ease_in_out, DEFVAL(2.0));
}

// Float interpolation with exponential decay (like UE5's FInterpTo)
real_t Z_Math::FInterp(real_t current, real_t target, real_t delta, real_t speed) {
    if (speed <= 0.0) {
        return target;
    }

    const real_t distance = target - current;
    if (Math::abs(distance) < CMP_EPSILON) {
        return target;
    }

    const real_t alpha = MIN(1.0, speed * delta);
    return current + distance * alpha;
}

// Float interpolation at constant speed
real_t Z_Math::FInterpConst(real_t current, real_t target, real_t delta, real_t speed) {
    const real_t distance = target - current;
    const real_t max_step = speed * delta;

    if (Math::abs(distance) <= max_step) {
        return target;
    }

    return current + Math::sin(distance) * max_step;
}

// Basic linear interpolation
real_t Z_Math::Lerp(real_t current, real_t target, real_t alpha) {
    return current + alpha * (target - current);
}

// Vector3 interpolation with exponential decay
Vector3 Z_Math::V3Interp(const Vector3 &current, const Vector3 &target, real_t delta, real_t speed) {
    return Vector3(
        FInterp(current.x, target.x, delta, speed),
        FInterp(current.y, target.y, delta, speed),
        FInterp(current.z, target.z, delta, speed)
    );
}

// Vector3 interpolation at constant speed
Vector3 Z_Math::V3InterpConst(const Vector3 &current, const Vector3 &target, real_t delta, real_t speed) {
    const Vector3 direction = target - current;
    const real_t distance = direction.length();
    const real_t max_step = speed * delta;

    if (distance <= max_step || distance < CMP_EPSILON) {
        return target;
    }

    return current + direction.normalized() * max_step;
}

// Vector3 linear interpolation
Vector3 Z_Math::V3Lerp(const Vector3 &current, const Vector3 &target, real_t alpha) {
    return current.lerp(target, alpha);
}

// Vector3 spherical interpolation (great for smooth rotations)
Vector3 Z_Math::V3slerp(const Vector3 &current, const Vector3 &target, real_t delta, real_t speed) {
    const real_t alpha = MIN(1.0, speed * delta);
    return current.slerp(target, alpha);
}

// Ease in (slow start, fast end)
real_t Z_Math::ease_in(real_t alpha, real_t exponent) {
    return Math::pow(alpha, exponent);
}

// Ease out (fast start, slow end)
real_t Z_Math::ease_out(real_t alpha, real_t exponent) {
	return 1.0 - (real_t)pow((1.0 - alpha), (double)exponent);
}

// Ease in/out (slow start and end, fast middle)
real_t Z_Math::ease_in_out(real_t alpha, real_t exponent) {
    if (alpha < 0.5) {
        return ease_in(alpha * 2.0, exponent) * 0.5;
    } else {
        return ease_out((alpha - 0.5) * 2.0, exponent) * 0.5 + 0.5;
    }
}
