#include "geometry.h"
#include "math_utils.h"

// Vec2d constructors
Vec2d_t vec2d(float x, float y) {
	const Vec2d_t v = {x, y};
	return v;
}

// Vec2d operations
inline float norm_vec2d(Vec2d_t v) {
	return sqrtf(powf(v.x, 2) + powf(v.y, 2));
}

inline float inverse_norm_vec2d(Vec2d_t v) {
	return Q_rsqrt(powf(v.x, 2) + powf(v.y, 2));
}

inline float angle_vec2d(Vec2d_t v1, Vec2d_t v2) {
	return acosf(
		internal_prod_vec2d(v1, v2)
		/ (norm_vec2d(v1) * norm_vec2d(v2))
		);
}

inline Vec2d_t normalize_vec2d(Vec2d_t v) {
	return multiply_by_scalar_vec2d(v, inverse_norm_vec2d(v));
}

inline float inverse_distance_vec2d(Vec2d_t v1, Vec2d_t v2) {
	return Q_rsqrt( powf(v1.x - v2.x, 2) + powf(v1.y - v2.y, 2) );
}

inline float distance_vec2d(Vec2d_t v1, Vec2d_t v2) {
	return sqrtf( powf(v1.x - v2.x, 2) + powf(v1.y - v2.y, 2) );
}


float internal_prod_vec2d(Vec2d_t v1, Vec2d_t v2) {
	return v1.x * v2.x + v1.y * v2.y;
}

inline Vec2d_t multiply_by_scalar_vec2d(Vec2d_t v, float k) {
	return vec2d(v.x * k, v.y * k);
}

inline Vec2d_t sum_vec2d(Vec2d_t v1, Vec2d_t v2) {
	return vec2d(v1.x + v2.x, v1.y + v2.y);
}

inline Vec2d_t subtract_vec2d(Vec2d_t v1, Vec2d_t v2) {
	return vec2d(v1.x - v2.x, v1.y - v2.y);
}

Vec2d_t circumference_vec2d(Vec2d_t center, float radius, float angle) {
	return vec2d(
		radius * cosf(angle) + center.x,
		radius * sinf(angle) + center.y	
	);
}

/* RECT CONSTRUCTORS */

Rect_t rect(float x, float y, float w, float h) {
	const Rect_t r = {x, y, w, h};
	return r;
}

Rect_t rect_from_uints(uint16_t x,uint16_t y, uint16_t w, uint16_t h) {
	const Rect_t r = {(float) x, (float) y, (float) w, (float) h};
	return r;
}

Rect_t rect_from_vec2d(Vec2d_t point, Vec2d_t size) {
	const Rect_t r = {point.x, point.y, size.x, size.y};
	return r;
}

Rect_t rect_from_points(Vec2d_t p1, Vec2d_t p2) {
	float xmin = fminf(p1.x, p2.x);
	float xmax = fmaxf(p1.x, p2.x);
	float ymin = fminf(p1.y, p2.y);
	float ymax = fmaxf(p1.y, p2.y);

	const Rect_t r = {
			xmin, 
			ymin, 
			fmaxf(0.0f, xmax - xmin),
			fmaxf(0.0f, ymax - ymin)
		};

	return r;
}

Rect_t* new_rect(float x, float y, float w, float h) {
	Rect_t* rect = (Rect_t*) malloc(sizeof(Rect_t));
	if (rect == NULL) {
		printf("new_rect: Failed to allocate memory for the Rect object\n");
		return NULL;
	}

	rect->x = x;
	rect->y = y;
	rect->w = w;
	rect->h = h;

	return rect;
}

void free_rect(Rect_t* rect) {
	if (rect == NULL) {
				printf("free_rect: Cannot free a NULL pointer\n");
				return;
		}
	free(rect);
}

/* RECT METHODS */

inline bool rect_collision(Rect_t *r1, Rect_t *r2) {
	return r1->x < r2->x + r2->w
		&& r1->x + r1->w > r2->x
		&& r1->y < r2->y + r2->h
		&& r1->y + r1->h > r2->y;
}

inline bool are_coords_inside_rect(Rect_t *r, float x, float y) {
	return r->x <= x
		&& r->x + r->w >= x
		&& r->y <= y
		&& r->y + r->h >= y;
}

inline bool is_point_inside_rect(Rect_t *r, Vec2d_t v) {
	return r->x <= v.x
		&& r->x + r->w >= v.x
		&& r->y <= v.y
		&& r->y + r->h >= v.y;
}

inline Vec2d_t rect_get_origin(Rect_t* r) {
	return vec2d(r->x, r->y);
}
