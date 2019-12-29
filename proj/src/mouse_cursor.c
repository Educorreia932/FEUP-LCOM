#include "mouse_cursor.h"
#include "input_events.h"
#include "sprite.h"
#include "math_utils.h"

typedef struct MouseCursor {
    Vec2d_t pos;
    Sprite_t* sprite;
    bool rendered;
} MouseCursor_t;

MouseCursor_t* cursor = NULL;

/* CONSTRUCTORS */
void initialize_cursor() {
    if (cursor == NULL) {

        // To make sure it exists
        initialize_mouse_input_events();

        cursor = (MouseCursor_t*)  malloc(sizeof(MouseCursor_t));

        if (cursor == NULL) {
            printf("new_cursor: Failed to allocate memory for the new Cursor\n");
            return;
        }

        // TODO: Use screen size
        cursor->pos = vec2d(200, 200);

        cursor->rendered = true;
        
        cursor->sprite = new_sprite(-2.0f, -2.0f, 1, "ui/wrench_cursor.bmp");
        if (cursor->sprite == NULL) {
            printf("new_testing_cursor: Failed to create the Cursor's Sprite\n");
            return;
        }
    }

}

void free_cursor() {
    if (cursor == NULL) {
		printf("free_cursor: Cannot free a NULL pointer\n");
		return;
  	}
    free_sprite(cursor->sprite);
    free(cursor);
}

/* Activate and Deactivate mouse cursor rendering */
inline void cursor_hide() {
    cursor->rendered = false;
}

inline void cursor_show() {
    cursor->rendered = true;
}

inline bool cursor_is_shown() {
    return cursor->rendered;
}

/* CURSOR METHODS */

void update_cursor() {
    cursor->pos.x = fclampf(
        cursor->pos.x + mouse_get_x_delta(),
        0,
        1024
    );
    cursor->pos.y = fclampf(
        cursor->pos.y - mouse_get_y_delta(),
        0,
        768
    );

    if (mouse_get_rb_down())
        printf("x: %u, y: %u \n", (int) cursor->pos.x, (int) cursor->pos.y);
}

void render_cursor() {
    if (cursor->rendered)
        draw_sprite_vec2d(cursor->sprite, cursor->pos, COLOR_NO_MULTIPLY, SPRITE_NORMAL);
}

inline bool is_cursor_inside_rect(Rect_t* rect) {
    return is_point_inside_rect(rect, cursor->pos);
}

inline float cursor_get_x() {
    return cursor->pos.x;
}

inline float cursor_get_y() {
    return cursor->pos.y;
}

inline Vec2d_t cursor_get_pos() {
    return cursor->pos;
}
