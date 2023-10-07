#ifndef _CARBON_UTIL_EASING_HPP_
#define _CARBON_UTIL_EASING_HPP_

#include <cmath>
#include <corecrt_math_defines.h>

// https://easinfgs.net/

namespace carbon::easing {
	inline float in_sine(const float t) {
        return std::sinf(1.5707963f * t);
    }

    inline float out_sine(const float t) {
        return 1.0f + std::sinf(1.5707963f * (t - 1.0f));
    }

    inline float ease_in_out_sine(const float t) {
        return 0.5f * (1.0f + std::sinf(3.1415926f * (t - 0.5f)));
    }

    inline float in_quad(const float t) {
        return t * t;
    }

    inline float out_quad(const float t) {
        return t * (2 - t);
    }

    inline float in_out_quad(const float t) {
        return t < 0.5f ? 2.0f * t * t : t * (4.0f - 2.0f * t) - 1.0f;
    }

    inline float in_cubic(const float t) {
        return t * t * t;
    }

    inline float out_cubic(const float t) {
        return 1 - std::pow(1.0f - t, 3.0f);
    }

    inline float in_out_cubic(float t) {
        return t < 0.5f ? 4.0f * t * t * t : 1.0f + --t * (2.0f * --t) * (2.0f * t);
    }

    inline float in_quart(float t) {
        t *= t;
        return t * t;
    }

    inline float out_quart(float t) {
        t = --t * t;
        return 1.0f - t * t;
    }

    inline float in_out_quart(float t) {
        if (t < 0.5f) {
            t *= t;
            return 8.0f * t * t;
        }
        t = --t * t;
        return 1.0f - 8.0f * t * t;
    }

    inline float in_quint(const float t) {
	    const auto t2 = t * t;
        return t * t2 * t2;
    }

    inline float out_quint(float t) {
        const auto t2 = --t * t;
        return 1 + t * t2 * t2;
    }

    inline float in_out_quint(float t) {
        if (t < 0.5f) {
            const auto t2 = t * t;
            return 16.0f * t * t2 * t2;
        }
        const auto t2 = --t * t;
        return 1.0f + 16.0f * t * t2 * t2;
    }

    inline float in_expo(const float t) {
        return (std::pow(2.0f, 8.0f * t) - 1.0f) / 255.0f;
    }

    inline float out_expo(const float t) {
        return 1.0f - std::pow(2.0f, -8.0f * t);
    }

    inline float in_out_expo(const float t) {
        if (t < 0.5f)
            return (std::pow(2.0f, 16.0f * t) - 1) / 510.0f;
        return 1.0f - 0.5f * std::pow(2.0f, -16.0f * (t - 0.5f));
    }

    inline float in_circ(const float t) {
        return 1.0f - std::sqrt(1.0f - t);
    }

    inline float out_circ(const float t) {
        return std::sqrt(t);
    }

    inline float in_out_circ(const float t) {
        if (t < 0.5f)
            return (1.0f - std::sqrt(1.0f - 2.0f * t)) * 0.5f;
        return (1.0f + std::sqrt(2.0f * t - 1.0f)) * 0.5f;
    }

    inline float in_back(const float t) {
        return t * t * (2.70158f * t - 1.70158f);
    }

    inline float out_back(float t) {
        return 1 + --t * t * (2.70158f * t + 1.70158f);
    }

    inline float in_out_back(float t) {
        if (t < 0.5f)
            return t * t * (7.0f * t - 2.5f) * 2.0f;
        return 1.0f + --t * t * 2.0f * (7.0f * t + 2.5f);
    }

    inline float in_elastic(const float t) {
        const auto t2 = t * t;
        return t2 * t2 * std::sinf(t *  M_PI * 4.5f);
    }

    inline float out_elastic(const float t) {
        const auto t2 = (t - 1.0f) * (t - 1.0f);
        return 1.0f - t2 * t2 * std::cosf(t * M_PI * 4.5f);
    }

    inline float in_out_elastic(const float t) {
        if (t < 0.45f) {
	        const auto t2 = t * t;
            return 8.0f * t2 * t2 * std::sinf(t * M_PI * 9.0f);
        }
        if (t < 0.55f)
	        return 0.5f + 0.75f * std::sinf(t * M_PI * 4.0f);
        const auto t2 = (t - 1) * (t - 1);
        return 1.0f - 8.0f * t2 * t2 * sinf(t * M_PI * 9.0f);
    }

    inline float in_bounce(const float t) {
        return std::pow(2.0f, 6.0f * (t - 1.0f)) * std::abs(std::sinf(t * M_PI * 3.5f));
    }

    inline float out_bounce(const float t) {
        return 1.0f - std::pow(2.0f, -6.0f * t) * std::abs(std::cosf(t * M_PI * 3.5f));
    }

    inline float in_out_bounce(const float t) {
        if (t < 0.5f)
            return 8.0f * std::pow(2.0f, 8.0f * (t - 1.0f)) * std::abs(std::sinf(t * M_PI * 7.0f));
        return 1.0f - 8.0f * std::pow(2.0f, -8.0f * t) * std::abs(std::sinf(t * M_PI * 7.0f));
    }

}

#endif