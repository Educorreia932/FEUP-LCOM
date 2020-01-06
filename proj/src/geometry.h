#pragma once

#include <lcom/lcf.h>

/** @file */

/** @defgroup geometry Geometry */

/** @addtogroup geometry
  *
  * @brief	The building blocks for our 2D vectors and physiscs
  *
  * @{
  */

/** @brief The delta time between each frame */
#define DELTATIME 0.01666667f // Time between frames

/**
 * @brief Structure to represent two dimensional vectors
 * @note  A point is also represented as a vector, because a point can be seen as the vector from the origin of the referencial to the point
 * 
 */
typedef struct Vec2d{
    /** @brief x The x component */
    float x;
    /** @brief y The y component */
    float y;
} Vec2d_t;

/**
 * @relates Vec2d
 * @brief Constructor for a vec2d
 * 
 * @param x The x component
 * @param y The y component
 * @return Vec2d_t The constructed vec2d
 */
Vec2d_t vec2d(float x, float y);

/* VEC2D_T METHODS */

/**
 * @relates Vec2d
 * @brief Returns the norm of a given vector
 * 
 * @param v The vector to calculate the norm
 * @return float The norm of the vector v
 */
float norm_vec2d(Vec2d_t v);

/**
 * @relates Vec2d
 * @brief Returns the inverse of the norm of a given vector
 * 
 * @param v The vector to calculate the inverse of the norm
 * @return float The inverse of the norm of the vector v
 */
float inverse_norm_vec2d(Vec2d_t v);

/**
 * @relates Vec2d
 * @brief The angle between two vectors measured in radians
 * @note For a better understanding of this function's behaviour, think about the internal product
 * 
 * @param v1 Vector 1
 * @param v2 Vector 2
 * @return float The smallest angle between the two vectors, measured in radians
 */
float angle_vec2d(Vec2d_t v1, Vec2d_t v2);

/**
 * @relates Vec2d
 * @brief The inverse of the distance between two points in a plane
 * 
 * @param v1 The first point
 * @param v2 The second point
 * @return float The inverse of the distance between two points in a plane
 */
float inverse_distance_vec2d(Vec2d_t v1, Vec2d_t v2);

/**
 * @relates Vec2d
 * @brief The distance between two points in a plane
 * 
 * @param v1 The first point
 * @param v2 The second point
 * @return float The distance between two points in a plane
 */
float distance_vec2d(Vec2d_t v1, Vec2d_t v2);

/**
 * @relates Vec2d
 * @brief Normalizes a given vector
 * 
 * @param v The vector to normalize
 * @return Vec2d_t The normalized vector of v
 */
Vec2d_t normalize_vec2d(Vec2d_t v);

/**
 * @relates Vec2d
 * @brief The internal product between two vectors
 * 
 * @param v1 The first vector
 * @param v2 The second vector
 * @return float The internal product between two vectors
 */
float internal_prod_vec2d(Vec2d_t v1, Vec2d_t v2);

/**
 * @relates Vec2d
 * @brief Returns a vector multiplied by a scalar
 * 
 * @param v The vector to be multiplied
 * @param k The scalar to multiply over
 * @return Vec2d_t The vector resulting from multiplying v by k
 */
Vec2d_t multiply_by_scalar_vec2d(Vec2d_t v, float k);

/**
 * @relates Vec2d
 * @brief Returns the sum of two vectors
 * 
 * @param v1 The first vector
 * @param v2 The second vector
 * @return Vec2d_t The vector resulting from v1 + v2
 */
Vec2d_t sum_vec2d(Vec2d_t v1, Vec2d_t v2);

/**
 * @relates Vec2d
 * @brief Returns the difference between two vectors
 * 
 * @param v1 The first vector
 * @param v2 The second vector
 * @return Vec2d_t The vector resulting from v1 - v2
 */
Vec2d_t subtract_vec2d(Vec2d_t v1, Vec2d_t v2);

/**
 * @relates Vec2d
 * @brief Converts a point in polar coordinates (in relation to any center) to cartesian coordinates
 * 
 * @param center The center of the circumference
 * @param radius The radius of the circumference
 * @param angle The angle of the point
 * @return Vec2d_t The vector with the point's coordinates in the cartesian system
 */
Vec2d_t circumference_vec2d(Vec2d_t center, float radius, float angle);

/**
 * @brief Represents a rectangle in a plane
 * 
 * @details The rectangle is represented by its top leftmost point (smallest x and y coordinates) and its size (width and height)
 * 
 * @note This is used for various purpuoses, such as:\n
 *          Colliders, sprite sizes, UI element's "hitbox", etc...
 */
typedef struct Rect {
    float x; /**< @brief The x coordinate of the top leftmost point*/
    float y; /**< @brief The y coordinate of the top leftmost point*/
    float w; /**< @brief The width of the rectangle*/
    float h; /**< @brief The height of the rectangle*/
} Rect_t;

/* CONSTRUCTORS */

/**
 * @relates Rect
 * @brief The default constructor
 * 
 * @param x The x coordinate of the top leftmost point
 * @param y The y coordinate of the top leftmost point
 * @param w The width of the rectangle
 * @param h The height of the rectangle
 * @return Rect_t The fully built rectangle
 */
Rect_t rect(float x, float y, float w, float h);

/**
 * @relates Rect
 * @brief Same as the default constructor, only that is receives uint16_t instead of floats as arguments
 * 
 * @param x The x coordinate of the top leftmost point
 * @param y The y coordinate of the top leftmost point
 * @param w The width of the rectangle
 * @param h The height of the rectangle
 * @return Rect_t The fully built rectangle
 */
Rect_t rect_from_uints(uint16_t x,uint16_t y, uint16_t w, uint16_t h);

/**
 * @relates Rect  
 * @relates Vec2d
 * @brief Constructs a rectangle from two vectors
 * 
 * @param point The top leftmost point
 * @param size A vector with the (width, size) of the rectangle
 * @return Rect_t The fully built rectangle
 */
Rect_t rect_from_vec2d(Vec2d_t point, Vec2d_t size) ;

/**
 * @relates Rect
 * @brief Constructs a rectangle from two points
 * 
 * @param p1 First point
 * @param p2 Second point
 * @return Rect_t The rectangle described by both points
 */
Rect_t rect_from_points(Vec2d_t p1, Vec2d_t p2);

/**
 * @relates Rect
 * @brief Creates a pointer to a rectangle
 * 
 * @param x The x coordinate of the top leftmost point
 * @param y The y coordinate of the top leftmost point
 * @param w The width of the rectangle
 * @param h The height of the rectangle
 * @return Rect_t A pointer to the fully built rectangle
 */
Rect_t* new_rect(float x, float y, float w, float h);

/**
 * @relates Rect  
 * @brief Frees the rect from memory
 * 
 * @param rect Pointer to the rect to free from memory
 */
void free_rect(Rect_t* rect);

/* RECT METHODS */

/**
 * @relates Rect
 * @brief Retuns true if the two rectangles overlap (aka: are colliding)
 * 
 * @param r1 Pointer to he first rectangle
 * @param r2 Pointer to the second rectangle
 * @return true Returned when the two rectangles overlap
 * @return false Returned otherwise
 */
bool rect_collision(Rect_t *r1, Rect_t *r2);

/**
 * @relates Rect
 * @brief Returns wether a given point is inside a rect or not
 * 
 * @param r Pointer to the rectangle
 * @param x The x coordinate of the point
 * @param y The y coordinate of the point
 * @return true Returned when the point (x, y) is inside the rectangle r
 * @return false Returned otherwise
 */
bool are_coords_inside_rect(Rect_t *r, float x, float y);

/**
 * @relates Rect
 * @relates Vec2d
 * @brief Returns wether a given point is inside a rect or not
 * 
 * @param r Pointer to the rectangle
 * @param v Vector describing the point
 * @return true Returned when the point described by the vector is inside the rectangle r
 * @return false Returned otherwise
 */
bool is_point_inside_rect(Rect_t *r, Vec2d_t v);

/**
 * @relates Rect
 * @brief Returns the top leftmost point of the rectangle
 * 
 * @param r Pointer to the rectangle
 * @return Vec2d_t The vector representing the top leftmost point of the rectangle
 */
Vec2d_t rect_get_origin(Rect_t* r);

/** @} */
