#include "player.h"
#include "geometry.h"
#include "math_utils.h"
#include "sprite.h"

/* PHYSICS STUFF */
#define BASE_GRAVITY 800.0f
#define FALLING_MULT 1.6f
#define MAX_VELOCITY 800.0f

#define IS_GROUNDED_MARGIN 8.0f

/* PLAYER CONSTANTS */
#define PLAYER_BASE_SPEED 230.0f // Raw pixels
#define PLAYER_BASE_JUMP 575.0f

#define PLAYER_DEFAULT_SPEED_MULT 1.0f
#define PLAYER_MIN_SPEED_MULT 0.4f
#define PLAYER_MAX_SPEED_MULT 1.6f
#define PLAYER_SPEED_MULT_STEP 0.1f
#define PLAYER_DEFAULT_JUMP_MULT 1.0f
#define PLAYER_MIN_JUMP_MULT 0.4f
#define PLAYER_MAX_JUMP_MULT 1.6f
#define PLAYER_JUMP_MULT_STEP 0.1f

struct Player {
		Rect_t rect;
		Sprite_t *sprite;
		float speed_mult, jump_mult;
		float y_speed, gravity;
		float x_spawn, y_spawn;
		bool heading_right;
		bool is_single_player;
};

// TODO: ALL OF THIS

Player_t* new_player() {
	return NULL;
}

Player_t* new_testing_player(bool is_single_player) {
	Player_t* player = (Player_t*) malloc(sizeof(Player_t));
	
	if (player == NULL) {
		printf("new_testing_player: Failed to allocate memory for the player object\n");
		return NULL;
	}

	printf("new_testing_player: Creating player sprite\n");

	player->sprite = new_sprite(0, 0, 1, "/home/lcom/labs/proj/assets/watt_tmp.bmp");

	if (player->sprite == NULL) {
		printf("new_testing_player: Failed to create the Sprite object\n");
		return NULL;
	}

	printf("new_testing_player: Customizing player Rect\n");
	player->rect = rect_from_uints(
		200.0f,
		200.0f, 
		sprite_get_width(player->sprite), 
		sprite_get_height(player->sprite)
	);

	printf("new_testing_player: Customizing player stats\n");
	player->speed_mult = PLAYER_DEFAULT_SPEED_MULT;
	player->jump_mult = PLAYER_DEFAULT_JUMP_MULT;
	player->y_speed = 0.0f;
	player->gravity = BASE_GRAVITY;
	player->x_spawn = 200;
	player->y_spawn = 200;
	player->heading_right = true;

	player->is_single_player = is_single_player;

	printf("new_testing_player: Finished making player\n");
	return player;
}

void free_player(Player_t* player) {
	if (player == NULL) {
		printf("free_player: Cannot free a NULL pointer\n");
		return;
  	}
	free_sprite(player->sprite);
	free(player);
}

// TODO: This is_grounded is very sketchy, improve it later down the road
bool player_is_grounded(Player_t* player, Platforms_t* plat) {
	Rect_t r;
	if (player->gravity > 0) {
		r = rect(
					player->rect.x + 2,
					player->rect.y + player->rect.h,
					player->rect.w - 2,
					IS_GROUNDED_MARGIN
				);
	}
	else {
		r = rect(
			player->rect.x + 2,
			player->rect.y - IS_GROUNDED_MARGIN,
			player->rect.w - 2,
			IS_GROUNDED_MARGIN
		);
	}
	return does_collide_platforms(plat, &r);
}

// TODO: Implement animations depending on movement
void player_movement(Player_t* player, Platforms_t* plat, Lasers_t* lasers, Resistances_t * resistances, KbdInputEvents_t* kbd_ev, MouseInputEvents_t* mouse_ev) {
	Rect_t previous_rect = player->rect;

	// Horizontal Movement
	float h_delta = 0;

	if (get_key(kbd_ev, KBD_ARROW_RIGHT)) {
		h_delta = DELTATIME * PLAYER_BASE_SPEED * player->speed_mult;
		player->heading_right = true;
	}
	
	if (get_key(kbd_ev, KBD_ARROW_LEFT)) {
		h_delta = DELTATIME * -PLAYER_BASE_SPEED * player->speed_mult;
		player->heading_right = false;
	}

	if (h_delta != 0) {
		player->rect.x += h_delta / 2;
		
		if (does_collide_platforms(plat, &player->rect)) {
			player->rect.x -= h_delta / 4;

			if (does_collide_platforms(plat, &player->rect))
				player->rect = previous_rect;
		}

		else {	
			previous_rect = player->rect;
			player->rect.x += h_delta / 2;
			if (does_collide_platforms(plat, &player->rect))
				player->rect = previous_rect;
		}
	}

	if (player->is_single_player) {
		if (get_key_down(kbd_ev, KBD_C))
			lasers_cycle_link_id(lasers);
		if (get_key_down(kbd_ev, KBD_X))
			player->gravity *= -1;
		if (get_key_down(kbd_ev, KBD_Q))
			player->jump_mult = fclampf(player->jump_mult - PLAYER_JUMP_MULT_STEP, PLAYER_MIN_JUMP_MULT, PLAYER_MAX_JUMP_MULT);
		if (get_key_down(kbd_ev, KBD_W))
			player->jump_mult = fclampf(player->jump_mult + PLAYER_JUMP_MULT_STEP, PLAYER_MIN_JUMP_MULT, PLAYER_MAX_JUMP_MULT);
		if (get_key_down(kbd_ev, KBD_A))
			player->speed_mult = fclampf(player->speed_mult - PLAYER_SPEED_MULT_STEP, PLAYER_MIN_SPEED_MULT, PLAYER_MAX_SPEED_MULT);
		if (get_key_down(kbd_ev, KBD_S))
			player->speed_mult = fclampf(player->speed_mult + PLAYER_SPEED_MULT_STEP, PLAYER_MIN_SPEED_MULT, PLAYER_MAX_SPEED_MULT);
	}

	// Vertical Movement
	previous_rect = player->rect;

	if (player->y_speed * player->gravity > 0)
		player->y_speed += DELTATIME * player->gravity;
	
	else
		player->y_speed += DELTATIME * FALLING_MULT * player->gravity;

	// Jump button
	if (get_key_down(kbd_ev, KBD_Z))
		if (player_is_grounded(player, plat))
			player->y_speed = -PLAYER_BASE_JUMP * player->jump_mult * fsign(player->gravity);

	// Kinda like terminal velocity
	player->y_speed = fclampf(player->y_speed, -MAX_VELOCITY, MAX_VELOCITY);

	player->rect.y += player->y_speed * DELTATIME
		+ 0.5f * powf(DELTATIME, 2) * player->gravity;
	
	if (does_collide_platforms(plat, &player->rect)) {
		player->rect = previous_rect;
		player->y_speed /= 3;
	}

	if (player_is_dead(lasers, &player->rect) || player_touches_resistance(resistances, &player->rect)) {
		player->rect.x = player->x_spawn;
		player->rect.y = player->y_spawn;
		player->y_speed = 0;
	}

}

void render_player(Player_t* player) {
	draw_sprite(player->sprite, &player->rect, COLOR_NO_MULTIPLY, !player->heading_right);
}

