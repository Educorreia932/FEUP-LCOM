#pragma once

#include "lcom/lcf.h"

typedef struct MainMenu MainMenu_t;

MainMenu_t* new_main_menu();
void free_main_menu(MainMenu_t* menu);

void update_main_menu();
void render_main_menu();
