#pragma once
#include <math.h>

constexpr float PI = 3.14159265f;
constexpr double PI_D = 3.1415926535897932;

template <typename T>
constexpr auto sq( const T& x )
{
	return x * x;
}

template<typename T>
constexpr T sign( T val )
{
	return static_cast<T>( ( val > (T)0.0 ) - ( val < (T)0.0 ) );
}

template<typename T>
T wrap_angle( T theta )
{
	const T modded = std::fmodf( theta,(T)2.0 * (T)PI_D );
	return (std::abs( modded ) > (T)PI_D) ?
		(modded - (T)2.0 * (T)PI_D * sign( modded )) :
		modded;
}

template<typename T>
constexpr T interpolate( const T& src,const T& dst,float alpha )
{
	return src + (dst - src) * alpha;
}

template<typename T>
constexpr T to_rad( T deg )
{
	return deg * PI / (T)180.0;
}