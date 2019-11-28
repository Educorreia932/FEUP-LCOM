#pragma once

#include <lcom/lcf.h>

float fsquare(float f); 
float fmin(float f1, float f2);
float fmax(float f1, float f2);
float fclamp(float f, float min, float max);

float fround(float f);
float ffloor(float f);

// Does not take 0 into account
float fsign(float f);

// thx Quake Arena 3
float Q_rsqrt( float number );
