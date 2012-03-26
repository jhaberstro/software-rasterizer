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

template< int Precision > constexpr float base();
template<> constexpr float base< 1 >() { return 2.0f; }
template<> constexpr float base< 2 >() { return 4.0f; }
template<> constexpr float base< 3 >() { return 8.0f; }
template<> constexpr float base< 4 >() { return 16.0f; }
template<> constexpr float base< 5 >() { return 32.0f; }
template<> constexpr float base< 6 >() { return 64.0f; }
template<> constexpr float base< 7 >() { return 128.0f; }
template<> constexpr float base< 8 >() { return 256.0f; }


template< int Precision >
class FixedPointReal
{
public:

    explicit FixedPointReal(float v) : _rep(iround(v * base< Precision >())) {}
    explicit FixedPointReal(int32_t i) : _rep(i) {}
    FixedPointReal(FixedPointReal const& other) : _rep(other._rep) {}
    FixedPointReal(FixedPointReal&& other) : _rep(std::move(other._rep)) {}

                            operator float() const                  { return _rep / base< Precision >(); }
    FixedPointReal          operator-() const                       { return FixedPointReal(-_rep); }
    FixedPointReal&         operator+=(FixedPointReal const& other) { _rep += other._rep; return *this; }
    FixedPointReal&         operator-=(FixedPointReal const& other) { _rep -= other._rep; return *this; }
    FixedPointReal&         operator*=(FixedPointReal const& other) { _rep = (int64_t(_rep) * int64_t(other._rep)) / (1 << Precision); return *this; }
    FixedPointReal&         operator/=(FixedPointReal const& other) { _rep = (int64_t(_rep) * (1 << Precision)) / other._rep; return *this; }

    friend FixedPointReal operator+ (FixedPointReal< Precision > const& lhs, FixedPointReal< Precision > const& rhs) {return FixedPointReal< Precision >(lhs._rep + rhs._rep); }
    friend FixedPointReal operator- (FixedPointReal< Precision > const& lhs, FixedPointReal< Precision > const& rhs) {return FixedPointReal< Precision >(lhs._rep - rhs._rep); }
    friend FixedPointReal operator* (FixedPointReal< Precision > const& lhs, FixedPointReal< Precision > const& rhs) {return FixedPointReal< Precision >((int32_t)(int64_t(lhs._rep) * int64_t(rhs._rep)) / (1 << Precision)); }
    friend FixedPointReal operator/ (FixedPointReal< Precision > const& lhs, FixedPointReal< Precision > const& rhs) {return FixedPointReal< Precision >((int32_t)(int64_t(lhs._rep) * (1 << Precision)) / rhs._rep); }
    friend bool           operator< (FixedPointReal< Precision > const& lhs, FixedPointReal< Precision > const& rhs) {return lhs._rep < rhs._rep; }
    friend bool           operator<=(FixedPointReal< Precision > const& lhs, FixedPointReal< Precision > const& rhs) {return lhs._rep <= rhs._rep; }
    friend bool           operator> (FixedPointReal< Precision > const& lhs, FixedPointReal< Precision > const& rhs) {return lhs._rep > rhs._rep; }
    friend bool           operator>=(FixedPointReal< Precision > const& lhs, FixedPointReal< Precision > const& rhs) {return lhs._rep >= rhs._rep; }
    friend bool           operator==(FixedPointReal< Precision > const& lhs, FixedPointReal< Precision > const& rhs) {return lhs._rep == rhs._rep; }
    friend bool           operator!=(FixedPointReal< Precision > const& lhs, FixedPointReal< Precision > const& rhs) {return lhs._rep != rhs._rep; }

private:

    int32_t _rep;
};

template< int Precision >
class FixedPointVector3
{
public:

    FixedPointVector3(float x, float y, float z) : x(x), y(y), z(z) {}
    FixedPointVector3(FixedPointReal< Precision > x, FixedPointReal< Precision > y, FixedPointReal< Precision > z) : x(x), y(y), z(z) {}
    FixedPointVector3(FixedPointVector3 const& other) : x(other.x), y(other.y), z(other.z) {}
    FixedPointVector3(FixedPointVector3&& other) : x(std::move(other.x)), y(std::move(other.y)), z(std::move(other.z)) {}

    FixedPointReal< Precision >& operator[](size_t index)               { return (&x)[index]; }
    FixedPointReal< Precision > const& operator[](size_t index) const   { return (&x)[index]; }
    FixedPointVector3  operator-() const                                { return FixedPointVector3(-x, -y, -z); }
    FixedPointVector3& operator+=(FixedPointVector3 const& other)       { x += other.x; y += other.y; z += other.z; return *this; }
    FixedPointVector3& operator-=(FixedPointVector3 const& other)       { x -= other.x; y -= other.y; z -= other.z; return *this; }
    FixedPointVector3& operator*=(FixedPointVector3 const& other)       { x *= other.x; y *= other.y; z *= other.z; return *this; }
    FixedPointVector3& operator/=(FixedPointVector3 const& other)       { x /= other.x; y /= other.y; z /= other.z; return *this; }

    friend FixedPointVector3 operator+(FixedPointVector3 const& lhs, FixedPointVector3 const& rhs);
    friend FixedPointVector3 operator-(FixedPointVector3 const& lhs, FixedPointVector3 const& rhs);
    friend FixedPointVector3 operator*(FixedPointVector3 const& lhs, FixedPointVector3 const& rhs);
    friend FixedPointVector3 operator/(FixedPointVector3 const& lhs, FixedPointVector3 const& rhs);


    FixedPointReal< Precision > x, y, z;
};

template< int P >
inline FixedPointVector3< P > operator+(FixedPointVector3< P > const& lhs, FixedPointVector3< P > const& rhs) {
    return FixedPointVector3< P >(lhs.x + rhs.x, lhs.y + rhs.y, lhs.z + rhs.z); 
}

template< int P >
inline FixedPointVector3< P > operator-(FixedPointVector3< P > const& lhs, FixedPointVector3< P > const& rhs) {
    return FixedPointVector3< P >(lhs.x - rhs.x, lhs.y - rhs.y, lhs.z - rhs.z); 
}

template< int P >
inline FixedPointVector3< P > operator*(FixedPointVector3< P > const& lhs, FixedPointVector3< P > const& rhs) {
    return FixedPointVector3< P >(lhs.x * rhs.x, lhs.y * rhs.y, lhs.z * rhs.z); 
}

template< int P >
inline FixedPointVector3< P > operator/(FixedPointVector3< P > const& lhs, FixedPointVector3< P > const& rhs) {
    return FixedPointVector3< P >(lhs.x / rhs.x, lhs.y / rhs.y, lhs.z / rhs.z); 
}


#endif // JHSR_FIXEDPOINTMATH_HPP
