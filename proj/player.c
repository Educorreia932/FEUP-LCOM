#include "player.h"
#include "geometry.h"
#include "math.h"
#include "sprite.h"

/* PHYSICS STUFF */
#define BASE_GRAVITY 800.0f
#define FALLING_MULT 1.6f
#define MAX_VELOCITY 800.0f

/* PLAYER CONSTANTS */
#define PLAYER_BASE_SPEED 8.0f // Raw pixels
#define PLAYER_BASE_JUMP 575.0f

struct Player {
		Rect_t rect;
		Sprite_t *sprite;
		float speed_mult, jump_mult;
		float y_speed, gravity;
};

// TODO: ALL OF THIS

Player_t* new_player() {
	return NULL;
}

Player_t* new_testing_player() {
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
		200,
		200, 
		sprite_get_width(player->sprite), 
		sprite_get_height(player->sprite)
	);

	printf("new_testing_player: Customizing player stats\n");
	player->speed_mult = 1.0f;
	player->jump_mult = 1.0f;
	player->y_speed = 0.0f;
	player->gravity = BASE_GRAVITY;

	printf("new_testing_player: Finished making player\n");
	return player;
}

void free_player(Player_t* player) {
	free_sprite(player->sprite);
	free(player);
}

// TODO: This is_grounded is very sketchy, improve it later down the road
bool player_is_grounded(Player_t* player, Platforms_t* plat) {
	Rect_t r = rect(
					player->rect.x + 2,
					player->rect.y + player->rect.h,
					player->rect.w - 2,
					8
				);
	return does_collide_platforms(plat, &r);
}

// TODO: Implement animations depending on movement
void player_movement(Player_t* player, Platforms_t* plat, KbdInputEvents_t* kbd_ev, MouseInputEvents_t* mouse_ev) {
	
	Rect_t previous_pos = player->rect;
	// Horizontal Movement
	// TODO: Add a secondary check for larger horizontal movements
	if (!(kbd_ev->right_arrow && kbd_ev->left_arrow)) {

		if (kbd_ev->right_arrow) {

			player->rect.x += PLAYER_BASE_SPEED * player->speed_mult;
			if (does_collide_platforms(plat, &player->rect))
				player->rect = previous_pos;
		
		}
		if (kbd_ev->left_arrow) {
			player->rect.x -= PLAYER_BASE_SPEED * player->speed_mult;
			if (does_collide_platforms(plat, &player->rect))
				player->rect = previous_pos;
		}
	}

	// TODO: Temporary fix to check alternating gravity
	if (kbd_ev->key_x_down)
		player->gravity *= -1;


	// Vertical Movement
	previous_pos = player->rect;
	if (player->y_speed * player->gravity > 0)
		player->y_speed += DELTATIME * player->gravity;
	else
		player->y_speed += DELTATIME * FALLING_MULT * player->gravity;

	// Jump button
	if (kbd_ev->key_z_down)
		if (player_is_grounded(player, plat))
			player->y_speed = -PLAYER_BASE_JUMP * player->jump_mult * fsign(player->gravity);

	// Kinda like terminal velocity
	player->y_speed = fclamp(player->y_speed, -MAX_VELOCITY, MAX_VELOCITY);

	player->rect.y += player->y_speed * DELTATIME
		+ 0.5f * fsquare(DELTATIME) * player->gravity;
	
	if (does_collide_platforms(plat, &player->rect)) {
		player->rect = previous_pos;
		player->y_speed /= 3;
	}

}

void render_player(Player_t* player) {
		draw_sprite(player->sprite, &player->rect, COLOR_NO_MULTIPLY);
}

