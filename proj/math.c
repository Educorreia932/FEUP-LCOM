#include "math.h"

inline float fsquare(float f) {
  return f*f;
}

inline float fmin(float f1, float f2) {
  return f1 < f2 ? f1 : f2; 
}

inline float fmax(float f1, float f2) {
  return f1 > f2 ? f1 : f2;
}

inline float fclamp(float f, float min, float max) {
  return fmax(min, fmin(f, max));
}

inline float fsign(float f) {
  return f >= 0 ? 1 : -1;
}


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
