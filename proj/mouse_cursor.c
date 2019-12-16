#include "mouse_cursor.h"
#include "sprite.h"
#include "input_events.h"
#include "math_utils.h"

struct MouseCursor {
    Vec2d_t pos;
    Sprite_t* sprite;
    MouseInputEvents_t* mouse_ev;
    bool rendered;
};

/* CONSTRUCTORS */
MouseCursor_t* new_cursor(MouseInputEvents_t *mouse_ev, const char* mouse_bmp_file_name) {
    MouseCursor_t* cursor = (MouseCursor_t*)  malloc(sizeof(MouseCursor_t));
    if (cursor == NULL) {
        printf("new_cursor: Failed to allocate memory for the new Cursor\n");
        return NULL;
    }

    if (mouse_ev == NULL) {
        printf("new_cursor: MouseInputEvents_t must be valid (it was a NULL pointer)\n");
        return NULL;
    }

    cursor->mouse_ev = mouse_ev;

    // TODO: Use screen size
    cursor->pos = vec2d(200, 200);

    cursor->rendered = true;
    
    cursor->sprite = new_sprite(0.0f, 0.0f, 1, mouse_bmp_file_name);
    if (cursor->sprite == NULL) {
        printf("new_testing_cursor: Failed to create the Cursor's Sprite\n");
        return NULL;
    }

    return cursor;
}

void free_cursor(MouseCursor_t* cursor) {
    if (cursor == NULL) {
		printf("free_cursor: Cannot free a NULL pointer\n");
		return;
  	}
    free_sprite(cursor->sprite);
    free(cursor);
}

/* Activate and Deactivate mouse cursor rendering */
inline void cursor_hide(MouseCursor_t* cursor) {
    cursor->rendered = false;
}

inline void cursor_show(MouseCursor_t* cursor) {
    cursor->rendered = true;
}

inline bool cursor_is_shown(MouseCursor_t* cursor) {
    return cursor->rendered;
}

/* CURSOR METHODS */

void update_cursor(MouseCursor_t* cursor) {
    cursor->pos.x = fclampf(
        cursor->pos.x + cursor->mouse_ev->x_delta,
        0,
        1024
    );
    cursor->pos.y = fclampf(
        cursor->pos.y - cursor->mouse_ev->y_delta,
        0,
        768
    );

    if (cursor->mouse_ev->right_button_down)
        printf("x: %u, y: %u \n", (int) cursor->pos.x, (int) cursor->pos.y);
}

void render_cursor(MouseCursor_t* cursor) {
    if (cursor->rendered)
        draw_sprite_vec2d(cursor->sprite, cursor->pos, COLOR_NO_MULTIPLY, false);
}

inline bool is_cursor_inside_rect(MouseCursor_t* cursor, Rect_t* rect) {
    return is_point_inside_rect(rect, cursor->pos);
}

inline bool cursor_left_button_down(MouseCursor_t* cursor) {
    return cursor->mouse_ev->left_button_down;
}

inline bool cursor_left_button(MouseCursor_t* cursor) {
    return cursor->mouse_ev->left_button;
}

inline bool cursor_right_button_down(MouseCursor_t* cursor) {
    return cursor->mouse_ev->right_button_down;
}

inline bool cursor_right_button(MouseCursor_t* cursor) {
    return cursor->mouse_ev->right_button;
}

inline float cursor_get_x(MouseCursor_t* cursor) {
    return cursor->pos.x;
}

inline float cursor_get_y(MouseCursor_t* cursor) {
    return cursor->pos.y;
}

inline Vec2d_t cursor_get_pos(MouseCursor_t* cursor) {
    return cursor->pos;
}
