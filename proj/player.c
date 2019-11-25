#include "player.h"
#include "geometry.h"
#include "sprite.h"


/* PLAYER CONSTANTS */
#define PLAYER_BASE_SPEED 8.0f
#define PLAYER_BASE_JUMP 20.0f

struct Player {
    Rect_t rect;
    Sprite_t *sprite;
    float speed_mult, jump_mult;
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

	printf("new_testing_player: Finished making player\n");
	return player;
}

void free_player(Player_t* player) {
  	free_sprite(player->sprite);
  	free(player);
}

// TODO: Implement animations depending on movement
void player_movement(Player_t* player, Platforms_t* plat, KbdInputEvents_t* kbd_ev, MouseInputEvents_t* mouse_ev) {
  
  // Horizontal Movement
  if (!(kbd_ev->right_arrow && kbd_ev->left_arrow)) {
    Rect_t previous_pos = player->rect;

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
  
  // Vertical Movement
  if (kbd_ev->key_z_down) {
    Rect_t previous_pos = player->rect;
    player->rect.y -= PLAYER_BASE_JUMP * player->jump_mult;
    if (does_collide_platforms(plat, &player->rect))
        player->rect = previous_pos;
  }
}

void render_player(Player_t* player) {
  	draw_sprite(player->sprite, &player->rect, COLOR_NO_MULTIPLY);
}

