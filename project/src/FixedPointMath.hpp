#ifndef JHSR_FIXEDPOINTMATH_HPP
#define JHSR_FIXEDPOINTMATH_HPP

#include <cstdint>
#include <algorithm>

inline int32_t iround(float x) {
    int t;
    asm(
        "flds   %1   \n\t"
        "fistpl %0   \n\t"
        : "=m" (t)
        : "m" (x)
        :
    );
    return t;
}

template< int Precision > constexpr float fixed_base();
template<> constexpr float fixed_base< 1 >() { return 2.0f; }
template<> constexpr float fixed_base< 2 >() { return 4.0f; }
template<> constexpr float fixed_base< 3 >() { return 8.0f; }
template<> constexpr float fixed_base< 4 >() { return 16.0f; }
template<> constexpr float fixed_base< 5 >() { return 32.0f; }
template<> constexpr float fixed_base< 6 >() { return 64.0f; }
template<> constexpr float fixed_base< 7 >() { return 128.0f; }
template<> constexpr float fixed_base< 8 >() { return 256.0f; }

template< int Precision > constexpr float fixed_shift();
template<> constexpr float fixed_shift< 1 >() { return 1 << 1; }
template<> constexpr float fixed_shift< 2 >() { return 1 << 2; }
template<> constexpr float fixed_shift< 3 >() { return 1 << 3; }
template<> constexpr float fixed_shift< 4 >() { return 1 << 4; }
template<> constexpr float fixed_shift< 5 >() { return 1 << 5; }
template<> constexpr float fixed_shift< 6 >() { return 1 << 6; }
template<> constexpr float fixed_shift< 7 >() { return 1 << 7; }
template<> constexpr float fixed_shift< 8 >() { return 1 << 8; }

template< int Precision >
inline int32_t fixed_mult(int32_t l, int32_t r) {
    return (int32_t)(int64_t(l) * int64_t(r)) / fixed_shift< Precision >();
}

template< int Precision >
inline int32_t fixed_div(int32_t l, int32_t r) {
    return (int32_t)((int64_t(l) * fixed_shift< Precision >()) / r);
}

#endif // JHSR_FIXEDPOINTMATH_HPP
