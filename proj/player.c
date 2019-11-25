#include "player.h"
#include "geometry.h"
#include "sprite.h"

struct Player {
    Rect_t rect;
    Sprite_t *sprite;
    float base_speed, base_jump, speed_mult, jump_mult;
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
  player->base_speed = 5;
  player->base_jump = 20;
  player->speed_mult = 1;
  player->jump_mult = 1;

  printf("new_testing_player: Finished making player\n");
  return player;
}

void free_player(Player_t* player) {
  free_sprite(player->sprite);
  free(player);
}


void render_player(Player_t* player) {
  draw_sprite(player->sprite, &player->rect, 0, COLOR_NO_MULTIPLY);
}
