#include "math_utils.h"

inline float fsign(float f) {
  	return f >= 0 ? 1 : -1;
}

inline float fclampf(float f, float min, float max) {
	return fmaxf(min, fminf(f, max));
}

inline uint8_t min_uint8(uint8_t a, uint8_t b) {
	return a < b ? a : b;
}

inline uint8_t max_uint8(uint8_t a, uint8_t b) {
	return a > b ? a : b;
}

inline uint8_t clamp_uint8(uint8_t n, uint8_t min, uint8_t max) {
	return max_uint8(min, min_uint8(n, max));
}

inline uint16_t min_uint16(uint16_t a, uint16_t b) {
	return a < b ? a : b;
}

inline uint16_t max_uint16(uint16_t a, uint16_t b) {
	return a > b ? a : b;
}

inline uint16_t clamp_uint16(uint16_t n, uint16_t min, uint16_t max) {
	return max_uint16(min, min_uint16(n, max));
}

uint8_t get_digit(uint16_t num, uint8_t n) {
    uint16_t r, t1, t2;
 
    t1 = pow(10, n+1);
    r = num % t1;
 
    if (n > 0) {
        t2 = pow(10, n);
        r = r / t2;
    }

    return r;
}

// Code below here was not created by us

// Thanks quake arena 3 :D
float Q_rsqrt( float number )
{
	long i;
	float x2, y;
	const float threehalfs = 1.5F;

	x2 = number * 0.5F;
	y  = number;
	i  = * ( long * ) &y;    // evil floating point bit level hacking
	i  = 0x5f3759df - ( i >> 1 );               // what the fuck? 
	// Somehow this makes a pretty accurate aprox for the newton method
	y  = * ( float * ) &i;
	// This is a newton method iteration
	y  = y * ( threehalfs - ( x2 * y * y ) );   // 1st iteration
//  y  = y * ( threehalfs - ( x2 * y * y ) );   // 2nd iteration,
											  // this can be removed

	return y;
}
