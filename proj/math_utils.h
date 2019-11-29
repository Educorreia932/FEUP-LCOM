#pragma once

#include <lcom/lcf.h>
#include <math.h>

float fclampf(float f, float min, float max);

// Does not take 0 into account
float fsign(float f);

// thx Quake Arena 3
float Q_rsqrt( float number );
